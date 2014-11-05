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
* ��������    ��void RCC_Configuration()
* ��������    �� ��λ��ʱ�ӿ��� ����
* ����        ��  ��
* ����ֵ      ��  ��
* ȫ�ֱ���    ��  ��
* ȫ�־�̬������  ��
* �ֲ���̬������  ��
***********************************************************/
void USART_RCC_Configuration()
{
  #if 0
    {
       ErrorStatus HSEStartUpStatus;                //�����ⲿ���پ�������״̬ö�ٱ���
       RCC_DeInit();                              //��λRCC�ⲿ�Ĵ�����Ĭ��ֵ
       RCC_HSEConfig(RCC_HSE_ON);                 //���ⲿ���پ���
       HSEStartUpStatus=RCC_WaitForHSEStartUp(); //�ȴ��ⲿ����ʱ��׼����
 
       if(HSEStartUpStatus==SUCCESS){              //�ⲿ����ʱ���Ѿ�׼����
 
              // ����FLASHԤ�����幦�ܣ�����FLASH�Ķ�ȡ�����г����б�����÷���λ�ã�RCC��ʼ���Ӻ������棬ʱ������֮��
              FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);  
              // FLASHʱ���ӳټ������ڣ��ȴ�����ͬ���������Ƽ����յ�Ƭ��ϵͳ����Ƶ�ʣ�
              // 0-24MHzʱ��ȡLatency=0��24-48MHzʱ��ȡLatency=1��48~72MHzʱ��ȡLatency=2��
              FLASH_SetLatency(FLASH_Latency_2);        
 
              RCC_HCLKConfig(RCC_SYSCLK_Div1);      // ����AHB(HCLK)==ϵͳʱ��/1
              RCC_PCLK2Config(RCC_HCLK_Div1);       // ����APB2(����)(PCLK2)==ϵͳʱ��/1  
              RCC_PCLK1Config(RCC_HCLK_Div2);       // ����APB1(����)(PCLK1)==ϵͳʱ��/2
              
              //ע��AHB��Ҫ�����ⲿ�洢��ʱ�ӡ�APB2����AD��I/O���߼�TIM������1��
              //                               APB1����DA��USB��SPI��I2C��CAN������2345����ͨTIM��
 
 
              RCC_PLLConfig(RCC_PLLSource_HSE_Div1,RCC_PLLMul_9);   // ����PLLʱ��==(�ⲿ���پ���ʱ��/1��* 9 ==72MHz
              RCC_PLLCmd(ENABLE);                                   // ʹ��PLLʱ��
              while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY)==RESET);   // �ȴ�PLLʱ�Ӿ���
              RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);           // ����ϵͳʱ��==PLLʱ��
              while(RCC_GetSYSCLKSource()!=0x08);                 // �ȴ�ϵͳʱ��Դ������
       }
                                                                                                                                                                     
       //------------------------����Ϊ��������ʱ�ӵĲ���-----------------------//
       //   RCC_AHBPeriphClockCmd (ABP2�豸1 | ABP2�豸2 , ENABLE); //����AHB�豸
       //   RCC_APB2PeriphClockCmd(ABP2�豸1 | ABP2�豸2 , ENABLE); //����ABP2�豸
       //   RCC_APB1PeriphClockCmd(ABP2�豸1 | ABP2�豸2 , ENABLE); //����ABP1�豸
  #endif
       RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_USART1 , ENABLE);   //��APB2����
} 
 
 
/****************************************
* ��������    ��GPIO_Configuration()
* ��������    �� GPIO����
* ����        ��  ��
* ����ֵ      ��  ��
* ȫ�ֱ���    ��  ��
* ȫ�־�̬������  ��
* �ֲ���̬������  ��
****************************************/
void USART_GPIO_Configuration()
{
       GPIO_InitTypeDef GPIO_InitStructure;    //����GPIO��ʼ���ṹ��
 
       //--------��USART1 ��TX ����Ϊ����������� AF_PP---------------------//
       GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9;         // �ܽ�λ�ö��壬��ſ�����NONE��ALL��0��15��
       GPIO_InitStructure.GPIO_Speed=GPIO_Speed_2MHz;  // ����ٶ�2MHz
       GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;   // �������ģʽ Out_PP
       GPIO_Init(GPIOA,&GPIO_InitStructure);          // E��GPIO��ʼ��
 
       //--------��USART1 ��RX ����Ϊ���ø������� IN_FLOATING---------------------//
       GPIO_InitStructure.GPIO_Pin=GPIO_Pin_10;   // �ܽ�λ�ö���
       //����ģʽ����������ٶ�������
       //GPIO_InitStructure.GPIO_Speed=GPIO_Speed_2MHz;    // ����ٶ�2MHz 
       GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING; // �������� IN_FLOATING                     
       GPIO_Init(GPIOA,&GPIO_InitStructure);              // C��GPIO��ʼ��
 
       
}
 
/****************************************************
* ��������    ��USART1_Configuration( )
* ��������    �� ����USART1���ݸ�ʽ�������ʵȲ���
* ����        ��  ��
* ����ֵ      ��  ��
* ȫ�ֱ���    ��  ��
* ȫ�־�̬������  ��
* �ֲ���̬������  ��
*******************************************************/
void USART_USART1_Configuration()
{
       USART_InitTypeDef USART_InitStructure;                      // �������ûָ�Ĭ�ϲ���
 
       USART_InitStructure.USART_BaudRate = 115200;                // ������115200
       USART_InitStructure.USART_WordLength = USART_WordLength_8b; // �ֳ�8λ
       USART_InitStructure.USART_StopBits = USART_StopBits_1;      // 1λֹͣ�ֽ�
       USART_InitStructure.USART_Parity = USART_Parity_No;         // ��żУ��
       USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;  // ��������
       USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;                  // ��Rx���պ�Tx���͹���
       USART_Init(USART1, &USART_InitStructure);        // ��ʼ��
//       USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);  // ���������ݼĴ�������������ж�
       USART_Cmd(USART1, ENABLE);                        // ��������                                                                             
                                                   
       //-----�����������1���ֽ��޷���ȷ���ͳ�ȥ������-----// 
 //      USART_ClearFlag(USART1, USART_FLAG_TC);     // ���־
}


/*********************************************
* ��������    �� NVIC_Configuration(void)
* ��������    �� NVIC(Ƕ���жϿ�����)����
* ����        ��  ��
* ����ֵ      ��  ��
* ȫ�ֱ���    ��  ��
* ȫ�־�̬������  ��
* �ֲ���̬������  ��
***********************************************/ 
void USART_NVIC_Configuration()
{
      NVIC_InitTypeDef NVIC_InitStructure;    //����һ���жϽṹ��
 
		// NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);   //�����ж����������ʼ��ַΪ0x08000000
		// NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0); //����NVIC���ȼ����飬��ʽ��
       	// ע��һ��16�����ȼ�����Ϊ��ռʽ����Ӧʽ���������ȼ���ռ�������ɴ˴���ȷ����NVIC_PriorityGroup_x������0��1��2��3��4��
       	// �ֱ������ռ���ȼ���1��2��4��8��16������Ӧ���ȼ���16��8��4��2��1�����涨�������ȼ������������е��жϼ������������ѡ��
       	// ��ռ����ߵĻ��������ж�����ִ�У�����Ӧ����ߵĻ��������ж�ִ��������ִ�С�
 
       NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;       // ͨ������Ϊ����1�ж�
       NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;      // �ж���Ӧ���ȼ�0
       NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;         // ���ж�
       NVIC_Init(&NVIC_InitStructure);                        // ��ʼ��
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
* ��������    ��CN1100_UsartAsDebug_Init()
* ��������    �� ��ʼ������
* ����        ��  ��
* ����ֵ      ��  ��
* ȫ�ֱ���    ��  ��
* ȫ�־�̬������  ��
* �ֲ���̬������  ��
****************************************************/
void CN1100_UsartAsDebug_Init(void)
{
//    USART_RCC_Configuration();
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_USART1 , ENABLE);   //��APB2����
    USART_GPIO_Configuration();
    USART_USART1_Configuration();
}



#if 0
/********�����жϷ����ӳ�����stm32f10x_it.c��*************************/
void USART_USART1_IRQHandler(void)
{
      if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)           // ���������ݼĴ�����
      {
             USART_SendData(USART1, USART_ReceiveData(USART1));     // �ط���PC
             while(USART_GetFlagStatus(USART1, USART_IT_TXE)==RESET);// �ȴ�����
      }
}
#endif
/******************* (C) www.armrunc.com *****END OF FILE****/
