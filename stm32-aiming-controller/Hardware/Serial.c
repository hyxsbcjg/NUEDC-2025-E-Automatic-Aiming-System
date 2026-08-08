#include "stm32f10x.h"                  // Device header
#include <stdio.h>
#include <stdarg.h>
#include "serial.h"
#include "myzdt.h"
#include "string.h"
#include "question.h"

uint8_t USART1_Pos;
uint8_t USART1_Flag;	
uint8_t USART1_Tx[4] = {0x20,0,0,0x30};		
uint8_t USART1_Rx[128];			

uint8_t USART2_Pos;
uint8_t USART2_Flag;
char USART2_Tx[128];		
char USART2_Rx[128];		

uint8_t USART3_Pos;
uint8_t USART3_Flag;
char USART3_Tx[128];		
char USART3_Rx[128];


uint8_t UART4_Pos;
uint8_t UART4_Flag;
uint8_t UART4_Rx[128];


/**
  * 函    数：串口初始化
  * 参    数：无
  * 返 回 值：无
  */
void USART1_Init(void)
{
	/*开启时钟*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);	//开启USART1的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	//开启GPIOA的时钟
	
	/*GPIO初始化*/
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);					//将PA9引脚初始化为复用推挽输出
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);					//将PA10引脚初始化为上拉输入
	
	/*USART初始化*/
	USART_InitTypeDef USART_InitStructure;					//定义结构体变量
	USART_InitStructure.USART_BaudRate = 115200;				//波特率
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//硬件流控制，不需要
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;	//模式，发送模式和接收模式均选择
	USART_InitStructure.USART_Parity = USART_Parity_No;		//奇偶校验，不需要
	USART_InitStructure.USART_StopBits = USART_StopBits_1;	//停止位，选择1位
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;		//字长，选择8位
	USART_Init(USART1, &USART_InitStructure);				//将结构体变量交给USART_Init，配置USART1
	
	/*中断输出配置*/
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);			//开启串口接收数据的中断
	
	/*NVIC中断分组*/
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);			//配置NVIC为分组2
	
	/*NVIC配置*/
	NVIC_InitTypeDef NVIC_InitStructure;					//定义结构体变量
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;		//选择配置NVIC的USART1线
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//指定NVIC线路使能
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;		//指定NVIC线路的抢占优先级为1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		//指定NVIC线路的响应优先级为1
	NVIC_Init(&NVIC_InitStructure);							//将结构体变量交给NVIC_Init，配置NVIC外设
	
	/*USART使能*/
	USART_Cmd(USART1, ENABLE);								//使能USART1，串口开始运行
}

/**
  * 函    数：串口发送一个字节
  * 参    数：Byte 要发送的一个字节
  * 返 回 值：无
  */
void USART1_SendByte(uint8_t Byte)
{
	USART_SendData(USART1, Byte);		//将字节数据写入数据寄存器，写入后USART自动生成时序波形
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);	//等待发送完成
	/*下次写入数据寄存器会自动清除发送完成标志位，故此循环后，无需清除标志位*/
}

/**
  * 函    数：串口发送一个数组
  * 参    数：Array 要发送数组的首地址
  * 参    数：Length 要发送数组的长度
  * 返 回 值：无
  */
void USART1_SendArray(uint8_t *Array, uint16_t Length)
{
	uint16_t i;
	for (i = 0; i < Length; i ++)		//遍历数组
	{
		USART1_SendByte(Array[i]);		//依次调用Serial_SendByte发送每个字节数据
	}
}

/**
  * 函    数：串口发送一个字符串
  * 参    数：String 要发送字符串的首地址
  * 返 回 值：无
  */
void USART1_SendString(char *String)
{
	uint8_t i;
	for (i = 0; String[i] != '\0'; i ++)//遍历字符数组（字符串），遇到字符串结束标志位后停止
	{
		USART1_SendByte(String[i]);		//依次调用Serial_SendByte发送每个字节数据
	}
}


int fputc(int ch, FILE *f)
{
    USART1_SendByte((uint8_t)ch);  // 把字符输出到串口1
    return ch;
}




/**
  * 函    数：USART1中断函数
  * 参    数：无
  * 返 回 值：无
  * 注意事项：此函数为中断函数，无需调用，中断触发后自动执行
  *           函数名为预留的指定名称，可以从启动文件复制
  *           请确保函数名正确，不能有任何差异，否则中断函数将不能进入
  */
void USART1_IRQHandler(void)
{
	if (USART_GetITStatus(USART1, USART_IT_RXNE) == SET)		//判断是否是USART1的接收事件触发的中断
	{
		if(USART1_Pos < 127) {  // 添加边界检查
            USART1_Rx[USART1_Pos++] = USART_ReceiveData(USART1);
        } else {
            USART1_Pos = 0;  // 重置防止溢出
            (void)USART_ReceiveData(USART1);  // 读取数据但丢弃
        }
		USART1_Flag = 1;		//置接收标志位变量为1
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);			//清除USART1的RXNE标志位
																//读取数据寄存器会自动清除此标志位
																//如果已经读取了数据寄存器，也可以不执行此代码
	}
}


// 发送函数
void K230_Send_Bytes(uint8_t d1, uint8_t d2)
{
    USART1_SendByte(0x20);  // 帧头
    USART1_SendByte(d1);
    USART1_SendByte(d2);
    USART1_SendByte(0x30);  // 帧尾
}

void USART1_K230_Proc(void)
{
    static uint8_t state = 0;
    static uint8_t buf[6];
    static uint8_t idx = 0;
    
    if (!USART1_Flag) return;
    USART1_Flag = 0;
    
    for (uint8_t i = 0; i < USART1_Pos; ++i)
    {
        uint8_t ch = USART1_Rx[i];
        
        switch (state)
        {
        case 0: // 等待帧头
            if (ch == 0x40 || ch == 0x60)  // 坐标帧头
            {
                buf[0] = ch;
                idx = 1;
                state = 1;
            }
            else if (ch == 0x20)  // 命令帧头（如果还需要）
            {
                buf[0] = ch;
                idx = 1;
                state = 2;
            }
            break;
            
        case 1: // 接收坐标数据
            buf[idx++] = ch;
            
            if (idx >= 6)  // 接收完成
            {
                // 检查帧尾
                if (buf[0] == 0x40 && buf[5] == 0x50)  // 矩形坐标帧
                {
                    // 检查是否是特殊标记
                    if (buf[1] == 0xAA && buf[2] == 0xAA && 
                        buf[3] == 0xAA && buf[4] == 0xAA)
                    {
                        // 未检测到矩形
                        Find_Rect_Flag = 0;
                    }
                    else
                    {
                        // 检测到矩形
                        Find_Rect_Flag = 1;
                        Blob1_cx = (buf[1] << 8) | buf[2];
                        Blob1_cy = (buf[3] << 8) | buf[4];
                    }
                }
                else if (buf[0] == 0x60 && buf[5] == 0x70)  // 紫色坐标帧
                {
                    Blob2_cx = (buf[1] << 8) | buf[2];
                    Blob2_cy = (buf[3] << 8) | buf[4];
                }
                
                state = 0;
                idx = 0;
            }
            break;
            
        case 2: // 处理命令帧（如果还需要）
            buf[idx++] = ch;
            
            if (idx >= 4)
            {
                if (buf[0] == 0x20 && buf[3] == 0x30)
                {
                    // 处理命令
                }
                state = 0;
                idx = 0;
            }
            break;
            
        default:
            state = 0;
            idx = 0;
            break;
        }
    }
    
    USART1_Pos = 0;
}




/**
  * 函    数：串口初始化
  * 参    数：无
  * 返 回 值：无
  */
void USART2_Init(void)
{
	/*开启时钟*/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);	//开启USART2的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	//开启GPIOA的时钟
	
	/*GPIO初始化*/
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);					//将PA2引脚初始化为复用推挽输出
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);					//将PA3引脚初始化为上拉输入
	
	/*USART初始化*/
	USART_InitTypeDef USART_InitStructure;					//定义结构体变量
	USART_InitStructure.USART_BaudRate = 38400;				//波特率
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//硬件流控制，不需要
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;	//模式，发送模式和接收模式均选择
	USART_InitStructure.USART_Parity = USART_Parity_No;		//奇偶校验，不需要
	USART_InitStructure.USART_StopBits = USART_StopBits_1;	//停止位，选择1位
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;		//字长，选择8位
	USART_Init(USART2, &USART_InitStructure);				//将结构体变量交给USART_Init，配置USART1
	
	/*中断输出配置*/
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);			//开启串口接收数据的中断
	
	/*NVIC中断分组*/
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);			//配置NVIC为分组2
	
	/*NVIC配置*/
	NVIC_InitTypeDef NVIC_InitStructure;					//定义结构体变量
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;		//选择配置NVIC的USART1线
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//指定NVIC线路使能
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;		//指定NVIC线路的抢占优先级为1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		//指定NVIC线路的响应优先级为1
	NVIC_Init(&NVIC_InitStructure);							//将结构体变量交给NVIC_Init，配置NVIC外设
	
	/*USART使能*/
	USART_Cmd(USART2, ENABLE);								//使能USART1，串口开始运行
}

/**
  * 函    数：串口发送一个字节
  * 参    数：Byte 要发送的一个字节
  * 返 回 值：无
  */
void USART2_SendByte(uint8_t Byte)
{
	USART_SendData(USART2, Byte);		//将字节数据写入数据寄存器，写入后USART自动生成时序波形
	while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);	//等待发送完成
	/*下次写入数据寄存器会自动清除发送完成标志位，故此循环后，无需清除标志位*/
}

/**
  * 函    数：串口发送一个数组
  * 参    数：Array 要发送数组的首地址
  * 参    数：Length 要发送数组的长度
  * 返 回 值：无
  */
void USART2_SendArray(uint8_t *Array, uint16_t Length)
{
	uint16_t i;
	for (i = 0; i < Length; i ++)		//遍历数组
	{
		USART2_SendByte(Array[i]);		//依次调用Serial_SendByte发送每个字节数据
	}
}

/**
  * 函    数：串口发送一个字符串
  * 参    数：String 要发送字符串的首地址
  * 返 回 值：无
  */
void USART2_SendString(char *String)
{
	uint8_t i;
	for (i = 0; String[i] != '\0'; i ++)//遍历字符数组（字符串），遇到字符串结束标志位后停止
	{
		USART2_SendByte(String[i]);		//依次调用Serial_SendByte发送每个字节数据
	}
}
/**
  * 函    数：USART2中断函数
  * 参    数：无
  * 返 回 值：无
  * 注意事项：此函数为中断函数，无需调用，中断触发后自动执行
  *           函数名为预留的指定名称，可以从启动文件复制
  *           请确保函数名正确，不能有任何差异，否则中断函数将不能进入
  */
void USART2_IRQHandler(void)
{
	if (USART_GetITStatus(USART2, USART_IT_RXNE) == SET)		//判断是否是USART2的接收事件触发的中断
	{
		if(USART2_Pos < 127) {  // 添加边界检查
            USART2_Rx[USART2_Pos++] = USART_ReceiveData(USART2);
        } else {
            USART2_Pos = 0;
            (void)USART_ReceiveData(USART2);
        }
        USART2_Flag = 1;									//置接收标志位变量为1
		USART_ClearITPendingBit(USART2, USART_IT_RXNE);			//清除USART1的RXNE标志位
																//读取数据寄存器会自动清除此标志位
															//如果已经读取了数据寄存器，也可以不执行此代码
	}
}


/**
  * 函    数：串口初始化
  * 参    数：无
  * 返 回 值：无
  */
void USART3_Init(void)
{
	/*开启时钟*/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);	//开启USART2的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	//开启GPIOA的时钟
	
	/*GPIO初始化*/
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);					
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);					
	
	/*USART初始化*/
	USART_InitTypeDef USART_InitStructure;					//定义结构体变量
	USART_InitStructure.USART_BaudRate = 115200;				//波特率
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//硬件流控制，不需要
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;	//模式，发送模式和接收模式均选择
	USART_InitStructure.USART_Parity = USART_Parity_No;		//奇偶校验，不需要
	USART_InitStructure.USART_StopBits = USART_StopBits_1;	//停止位，选择1位
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;		//字长，选择8位
	USART_Init(USART3, &USART_InitStructure);				//将结构体变量交给USART_Init，配置USART1
	
//	/*中断输出配置*/
//	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);			//开启串口接收数据的中断
//	
//	/*NVIC中断分组*/
//	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);			//配置NVIC为分组2
//	
//	/*NVIC配置*/
//	NVIC_InitTypeDef NVIC_InitStructure;					//定义结构体变量
//	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;		//选择配置NVIC的USART3线
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//指定NVIC线路使能
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;		//指定NVIC线路的抢占优先级为1
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		//指定NVIC线路的响应优先级为1
//	NVIC_Init(&NVIC_InitStructure);							//将结构体变量交给NVIC_Init，配置NVIC外设
	
	/*USART使能*/
	USART_Cmd(USART3, ENABLE);								//使能USART1，串口开始运行
}

/**
  * 函    数：串口发送一个字节
  * 参    数：Byte 要发送的一个字节
  * 返 回 值：无
  */
void USART3_SendByte(uint8_t Byte)
{
	USART_SendData(USART3, Byte);		//将字节数据写入数据寄存器，写入后USART自动生成时序波形
	while (USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);	//等待发送完成
	/*下次写入数据寄存器会自动清除发送完成标志位，故此循环后，无需清除标志位*/
}

/**
  * 函    数：串口发送一个数组
  * 参    数：Array 要发送数组的首地址
  * 参    数：Length 要发送数组的长度
  * 返 回 值：无
  */
void USART3_SendArray(uint8_t *Array, uint16_t Length)
{
	uint16_t i;
	for (i = 0; i < Length; i ++)		//遍历数组
	{
		USART3_SendByte(Array[i]);		//依次调用Serial_SendByte发送每个字节数据
	}
}

/**
  * 函    数：串口发送一个字符串
  * 参    数：String 要发送字符串的首地址
  * 返 回 值：无
  */
void USART3_SendString(char *String)
{
	uint8_t i;
	for (i = 0; String[i] != '\0'; i ++)//遍历字符数组（字符串），遇到字符串结束标志位后停止
	{
		USART3_SendByte(String[i]);		//依次调用Serial_SendByte发送每个字节数据
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
    ret = vsprintf(USART3_Tx, format, args);
    
    // 结束可变参数列表
    va_end(args);
    
    // 检查是否超出缓冲区大小
    if (ret > 0 && ret < 128)
    {
        // 通过UART2发送格式化后的字符串
        USART3_SendString(USART3_Tx);
    }
    
    return ret;
}

///**
//  * 函    数：USART2中断函数
//  * 参    数：无
//  * 返 回 值：无
//  * 注意事项：此函数为中断函数，无需调用，中断触发后自动执行
//  *           函数名为预留的指定名称，可以从启动文件复制
//  *           请确保函数名正确，不能有任何差异，否则中断函数将不能进入
//  */
//void USART3_IRQHandler(void)
//{
//	if (USART_GetITStatus(USART3, USART_IT_RXNE) == SET)		//判断是否是USART1的接收事件触发的中断
//	{
//		USART3_Rx[USART3_Pos++] = USART_ReceiveData(USART3);				//读取数据寄存器，存放在接收的数据变量
//		USART3_Flag = 1;										//置接收标志位变量为1
//		USART_ClearITPendingBit(USART3, USART_IT_RXNE);			//清除USART1的RXNE标志位
//																//读取数据寄存器会自动清除此标志位
//															//如果已经读取了数据寄存器，也可以不执行此代码
//	}
//}




//串口通用函数————————————————————
void USART_SendByte(USART_TypeDef* USARTx, uint8_t Byte)
{
    USART_SendData(USARTx, Byte);                                           // 发送数据
    while(USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);             // 等待发送完成
}

/**
 * @brief    通用串口发送数组函数
 * @param    USARTx: 串口外设 (USART1, USART2, UART4, USART6等)
 * @param    Array: 要发送的数组指针
 * @param    Length: 数组长度
 * @retval   none
 */
void USART_SendArray(USART_TypeDef* USARTx, uint8_t *Array, uint16_t Length)
{
    uint16_t i;
    for (i = 0; i < Length; i++)        // 遍历数组
    {
        USART_SendByte(USARTx, Array[i]); // 依次发送每个字节数据
    }
}


/**
  * 函    数：串口初始化
  * 参    数：无
  * 返 回 值：无
  */

/**
 * @brief  UART4初始化函数
 * @param  无
 * @retval 无
 * @note   UART4引脚映射：PC10(TX), PC11(RX)
 */
void UART4_Init(void)
{
    /*开启时钟*/
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    
    /*GPIO初始化*/
    GPIO_InitTypeDef GPIO_InitStructure;
    
    // 配置PC10为TX引脚 (复用推挽输出)
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    
    // 配置PC11为RX引脚 (上拉输入)
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    
    /*UART初始化*/
    USART_InitTypeDef USART_InitStructure;
    USART_InitStructure.USART_BaudRate = 115200;  // 步进电机通常用38400
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_Init(UART4, &USART_InitStructure);
    
    /*中断输出配置*/
    USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);  // 开启接收中断
    
    /*NVIC配置*/
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;  // 比其他串口优先级低一点
    NVIC_Init(&NVIC_InitStructure);
    
    /*UART使能*/
    USART_Cmd(UART4, ENABLE);
}

/**
 * @brief  UART4发送一个字节
 * @param  Byte 要发送的字节
 * @retval 无
 */
void UART4_SendByte(uint8_t Byte)
{
    USART_SendData(UART4, Byte);
    while (USART_GetFlagStatus(UART4, USART_FLAG_TXE) == RESET);
}

/**
 * @brief  UART4发送数组
 * @param  Array 数组首地址
 * @param  Length 数组长度
 * @retval 无
 */
void UART4_SendArray(uint8_t *Array, uint16_t Length)
{
    uint16_t i;
    for (i = 0; i < Length; i++)
    {
        UART4_SendByte(Array[i]);
    }
}



/**
 * @brief  UART4中断处理函数
 * @param  无
 * @retval 无
 */
void UART4_IRQHandler(void)
{
    if (USART_GetITStatus(UART4, USART_IT_RXNE) == SET)
    {
        if(UART4_Pos < 127) {
            UART4_Rx[UART4_Pos++] = USART_ReceiveData(UART4);
        } else {
            UART4_Pos = 0;
            (void)USART_ReceiveData(UART4);
        }
        UART4_Flag = 1;
        USART_ClearITPendingBit(UART4, USART_IT_RXNE);
    }
}

/**
 * @brief  UART4步进电机数据处理
 * @param  无
 * @retval 无
 * @note   解析步进电机返回的脉冲数据
 */
void UART4_Emm_Proc(void)
{
    static uint8_t state = 0;
    static uint8_t buf[8];  // 存储完整数据帧
    static uint8_t idx = 0;
    
    if (!UART4_Flag) return;
    UART4_Flag = 0;
    
    for (uint8_t i = 0; i < UART4_Pos; ++i)
    {
        uint8_t ch = UART4_Rx[i];
        
        switch (state)
        {
        case 0: // 等待地址
            if (ch == 0x01)  // 假设电机地址为0x01
            {
                buf[0] = ch;
                idx = 1;
                state = 1;
            }
            break;
            
        case 1: // 等待功能码
            buf[idx++] = ch;
            if (ch == 0x32)  // 脉冲数读取响应
            {
                state = 2;
            }
            else
            {
                state = 0;  // 功能码不匹配，重新开始
                idx = 0;
            }
            break;
            
        case 2: // 接收数据（符号+4字节脉冲数+校验）
            buf[idx++] = ch;
            
            if (idx >= 8)  // 完整数据包：地址+功能码+符号+4字节数据+校验 = 8字节
            {
                // 验证校验字节
                if (buf[7] == 0x6B)
                {
                    // 解析脉冲数
                    int32_t pulse = ((uint32_t)buf[3] << 24) |
                                   ((uint32_t)buf[4] << 16) |
                                   ((uint32_t)buf[5] << 8)  |
                                   ((uint32_t)buf[6]);
                    
                    // 根据符号位判断正负
                    if (buf[2] == 0x01)  // 负数
                    {
                        pulse = -pulse;
                    }
                    
                    // 更新全局变量
                    Motor_Pulse_Count = pulse;
                }
                
                state = 0;
                idx = 0;
            }
            break;
            
        default:
            state = 0;
            idx = 0;
            break;
        }
    }
    
    UART4_Pos = 0;
}




/**
 * @brief  UART5初始化函数
 * @param  无
 * @retval 无
 * @note   UART5引脚映射：PC12(TX), PD2(RX)
 *         注意：UART5的TX和RX分别在不同的GPIO组
 */
void UART5_Init(void)
{
    /*开启时钟*/
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, ENABLE);    // 开启UART5的时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);    // 开启GPIOC的时钟 (PC12-TX)
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);    // 开启GPIOD的时钟 (PD2-RX)
    
    /*GPIO初始化*/
    GPIO_InitTypeDef GPIO_InitStructure;
    
    // 配置PC12为TX引脚 (复用推挽输出)
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;          // 复用推挽输出
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;               // PC12
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;        // 速度50MHz
    GPIO_Init(GPIOC, &GPIO_InitStructure);                   // 初始化GPIOC
    
    // 配置PD2为RX引脚 (上拉输入)
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;            // 上拉输入
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;                // PD2
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOD, &GPIO_InitStructure);                   // 初始化GPIOD
    
    /*UART初始化*/
    USART_InitTypeDef USART_InitStructure;                   // 定义结构体变量
    USART_InitStructure.USART_BaudRate = 115200;             // 波特率115200
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // 无硬件流控制
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx; // 收发模式
    USART_InitStructure.USART_Parity = USART_Parity_No;      // 无奇偶校验
    USART_InitStructure.USART_StopBits = USART_StopBits_1;   // 1位停止位
    USART_InitStructure.USART_WordLength = USART_WordLength_8b; // 8位数据长度
    USART_Init(UART5, &USART_InitStructure);                 // 配置UART5
    

    
    /*UART使能*/
    USART_Cmd(UART5, ENABLE);                                // 使能UART5，串口开始运行
}


