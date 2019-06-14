
/***************************************************************************************************
*                    (c) Copyright 1992-2019 Syncretize technologies co.,ltd.
*                                       All Rights Reserved
*
*\File          drv_spi_flash.h
*\Description   
*\Note          
*\Log           2019.03.26    Ver 1.0    Jerry
*               创建文件。
***************************************************************************************************/
#ifndef _DRV_SPI_FLASH_H
#define _DRV_SPI_FLASH_H

#include "stm32f10x.h"
#include "spiffs.h"

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
#define SPI_FLASH_SPI                           SPI2
#define SPI_FLASH_SPI_CLK                       RCC_APB1Periph_SPI2
#define SPI_FLASH_SPI_SCK_PIN                   GPIO_Pin_13                  
#define SPI_FLASH_SPI_SCK_GPIO_PORT             GPIOB                       
#define SPI_FLASH_SPI_SCK_GPIO_CLK              RCC_APB2Periph_GPIOB
#define SPI_FLASH_SPI_MISO_PIN                  GPIO_Pin_14                  
#define SPI_FLASH_SPI_MISO_GPIO_PORT            GPIOB                       
#define SPI_FLASH_SPI_MISO_GPIO_CLK             RCC_APB2Periph_GPIOB
#define SPI_FLASH_SPI_MOSI_PIN                  GPIO_Pin_15                  
#define SPI_FLASH_SPI_MOSI_GPIO_PORT            GPIOB                       
#define SPI_FLASH_SPI_MOSI_GPIO_CLK             RCC_APB2Periph_GPIOB
#define SPI_FLASH_CS_PIN                        GPIO_Pin_12                  
#define SPI_FLASH_CS_GPIO_PORT                  GPIOB                       
#define SPI_FLASH_CS_GPIO_CLK                   RCC_APB2Periph_GPIOB

#define  SPI_FLASH_CS_LOW()       GPIO_ResetBits(GPIOB, GPIO_Pin_12)
#define  SPI_FLASH_CS_HIGH()      GPIO_SetBits(GPIOB, GPIO_Pin_12)

//#define SPI_FLASH_PageSize      4096
#define SPI_FLASH_PageSize              256
#define SPI_FLASH_PerWritePageSize      256

#define W25X_WriteEnable		      0x06 
#define W25X_WriteDisable		      0x04 
#define W25X_ReadStatusReg		    0x05 
#define W25X_WriteStatusReg		    0x01 
#define W25X_ReadData			        0x03 
#define W25X_FastReadData		      0x0B 
#define W25X_FastReadDual		      0x3B 
#define W25X_PageProgram		      0x02 
#define W25X_BlockErase			      0xD8 
#define W25X_SectorErase		      0x20 
#define W25X_ChipErase			      0xC7 
#define W25X_PowerDown			      0xB9 
#define W25X_ReleasePowerDown	    0xAB 
#define W25X_DeviceID			        0xAB 
#define W25X_ManufactDeviceID   	0x90 
#define W25X_JedecDeviceID		    0x9F 

#define WIP_Flag                  0x01  /* Write In Progress (WIP) flag */
#define Dummy_Byte                0xFF

#define  FLASH_WriteAddress     0x000000    //短信修改设置保存首地址
#define  FLASH_WriteAQData      0x5000      //AQ保存首地址
#define  FLASH_CAPACITY         0x3FFFFF    //FLASH总容量
#define  FLASH_AQY              0x300000    //AQ保存末地址

#define  countof(a)      (sizeof(a) / sizeof(*(a)))
#define  BufferSize     (countof(Device_info.MasterIp)-1)

void SPI_FLASH_Init(void);
void SPI_FLASH_SectorErase(u32 SectorAddr, u8 Cmd);
void SPI_FLASH_BulkErase(void);
void SPI_FLASH_PageWrite(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite);
void SPI_FLASH_BufferWrite(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite);
void SPI_FLASH_BufferRead(u8* pBuffer, u32 ReadAddr, u16 NumByteToRead);
uint32_t  SPI_FLASH_ReadID(void);
uint32_t  SPI_FLASH_ReadDeviceID(void);
void SPI_FLASH_StartReadSequence(u32 ReadAddr);
void SPI_Flash_PowerDown(void);
void SPI_Flash_WAKEUP(void);

uint8_t  SpiFlashCheckBusy(void);
uint8_t  SPI_FLASH_ReadByte(void);
uint8_t  SPI_FLASH_SendByte(u8 byte);
uint16_t SPI_FLASH_SendHalfWord(u16 HalfWord);
void SPI_FLASH_WriteEnable(void);
void SPI_FLASH_WaitForWriteEnd(void);

/*文件系统接口*/
s32_t SPI_Flash_Erase(u32_t Addr, u32_t Num);
s32_t SPI_Flash_Read(u32_t ReadAddr, u32_t NumByteToRead, u8_t * pBuffer);
s32_t SPI_Flash_Write(u32_t WriteAddr, u32_t NumByteToWrite, u8_t * pBuffer);

#endif /*_DRV_SPI_FLASH_H*/



/***************************************************************************************************
******************************************** END OF FILE *******************************************
***************************************************************************************************/




