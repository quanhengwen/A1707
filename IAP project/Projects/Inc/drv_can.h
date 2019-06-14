/***************************************************************************************************
*                    (c) Copyright 2008-2017  Syncretize technologies co.,ltd.
*										All Rights Reserved
*
*\File          drv_can.h
*\Description   
*\Note          
*\Log           2017.10.17    Ver 1.0    Job
*               �����ļ���
***************************************************************************************************/
#ifndef _DRV_CAN_H
#define _DRV_CAN_H
#include "common_lib.h"
#include "stm32f1xx_hal.h"





//������
typedef enum CAN_BR_type_st
{
    CAN_BAUDRATE_100K,
    CAN_BAUDRATE_200K,
    CAN_BAUDRATE_500K,
    CAN_BAUDRATE_1M,
}CAN_BR_type;

#define CAN_RX_BUFFER_SIZE 20
#define CAN_TX_TRY_TIMES 2

typedef struct CANBusRx_type_st
{
    CAN_RxHeaderTypeDef CANRxHead;
	uint8_t Data[8];
}CANBusRx_type;

typedef struct CANBusTx_type_st
{
    CAN_TxHeaderTypeDef CANTxHead;
	uint8_t Data[8];
}CANBusTx_type;

typedef struct CANBus_type_st
{
    CAN_TypeDef* can;
    GpioType*   TxGpio; //can���ͽ�
    GpioType*   RxGpio; //can���ս�
    CAN_BR_type baudrate;
    //������Ҫ����
//    CAN_RxHeaderTypeDef CANRxBuf[CAN_RX_BUFFER_SIZE];
	CANBusRx_type CANRxBuf[CAN_RX_BUFFER_SIZE];
    u32 CanRx_Read;
    u32 CanRx_Write;
}CANBus_type;

void stm32_CAN_init(CANBus_type* can_dev);
bool stm32_CAN_recv(CANBus_type* can_dev, CANBusRx_type* rx);
bool stm32_CAN_send(CANBus_type* can_dev, CANBusTx_type* tx);
bool stm32_CAN_clear(CANBus_type* can_dev);

void stm32_CAN_recv_isr(CANBus_type* can_dev, u8 can_fifo);
#endif /*_DRV_CAN_H*/
