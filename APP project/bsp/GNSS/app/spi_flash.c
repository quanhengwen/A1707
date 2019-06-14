
/***************************************************************************************************
*                    (c) Copyright 1992-2019 Syncretize technologies co.,ltd.
*                                       All Rights Reserved
*
*\File          spi_flash.c
*\Description   
*\Note          
*\Log           2019.03.26    Ver 1.0    Jerry
*               创建文件。
***************************************************************************************************/
#include "drv_spi_flash.h"
#include "spi_flash.h"
#include "app_board.h"
#include <dfs_elm.h>
#include <dfs_fs.h>
#include "dfs_posix.h"
#include "finsh.h"


//#define USE_SPIFFS 


#define FILE_NUM  20
uint8_t file_list[FILE_NUM][24];
/*文件系统的文件个数*/
uint8_t file_cnt = 0;

rt_mq_t mq_gps_data;//GPS数据解析后发送的消息队列

rt_sem_t sem_storage;//用于同步文件存储的信号量

rt_mutex_t mutex_file;//用于文件操作的互斥量

uint8_t record_flag;//如果等于1，则进行数据记录，否则不进行GPS数据记录

#define GPS_DATA_BUF_LENGTH  600 //缓冲区大小，代表多少组GPS数据

typedef struct
{
	uint8_t year;
	uint8_t month;
	uint8_t day;
	uint8_t hour;
	uint8_t minute;
	uint8_t second;
	uint16_t mili_second;//毫秒
	float longitude;//经度
	float latitude;//纬度
}__attribute__((packed)) gnss_data_type;


gnss_data_type gnss_data_buf1[GPS_DATA_BUF_LENGTH];//GPS数据暂存区
gnss_data_type gnss_data_buf2[GPS_DATA_BUF_LENGTH];//GPS数据写入文件时的缓存


spiffs SPIFlashFS;
spiffs_config SPIFlashCfg={
	.phys_size        = SPIFLASH_CFG_PHYS_SZ,         // SPI Flash的总容量
    .phys_addr        = SPIFLASH_CFG_PHYS_ADDR,       // 起始地址
    .phys_erase_block = SPIFLASH_CFG_PHYS_ERASE_SZ,   // FLASH最大可擦除的块大小(W25X64可以按扇区擦除(4K)或者按块擦除(64K))
    .log_block_size   = SPIFLASH_CFG_LOG_BLOCK_SZ,    // 块的大小(W25X64每块包含16个扇区，每个扇区4K字节，每块的总容量为：16X4=64K=65535字节)
    .log_page_size    = SPIFLASH_CFG_LOG_PAGE_SZ,     //  (W25X64每页包含256个字节，16个页构成一个扇区)
    
    .hal_read_f =  SPI_Flash_Read,    //读
    .hal_write_f = SPI_Flash_Write,   //写
    .hal_erase_f = SPI_Flash_Erase,   //擦除
};
	

static u8_t FS_Work_Buf[SPIFLASH_CFG_LOG_PAGE_SZ*2];
static u8_t FS_FDS[32*4];
static u8_t FS_Cache_Buf[(SPIFLASH_CFG_LOG_PAGE_SZ+32)*4];
 
//char WriteBuf[]={"L05 03 0 1 1 2 7 00010 02000 005 1\nL05 03 1 1 1 0 23 0 1\nP03 7 00030 20000 15 1\nM03 03 1\nL02 01 01\nP01 22 1 1\nM03 03 1\nL02 01 01\nP03 7 00010 20000 5 1\nP01 22 0 1\n"};

//char ReadBuf[2000];	

//格式化文件系统
void Format_SPI_Flash_File_System(void) 
{
	SPIFFS_unmount(&SPIFlashFS);                      
	SPIFFS_format(&SPIFlashFS);	
	//挂载SPIFS	
    int res = SPIFFS_mount(&SPIFlashFS,
                           &SPIFlashCfg,
                           FS_Work_Buf,
                           FS_FDS,
                           sizeof(FS_FDS),
                           FS_Cache_Buf,
                           sizeof(FS_Cache_Buf),
                           0);
}

/***************************************************************************************************
*\Function      Mount_SPI_Flash_File_System
*\Description    加载SPI FLASH文件系统
*\Parameter     
*\Return        void
*\Note          
*\Log           2019.03.26    Ver 1.0    Jerry
*               创建函数。
***************************************************************************************************/
void Mount_SPI_Flash_File_System(void) 
{
	//挂载SPIFS	
    int res = SPIFFS_mount(&SPIFlashFS,
                           &SPIFlashCfg,
                           FS_Work_Buf,
                           FS_FDS,
                           sizeof(FS_FDS),
                           FS_Cache_Buf,
                           sizeof(FS_Cache_Buf),
                           0);
	if(SPIFFS_ERR_NOT_A_FS == res )
	{
		rt_kprintf("SPI FLASH SPIFFS文件系统挂在失败\r\n");
		Format_SPI_Flash_File_System(); //挂载失败，格式化后重新挂载       
	}	
	else if(SPIFFS_OK == res)
	{
		rt_kprintf("SPI FLASH SPIFFS文件系统挂在成功\r\n");
	}
 }
		

 
 
 /***************************************************************************************************
*\Function      DotFormat
*\Description   格式化数据
*\Parameter     
*\Return        void
*\Note          
*\Log           2019.05.07    Ver 1.0    Often
*               创建函数。
***************************************************************************************************/
 static void DotFormat(uint64_t _ullVal, char *_sp) 
{
	/* 数值大于等于10^9 */
	if (_ullVal >= (uint64_t)1e9) 
	{
		_sp += sprintf (_sp, "%d.", (uint32_t)(_ullVal / (uint64_t)1e9));
		_ullVal %= (uint64_t)1e9;
		_sp += sprintf (_sp, "%03d.", (uint32_t)(_ullVal / (uint64_t)1e6));
		_ullVal %= (uint64_t)1e6;
		sprintf (_sp, "%03d.%03d", (uint32_t)(_ullVal / 1000), (uint32_t)(_ullVal % 1000));
		return;
	}
	
	/* 数值大于等于10^6 */
	if (_ullVal >= (uint64_t)1e6) 
	{
		_sp += sprintf (_sp,"%d.", (uint32_t)(_ullVal / (uint64_t)1e6));
		_ullVal %= (uint64_t)1e6;
		sprintf (_sp,"%03d.%03d", (uint32_t)(_ullVal / 1000), (uint32_t)(_ullVal % 1000));
		return;
	}
	
	/* 数值大于等于10^3 */
	if (_ullVal >= 1000) 
	{
		sprintf (_sp, "%d.%03d", (uint32_t)(_ullVal / 1000), (uint32_t)(_ullVal % 1000));
		return;
	}
	
	/* 其它数值 */
	sprintf (_sp,"%d",(uint32_t)(_ullVal));
}

 /***************************************************************************************************
*\Function      ViewSpiFlashCapacity
*\Description   打印出文件系统的总容量、可用容量、已用容量
*\Parameter     
*\Return        void
*\Note          
*\Log           2019.05.07    Ver 1.0    Often
*               创建函数。
***************************************************************************************************/
static void ViewSpiFlashCapacity(void)
{
	uint8_t buf[15], buf1[15], buf2[15];
	u32_t total;
	u32_t used;
	u32_t free;
	
	rt_mutex_take(mutex_file, RT_WAITING_FOREVER);
	
#ifdef USE_SPIFFS
	/* 获取SPI Flash卡总容量和剩余容量 */
	SPIFFS_info(&SPIFlashFS,&total , &used);	
	free = total - used;
#else
	int result1;
	struct statfs file_sys_stat;
		
	result1 = statfs("/", &file_sys_stat);

	total = (long long)file_sys_stat.f_bsize * (long long)file_sys_stat.f_blocks;
	used = (long long)file_sys_stat.f_bsize * (long long)(file_sys_stat.f_blocks - file_sys_stat.f_bfree);
	free = (long long)file_sys_stat.f_bsize * (long long)file_sys_stat.f_bfree;	
#endif	
	
	rt_mutex_release(mutex_file);
	

	rt_kprintf("------------------------------------------------------------------\r\n");
	
	DotFormat(total, (char *)buf);
	DotFormat(used, (char *)buf1);
	DotFormat(free, (char *)buf2);
	
	rt_kprintf("SPI FLASH的容量 = %10s字节，已用容量 = %10s字节，剩余容量 = %10s字节\r\n", buf, buf1, buf2);
	rt_kprintf("------------------------------------------------------------------\r\n");
	
}

/***************************************************************************************************
*\Function      ViewRootDir
*\Description   列出所有的文件
*\Parameter     
*\Return        void
*\Note          
*\Log           2019.05.07    Ver 1.0    Often
*               创建函数。
***************************************************************************************************/
static void ViewRootDir(void)
{
	file_cnt = 0;
	uint8_t cnt = 0;
	uint8_t buf[15];
	int result;
	
	rt_mutex_take(mutex_file, RT_WAITING_FOREVER); 
	
#ifdef USE_SPIFFS
	
	spiffs_DIR *spiffs_dir = RT_NULL;
	spiffs_DIR d;
	struct spiffs_dirent *ffs_dirent;
	struct spiffs_dirent e;
	spiffs_stat file_stat;
	
	spiffs_dir = SPIFFS_opendir(&SPIFlashFS, "/", &d);
	if (spiffs_dir == RT_NULL)
	{
		rt_kprintf("open directory error!\n");
	}
	else
	{
		d.fs->err_code = 0;
		ffs_dirent = SPIFFS_readdir(&d, &e);
		while(d.fs->err_code == 0)
		{
			
			strcpy((char *)(file_list[file_cnt++] + 1), (char *)(e.name));
			d.fs->err_code = 0;
			ffs_dirent = SPIFFS_readdir(&d, &e);
		}
		
	}
	result = SPIFFS_closedir(&d);
	
	if(file_cnt == 0)
	{
		rt_kprintf("没有文件存储\r\n");
	}
	else
	{
		rt_kprintf("------------------------------------------------------------------\r\n");
		rt_kprintf("序号 | 文件名                 |      文件大小 \r\n");
		
		for(cnt = 0; cnt < file_cnt; cnt++)
		{
			result = SPIFFS_stat(&SPIFlashFS, (char *)(file_list[cnt] + 1), &file_stat);
			DotFormat(file_stat.size, (char *)buf);
			rt_kprintf("%d      %20s    %15s\r\n", cnt, file_list[cnt] + 1, buf);
		}
		
	}
	
	printf("------------------------------------------------------------------\r\n");
	d.block = 0;
	d.entry = 0;
	
#else
	
	struct dirent  *dirent_tmp;
	DIR *dir;
	
	
	struct stat file_stat;
	
	/*打开SPI FLASH文件系统的根目录*/
	dir = opendir("/");
	if (dir == RT_NULL)
	{
		rt_kprintf("open directory error!\n");
	}
	else
	{
		
		dirent_tmp = readdir(dir);
		while(dirent_tmp->d_type == 0x01)
		{
			strcpy((char *)(file_list[file_cnt++] + 1), dirent_tmp->d_name);			
			dirent_tmp = readdir(dir);
		}
		
	}
	
	
	
	
	
	
//	    struct stat stat;
//    int length;
//    char *fullpath, *path;

//    fullpath = NULL;

//	
//	path = rt_strdup("/");

//    /* list directory */
//    if (dfs_file_open(&fd, path, O_DIRECTORY) == 0)
//    {
//        rt_kprintf("Directory %s:\n", path);
//        do
//        {
//            memset(&dirent, 0, sizeof(struct dirent));
//            length = dfs_file_getdents(&fd, &dirent, sizeof(struct dirent));
//            if (length > 0)
//            {
//                memset(&stat, 0, sizeof(struct stat));

//                /* build full path for each file */
//                fullpath = dfs_normalize_path(path, dirent.d_name);
//                if (fullpath == NULL)
//                    break;

//                if (dfs_file_stat(fullpath, &stat) == 0)
//                {
//                    rt_kprintf("%-20s", dirent.d_name);
//                    if (S_ISDIR(stat.st_mode))
//                    {
//                        rt_kprintf("%-25s\n", "<DIR>");
//                    }
//                    else
//                    {
//                        rt_kprintf("%-25lu\n", stat.st_size);
//                    }
//                }
//                else
//                    rt_kprintf("BAD file: %s\n", dirent.d_name);
//                rt_free(fullpath);
//            }
//        }while(length > 0);

//        dfs_file_close(&fd);
//    }
//    else
//    {
//        rt_kprintf("No such directory\n");
//    }
//    if (pathname == NULL)
//        rt_free(path);
	
	
	
	

	if(file_cnt == 0)
	{
		rt_kprintf("没有文件存储\r\n");
	}
	else
	{
		rt_kprintf("------------------------------------------------------------------\r\n");
		rt_kprintf("序号 | 文件名                 |      文件大小 \r\n");
		
		for(cnt = 0; cnt < file_cnt; cnt++)
		{
			result = stat((char *)file_list[cnt], &file_stat);
			DotFormat(file_stat.st_size, (char *)buf);
			rt_kprintf("%d      %20s    %15s\r\n", cnt, file_list[cnt] + 1, buf);
		}
		
	}
	
	printf("------------------------------------------------------------------\r\n");
	
	closedir(dir);
#endif	
	
	rt_mutex_release(mutex_file);
}


/***************************************************************************************************
*\Function      READ
*\Description   打印出对应的文件  
*\Parameter     
*\Return        void
*\Note          
*\Log           2019.05.07    Ver 1.0    Often
*               创建函数。
***************************************************************************************************/
void READ(u8 cmd)
{
	if(cmd >= file_cnt)
	{
		rt_kprintf("文件不存在，请重新刷新分拣列表\r\n");
		return;
	}
	
	gnss_data_type gnss_data_tmp;
	uint8_t buf[50];
	uint8_t last_record_flag;
	last_record_flag = record_flag;
	record_flag = 0;
	
	rt_mutex_take(mutex_file, RT_WAITING_FOREVER); 
	
#ifdef USE_SPIFFS
	
#else
	int fd;
	int result;

	uint32_t cnt = 0;
	
	fd = open((char *)file_list[cmd], O_RDONLY); 
	if (fd>= 0)
	{
		rt_kprintf("------------------------------------------------------------------\r\n");
		rt_kprintf("即将打印文件 %s  的数据\r\n", file_list[cmd] + 1);
		result = read(fd, &gnss_data_tmp, sizeof(gnss_data_tmp));
		while(result >= 16)
		{
			snprintf((char*)buf, 50, "20%02d-%02d-%02d %02d:%02d:%02d.%03d,%.6f,%.6f", gnss_data_tmp.year, gnss_data_tmp.month,
			gnss_data_tmp.day, gnss_data_tmp.hour, gnss_data_tmp.minute, gnss_data_tmp.second, gnss_data_tmp.mili_second, gnss_data_tmp.longitude, gnss_data_tmp.latitude);
			rt_kprintf("%s\n", buf);
			
			cnt += result;
			
			result = read(fd, &gnss_data_tmp, sizeof(gnss_data_tmp));
		}
		result = close(fd);
		
		rt_kprintf("数据打印完毕，文件长度为:%d字节\r\n", cnt);
		rt_kprintf("------------------------------------------------------------------\r\n");
	}
	
#endif	
	record_flag = last_record_flag;
	
	rt_mutex_release(mutex_file);
}
/***************************************************************************************************
*\Function      READ
*\Description   打印出对应的文件  
*\Parameter     
*\Return        void
*\Note          
*\Log           2019.05.07    Ver 1.0    Often
*               创建函数。
***************************************************************************************************/
void READ_NAME(u8* cmd)
{
//	if(cmd >= file_cnt)
//	{
//		rt_kprintf("文件不存在，请重新刷新分拣列表\r\n");
//		return;
//	}
	
	gnss_data_type gnss_data_tmp;
	uint8_t buf[50];
	char tmp[30];
	uint8_t last_record_flag;
	last_record_flag = record_flag;
	record_flag = 0;
	
	rt_mutex_take(mutex_file, RT_WAITING_FOREVER); 
	
#ifdef USE_SPIFFS
	
#else
	int fd;
	int result;

	uint32_t cnt = 0;
	tmp[0] = '/';
	strcpy((char*) (tmp + 1), (const char*)cmd);
	
	fd = open((char *)tmp, O_RDONLY); 
	if (fd>= 0)
	{
		rt_kprintf("------------------------------------------------------------------\r\n");
		rt_kprintf("即将打印文件 %s  的数据\r\n", tmp);
		result = read(fd, &gnss_data_tmp, sizeof(gnss_data_tmp));
		while(result >= 16)
		{
			snprintf((char*)buf, 50, "20%02d-%02d-%02d %02d:%02d:%02d.%03d,%.6f,%.6f", gnss_data_tmp.year, gnss_data_tmp.month,
			gnss_data_tmp.day, gnss_data_tmp.hour, gnss_data_tmp.minute, gnss_data_tmp.second, gnss_data_tmp.mili_second, gnss_data_tmp.longitude, gnss_data_tmp.latitude);
			rt_kprintf("%s\n", buf);
			
			cnt += result;
			
			result = read(fd, &gnss_data_tmp, sizeof(gnss_data_tmp));
		}
		result = close(fd);
		
		rt_kprintf("数据打印完毕，文件长度为:%d字节\r\n", cnt);
		rt_kprintf("------------------------------------------------------------------\r\n");
	}
	
#endif	
	record_flag = last_record_flag;
	
	rt_mutex_release(mutex_file);
}
/***************************************************************************************************
*\Function      DELET
*\Description   删除对应的文件  
*\Parameter     输入的是文件对应的序号
*\Return        void
*\Note          
*\Log           2019.05.07    Ver 1.0    Often
*               创建函数。
***************************************************************************************************/
void DELET(u8 cmd)
{
	int result;
	
	if(cmd >= file_cnt)
	{
		rt_kprintf("文件不存在，请重新刷新分拣列表\r\n");
		return;
	}
	rt_mutex_take(mutex_file, RT_WAITING_FOREVER); 
	
#ifdef USE_SPIFFS
	result = SPIFFS_remove(&SPIFlashFS, (char *)(file_list[cmd] + 1));
#else
	unlink((char *)file_list[cmd]);
#endif	
	
	
	ViewSpiFlashCapacity();
	ViewRootDir();
	
	rt_mutex_release(mutex_file);
}

/***************************************************************************************************
*\Function      LIST
*\Description   列出容量和所有的文件  
*\Parameter     
*\Return        void
*\Note          
*\Log           2019.05.07    Ver 1.0    Often
*               创建函数。
***************************************************************************************************/
void LIST(void)
{
	ViewSpiFlashCapacity();
	ViewRootDir();
}

/***************************************************************************************************
*\Function      FORMAT
*\Description   格式化文件系统  
*\Parameter     
*\Return        void
*\Note          
*\Log           2019.05.07    Ver 1.0    Often
*               创建函数。
***************************************************************************************************/
void FORMAT(void)
{
	int result;
	
	rt_mutex_take(mutex_file, RT_WAITING_FOREVER);	
	
#ifdef USE_SPIFFS
//	result = SPIFFS_format(&SPIFlashFS);
	Format_SPI_Flash_File_System();
#else
	dfs_mkfs("elm", "w25q64");
#endif	

	rt_mutex_release(mutex_file);
}
extern void gps_enable(u8 enable_flag);
/***************************************************************************************************
*\Function      STOP
*\Description   停止文件记录  
*\Parameter     
*\Return        void
*\Note          
*\Log           2019.05.07    Ver 1.0    Often
*               创建函数。
***************************************************************************************************/
void STOP(void)
{
	record_flag = 0;
	gps_enable(0);//0关闭gps
	rt_kprintf("关闭文件记录\r\n");
}

/***************************************************************************************************
*\Function      START
*\Description   开始文件记录  
*\Parameter     
*\Return        void
*\Note          
*\Log           2019.05.07    Ver 1.0    Often
*               创建函数。
***************************************************************************************************/
void START(void)
{
	record_flag = 1;
	gps_enable(1);//1打开gps
	rt_kprintf("开启文件记录\r\n");
}

/***************************************************************************************************
*\Function      CMD
*\Description   列出操作命令  
*\Parameter     
*\Return        void
*\Note          
*\Log           2019.05.07    Ver 1.0    Often
*               创建函数。
***************************************************************************************************/
void CMD(void)
{
	rt_kprintf("------------------------------------------------------------------\r\n");
	rt_kprintf("操作命令如下：\r\n");
	rt_kprintf("1、LIST()        列出所有的文件\r\n");
	rt_kprintf("2、FORMAT()      格式化文件系统\r\n");
	rt_kprintf("3、READ(x)       打印出某个文件,x为列表中的序号\r\n");
	rt_kprintf("4、DELET(x)      删除摸个文件,x为列表中的序号\r\n");
	rt_kprintf("5、READ_NAME(x)  打印出某个文件,文件名\r\n");
	rt_kprintf("6、STOP()        停止文件记录\r\n");
	rt_kprintf("7、START()       开始文件记录\r\n");
	rt_kprintf("8、CMD()         显示操作命令\r\n");
	rt_kprintf("9、ls()          使用系统命令显示所有文件\r\n");
	rt_kprintf("------------------------------------------------------------------\r\n");
}

/***************************************************************************************************
*\Function      rt_thread_spiflash_entry
*\Description   文件系统的线程
*\Parameter     
*\Return        void
*\Note          
*\Log           2019.03.26    Ver 1.0    Jerry
*               创建函数。
***************************************************************************************************/
void rt_thread_storage_entry(void* parameter)
{
	rt_err_t err;
	uint8_t file_name[24];//存储文件名
	gnss_data_type gnss_data;
	int fd;
	
	err = rt_sem_take(sem_storage, RT_WAITING_FOREVER);
	if (err == RT_EOK)
	{
		/*第一次进入，取年月日时分秒为文件名*/
		memcpy(&gnss_data, &gnss_data_buf2[0], 16);
		snprintf((char*)file_name, 24, "/20%d%02d%02d%02d%02d%02d.bin", gnss_data.year, gnss_data.month,
		gnss_data.day, gnss_data.hour, gnss_data.minute, gnss_data.second);
		
		rt_mutex_take(mutex_file, RT_WAITING_FOREVER);
		
		fd = open((char*)file_name, O_CREAT | O_RDWR);
		if (fd>= 0)
		{
			write(fd, gnss_data_buf2, sizeof(gnss_data_buf2));
			close(fd);
			rt_kprintf("Write done.\n");
		}
		
		rt_mutex_release(mutex_file);
	}
	
	while(1)
	{
		err = rt_sem_take(sem_storage, RT_WAITING_FOREVER);
		if (err == RT_EOK)
		{
			rt_mutex_take(mutex_file, RT_WAITING_FOREVER);
			
			fd = open((char*)file_name, O_RDWR | O_APPEND);
			if (fd>= 0)
			{
				write(fd, gnss_data_buf2, sizeof(gnss_data_buf2));
				close(fd);
				rt_kprintf("Write done.\n");
			}
			
			rt_mutex_release(mutex_file);
		}		
	}
}
/***************************************************************************************************
*\Function      rt_thread_spiflash_entry
*\Description   文件系统的线程
*\Parameter     
*\Return        void
*\Note          
*\Log           2019.03.26    Ver 1.0    Jerry
*               创建函数。
***************************************************************************************************/
void rt_thread_spiflash_entry(void* parameter)
{	
	rt_err_t err;
	rt_thread_t tid;
	uint8_t mq_rev[16];
	uint8_t i;
		
	for(i = 0; i < FILE_NUM; i++)
	{
		file_list[i][0] = '/';
	}
	
	/*允许记录文件*/
	record_flag = 1;
	
	/*GPS数据用的消息队列*/
	mq_gps_data = rt_mq_create("gps_mq", 16, 40, RT_IPC_FLAG_FIFO);
	
	/*缓冲区1接受完同步记录文件的信号量*/
	sem_storage = rt_sem_create("dsem", 0, RT_IPC_FLAG_FIFO);
	
	mutex_file = rt_mutex_create("dmutex", RT_IPC_FLAG_FIFO);
	
#ifdef USE_SPIFFS
	SPI_FLASH_Init();//外设初始化	
	Mount_SPI_Flash_File_System();//spi flash文件系统初始化	
	rt_kprintf("\n");
	spiffs_file fc;
	
//	//打开文件，如果文件不存在，自动创建
//	 fc = SPIFFS_open(&SPIFlashFS, "my1_file.ini", SPIFFS_CREAT | SPIFFS_TRUNC | SPIFFS_RDWR, 0);
//	//写文件
//	if (SPIFFS_write(&SPIFlashFS, fc, WriteBuf, sizeof(WriteBuf)) < 0)
//	rt_kprintf(" w_errno %i\n", SPIFFS_errno(&SPIFlashFS));
//	SPIFFS_close(&SPIFlashFS, fc); 
//	//读文件
//	fc = SPIFFS_open(&SPIFlashFS, "my1_file.ini", SPIFFS_RDWR, 0);
//	if (SPIFFS_read(&SPIFlashFS, fc, ReadBuf, sizeof(WriteBuf)) < 0)
//	rt_kprintf(" r_errno %i\n", SPIFFS_errno(&SPIFlashFS));
//	SPIFFS_close(&SPIFlashFS, fc);
//	//打印输出文件内容
//	rt_kprintf("%s\n", ReadBuf);
	
//	ViewSpiFlashCapacity();
	
	
//	fc = SPIFFS_open(&SPIFlashFS, "01.ini", SPIFFS_CREAT | SPIFFS_TRUNC | SPIFFS_RDWR, 0);
//	if (SPIFFS_write(&SPIFlashFS, fc, "123", sizeof("123")) < 0)
//	{
//		rt_kprintf("err \r\n");
//	}
//	SPIFFS_close(&SPIFlashFS, fc);
//	
//	fc = SPIFFS_open(&SPIFlashFS, "02.ini", SPIFFS_CREAT | SPIFFS_TRUNC | SPIFFS_RDWR, 0);
//	if (SPIFFS_write(&SPIFlashFS, fc, "456", sizeof("456")) < 0)
//	{
//		rt_kprintf("err \r\n");
//	}
//	SPIFFS_close(&SPIFlashFS, fc);
	
//	SPIFFS_vis(&SPIFlashFS);

#else 

	w25qxx_init(&W25Q64, "w25q64");
	dfs_init();  
	elm_init();
	
	u32 fail_count = 0;

	/* mount fat partition as root directory */
	while (fail_count++ < 2)
	{
		//挂载成功则退出 否则尝试一次格式化再挂载（用于第一次上电）
		if (dfs_mount("w25q64", "/", "elm", 0, 0) == 0)
		{
			rt_kprintf("File System initialized!\n");  
			break;
		}
		dfs_mkfs("elm", "w25q64");
		rt_kprintf("W25Q64 format...\n");
	}


//	int fd;
	
//	uint8_t test[160] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16};

//	fd = open("/123.bin", O_CREAT | O_RDWR);
//	if (fd>= 0)
//	{
//		write(fd, ReadBuf, sizeof(ReadBuf));
//		close(fd);
//		rt_kprintf("Write done.\n");
//	}

//	fd = open("/20190507103849.bin", O_CREAT | O_RDWR);
//	if (fd>= 0)
//	{
//		write(fd, "123456546sf", sizeof("123456546sf"));
//		close(fd);
//		rt_kprintf("Write done.\n");
//	}
	
#endif
	
	CMD();
	
	ViewSpiFlashCapacity();
	
	ViewRootDir();
	
	tid = rt_thread_create("storeT", rt_thread_storage_entry, RT_NULL, 2048, 15, 5);
    if (tid != RT_NULL)
        rt_thread_startup(tid); 
	
	rt_tick_t last_tick;
	rt_tick_t current_tick;
	uint16_t data_cnt = 0;
	
	rt_thread_delay(200);
	
	int fd;
	
	while (1)
	{
		
		if(record_flag)
		{
			err = rt_mq_recv (mq_gps_data, mq_rev, 16, 1);
			if(err == RT_EOK)
			{
				
				memcpy(&gnss_data_buf1[data_cnt++], mq_rev, 16);
				if(data_cnt>= GPS_DATA_BUF_LENGTH)
				{
					data_cnt = 0;
					/*数据满了，复制到缓冲区2，发送信号量*/
					memcpy(gnss_data_buf2, gnss_data_buf1, sizeof(gnss_data_buf1));
					rt_sem_release(sem_storage);
					
				}
			}			
		}
		else
		{
			rt_thread_delay(10);
		}
		rt_thread_delay(2);
		
//		last_tick = rt_tick_get();
//		fd = open("/123.bin", O_RDWR | O_APPEND);
//		if (fd>= 0)
//		{
//			write(fd, gnss_data_buf2, sizeof(gnss_data_buf2));
//			close(fd);
//		}
//		
////		fc = SPIFFS_open(&SPIFlashFS, "02.ini", SPIFFS_CREAT | SPIFFS_APPEND | SPIFFS_RDWR, 0);
////		if (SPIFFS_write(&SPIFlashFS, fc, ReadBuf, sizeof(ReadBuf)) < 0)
////		{
////			rt_kprintf("err \r\n");
////		}
////		SPIFFS_close(&SPIFlashFS, fc);
//	
		
		
//		current_tick = rt_tick_get();
//		rt_kprintf("用时%d\r\n", current_tick - last_tick);
		
//		ViewSpiFlashCapacity();

//		ViewRootDir();
		rt_thread_delay(20);
	}
}


void rt_thread_spiflash_init(void)
{
	rt_thread_t spiflash_init_thread;                    
	spiflash_init_thread = rt_thread_create("w25q64",
		rt_thread_spiflash_entry,RT_NULL,2048,10,5);
	if (spiflash_init_thread != RT_NULL)
		rt_thread_startup(spiflash_init_thread);
}





FINSH_FUNCTION_EXPORT(READ, ...)
FINSH_FUNCTION_EXPORT(LIST, ...)
FINSH_FUNCTION_EXPORT(DELET, ...)
FINSH_FUNCTION_EXPORT(FORMAT, ...)
FINSH_FUNCTION_EXPORT(STOP, ...)
FINSH_FUNCTION_EXPORT(START, ...)
FINSH_FUNCTION_EXPORT(READ_NAME, ...)
FINSH_FUNCTION_EXPORT(CMD, ...)

/***************************************************************************************************
******************************************** END OF FILE *******************************************
***************************************************************************************************/


