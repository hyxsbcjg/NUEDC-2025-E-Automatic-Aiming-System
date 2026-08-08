#include "UART.h"
#include "track.h"
#include "peripheral.h"
#include "question.h"
#include "control.h"
#include <stdarg.h>
#include <stdio.h>

char Tx_User_Buf[128];
char Rx_User_Buf[128];
uint8_t ucUART_User_Pos = 0;
uint8_t UART_User_Flag = 0;
uint64_t UART_User_Time = 0;

char Tx_K230_Buf[4] = {0x50,0,0,0x51};
char Rx_K230_Buf[30];
uint8_t ucUART_K230_Pos = 0;
uint8_t UART_K230_Flag = 0;
uint64_t UART_K230_Time = 0;

char Tx_X_Buf[128];
uint8_t ucUART_X_Pos = 0;
uint8_t UART_X_Flag = 0;

char Tx_Y_Buf[128];
uint8_t ucUART_Y_Pos = 0;
uint8_t UART_Y_Flag = 0;

//Cmd 摄像头对单片机的命令
uint8_t Cmd;




//Founction
void UART_User_Proc(void);
void UART_K230_Proc(void);


//========================Debug部分串口=================
void UART2_Send_Bytes(char *buf, int len)
{
  while(len--)
  {
    DL_UART_Main_transmitDataBlocking(UART2_User_INST, *buf);
    buf++;
  }
}


void UART2_SendString(char *str)
{
    while (*str != '\0')  
    {
        DL_UART_Main_transmitDataBlocking(UART2_User_INST, *str++);
    }
}


// 自制的my_printf函数
int my_printf(const char *format, ...)
{
    va_list args;
    int ret;
    
    // 初始化可变参数列表
    va_start(args, format);
    
    // 使用vsprintf将格式化字符串写入缓冲区
    ret = vsprintf(Tx_User_Buf, format, args);
    
    // 结束可变参数列表
    va_end(args);
    
    // 检查是否超出缓冲区大小
    if (ret > 0 && ret < 128)
    {
        // 通过UART2发送格式化后的字符串
        UART2_SendString(Tx_User_Buf);
    }
    
    return ret;
}



uint8_t R[4] = {0x50,0x11,0x62,0x51};

void UART2_User_INST_IRQHandler(void) 
{
	switch(DL_UART_getPendingInterrupt(UART2_User_INST) )
    {
		case DL_UART_IIDX_RX:
			Rx_User_Buf[ucUART_User_Pos++] = DL_UART_Main_receiveData(UART2_User_INST);
			UART_User_Flag = 1;

		break;
		default:
			
		break;
		
		
	}
}











//========================K230部分串口=================

//发送数据包(0x50,data1,data2,0x51)
void UART1_Send_Bytes(char *buf, int len)
{
	  while(len--)
	  {
		DL_UART_Main_transmitDataBlocking(UART1_K230_INST, *buf);
		buf++;
	  }
}


void UART1_SendString(char *str)
{
    while (*str != '\0')  
    {
        DL_UART_Main_transmitDataBlocking(UART1_K230_INST, *str++);
    }
}

void UART1_K230_INST_IRQHandler(void) 
{
	switch(DL_UART_getPendingInterrupt(UART1_K230_INST) )
    {
		case DL_UART_IIDX_RX:
			Rx_K230_Buf[ucUART_K230_Pos++] = DL_UART_Main_receiveData(UART1_K230_INST);

			UART_SendByte(UART2,0x11);
			UART_K230_Flag = 1;
			
		break;
		default:
			
		break;
		
		
	}
}


//循迹数据定义(0xAA,L_cxh,L_cxl,M_cxh,M_cxy,flags,0x55)
/* ----------- 全局量（示例，确保已定义） ----------- */



/* ---------------- UART_K230_Proc ----------------- */
void UART_K230_Proc(void)
{
    /* 状态机：0=WAIT_HEAD  1=PAYLOAD  2=WAIT_TAIL */
    static uint8_t state   = 0;
    static uint8_t payload[5];   /* L_H L_L R_H R_L FLAGS */
    static uint8_t idx     = 0;

    if (!UART_K230_Flag) return;         /* 没有新数据 */
    UART_K230_Flag = 0;                  /* 清标志 */

    for (uint8_t i = 0; i < ucUART_K230_Pos; ++i)
    {
        uint8_t ch = Rx_K230_Buf[i];

        switch (state)
        {
        /* ---------- 0. 等待帧头 AA ---------- */
        case 0:
            if (ch == 0xAA)
            {
                idx   = 0;               /* 重置计数 */
                state = 1;               /* 转到接收 Payload */
            }
            break;

        /* ---------- 1. 收集 Payload 5 字节 ---------- */
        case 1:
            if (idx < sizeof(payload))
                payload[idx++] = ch;

            if (idx >= sizeof(payload))
                state = 2;               /* 准备读取尾字节 */
            break;

        /* ---------- 2. 等待帧尾 55 并解析 ---------- */
        case 2:
            if (ch == 0x55)
            {
                /* -------------- 解析 -------------- */


              
            }
            /* 无论尾字节是否正确，都回到等待头状态 */
            state = 0;
            break;

        default:
            state = 0;
            break;
        }
    }

    ucUART_K230_Pos = 0;   /* 清空接收计数，准备下一批数据 */
}
























/**
 * @brief    MSPM0 串口发送单字节函数
 * @param    uart: UART实例 (如 UART0, UART1, UART2 等)
 * @param    byte: 要发送的字节数据
 * @retval   none
 */
void UART_SendByte(UART_Regs *uart, uint8_t byte)
{
    // 使用阻塞方式发送单个字节
    DL_UART_Main_transmitDataBlocking(uart, byte);
}

/**
 * @brief    MSPM0 串口发送数组函数
 * @param    uart: UART实例 (如 UART0, UART1, UART2 等)
 * @param    array: 要发送的数组指针
 * @param    length: 数组长度
 * @retval   none
 */
void UART_SendArray(UART_Regs *uart, uint8_t *array, uint16_t length)
{
    uint16_t i;
    for (i = 0; i < length; i++)
    {
        UART_SendByte(uart, array[i]);
    }
}
