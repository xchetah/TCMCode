/**
  ******************************************************************************
  * @file    ARC_USART.c
  * @author  armrunc (www.armrunc.com)
  * @version V1.0.0
  * @brief   ARC middleware. 
  *          This file provides USART middleware functions.
  ******************************************************************************
  * @copy
  *
  * For non-commercial research and private study only.
  *
  * <h2><center>&copy; COPYRIGHT www.armrunc.com </center></h2>
  */ 
  
/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "ARC_USART.h"
#include "ARC_RCC.h"
#include "ARC_GPIO.h"
#include <stdio.h>
#include "CN1100_common.h"

/***************************************************
* 函数名称    ：void RCC_Configuration()
* 功能描述    ： 复位和时钟控制 配置
* 参数        ：  无
* 返回值      ：  无
* 全局变量    ：  无
* 全局静态变量：  无
* 局部静态变量：  无
***********************************************************/
void USART_RCC_Configuration()
{
  #if 0
    {
       ErrorStatus HSEStartUpStatus;                //定义外部高速晶振启动状态枚举变量
       RCC_DeInit();                              //复位RCC外部寄存器到默认值
       RCC_HSEConfig(RCC_HSE_ON);                 //打开外部高速晶振
       HSEStartUpStatus=RCC_WaitForHSEStartUp(); //等待外部高速时钟准备好
 
       if(HSEStartUpStatus==SUCCESS){              //外部高速时钟已经准备好
 
              // 开启FLASH预读缓冲功能，加速FLASH的读取。所有程序中必须的用法，位置：RCC初始化子函数里面，时钟起振之后
              FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);  
              // FLASH时序延迟几个周期，等待总线同步操作。推荐按照单片机系统运行频率，
              // 0-24MHz时，取Latency=0；24-48MHz时，取Latency=1；48~72MHz时，取Latency=2。
              FLASH_SetLatency(FLASH_Latency_2);        
 
              RCC_HCLKConfig(RCC_SYSCLK_Div1);      // 配置AHB(HCLK)==系统时钟/1
              RCC_PCLK2Config(RCC_HCLK_Div1);       // 配置APB2(高速)(PCLK2)==系统时钟/1  
              RCC_PCLK1Config(RCC_HCLK_Div2);       // 配置APB1(低速)(PCLK1)==系统时钟/2
              
              //注：AHB主要负责外部存储器时钟。APB2负责AD，I/O，高级TIM，串口1。
              //                               APB1负责DA，USB，SPI，I2C，CAN，串口2345，普通TIM。
 
 
              RCC_PLLConfig(RCC_PLLSource_HSE_Div1,RCC_PLLMul_9);   // 配置PLL时钟==(外部高速晶体时钟/1）* 9 ==72MHz
              RCC_PLLCmd(ENABLE);                                   // 使能PLL时钟
              while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY)==RESET);   // 等待PLL时钟就绪
              RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);           // 配置系统时钟==PLL时钟
              while(RCC_GetSYSCLKSource()!=0x08);                 // 等待系统时钟源的启动
       }
                                                                                                                                                                     
       //------------------------以下为开启外设时钟的操作-----------------------//
       //   RCC_AHBPeriphClockCmd (ABP2设备1 | ABP2设备2 , ENABLE); //启动AHB设备
       //   RCC_APB2PeriphClockCmd(ABP2设备1 | ABP2设备2 , ENABLE); //启动ABP2设备
       //   RCC_APB1PeriphClockCmd(ABP2设备1 | ABP2设备2 , ENABLE); //启动ABP1设备
  #endif
       RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_USART1 , ENABLE);   //打开APB2外设
} 
 
 
/****************************************
* 函数名称    ：GPIO_Configuration()
* 功能描述    ： GPIO配置
* 参数        ：  无
* 返回值      ：  无
* 全局变量    ：  无
* 全局静态变量：  无
* 局部静态变量：  无
****************************************/
void USART_GPIO_Configuration()
{
       GPIO_InitTypeDef GPIO_InitStructure;    //定义GPIO初始化结构体
 
       //--------将USART1 的TX 配置为复用推挽输出 AF_PP---------------------//
       GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9;         // 管脚位置定义，标号可以是NONE、ALL、0至15。
       GPIO_InitStructure.GPIO_Speed=GPIO_Speed_2MHz;  // 输出速度2MHz
       GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;   // 推挽输出模式 Out_PP
       GPIO_Init(GPIOA,&GPIO_InitStructure);          // E组GPIO初始化
 
       //--------将USART1 的RX 配置为复用浮空输入 IN_FLOATING---------------------//
       GPIO_InitStructure.GPIO_Pin=GPIO_Pin_10;   // 管脚位置定义
       //输入模式下配置输出速度无意义
       //GPIO_InitStructure.GPIO_Speed=GPIO_Speed_2MHz;    // 输出速度2MHz 
       GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING; // 浮空输入 IN_FLOATING                     
       GPIO_Init(GPIOA,&GPIO_InitStructure);              // C组GPIO初始化
 
       
}
 
/****************************************************
* 函数名称    ：USART1_Configuration( )
* 功能描述    ： 配置USART1数据格式、波特率等参数
* 参数        ：  无
* 返回值      ：  无
* 全局变量    ：  无
* 全局静态变量：  无
* 局部静态变量：  无
*******************************************************/
void USART_USART1_Configuration()
{
       USART_InitTypeDef USART_InitStructure;                      // 串口设置恢复默认参数
 
       USART_InitStructure.USART_BaudRate = 115200;                // 波特率115200
       USART_InitStructure.USART_WordLength = USART_WordLength_8b; // 字长8位
       USART_InitStructure.USART_StopBits = USART_StopBits_1;      // 1位停止字节
       USART_InitStructure.USART_Parity = USART_Parity_No;         // 奇偶校验
       USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;  // 无流控制
       USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;                  // 打开Rx接收和Tx发送功能
       USART_Init(USART1, &USART_InitStructure);        // 初始化
//       USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);  // 若接收数据寄存器满，则产生中断
       USART_Cmd(USART1, ENABLE);                        // 启动串口                                                                             
                                                   
       //-----如下语句解决第1个字节无法正确发送出去的问题-----// 
 //      USART_ClearFlag(USART1, USART_FLAG_TC);     // 清标志
}


/*********************************************
* 函数名称    ： NVIC_Configuration(void)
* 功能描述    ： NVIC(嵌套中断控制器)配置
* 参数        ：  无
* 返回值      ：  无
* 全局变量    ：  无
* 全局静态变量：  无
* 局部静态变量：  无
***********************************************/ 
void USART_NVIC_Configuration()
{
      NVIC_InitTypeDef NVIC_InitStructure;    //定义一个中断结构体
 
		// NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);   //设置中断向量表的起始地址为0x08000000
		// NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0); //设置NVIC优先级分组，方式。
       	// 注：一共16个优先级，分为抢占式和响应式。两种优先级所占的数量由此代码确定，NVIC_PriorityGroup_x可以是0、1、2、3、4，
       	// 分别代表抢占优先级有1、2、4、8、16个和响应优先级有16、8、4、2、1个。规定两种优先级的数量后，所有的中断级别必须在其中选择，
       	// 抢占级别高的会打断其他中断优先执行，而响应级别高的会在其他中断执行完优先执行。
 
       NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;       // 通道设置为串口1中断
       NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;      // 中断响应优先级0
       NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;         // 打开中断
       NVIC_Init(&NVIC_InitStructure);                        // 初始化
} 

void ARC_COM_PARAM_Init()
{
    USART_InitTypeDef USART_InitStructure;

    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

    /* USART configuration */
    USART_Init(USART1, &USART_InitStructure);
}

void ARC_COM_Init()
{
    ARC_COM_RCC_Init();
    ARC_COM_GPIO_Init();
    ARC_COM_PARAM_Init();
}

/**************************************************
* 函数名称    ：CN1100_UsartAsDebug_Init()
* 功能描述    ： 初始化函数
* 参数        ：  无
* 返回值      ：  无
* 全局变量    ：  无
* 全局静态变量：  无
* 局部静态变量：  无
****************************************************/
void CN1100_UsartAsDebug_Init(void)
{
//    USART_RCC_Configuration();
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_USART1 , ENABLE);   //打开APB2外设
    USART_GPIO_Configuration();
    USART_USART1_Configuration();
}



#if 0
/********这是中断服务子程序，在stm32f10x_it.c中*************************/
void USART_USART1_IRQHandler(void)
{
      if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)           // 若接收数据寄存器满
      {
             USART_SendData(USART1, USART_ReceiveData(USART1));     // 回发给PC
             while(USART_GetFlagStatus(USART1, USART_IT_TXE)==RESET);// 等待发完
      }
}
#endif
/******************* (C) www.armrunc.com *****END OF FILE****/
