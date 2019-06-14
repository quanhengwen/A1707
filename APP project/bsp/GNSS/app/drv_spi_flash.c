
/***************************************************************************************************
*                    (c) Copyright 1992-2019 Syncretize technologies co.,ltd.
*                                       All Rights Reserved
*
*\File          drv_spi_flash.c
*\Description   
*\Note          
*\Log           2019.03.26    Ver 1.0    Jerry
*               创建文件。
***************************************************************************************************/
#include "drv_spi_flash.h"


/***************************************************************************************************
*\Function      SPI_FLASH_Init
*\Description   Initializes the peripherals used by the SPI FLASH driver.
*\Parameter     
*\Return        void
*\Note          
*\Log           2019.03.26    Ver 1.0    Jerry
*               创建函数。
***************************************************************************************************/
void SPI_FLASH_Init(void)
{
	SPI_InitTypeDef  SPI_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

	/* Enable SPI2 and GPIO clocks */
	/*!< SPI_FLASH_SPI_CS_GPIO, SPI_FLASH_SPI_MOSI_GPIO, 
	SPI_FLASH_SPI_MISO_GPIO, SPI_FLASH_SPI_DETECT_GPIO 
	and SPI_FLASH_SPI_SCK_GPIO Periph clock enable */
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB , ENABLE);

	/*!< SPI_FLASH_SPI Periph clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);


	/*!< Configure SPI_FLASH_SPI pins: SCK */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/*!< Configure SPI_FLASH_SPI pins: MISO */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/*!< Configure SPI_FLASH_SPI pins: MOSI */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/*!< Configure SPI_FLASH_SPI_CS_PIN pin: SPI_FLASH Card CS pin */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/* Deselect the FLASH: Chip Select high */
	SPI_FLASH_CS_HIGH();

	/* SPI2 configuration */
	// W25X16: data input on the DIO pin is sampled on the rising edge of the CLK. 
	// Data on the DO and DIO pins are clocked out on the falling edge of CLK.
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_Init(SPI2, &SPI_InitStructure);

	/* Enable SPI2  */
	SPI_Cmd(SPI2, ENABLE);
}

/***************************************************************************************************
*\Function      SPI_FLASH_SectorErase
*\Description   Erases the specified FLASH sector.
*\Parameter     SectorAddr  address of the sector to erase.
*\Parameter     Cmd
*\Return        void
*\Note          
*\Log           2019.03.26    Ver 1.0    Jerry
*               创建函数。
***************************************************************************************************/
void SPI_FLASH_SectorErase(u32 SectorAddr,u8 Cmd)
{
	/* Send write enable instruction */
	SPI_FLASH_WriteEnable();
	SPI_FLASH_WaitForWriteEnd();
	/* Sector Erase */
	/* Select the FLASH: Chip Select low */
	SPI_FLASH_CS_LOW();
	/* Send Sector Erase instruction */
	SPI_FLASH_SendByte(Cmd);
	/* Send SectorAddr high nibble address byte */
	SPI_FLASH_SendByte((SectorAddr & 0xFF0000) >> 16);
	/* Send SectorAddr medium nibble address byte */
	SPI_FLASH_SendByte((SectorAddr & 0xFF00) >> 8);
	/* Send SectorAddr low nibble address byte */
	SPI_FLASH_SendByte(SectorAddr & 0xFF);
	/* Deselect the FLASH: Chip Select high */
	SPI_FLASH_CS_HIGH();
	/* Wait the end of Flash writing */
	SPI_FLASH_WaitForWriteEnd();
}


/***************************************************************************************************
*\Function      SPI_FLASH_BulkErase
*\Description   Erases the entire FLASH.
*\Parameter     
*\Return        void
*\Note          
*\Log           2019.03.26    Ver 1.0    Jerry
*               创建函数。
***************************************************************************************************/
void SPI_FLASH_BulkErase(void)
{
	/* Send write enable instruction */
	SPI_FLASH_WriteEnable();

	/* Bulk Erase */
	/* Select the FLASH: Chip Select low */
	SPI_FLASH_CS_LOW();
	/* Send Bulk Erase instruction  */
	SPI_FLASH_SendByte(W25X_ChipErase);
	/* Deselect the FLASH: Chip Select high */
	SPI_FLASH_CS_HIGH();

	/* Wait the end of Flash writing */
	SPI_FLASH_WaitForWriteEnd();
}

/***************************************************************************************************
*\Function      SPI_Flash_Erase
*\Description     擦除多个扇区 (文件系统接口函数)
*\Parameter     Addr擦除起始地址  
*\Parameter     Num要擦除扇区个数
*\Return        s32_t
*\Note          
*\Log           2019.03.26    Ver 1.0    Jerry
*               创建函数。
***************************************************************************************************/
s32_t SPI_Flash_Erase(u32_t Addr,u32_t Num)   
{   
	  
	u32_t secpos;
	u32_t Address;
	u32_t i;
	secpos = Addr/4096; //扇区地址 (计算给定的地址处于第几个扇区)

	for(i=0;i<Num;i++)
	{
		Address = secpos * 4096;        //根据扇区计算扇区的起始地址
		SPI_FLASH_WriteEnable();                  //SET WEL 	 
		SpiFlashCheckBusy();  
		SPI_FLASH_CS_LOW();                        //使能器件   
		SPI_FLASH_SendByte(W25X_SectorErase);      //发送扇区擦除指令 
		SPI_FLASH_SendByte((u8_t)((Address)>>16));  //发送24bit地址    
		SPI_FLASH_SendByte((u8_t)((Address)>>8));   
		SPI_FLASH_SendByte((u8_t)Address);  
		SPI_FLASH_CS_HIGH();                        //取消片选     	      
		SpiFlashCheckBusy();   				            //等待擦除完成
		secpos++;	
	}
	  
	return 0;
} 


/***************************************************************************************************
*\Function      SPI_Flash_Read
*\Description    在指定的起始地址处读取指定长度的数据 (文件系统接口函数)
*\Parameter     ReadAddr开始读取的地址(24bit)
*\Parameter     NumByteToRead要读取的字节数(最大65535)
*\Parameter     pBuffer:数据存储区  
*\Return        s32_t
*\Note          
*\Log           2019.03.26    Ver 1.0    Jerry
*               创建函数。
***************************************************************************************************/
s32_t SPI_Flash_Read(u32_t ReadAddr,u32_t NumByteToRead,u8_t * pBuffer)   
{ 
	SPI_FLASH_BufferRead(pBuffer,ReadAddr,NumByteToRead);
	return 0;		
} 

/***************************************************************************************************
*\Function      SPI_Flash_Write
*\Description   在指定的起始地址处开始写入最大256字节的数据 (文件系统接口函数)
*\Parameter     WriteAddr:开始写入的地址(24bit)
*\Parameter     NumByteToWrite:要写入的字节数(最大256),该数不应该超过该页的剩余字节数!!!	 
*\Parameter     pBuffer:数据存储区 
*\Return        s32_t
*\Note          
*\Log           2019.03.26    Ver 1.0    Jerry
*               创建函数。
***************************************************************************************************/
s32_t SPI_Flash_Write(u32_t WriteAddr,u32_t NumByteToWrite,u8_t * pBuffer)
{
	SPI_FLASH_BufferWrite(pBuffer, WriteAddr, NumByteToWrite);
	return 0;
}

/***************************************************************************************************
*\Function      SPI_FLASH_PageWrite
*\Description   Writes more than one byte to the FLASH with a single WRITE
                 cycle(Page WRITE sequence). The number of byte can't exceed
                  the FLASH page size.
*\Parameter     pBuffer :pointer to the buffer  containing the data to be
                    written to the FLASH.
*\Parameter     WriteAddr :FLASH's internal address to write to.
*\Parameter     NumByteToWrite :number of bytes to write to the FLASH,
                    must be equal or less than "SPI_FLASH_PageSize" value.
*\Return        void :
*\Note          
*\Log           2019.03.26    Ver 1.0    Jerry
*               创建函数。
***************************************************************************************************/
void SPI_FLASH_PageWrite(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite)
{
  /* Enable the write access to the FLASH */
  SPI_FLASH_WriteEnable();

  /* Select the FLASH: Chip Select low */
  SPI_FLASH_CS_LOW();
  /* Send "Write to Memory " instruction */
  SPI_FLASH_SendByte(W25X_PageProgram);
  /* Send WriteAddr high nibble address byte to write to */
  SPI_FLASH_SendByte((WriteAddr & 0xFF0000) >> 16);
  /* Send WriteAddr medium nibble address byte to write to */
  SPI_FLASH_SendByte((WriteAddr & 0xFF00) >> 8);
  /* Send WriteAddr low nibble address byte to write to */
  SPI_FLASH_SendByte(WriteAddr & 0xFF);

  if(NumByteToWrite > SPI_FLASH_PerWritePageSize)
  {
     NumByteToWrite = SPI_FLASH_PerWritePageSize;
     //printf("\n\r Err: SPI_FLASH_PageWrite too large!");
  }

  /* while there is data to be written on the FLASH */
  while (NumByteToWrite--)
  {
    /* Send the current byte */
    SPI_FLASH_SendByte(*pBuffer);
    /* Point on the next byte to be written */
    pBuffer++;
  }

  /* Deselect the FLASH: Chip Select high */
  SPI_FLASH_CS_HIGH();

  /* Wait the end of Flash writing */
  SPI_FLASH_WaitForWriteEnd();
}


/***************************************************************************************************
*\Function      SPI_FLASH_BufferWrite
*\Description   Writes block of data to the FLASH. In this function, the
*                  number of WRITE cycles are reduced, using Page WRITE sequence.
*\Parameter     pBuffer :pointer to the buffer  containing the data to be
				written to the FLASH.
*\Parameter     WriteAddr :FLASH's internal address to write to.
*\Parameter     NumByteToWrite :number of bytes to write to the FLASH.
*\Return        void :
*\Note          
*\Log           2019.03.26    Ver 1.0    Jerry
*               创建函数。
***************************************************************************************************/
void SPI_FLASH_BufferWrite(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite)
{
  u8 NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0, temp = 0;

  Addr = WriteAddr % SPI_FLASH_PageSize;
  count = SPI_FLASH_PageSize - Addr;
  NumOfPage =  NumByteToWrite / SPI_FLASH_PageSize;
  NumOfSingle = NumByteToWrite % SPI_FLASH_PageSize;

  if (Addr == 0) /* WriteAddr is SPI_FLASH_PageSize aligned  */
  {
    if (NumOfPage == 0) /* NumByteToWrite < SPI_FLASH_PageSize */
    {
      SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumByteToWrite);
    }
    else /* NumByteToWrite > SPI_FLASH_PageSize */
    {
      while (NumOfPage--)
      {
        SPI_FLASH_PageWrite(pBuffer, WriteAddr, SPI_FLASH_PageSize);
        WriteAddr +=  SPI_FLASH_PageSize;
        pBuffer += SPI_FLASH_PageSize;
      }

      SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumOfSingle);
    }
  }
  else /* WriteAddr is not SPI_FLASH_PageSize aligned  */
  {
    if (NumOfPage == 0) /* NumByteToWrite < SPI_FLASH_PageSize */
    {
      if (NumOfSingle > count) /* (NumByteToWrite + WriteAddr) > SPI_FLASH_PageSize */
      {
        temp = NumOfSingle - count;

        SPI_FLASH_PageWrite(pBuffer, WriteAddr, count);
        WriteAddr +=  count;
        pBuffer += count;

        SPI_FLASH_PageWrite(pBuffer, WriteAddr, temp);
      }
      else
      {
        SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumByteToWrite);
      }
    }
    else /* NumByteToWrite > SPI_FLASH_PageSize */
    {
      NumByteToWrite -= count;
      NumOfPage =  NumByteToWrite / SPI_FLASH_PageSize;
      NumOfSingle = NumByteToWrite % SPI_FLASH_PageSize;

      SPI_FLASH_PageWrite(pBuffer, WriteAddr, count);
      WriteAddr +=  count;
      pBuffer += count;

      while (NumOfPage--)
      {
        SPI_FLASH_PageWrite(pBuffer, WriteAddr, SPI_FLASH_PageSize);
        WriteAddr +=  SPI_FLASH_PageSize;
        pBuffer += SPI_FLASH_PageSize;
      }

      if (NumOfSingle != 0)
      {
        SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumOfSingle);
      }
    }
  }
}

/***************************************************************************************************
*\Function      SPI_FLASH_BufferRead
*\Description   Reads a block of data from the FLASH.
*\Parameter     pBuffer : pointer to the buffer that receives the data read
*                    from the FLASH.
*\Parameter     ReadAddr :FLASH's internal address to read from.
*\Parameter     NumByteToRead :number of bytes to read from the FLASH.
*\Return        void :
*\Note          
*\Log           2019.03.26    Ver 1.0    Jerry
*               创建函数。
***************************************************************************************************/
void SPI_FLASH_BufferRead(u8* pBuffer, u32 ReadAddr, u16 NumByteToRead)
{
  /* Select the FLASH: Chip Select low */
  SPI_FLASH_CS_LOW();

  /* Send "Read from Memory " instruction */
  SPI_FLASH_SendByte(W25X_ReadData);

  /* Send ReadAddr high nibble address byte to read from */
  SPI_FLASH_SendByte((ReadAddr & 0xFF0000) >> 16);
  /* Send ReadAddr medium nibble address byte to read from */
  SPI_FLASH_SendByte((ReadAddr& 0xFF00) >> 8);
  /* Send ReadAddr low nibble address byte to read from */
  SPI_FLASH_SendByte(ReadAddr & 0xFF);

  while (NumByteToRead--) /* while there is data to be read */
  {
    /* Read a byte from the FLASH */
    *pBuffer = SPI_FLASH_SendByte(Dummy_Byte);
    /* Point to the next location where the byte read will be saved */
    pBuffer++;
  }

  /* Deselect the FLASH: Chip Select high */
  SPI_FLASH_CS_HIGH();
}

/***************************************************************************************************
*\Function      SPI_FLASH_ReadID
*\Description   Reads FLASH identification.
*\Parameter      :
*\Return        uint32_t :
*\Note          
*\Log           2019.03.26    Ver 1.0    Jerry
*               创建函数。
***************************************************************************************************/
uint32_t SPI_FLASH_ReadID(void)
{
  u32 Temp = 0, Temp0 = 0, Temp1 = 0, Temp2 = 0;

  /* Select the FLASH: Chip Select low */
  SPI_FLASH_CS_LOW();

  /* Send "RDID " instruction */
  SPI_FLASH_SendByte(W25X_JedecDeviceID);

  /* Read a byte from the FLASH */
  Temp0 = SPI_FLASH_SendByte(Dummy_Byte);

  /* Read a byte from the FLASH */
  Temp1 = SPI_FLASH_SendByte(Dummy_Byte);

  /* Read a byte from the FLASH */
  Temp2 = SPI_FLASH_SendByte(Dummy_Byte);

  /* Deselect the FLASH: Chip Select high */
  SPI_FLASH_CS_HIGH();

  Temp = (Temp0 << 16) | (Temp1 << 8) | Temp2;

  return Temp;
}

/***************************************************************************************************
*\Function      SPI_FLASH_ReadDeviceID
*\Description   Reads FLASH identification.
*\Parameter      :
*\Return        uint32_t :
*\Note          
*\Log           2019.03.26    Ver 1.0    Jerry
*               创建函数。
***************************************************************************************************/
uint32_t SPI_FLASH_ReadDeviceID(void)
{
  u32 Temp = 0;

  /* Select the FLASH: Chip Select low */
  SPI_FLASH_CS_LOW();

  /* Send "RDID " instruction */
  SPI_FLASH_SendByte(W25X_DeviceID);
  SPI_FLASH_SendByte(Dummy_Byte);
  SPI_FLASH_SendByte(Dummy_Byte);
  SPI_FLASH_SendByte(Dummy_Byte);
  
  /* Read a byte from the FLASH */
  Temp = SPI_FLASH_SendByte(Dummy_Byte);

  /* Deselect the FLASH: Chip Select high */
  SPI_FLASH_CS_HIGH();

  return Temp;
}

/***************************************************************************************************
*\Function      SPI_FLASH_StartReadSequence
*\Description   Initiates a read data byte (READ) sequence from the Flash.
*                  This is done by driving the /CS line low to select the device,
*                  then the READ instruction is transmitted followed by 3 bytes
*                  address. This function exit and keep the /CS line low, so the
*                  Flash still being selected. With this technique the whole
*                  content of the Flash is read with a single READ instruction.
*\Parameter     ReadAddr :  FLASH's internal address to read from.
*\Return        void : 
*\Note           
*\Log           2019.03.26    Ver 1.0    Jerry
*               创建函数。
***************************************************************************************************/
void SPI_FLASH_StartReadSequence(u32 ReadAddr)
{
  /* Select the FLASH: Chip Select low */
  SPI_FLASH_CS_LOW();

  /* Send "Read from Memory " instruction */
  SPI_FLASH_SendByte(W25X_ReadData);

  /* Send the 24-bit address of the address to read from -----------------------*/
  /* Send ReadAddr high nibble address byte */
  SPI_FLASH_SendByte((ReadAddr & 0xFF0000) >> 16);
  /* Send ReadAddr medium nibble address byte */
  SPI_FLASH_SendByte((ReadAddr& 0xFF00) >> 8);
  /* Send ReadAddr low nibble address byte */
  SPI_FLASH_SendByte(ReadAddr & 0xFF);
}

/***************************************************************************************************
*\Function      SPI_FLASH_ReadByte
*\Description    Reads a byte from the SPI Flash.
*                  This function must be used only if the Start_Read_Sequence
*                  function has been previously called.
*\Parameter      : 
*\Return        uint8_t : Byte Read from the SPI Flash.
*\Note           
*\Log           2019.03.26    Ver 1.0    Jerry
*               创建函数。
***************************************************************************************************/
uint8_t SPI_FLASH_ReadByte(void)
{
  return (SPI_FLASH_SendByte(Dummy_Byte));
}


/***************************************************************************************************
*\Function      SPI_FLASH_SendByte
*\Description   Sends a byte through the SPI interface and return the byte
*                  received from the SPI bus.
*\Parameter     byte :  byte to send.
*\Return        uint8_t : The value of the received byte.
*\Note          
*\Log           2019.03.26    Ver 1.0    Jerry
*               创建函数。
***************************************************************************************************/
uint8_t SPI_FLASH_SendByte(u8 byte)
{
  /* Loop while DR register in not emplty */
  while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);

  /* Send byte through the SPI2 peripheral */
  SPI_I2S_SendData(SPI2, byte);

  /* Wait to receive a byte */
  while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);

  /* Return the byte read from the SPI bus */
  return SPI_I2S_ReceiveData(SPI2);
}


/***************************************************************************************************
*\Function      SPI_FLASH_SendHalfWord
*\Description   Sends a Half Word through the SPI interface and return the
*                  Half Word received from the SPI bus.
*\Parameter     HalfWord :  Half Word to send.
*\Return        uint16_t : The value of the received Half Word.
*\Note          
*\Log           2019.03.26    Ver 1.0    Jerry
*               创建函数。
***************************************************************************************************/
uint16_t SPI_FLASH_SendHalfWord(u16 HalfWord)
{
  /* Loop while DR register in not emplty */
  while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);

  /* Send Half Word through the SPI2 peripheral */
  SPI_I2S_SendData(SPI2, HalfWord);

  /* Wait to receive a Half Word */
  while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);

  /* Return the Half Word read from the SPI bus */
  return SPI_I2S_ReceiveData(SPI2);
}


/***************************************************************************************************
*\Function      SPI_FLASH_WriteEnable
*\Description    Enables the write access to the FLASH.
*\Parameter      : 
*\Return        void : 
*\Note          
*\Log           2019.03.26    Ver 1.0    Jerry
*               创建函数。
***************************************************************************************************/
void SPI_FLASH_WriteEnable(void)
{
  /* Select the FLASH: Chip Select low */
  SPI_FLASH_CS_LOW();

  /* Send "Write Enable" instruction */
  SPI_FLASH_SendByte(W25X_WriteEnable);

  /* Deselect the FLASH: Chip Select high */
  SPI_FLASH_CS_HIGH();
}


/***************************************************************************************************
*\Function      SPI_FLASH_WaitForWriteEnd
*\Description   Polls the status of the Write In Progress (WIP) flag in the
*                  FLASH's status  register  and  loop  until write  opertaion
*                  has completed.
*\Parameter      : 
*\Return        void : 
*\Note          
*\Log           2019.03.26    Ver 1.0    Jerry
*               创建函数。
***************************************************************************************************/
void SPI_FLASH_WaitForWriteEnd(void)
{
  u8 FLASH_Status = 0;

  /* Select the FLASH: Chip Select low */
  SPI_FLASH_CS_LOW();

  /* Send "Read Status Register" instruction */
  SPI_FLASH_SendByte(W25X_ReadStatusReg);

  /* Loop as long as the memory is busy with a write cycle */
  do
  {
    /* Send a dummy byte to generate the clock needed by the FLASH
    and put the value of the status register in FLASH_Status variable */
    FLASH_Status = SPI_FLASH_SendByte(Dummy_Byte);	 
  }
  while ((FLASH_Status & WIP_Flag) == SET); /* Write in progress */

  /* Deselect the FLASH: Chip Select high */
  SPI_FLASH_CS_HIGH();
}


/***************************************************************************************************
*\Function      SPI_Flash_PowerDown
*\Description   进入掉电模式
*\Parameter       
*\Return        void  
*\Note          
*\Log           2019.03.26    Ver 1.0    Jerry
*               创建函数。
***************************************************************************************************/
void SPI_Flash_PowerDown(void)   
{ 
  /* Select the FLASH: Chip Select low */
  SPI_FLASH_CS_LOW();

  /* Send "Power Down" instruction */
  SPI_FLASH_SendByte(W25X_PowerDown);

  /* Deselect the FLASH: Chip Select high */
  SPI_FLASH_CS_HIGH();
}   


/***************************************************************************************************
*\Function      SPI_Flash_WAKEUP
*\Description   唤醒
*\Parameter       
*\Return        void  
*\Note          
*\Log           2019.03.26    Ver 1.0    Jerry
*               创建函数。
***************************************************************************************************/
void SPI_Flash_WAKEUP(void)   
{
  /* Select the FLASH: Chip Select low */
  SPI_FLASH_CS_LOW();

  /* Send "Power Down" instruction */
  SPI_FLASH_SendByte(W25X_ReleasePowerDown);

  /* Deselect the FLASH: Chip Select high */
  SPI_FLASH_CS_HIGH();                   //等待TRES1
}   

/***************************************************************************************************
*\Function      SpiFlashCheckBusy
*\Description   SpiFLash忙检测
*\Parameter       
*\Return        uint8_t  0 表示不忙 1表示忙
*\Note          
*\Log           2019.03.26    Ver 1.0    Jerry
*               创建函数。
***************************************************************************************************/
uint8_t  SpiFlashCheckBusy(void)
{
	uint32_t Delay=0;
	Delay=0x4fff;
	while(Delay)
	{
    SPI_FLASH_CS_LOW();
    SPI_FLASH_SendByte(W25X_ReadStatusReg);
		if((SPI_FLASH_SendByte(0xa5)&0x01)==0) break;
    SPI_FLASH_CS_HIGH();
		Delay--;
	}
	SPI_FLASH_CS_HIGH();
	if(Delay)  return 0;
	else       return 1;
}   



/***************************************************************************************************
******************************************** END OF FILE *******************************************
***************************************************************************************************/
