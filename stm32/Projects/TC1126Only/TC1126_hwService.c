/******************************************************************************
 * 版权所有(C) TRUECOREING
 * DEPARTMENT:
 * MANUAL_PERCENT:
 * 文件名称: TC1126_hwService.c
 * 文件标识:    
 * 内容摘要: 
 * 其它说明:
 * 当前版本: 
 * 作    者: 
 * 完成日期:
 * 当前责任人: 
 *
 * 修改记录1: 代码合规
 * 修改日期: 2014-09-15
 * 版 本 号:
 * 修 改 人: Wangpc
 * 修改内容: 
 *
 * 修改记录2: Add one feature that acquire 10 fingers then parse 5 fingers 
 * 修改日期: 2014-11-12
 * 版 本 号:
 * 修 改 人: Wangpc(R01)
 * 修改内容: 
 *****************************************************************************/



#ifndef CN1100_HWSERVICE_C
#define  CN1100_HWSERVICE_C


#include "CN1100_common.h"
#ifdef CN1100_STM32
#include "CN1100_STM32.h"
#include "ARC_RCC.h"
#endif
#ifdef CN1100_LINUX
#include "CN1100_linux.h"
#endif
#include "CN1100_Function.h"

#ifdef CN1100_STM32


/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
void STM32_LEDx_TURN_ONorOFF(uint8_t LEDx, uint8_t ONorOFF)
{
    #ifdef STM32VC_LCD
    if(LED1 == LEDx)
    {
        if(LED_ON == ONorOFF)
        {
            GPIO_ResetBits(GPIOB, GPIO_Pin_0); /*  Turn On the LED1 */
        }
        else
        { 
            GPIO_SetBits(GPIOB, GPIO_Pin_0); /*  Turn Off the LED1 */
        }
    }
    else if(LED2 == LEDx)
    {
        if(LED_ON == ONorOFF)
        {
            GPIO_ResetBits(GPIOB, GPIO_Pin_1); /*  Turn On the LED2 */
        }
        else
        {
            GPIO_SetBits(GPIOB, GPIO_Pin_1); /*  Turn Off the LED2 */
        }
    }
    #else
    if(LED1 == LEDx)
    {
        if(LED_ON == ONorOFF)
        {
            GPIO_ResetBits(GPIOA, GPIO_Pin_1); /*  Turn On the LED1 */
        }
        else
        {
            GPIO_SetBits(GPIOA, GPIO_Pin_1); /*  Turn Off the LED1 */
        }
    }
    else if(LED2 == LEDx)
    {
        if(LED_ON == ONorOFF)
        {
            GPIO_ResetBits(GPIOA, GPIO_Pin_2); /*  Turn On the LED2 */
        }
        else
        {
            GPIO_SetBits(GPIOA, GPIO_Pin_2); /*  Turn Off the LED2 */
        }
    }
    #endif
}

/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/

#ifdef SHUT_DOWN_TEST
void STM32_ShutDown_TouchIC(void)
{
    /* CN1100_print("   ShutDownTest... \n");  */

   /******************************************************
       * 1. Reset The TC1126 first
       * 2. Setup OSCC
       * 3. Set External Clock
       * 4. Make sure RSTN pin as FLoating (Tri-State)
       ******************************************************/
    GPIO_ResetBits(GPIOA, GPIO_Pin_4); /*  Reset the RSTN */
    Tiny_Delay(500);                     /*  200 us */
    GPIO_SetBits(GPIOA, GPIO_Pin_4);    /*  Set the RSTN */
    Tiny_Delay(500);                    /*  200 us */
    
    SPI_write_singleData(OSCC_REG, 0x0580); 
    SPI_write_singleData(FCTL_REG, FCTL_S_SEL); 
    
    /*  Setup RSTN as Tri-State statement */
    {
    GPIO_InitTypeDef GPIO_InitStructure;
    /* Configure the RESET pin */
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    }

    /* CN1100_print("ShutDownTest ... Done!\n"); */
}
#endif

/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
void STM32_RCC_initialize(void)
{
    #ifdef CN1100_STM32
    ErrorStatus HSEStartUpStatus;      
    /* RCC system reset(for debug purpose) */
    RCC_DeInit(); 
    /* Enable HSE */
    RCC_HSEConfig(RCC_HSE_ON);
    /* Wait till HSE is ready */
    HSEStartUpStatus = RCC_WaitForHSEStartUp();
    if(SUCCESS == HSEStartUpStatus)
    {
        /* Enable Prefetch Buffer */
        FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);
        /* Flash 2 wait state */
        FLASH_SetLatency(FLASH_Latency_2);
        /* HCLK = SYSCLK */
        RCC_HCLKConfig(RCC_SYSCLK_Div1); 
        /* PCLK2 = HCLK */
        RCC_PCLK2Config(RCC_HCLK_Div1); 
        /* PCLK1 = HCLK/2 */
        RCC_PCLK1Config(RCC_HCLK_Div2);  /*  36MZH */
        /* On STICE the PLL output clock is fixed to 48 MHz */
        RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);
        /* Enable PLL */ 
        RCC_PLLCmd(ENABLE);
        /* Wait till PLL is ready */
        while(RESET == RCC_GetFlagStatus(RCC_FLAG_PLLRDY));
        /* Select PLL as system clock source */
        RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
        /* Wait till PLL is used as system clock source */
        while(RCC_GetSYSCLKSource() != 0x08);
    }
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP , ENABLE);
#endif
}



#ifdef USB_COMMUNICATION
/*******************************************************************************
* Function Name  : STM32_USB_NVIC_Configuration
* Description    : Configures Vector Table base location.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void STM32_USB_NVIC_Configuration(void)
{    
    /* NVIC_InitTypeDef NVIC_InitStructure; */
    #ifdef  VECT_TAB_RAM  
    /* Set the Vector Table base location at 0x20000000 */ 
    NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0); 
    #else  /* VECT_TAB_FLASH  */
    /* Set the Vector Table base location at 0x08000000 */ 
    NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);   
    #endif 
}
#endif

void  STM32_GPIO_InitProcess(void)
{
    uint16_t i;
    GPIO_InitTypeDef GPIO_InitStructure;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);   
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);   
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);   
    
    
    #ifdef STM32VC_LCD
    /* Configure the LED0 pin : PB0 */
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    /* Configure the LED1 pin : PB1 */
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    #else
    /* Configure the LED0 pin : PA1 */
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    /* Configure the LED1 pin : PA2 */
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    #endif
    
    /* Configure the RSTN from STM32 to Touch_IC pin */
    #ifdef STM32VC_LCD
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_7;
    #else
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_4;
    #endif
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
 /******************************************
    * Configure the LED8-ABCDEFG pin 
    * PC4, PC5, PC6, PC7, PC8, PC9,PC10, PC11
    ****************************************** */
    for (i=0; i<7; i++)
    {
    GPIO_InitStructure.GPIO_Pin   = (GPIO_Pin_4<<i);
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    }
    
    #ifdef SLEEP_EVENT_SIM
 /****************************************************
    * Change to use PIN_E (to PC8) changed as Input
    **************************************************** */
    GPIO_InitStructure.GPIO_Pin   = (GPIO_Pin_8);
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    #endif

    #ifdef STM32VC_LCD
 /****************************************************
    * Configure KEY1 (GPIOC-4)
    **************************************************** */
    GPIO_InitStructure.GPIO_Pin   = (GPIO_Pin_4);
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    
 /****************************************************
    * Configure KEY2 (GPIOC-5)
    **************************************************** */
    GPIO_InitStructure.GPIO_Pin   = (GPIO_Pin_5);
    /* GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING; */
    /* GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; */
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    #endif
    
 /******************************************
    * Configure the LED8-COMM123 pin 
    * GPIOB12, GPIOB13, GPIOB14, GPIOB15
    ****************************************** */
    for (i=0; i<3; i++)
    {
    GPIO_InitStructure.GPIO_Pin   = (GPIO_Pin_12<<i);
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    }
}



/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/

void STM32_ScanDoneIRQ_Setup(void)
{
    CN1100_FrameScanDone_RCC_Init();
    CN1100_FrameScanDone_GPIO_Init();
    CN1100_FrameScanDone_NVIC_Init();
    CN1100_FrameScanDone_EXTI_Init();
    
    #ifdef SLEEP_EVENT_SIM
    {
    NVIC_InitTypeDef NVIC_InitStructure;
    
    /* 2 bit for pre-emption priority, 2 bits for subpriority */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    
    NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure); 
    }
    {
    EXTI_InitTypeDef EXTI_InitStructure;
    
    /* Connect Button EXTI Line to Button GPIO Pin */
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource8);
    
    /* Configure Button EXTI line */
    EXTI_InitStructure.EXTI_Line = EXTI_Line8;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;  
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
    }
    #endif
    
    #ifdef STM32VC_LCD
    
    /********************************************************
    * KEY1 (GPIOC-4) Button Press Interrupt Configuration
    ******************************************************** */
    {
    NVIC_InitTypeDef NVIC_InitStructure;
    
    /* 2 bit for pre-emption priority, 2 bits for subpriority */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    
    NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure); 
    }
    {
    EXTI_InitTypeDef EXTI_InitStructure;
    
    /* Connect Button EXTI Line to Button GPIO Pin */
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource4);
    
    /* Configure Button EXTI line */
    EXTI_InitStructure.EXTI_Line = EXTI_Line4;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;  
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
    }
    
    /********************************************************
    * KEY2 (GPIOC-5) Button Press Interrupt Configuration
    ******************************************************** */
    {
    NVIC_InitTypeDef NVIC_InitStructure;
    
    /* 2 bit for pre-emption priority, 2 bits for subpriority */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    
    NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure); 
    }
    {
    EXTI_InitTypeDef EXTI_InitStructure;
    
    /* Connect Button EXTI Line to Button GPIO Pin */
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource5);
    
    /* Configure Button EXTI line */
    EXTI_InitStructure.EXTI_Line = EXTI_Line5;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;  
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
    }
    #endif
    
    CN1100_print("C.");
}

/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
void STM32_TouchICReset(void)
{
    #ifdef CN1100_STM32
    #ifdef STM32VC_LCD
    GPIO_ResetBits(GPIOA, GPIO_Pin_7); /*  Reset the RSTN */
    STM32_SetLevelOnIRQ(0);        /*  0: 0x40, 1: 0xBA */
    Tiny_Delay(50);                     /*  200 us */
    GPIO_SetBits(GPIOA, GPIO_Pin_7);    /*  Set the RSTN */
    #else
    GPIO_ResetBits(GPIOA, GPIO_Pin_4); /*  Reset the RSTN */
    STM32_SetLevelOnIRQ(0);        /*  0: 0x40, 1: 0xBA */
    Tiny_Delay(50);                     /*  200 us */
    GPIO_SetBits(GPIOA, GPIO_Pin_4);    /*  Set the RSTN */
    #endif
    #endif
}

/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/

void STM32_SetLevelOnIRQ(uint16_t level)
{
    #ifdef CN1100_STM32
    GPIO_InitTypeDef GPIO_InitStructure;
    
    /* Configure the FrameScanDone pin */
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    if(level)
    {
        GPIO_SetBits(GPIOC, GPIO_Pin_2);
    }
    else
    {
        GPIO_ResetBits(GPIOC, GPIO_Pin_2);
    }
    #endif
}


/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/

void STM32_SetIRQAsInput(void)
{
    #ifdef CN1100_STM32
    GPIO_InitTypeDef GPIO_InitStructure;
    
    /* Configure the FrameScanDone pin */
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    #endif
}


/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/

void STM32_WakeupToucIC(void)
{
    #ifdef CN1100_STM32
    uint32_t i;
 
    STM32_SetLevelOnIRQ(1);
   
    for(i=0; i<4000; i++) 
    { 
        i++;
    }
    
    STM32_SetIRQAsInput();
    #endif
}



/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/

void STM32_ExtiIRQControl(uint8_t FuncSelect)
{
    #ifdef CN1100_STM32
    EXTI_InitTypeDef EXTI_InitStructure;
    /* Connect Button EXTI Line to Button GPIO Pin */
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource2);
    
    /* Configure Button EXTI line */
    EXTI_InitStructure.EXTI_Line = EXTI_Line2;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; 
    if(ENABLE == FuncSelect)
    {
        EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    }
    else
    {
        if(DISABLE == FuncSelect)
        {    
            EXTI_InitStructure.EXTI_LineCmd = DISABLE;
        }
    }
    EXTI_Init(&EXTI_InitStructure);
    #endif
}


/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
void Stupid_Delay(uint32_t TimeCount)
{
    uint32_t i, j;
    
    for (i=0; i<TimeCount; i++)
    {
        for (j=0; j<72000; j++) 
        {
            j++;
        }
    }
}


/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
void Tiny_Delay(uint32_t TimeCount)
{
    uint32_t i, j=0;
    
    for (i=0; i<TimeCount; i++)
    {
        j++;
    }
}

#endif


/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
void TC1126_Init_StartADCByReg21(void)
{
    SPI_write_singleData(ADCM_REG,  ADCM_ADC_SPEED(ADC_SPEED_SET) | ADCM_ACS(ACS_SPEED_SET) | ADCM_SHRT_CKT_EN 
                        | ADCM_TIMING_EN | ADCM_MB_EN | ADCM_ACTV_CONF | ADCM_XMTR_STR(XMTR_STRENGTH_SET));
    SPI_write_singleData(ADCM_REG,  ADCM_ADC_SPEED(ADC_SPEED_SET) | ADCM_ACS(ACS_SPEED_SET) 
                        | ADCM_XMTR_STR_ENB | ADCM_TIMING_EN | ADCM_MB_EN | ADCM_XMTR_STR(XMTR_STRENGTH_SET));
    SPI_write_singleData(FLAG_REG,  0x001f);
}


/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/

void TC1126_Init_StartADCAtBegin(void)
{
 /*******************************************************
    * Start the Burst and ADC
    ******************************************************* */
    switch(bdt.ModeSelect)
    {
        case iAUTOSCAN_MODE:
        case SLEEP_MODE:
        case DOZE_MODE:
        {
            TC1126_Init_StartADCByReg21();
            break;
        }
    }

    /*******************************************************
    * Setupthe first burst flag
    ********************************************************/
    bdt.BSDSTS.First_Burst = 0;              // Useless now
    bdt.BSDSTS.Buff_Select = BUFFER_A;       // Useless now
    bdt.BSDSTS.Buf_A_Ready = DATA_UNREADY;   // Useless now
    bdt.BSDSTS.Buf_B_Ready = DATA_UNREADY;   // Useless now
    bdt.BSDSTS.ADC_Release = ADC_RELEASED;   // Useless now
    bdt.BSDSTS.iBuf_A_Fill = FRAME_UNFILLED; // Useful
    bdt.BSDSTS.iBuf_B_Fill = FRAME_UNFILLED; // Useful
    bdt.BSDSTS.Buf_A_Pring = 0;              // Useless now
    bdt.BSDSTS.Buf_B_Pring = 0;              // Useless now
}



/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/

void TC1126_Init_HostCPUStep1(void)
{
#ifdef CN1100_STM32
    STM32_GPIO_InitProcess();   /* initialize GPIOA\B\C. */
    #ifdef SLEEP_EVENT_SIM
    STM32_WakeupToucIC();
    #endif
    STM32_RCC_initialize();        /*  initialize RCC     */
    /***********************************
    * RESET the Touch IC
    *********************************** */
    STM32_TouchICReset();
    Tiny_Delay(1600);                     /*  200 us */
    STM32_TouchICReset();
    STM32_SetIRQAsInput();
    /***********************************
    * initialize SPI or I2C
    *********************************** */
    #ifdef CN1100_I2C_COMM
    ARC_I2C_Init();
    I2C_Cmd(I2C_Sel, ENABLE);
    #else
    ARC_SPI_Init();
    SPI_Cmd(SPI1, ENABLE);
    #endif  
    CN1100_UsartAsDebug_Init();/* initialize USART send init */
#endif
}

void TC1126_Init_HostCPUStep2(void)
{
#ifdef CN1100_STM32
    #ifndef CN1100_I2C_COMM
    ARC_SPI_FAST_Init();
    SPI_Cmd(SPI1, ENABLE);
    #endif
  
    STM32_ScanDoneIRQ_Setup();
    /**************************************
    * initialize USB or Setup USART Interrupt Method
    ************************************** */
    #ifdef USB_COMMUNICATION 
    STM32_USB_NVIC_Configuration();
    Set_System();
    USB_Interrupts_Config();
    Set_USBClock();   
    USB_Init();
    #else   /* initialize USART receive interrupt */
    USART_NVIC_Configuration();
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); /*  若接收数据寄存器满，则产生中断  */
         /* -----如下语句解决第1个字节无法正确发送出去的问题 */
    USART_ClearFlag(USART1, USART_FLAG_TC); /*  清标志位 */
    #endif
    STM32_LEDx_TURN_ONorOFF(LED1, LED_OFF);
    STM32_LEDx_TURN_ONorOFF(LED2, LED_OFF);
#endif
}

void TC1126_Init_HostCPUStep3(void)
{
#ifdef CN1100_STM32
   /*************************************************
    * initialize SysTick for exact time delay    
    * devided by 1000, the timer will be 1ms now 
    ************************************************* */
    ARC_SysTick_Init();
    
    #ifdef SHUT_DOWN_TEST
    STM32_ShutDown_TouchIC();
    #endif
#endif
}
/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/

void TC1126_Init_HardwareRegs(void)
{
    TC1126_Init_HostCPUStep1(); // STM32
    TC1126_Init_AllRegisters();
    TC1126_Init_HostCPUStep2(); // STM32
    
    #if 0
    {
        uint16_t  i, tempa;
        for(i=OSCC_REG; i<=FCTL_REG; i++)
        {
            if((i!=0x2E) && (i!=0x30) )
            {
                tempa = SPI_read_singleData(i);
                CN1100_print("REG(%x) = 0x%x\n", i, tempa);
            }
        }
        while(1);
    }
    #endif
    
    CN1100_print("===\n\n\n");
    TC1126_Init_StartADCAtBegin();    

    TC1126_Init_HostCPUStep3(); // STM32
    
}


/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
void TC1126_Init_GlobalVariables(void)
{
    uint16_t i, j;
    
    //bdt.XmtrDriveOut    = 1<<XMTR_STR;     /*  XTMR Input Signal Switch (1^Index) */
    //bdt.XmtrDriveCount  = 0;   /*  XTMR input switch index (0-27) */
    //bdt.int_wait        = 0;   /*  We may read data once every Several Interrupts; */
    
    bdt.BSDSTS.First_Burst = 0;              // Useless now
    bdt.BSDSTS.Buff_Select = BUFFER_A;       // Useless now
    bdt.BSDSTS.Buf_A_Ready = DATA_UNREADY;   // Useless now
    bdt.BSDSTS.Buf_B_Ready = DATA_UNREADY;   // Useless now
    bdt.BSDSTS.ADC_Release = ADC_RELEASED;   // Useless now
    bdt.BSDSTS.iBuf_A_Fill = FRAME_UNFILLED; // Useful
    bdt.BSDSTS.iBuf_B_Fill = FRAME_UNFILLED; // Useful
    bdt.BSDSTS.Buf_A_Pring = 0;              // Useless now
    bdt.BSDSTS.Buf_B_Pring = 0;              // Useless now

    bdt.MEM_MAX_XY_Count   = 0;
    bdt.MEM_MAX_X_XMTR     = XMTR_NUM;
    bdt.MEM_MAX_Y_RECV     = RECV_NUM;
    
    bdt.MEM_MIN_XY_Count   = 0;
    bdt.MEM_MIN_X_XMTR     = XMTR_NUM;
    bdt.MEM_MIN_Y_RECV     = RECV_NUM;
    bdt.PowerOnWithFinger  = 0;
    
    #ifdef CN1100_iSCANMODE
    bdt.ModeSelect      = iAUTOSCAN_MODE;
    #endif
    
    #ifdef CN1100_SLEEPMODE
    bdt.ModeSelect      = SLEEP_MODE;
    #endif
    
    #ifdef CN1100_DOZEMODE
    bdt.ModeSelect      = DOZE_MODE;
    #endif
    
    for (i = 0; i < XMTR_NUM; i++)
    for (j = 0; j < RECV_NUM; j++)
    {
        bdt.FrameDatLoadA[i][j]    = 0;
        bdt.FrameDatLoadB[i][j]    = 0;
        bdt.BFD.BaseDat[i][j]      = 0;
        bdt.BFD.BaseDatSaved[i][j] = 0;
        bdt.BFD.SumBaseDat[i][j]   = 0;
        bdt.FrameDataSave[i][j]    = 0;
    }

    #ifdef PRESS_KEY_DETECT

    #if (KXMTR_NUM == 1)
    for (j = 0; j < RECV_NUM; j++)
    #endif

    #if (KRECV_NUM == 1)
    for (j = 0; j < XMTR_NUM; j++)
    #endif
    {
        bdt.DeltaDat_kp[j] = 0;
    }
    #endif

    bdt.BFD.SettledPointExistTime  = 0;    
    bdt.BFD.AdjustCount            = 0;
    bdt.BFD.AbnormalUpdateDelay    = 0;
    bdt.BFD.InitCount              = 0;   /*  count for saving basedata; */
    bdt.BFD.bbdc.FingerExist       = NO_FINGER;
    bdt.BFD.bbdc.PreFingerExt      = FINGER_SHOW;
    bdt.BFD.bbdc.NoFingerCnt4Base  = 0;
    bdt.BFD.bbdc.BaseUpdateCase    = BASELINE_UPDATING_CASE;
    bdt.MTD.mtdc.Doze_FirstIn      = 0;
    bdt.MTD.mtdc.Doze_OddNum       = 0;
    bdt.MTD.NoFingerCnt4Doze       = 0;

    bdt.MaxValueInFrame            = 0;
    bdt.ThrLow4DistanceFilter      = THR072;
    bdt.ThrHigh4DistanceFilter     = THR144;
    
    
    bdt.NDD.Noise_Sum     = 0;
    bdt.NDD.Noise_Count   = 0;
    bdt.NDD.Frame_Count   = 0;
    bdt.NDD.Battery_Level = 0;
    
    #ifdef COMMUNICATION_WITH_PC
    dbg.DebugInfoLevel    = DEBUG_INFO_NONE;
    dbg.validCommand      = USART_RECEIVED_CMD;
    dbg.UartParaNum       = 0;
    dbg.debug_value_num   = DEBUG_VALUE_NUM_MAX;
    for (i = 0; i < DEBUG_VALUE_NUM_MAX; i++) 
    {
        dbg.debug_value[i] = 0;
    }
    #ifdef USB_COMMUNICATION
    dbg.USB_ReceiveFlg = FALSE;
    dbg.TimeCount = 0;
    dbg.USB_dataflag = USB_DATA_UNREADY;
    dbg.TbufCount    = 0;
    dbg.RowCount     = 0;
    #endif
    #endif
    
    #ifdef TPD_PROXIMITY
    bdt.FDC.Flag             = 0;       // 
    bdt.FDC.WFNum            = 0;       // 
    bdt.FDC.BigNum           = 0;       // 
    bdt.FaceDetectDelay      = 0;
    #endif
    bdt.PrevFingerDetectNum  = 0;
    bdt.PrevFingerDetectNum2 = 0;

    bdt.MaxNoise_Sum = 0;
    bdt.MaxNoise_SumCount = 0;
    bdt.Noise_Sum = 0;
    bdt.updatecount =350;


    for (i = 0; i < FINGER_NUM_MAX;i++)
    {
        bdt.DPD[i].FingerStateFlag      = 0;    /*  No Finger */
        bdt.DPD[i].FingerStateFlagEdge  = 0;
        bdt.DPD[i].JustPassStateFlag4   = 0;
        bdt.DPD[i].AdjustDistance       = FINGER_ADJUST_DISTANCE_MAX;
        bdt.DPD[i].AdjustState          = STATE_STICK_START;
        bdt.DPD[i].AdjStickCounter      = 0;
        bdt.DPD[i].AdjustOrigin_x       = 0;
        bdt.DPD[i].AdjustOrigin_y       = 0;
        
        bdt.DPD[i].StayCount            = 0;
        bdt.DPD[i].Stay_XSum            = 0;
        bdt.DPD[i].Stay_YSum            = 0;

#ifdef SUPER_FILTER4EDGE
        bdt.DPD[i].EdgeShift_L          = 3;
        bdt.DPD[i].EdgeOffset_L         = 8;
        bdt.DPD[i].EdgeShift_R          = 3;
        bdt.DPD[i].EdgeOffset_R         = 8;
        bdt.DPD[i].EdgeShift_T          = 3;
        bdt.DPD[i].EdgeOffset_T         = 8;
        bdt.DPD[i].EdgeShift_B          = 3;
        bdt.DPD[i].EdgeOffset_B         = 8;
        bdt.DPD[i].FingerRealNum1_X     = 0;
        bdt.DPD[i].FingerRealNum2_X     = 0;
        bdt.DPD[i].FingerRealNum2R_X    = 0;
        bdt.DPD[i].FingerRealNum1_Y     = 0;
        bdt.DPD[i].FingerRealNum2_Y     = 0;
        bdt.DPD[i].FingMovingSpeed      = FINGER_FINGER_SPEED_LOW;
#endif        
        for (j = 0;j < FINGER_INFO_SAVE_NUM;j++)
        {
            bdt.DPD[i].Prev_Finger_X[j]  = 0;
            bdt.DPD[i].Prev_Finger_Y[j]  = 0;
        }

        
        bdt.DPD[i].EdgeInfo.EdgeNum = 0;
        bdt.DPD[i].EdgeInfo.PrevEdgeNum = 0;

        #ifdef FROMOUT2IN_INORDER
        for (j = 0;j < 3; j++)
        {
            bdt.DPD[i].Finger_X_Erpt[j]  = 0; 
            bdt.DPD[i].Finger_Y_Erpt[j]  = 0;
            bdt.DPD[i].Finger_X_Erpt1[j] = 0;
            bdt.DPD[i].Finger_Y_Erpt1[j] = 0;
        }
        #endif        
        
        bdt.DPD[i].Point_Hold        = 0;
        bdt.DPD[i].LifeNumber        = 0;
        bdt.DPD[i].Finger_X_Reported = 0;
        bdt.DPD[i].Finger_Y_Reported = 0;
        bdt.DPD[i].Finger_X_Raw      = 0;
        bdt.DPD[i].Finger_Y_Raw      = 0;

        #ifdef RAWPOINT4FINGMATCH
        bdt.DPD[i].Finger_XRAW[0] = 0;
        bdt.DPD[i].Finger_XRAW[1] = 0;
        bdt.DPD[i].Finger_XRAW[2] = 0;
        bdt.DPD[i].Finger_YRAW[0] = 0;
        bdt.DPD[i].Finger_YRAW[1] = 0;
        bdt.DPD[i].Finger_YRAW[2] = 0;
        #endif

        #ifdef STM32VC_LCD
        bdt.DPD[i].Finger_X_LCD = 0;
        bdt.DPD[i].Finger_Y_LCD = 0;
        #endif
    }
    
    /******************************************
    * bdt.BFD.ProtectTimeCount = PROTECT_TIME;
    ****************************************** */
    bdt.BFD.FingerLeftProtectTime = PROTECT_TIME;
    bdt.BFD.AfterBaseUpdatedTime  = 0;
    bdt.PressKeyFlag1             = NO_KEY_PRESSED;
    bdt.BFD.DeltaSum              = 0;
    bdt.BFD.DeltaSumMaxThreshold  = XMTR_NUM*RECV_NUM*10; /* 0 -0xfff; */  
    bdt.BaseChangeFlag            = 0;
    bdt.ThresholdPara             = THRESHOLD_PARA_MAX;
    bdt.FingerRealNum             = 0;
    bdt.FingerRealNum1            = 0;

    bdt.REG3E_Value               = 0;
    bdt.LFrame_NUM                = 0;

    for (i = 0; i < FINGER_NUM_MAX;i++)
    {
        bdt.LFrame_X_XMTR[i] = XMTR_NUM;
        bdt.LFrame_Y_RECV[i] = RECV_NUM;
    }
    
    #ifdef FREQHOP_BYSTRETCH
    bdt.NoiseDataFrFrame      = 0;
    bdt.SumNoiseDataFrFrame   = 0;
    bdt.TxScanNoiseCount      = 0;
    bdt.StretchValue          = STRETCH_STNUM;
    bdt.StretchInReg          = STRETCH_STNUM; // No StretchInReg could be STRETCH_STNUM later
    for (i = 0; i < 15;i++)
    {
        bdt.NoiseDataTable[i] = 0;
    }
    bdt.FingerDelay = 0;
    bdt.TxScanValid = 0;
    bdt.CurNoiseThrHi = 0;
    bdt.CurNoiseThrLo = 0;
    bdt.CurNoiseSum = 0;
    #endif
    
    bdt.PCBA.ProtectTime           = PROTECT_TIME;
    bdt.PCBA.FrameMaxSample        = FRAME_MAX_SAMPLE;
    bdt.PCBA.AbnormalMaxDiff       = ABNORMAL_MAX_DIFF;
    bdt.PCBA.AbnormalNumDiff       = ABNORMAL_NUM_DIFF;
    //bdt.PCBA.FixPointMaxTime       = FIX_POINT_MAX_TIME;    // useless now(QFU)
    bdt.PCBA.MaxUpdateTime         = MAX_MUST_UPDATE_PERIOD;
    
    bdt.PCBA.HighRefSet            = HIREF_SETTING;
    bdt.PCBA.LowRefSet             = LOREF_SETTING;
    bdt.PCBA.HighRefGainSet        = HIREF_GAIN_SET;
    bdt.PCBA.LowRefGainSet         = LOREF_GAIN_SET;
    bdt.PCBA.HighRefPlSet          = HIREF_PL_SET;
    bdt.PCBA.LowRefPlSet           = LOREF_PL_SET;
    bdt.PCBA.TxPolarity            = TX_DRIVE_PL;
    
    #ifdef SCREEN_FULL_ADAPTIVE
    bdt.PCBA.HighRefGainSet        = 0;
    bdt.PCBA.LowRefGainSet         = 0;
    bdt.PCBA.RefHLSetCount         = 0;
    bdt.PCBA.RefHLSetEndFlag       = 0;
    for(i=0; i<CIRCLE_MAXCOUNT; i++)
        bdt.PCBA.RefHLSetAve[i]    = 0;
    #endif
    
    bdt.PCBA.FinAdjDisMin          = FINGER_ADJUST_DISTANCE_MIN;
    bdt.PCBA.FinAdjDisMax          = FINGER_ADJUST_DISTANCE_MAX;
    bdt.PCBA.MaxValueNoFinger      = MAX_VAL_NON_FINGER;
    bdt.PCBA.FinThrMin             = ROUGH_FINGER_THR;
    bdt.PCBA.FinAdjDisX            = FINGER_ADJUST_DISTANCE_MIN;
    
    bdt.PCBA.RcvmRcvrFcapSet       = RCVM_RCVR_FCAP_SET;
    bdt.PCBA.DurReset              = DUR_RESET;
    bdt.PCBA.DurInteg              = DUR_INTEG;
    bdt.PCBA.DurStretch            = DUR_STRETCH;
    bdt.PCBA.BurstCnt              = BURST_CNT;
    
    bdt.Prepare2SleepMode = 0;
    bdt.ClockCount4ANY    = 0;
    
    for (i = 0;i < XMTR_NUM;i++)
    {
        bdt.AveDeltaXmtr[i] = 0;
    }
    
    for ( i = 0;i < FINGER_NUM_MAX;i++)
    {
        bdt.DPD[i].DataInvalidFlag = 0;       /*  Default we think all data is valid */
        for (j = 0;j < FINGER_NUM_MAX;j++)
        {
            bdt.DPD[i].Extremum_Pos_X[j] = 0;
            bdt.DPD[i].Extremum_Pos_Y[j] = 0;
        }
    }
    
    #ifdef STM32VC_LCD
    bdt.BiggestBar            = 0;
    bdt.LcdIsrFlag            = 0;
    bdt.Button_Glitch_Protect = 0;
    bdt.LcdShowDataType       = DEBUG_INFO_NONE;
    bdt.LcdShowDataIndex      = 0;
    #endif
    for(j=0; j<RECV_NUM; j++)
    {
        bdt.RxFcapValue[j] = bdt.PCBA.RcvmRcvrFcapSet;
    }

    #ifdef SHOW_H_DATA
    bdt.Left_h   = 0;
    bdt.Right_h  = 0;
    bdt.Top_h    = 0;
    bdt.Bottom_h = 0;
    #endif
    #ifdef CHANNEL_ADAPTIVE
    bdt.AllAveValue = 0;
    bdt.AbnormalTxChNum = 0;
    bdt.AbnormalRxChNum = 0;
    bdt.AdjustRxChFlag = 0;
    for(i=0; i<SXMTR_NUM; i++)
    {
        bdt.TxAveValue[i] = 0;        
        bdt.TxAbnormalCh[i] = -1;    //不能初始化为0，因为我们需要0通道
    }
    for(j=0; j<SRECV_NUM; j++)
    {
        bdt.RxAveValue[j] = 0;
        bdt.RxAbnormalCh[j] = -1;
    }
    for(i=0; i<ABCHANDPOINT_MAXNUM; i++)
    {
        bdt.RealAbnormalTxCh[i] = -1;
        bdt.RealAbnormalRxCh[i] = -1;
    }
    bdt.FindOkFlag = 0;
    bdt.AbnormalPointNum = 0;
    bdt.AbPointTxiCoord = -1;
    bdt.AbPointRxiCoord = -1;
    bdt.AbPointTxjCoord = -1;
    bdt.AbPointRxjCoord = -1;
    bdt.TxRxiFCAP = 0;
    bdt.TxRxjFCAP = 0;
    for(i=0; i<ABNORMALPOINT_MAXNUM; i++)
    {
        bdt.AbnormalPoint[i] = 0;
    }
    #endif
    
    #ifdef SLIPDIRJUDGEMENT
    #ifdef DIRDEBUG
    bdt.Debug_X = 0;
    bdt.Debug_Y = 0;
    for(i=0; i<12; i++)
        bdt.Debug[i] = 0;
    #endif
    bdt.SlipDirFlag = 0;
	bdt.EdgeCount = 0;
    #endif
    
    bdt.FingerReqNum = FINGER_REQUIREMENT_NUM;   //R01 -a
}

/******************************************************************************
* We always set "ONE_MORE_LINE_SCAN" in the array first
******************************************************************************/
#if (defined(PHONESCREEN_13X10) || defined(SH_FEATUREPHONE_13X9))
uint8_t  const XmtrOrder[XMTR_NUM+1] = {SCN_T0,SCN_T0,SCN_T1,SCN_T2,SCN_T3,SCN_T4,SCN_T5,SCN_T6,SCN_T7,SCN_T8,SCN_T9,SCN_T10,SCN_T11,SCN_T12};
#elif (defined(FPCSCREEN_17X10) )
uint8_t  const XmtrOrder[XMTR_NUM+1] = {SCN_T0,SCN_T0,SCN_T1,SCN_T2,SCN_T3,SCN_T4,SCN_T5,SCN_T6,SCN_T7,SCN_T8,SCN_T9,SCN_T10,SCN_T11,SCN_T12,SCN_T13,SCN_T14,SCN_T15,SCN_T16};
#else
uint8_t  const XmtrOrder[XMTR_NUM+1] = {SCN_T0,SCN_T0,SCN_T1,SCN_T2,SCN_T3,SCN_T4,SCN_T5,SCN_T6,SCN_T7,SCN_T8,SCN_T9,SCN_T10,SCN_T11,SCN_T12,SCN_T13,SCN_T14};
#endif


/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
void TC1126_Init_TxMappingRegisters(void)
{
    uint16_t i;
    uint16_t TX0TO15_ENABLE_FLAG = 0;
    uint16_t TX16_ENABLE_FLAG    = 0;
    uint16_t TX0TO15_PLFLAG      = 0;
    uint16_t TX16_PLFLAG         = 0;

#ifdef ONE_MORE_LINE_SCAN
    for (i = 0;i <= XMTR_NUM;i++)
    {
        if(XmtrOrder[i] < 16)
        {
            SPI_write_singleData(TXMAP_REG(i),TXMAP_TX_CH(XmtrOrder[i]));
            TX0TO15_ENABLE_FLAG |= (1<<XmtrOrder[i]);
            if(1 == bdt.PCBA.TxPolarity) TX0TO15_PLFLAG |= (1<<XmtrOrder[i]);
        }
        else if(16 == XmtrOrder[i])
        {
            SPI_write_singleData(TXMAPTOTX16ONLY_REG, TXMAPTO16O_Bit(i));
            SPI_write_singleData(TXMAP_REG(i),0);
            TX16_ENABLE_FLAG |= RMP3_TX_EN16;
            if(1 == bdt.PCBA.TxPolarity) TX16_PLFLAG |= RMP3_TX_PL16;
        }
    }
#else
    for (i = 0;i < XMTR_NUM;i++)
    {
        if(XmtrOrder[i+1] < 16)
        {
            SPI_write_singleData(TXMAP_REG(i),TXMAP_TX_CH(XmtrOrder[i+1]));
            TX0TO15_ENABLE_FLAG |= (1<<XmtrOrder[i+1]);
            if(1 == bdt.PCBA.TxPolarity) TX0TO15_PLFLAG |= (1<<XmtrOrder[i+1]);
        }
        else if(16 == XmtrOrder[i+1])
        {
            SPI_write_singleData(TXMAPTOTX16ONLY_REG, TXMAPTO16O_Bit(i));
            SPI_write_singleData(TXMAP_REG(i),0);
            TX16_ENABLE_FLAG |= RMP3_TX_EN16;
            if(1 == bdt.PCBA.TxPolarity) TX16_PLFLAG |= RMP3_TX_PL16;
        }
    }
#endif    
    /********************************************************
    * Setup the polority of the XMTR OUTPUT pin
    ******************************************************** */
    SPI_write_singleData(TXEN_REG, TX0TO15_ENABLE_FLAG);       /*  0-11 */
    SPI_write_singleData(TPL1_REG, TX0TO15_PLFLAG); /*  cfg_reg30, 12'h000 */
    SPI_write_singleData(RMP3_REG, TX16_PLFLAG|TX16_ENABLE_FLAG|RMP3_CH_MAP8(SCN_R8)|RMP3_CH_MAP9(SCN_R9));
}



/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
void TC1126_Init_TxMappingRegisters4SLEEP(void)
{
    uint16_t i;
    uint16_t TX0_MAPPING         = 0;
    uint16_t TX1_MAPPING         = 0;
    uint16_t TX0To15_PLFLAG      = 0;
    uint16_t TX16_PLFLAG         = 0;
    uint16_t TX0To15_EN          = 0;
    uint16_t TX16_EN             = 0;

    #if (DOZE_TX_DRIVE_PL == TX_DRIVE_NORM)
    uint32_t Sleep_TxPolarity    = 0; /*  TEMP setting HERE(QFU) */
    #endif

    #if (DOZE_TX_DRIVE_PL == TX_DRIVE_INVS)
    uint32_t Sleep_TxPolarity    = 1; /*  TEMP setting HERE(QFU) */
    #endif

    #if (2 == DOZE_TXREADNUM)
    /***********************************************
    * 1. Enable setting is not changed;
    * 2. Mapping setting should be changed;
    * 3. Polarity should be changed (maybe);
    *********************************************** */
    for (i = 0; i < ((XMTR_NUM>>1)+0); i++)
    {
        if(XmtrOrder[i+1] < 16)
        {
            TX0_MAPPING |= (1<<XmtrOrder[i+1]);
            TX0To15_EN  |= (1<<XmtrOrder[i+1]);
            if(1 == Sleep_TxPolarity)
            {    
                TX0To15_PLFLAG |= (1<<XmtrOrder[i+1]);
            }  
        }
        else if(16 == XmtrOrder[i+1])
        {
            SPI_write_singleData(TXMAPTOTX16ONLY_REG, (1<<0)); /*  SCN16 to T0 */
            TX16_EN  |= RMP3_TX_EN16;
            if(1 == Sleep_TxPolarity) 
            {
                TX16_PLFLAG |= RMP3_TX_PL16;
            }
        }
    }
    SPI_write_singleData(TXMAP0_REG, TX0_MAPPING);           /*  T0 */
    
    for (i = ((XMTR_NUM>>1)+1); i < XMTR_NUM; i++)
    {
        if(XmtrOrder[i+1] < 16)
        {
            TX1_MAPPING |= (1<<XmtrOrder[i+1]);
            TX0To15_EN  |= (1<<XmtrOrder[i+1]);
            if(1 == Sleep_TxPolarity) TX0To15_PLFLAG |= (1<<XmtrOrder[i+1]);
        }
        else if(16 == XmtrOrder[i+1])
        {
            SPI_write_singleData(TXMAPTOTX16ONLY_REG, (1<<1)); /*  SCN16 to T1 */
            TX16_EN  |= RMP3_TX_EN16;
            if(1 == Sleep_TxPolarity) TX16_PLFLAG |= RMP3_TX_PL16;
        }
    }
    SPI_write_singleData(TXMAP1_REG, TX1_MAPPING);           /*  T1 */
    
    SPI_write_singleData(TXEN_REG,   TX0To15_EN);            /*  EN for T0 and T15 */
    SPI_write_singleData(TPL1_REG,   TX0To15_PLFLAG);        /*  PL for T0 and T15 */
    SPI_write_singleData(RMP3_REG,   TX16_PLFLAG|TX16_EN|RMP3_CH_MAP8(SCN_R8)|RMP3_CH_MAP9(SCN_R9));
    SPI_write_singleData(DURV_REG,   DURV_RESET_DUR(bdt.PCBA.DurReset) | DURV_INTEG_DUR(60)); /*  (QFU) It needs more DEBUG */
    #endif
    
    #if (3 == DOZE_TXREADNUM)
    for (i = 0; i < 5; i++)
    {
        if(XmtrOrder[i+1] < 16)
            {
            TX0_MAPPING |= (1<<XmtrOrder[i+1]);
            TX0To15_EN  |= (1<<XmtrOrder[i+1]);
            if(1 == Sleep_TxPolarity)
            {
                TX0To15_PLFLAG |= (1<<XmtrOrder[i+1]);
            }
        }
        else if(16 == XmtrOrder[i+1])
        {
            SPI_write_singleData(TXMAPTOTX16ONLY_REG, (1<<0)); /*  SCN16 to T0 */
            TX16_EN  |= RMP3_TX_EN16;
            if(1 == Sleep_TxPolarity)
            {
                TX16_PLFLAG |= RMP3_TX_PL16;
            }
        }
    }
    SPI_write_singleData(TXMAP0_REG, TX0_MAPPING);           /*  T0 */
    
    for (i = 5; i < 10; i++)
    {
        if(XmtrOrder[i+1] < 16)
        {
            TX1_MAPPING |= (1<<XmtrOrder[i+1]);
            TX0To15_EN  |= (1<<XmtrOrder[i+1]);
            if(1 == Sleep_TxPolarity) 
            {
                TX0To15_PLFLAG |= (1<<XmtrOrder[i+1]);
            }
        }
        else if(16 == XmtrOrder[i+1])
        {
            SPI_write_singleData(TXMAPTOTX16ONLY_REG, (1<<1)); /*  SCN16 to T1 */
            TX16_EN  |= RMP3_TX_EN16;
            if(1 == Sleep_TxPolarity)
            {
                TX16_PLFLAG |= RMP3_TX_PL16;
            }
        }
    }
    SPI_write_singleData(TXMAP1_REG, TX1_MAPPING);           /*  T1 */
    
    for(i = 10; i < XMTR_NUM; i++)
    {
        if(XmtrOrder[i+1] < 16)
        {
            TX0_MAPPING |= (1<<XmtrOrder[i+1]);
            TX0To15_EN  |= (1<<XmtrOrder[i+1]);
            if(1 == Sleep_TxPolarity)
            {
                TX0To15_PLFLAG |= (1<<XmtrOrder[i+1]);
            }
        }
        else if(16 == XmtrOrder[i+1])
        {
            SPI_write_singleData(TXMAPTOTX16ONLY_REG, (1<<2)); /*  SCN16 to T0 */
            TX16_EN  |= RMP3_TX_EN16;
            if(1 == Sleep_TxPolarity)
            {
                TX16_PLFLAG |= RMP3_TX_PL16;
            }
        }
    }
    SPI_write_singleData(TXMAP2_REG, TX0_MAPPING);           /*  repeat to use VAR "TX0_MAPPING" */
    
    SPI_write_singleData(TXEN_REG,   TX0To15_EN);            /*  EN for T0 and T15 */
    SPI_write_singleData(TPL1_REG,   TX0To15_PLFLAG);        /*  PL for T0 and T15 */
    SPI_write_singleData(RMP3_REG,   TX16_PLFLAG|TX16_EN|RMP3_CH_MAP8(SCN_R8)|RMP3_CH_MAP9(SCN_R9));
    SPI_write_singleData(DURV_REG,   DURV_RESET_DUR(bdt.PCBA.DurReset) | DURV_INTEG_DUR(48)); /*  (QFU) It needs more DEBUG */
    #endif
}



/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
void TC1126_Init_RefHLRegWRITE(void)
{
    uint16_t high;
    uint16_t low;
    high  = REFH_REFHI_EN | REFH_REFHI_INP;
    low   = REFL_REFLO_EN | REFL_REFLO_INP;
    high |= REFH_REFHI_TCAP(bdt.PCBA.HighRefSet) | REFH_REFHI_FCAP(bdt.PCBA.HighRefGainSet); /*  Pos1PF; */
    low  |= REFL_REFLO_TCAP(bdt.PCBA.LowRefSet)  | REFL_REFLO_FCAP(bdt.PCBA.LowRefGainSet);  /*  Neg2PF; */
    high |= (bdt.PCBA.HighRefPlSet<<5);
    low  |= (bdt.PCBA.LowRefPlSet<<5);
    
    #ifdef COEF_SCALE_ENABLE
    high |= REFH_SCALE_EN;
    high |= REFH_SCALE_MODE(SCALE_MODE_SELECT);
    #endif
    SPI_write_singleData(REFH_REG, high);
    SPI_write_singleData(REFL_REG, low); 
}


/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
void TC1126_Init_VarRegSetting(void)
{
    TC1126_Init_RefHLRegWRITE();
}



/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
void TC1126_Init_CapHighLowSetting(void)
{
    #if 1
    bdt.PCBA.HighRefSet     = HIREF_SETTING;
    bdt.PCBA.HighRefGainSet = HIREF_GAIN_SET;
    bdt.PCBA.LowRefSet      = LOREF_SETTING;
    bdt.PCBA.LowRefGainSet  = LOREF_GAIN_SET;
    bdt.PCBA.HighRefPlSet   = HIREF_PL_SET;
    bdt.PCBA.LowRefPlSet    = LOREF_PL_SET;
    
    #ifdef SCREEN_FULL_ADAPTIVE
    bdt.PCBA.HighRefGainSet = 0;
    bdt.PCBA.LowRefGainSet  = 0;
    #endif
    #endif
    TC1126_Init_RefHLRegWRITE();
}



/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
void TC1126_Init_CapHighLowSetting4SLEEP(void)
{
    #ifdef SCREEN_FULL_ADAPTIVE
    uint16_t high;
    uint16_t low;
    
    high  = REFH_REFHI_EN | REFH_REFHI_INP;
    low   = REFL_REFLO_EN | REFL_REFLO_INP;
    high |= REFH_REFHI_TCAP(DOZE_HIREF_SETTING) | REFH_REFHI_FCAP(DOZE_HIREF_GAIN_SET); /*  Pos1PF; */
    low  |= REFL_REFLO_TCAP(DOZE_LOREF_SETTING)  | REFL_REFLO_FCAP(DOZE_LOREF_GAIN_SET);  /*  Neg2PF; */
    high |= (bdt.PCBA.HighRefPlSet<<5);
    low  |= (bdt.PCBA.LowRefPlSet<<5);
    #ifdef COEF_SCALE_ENABLE
    high |= REFH_SCALE_EN;
    high |= REFH_SCALE_MODE(SCALE_MODE_SELECT);
    #endif
    SPI_write_singleData(REFH_REG, high);
    SPI_write_singleData(REFL_REG, low); 
    
    #else

    bdt.PCBA.HighRefSet     = DOZE_HIREF_SETTING;
    bdt.PCBA.HighRefGainSet = DOZE_HIREF_GAIN_SET;
    bdt.PCBA.LowRefSet      = DOZE_LOREF_SETTING;
    bdt.PCBA.LowRefGainSet  = DOZE_LOREF_GAIN_SET;
    bdt.PCBA.HighRefPlSet   = DOZE_HIREF_PL_SET;
    bdt.PCBA.LowRefPlSet    = DOZE_LOREF_PL_SET;
    TC1126_Init_RefHLRegWRITE();
    #endif
}




/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
void TC1126_Init_AbsModeSetting(void)
{
    #ifdef COEF_SCALE_ENABLE
    SPI_write_singleData(TFC0_REG, TFC0_T0R0_FCAP_COEF(8) | TFC0_T0R1_FCAP_COEF(8)
                        | TFC0_T0R2_FCAP_COEF(8) | TFC0_T0R3_FCAP_COEF(8));  
    SPI_write_singleData(TFC1_REG, TFC1_T0R4_FCAP_COEF(8) | TFC1_T0R5_FCAP_COEF(8)
                        | TFC1_T0R6_FCAP_COEF(8) | TFC1_T0R7_FCAP_COEF(8));  
    SPI_write_singleData(TFC2_REG, TFC2_T0R8_FCAP_COEF(8) | TFC2_T0R9_FCAP_COEF(8)
                        | TFC2_TNR0_FCAP_COEF(8) | TFC2_SPECIAL_RXI_COORD(10));  
    SPI_write_singleData(DIAG_REG, DIAG_SEPCIAL_TXRXI_FCAP(2/*RCVM_RCVR_FCAP_SET*/) 
                        | DIAG_SEPCIAL_TXI_COORD(14)); 
    SPI_write_singleData(TXMAPTOTX16MORE_REG, TXMAPTO16M_TXJ_CORD(14)
                        | TXMAPTO16M_RXJ_CORD(10) | TXMAPTO16M_TXRXJ_FCAP(2/*RCVM_RCVR_FCAP_SET*/)); 
    #endif
    SPI_write_singleData(FLEN_REG, RECV_NUM*DOZE_TXREADNUM);                 /*  XMTR_NUM * RECV_NUM */
}




/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
void TC1126_Init_TransModeSetting(void)
{
    #ifdef COEF_SCALE_ENABLE
    uint16_t fcapvalue;
    fcapvalue = (bdt.PCBA.RcvmRcvrFcapSet<<2);
    SPI_write_singleData(TFC0_REG, TFC0_T0R0_FCAP_COEF(fcapvalue) | TFC0_T0R1_FCAP_COEF(fcapvalue)
                        | TFC0_T0R2_FCAP_COEF(fcapvalue) | TFC0_T0R3_FCAP_COEF(fcapvalue));  
    SPI_write_singleData(TFC1_REG, TFC1_T0R4_FCAP_COEF(fcapvalue) | TFC1_T0R5_FCAP_COEF(fcapvalue)
                        | TFC1_T0R6_FCAP_COEF(fcapvalue) | TFC1_T0R7_FCAP_COEF(fcapvalue));  
    SPI_write_singleData(TFC2_REG, TFC2_T0R8_FCAP_COEF(fcapvalue) | TFC2_T0R9_FCAP_COEF(fcapvalue)
                        | TFC2_TNR0_FCAP_COEF(fcapvalue) | TFC2_SPECIAL_RXI_COORD(INVALID_CHORPOINT)); 
    
    SPI_write_singleData(DIAG_REG, DIAG_SEPCIAL_TXRXI_FCAP(RCVM_RCVR_FCAP_SET) 
                        | DIAG_SEPCIAL_TXI_COORD(INVALID_CHORPOINT)); 
    SPI_write_singleData(TXMAPTOTX16MORE_REG, TXMAPTO16M_TXJ_CORD(INVALID_CHORPOINT)
                        | TXMAPTO16M_RXJ_CORD(INVALID_CHORPOINT) | TXMAPTO16M_TXRXJ_FCAP(RCVM_RCVR_FCAP_SET)); 
    #endif
    
    #ifdef ONE_MORE_LINE_SCAN
    SPI_write_singleData(FLEN_REG, FLEN_FRAME_LEN(XMTR_NUM*RECV_NUM + RECV_NUM)); /*  FLEN_FRAME_LEN = XMTR_NUM * RECV_NUM  */
    #else
    SPI_write_singleData(FLEN_REG, FLEN_FRAME_LEN(XMTR_NUM*RECV_NUM)); /*  FLEN_FRAME_LEN = XMTR_NUM * RECV_NUM  */
    #endif
}
/*******************************************************************************
* Function Name  : TC1126_ChAdaptive_RefHLRegWRITE
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
void TC1126_ChAdaptive_RefHLRegWRITE(void)
{
    uint16_t high;
    uint16_t low;
    high = (SPI_read_singleData(REFH_REG)&(0xfffc));
    low = (SPI_read_singleData(REFL_REG)&(0xfffc));

    high |= REFH_REFHI_FCAP(bdt.PCBA.HighRefGainSet);
    low  |= REFL_REFLO_FCAP(bdt.PCBA.LowRefGainSet);

    SPI_write_singleData(REFH_REG, high);
    SPI_write_singleData(REFL_REG, low); 
}


#ifdef CHANNEL_ADAPTIVE
/*******************************************************************************
* Function Name  : TC1126_RxChAdaptive_TransModeSetting
* Description    : 往寄存器写入每个Rx通道FCAP的值
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
void TC1126_RxChAdaptive_TransModeSetting(void)
{    
    // 注：此时的TFC0_T0R0_FCAP_COEF要与TFC2_TNR0_FCAP_COEF保持一致
    SPI_write_singleData(TFC0_REG, TFC0_T0R0_FCAP_COEF((bdt.RxFcapValue[0]<<2)) | TFC0_T0R1_FCAP_COEF((bdt.RxFcapValue[1]<<2))
                         | TFC0_T0R2_FCAP_COEF((bdt.RxFcapValue[2]<<2)) | TFC0_T0R3_FCAP_COEF((bdt.RxFcapValue[3]<<2)));  
    SPI_write_singleData(TFC1_REG, TFC1_T0R4_FCAP_COEF((bdt.RxFcapValue[4]<<2)) | TFC1_T0R5_FCAP_COEF((bdt.RxFcapValue[5]<<2))
                         | TFC1_T0R6_FCAP_COEF((bdt.RxFcapValue[6]<<2)) | TFC1_T0R7_FCAP_COEF((bdt.RxFcapValue[7]<<2)));  
    SPI_write_singleData(TFC2_REG, TFC2_T0R8_FCAP_COEF((bdt.RxFcapValue[8]<<2)) | TFC2_T0R9_FCAP_COEF((bdt.RxFcapValue[9]<<2))
                         | TFC2_TNR0_FCAP_COEF((bdt.RxFcapValue[0]<<2)) | TFC2_SPECIAL_RXI_COORD(INVALID_CHORPOINT));
}

/*******************************************************************************
* Function Name  : TC1126_ChAdaptive_TransModeSetting
* Description    : 往寄存器写入需要调节的异常通道或者异常点的FCAP值
* Input          : TxorRxFlag：标记SCALE_MODE类型；
* Output         : 
* Return         : 
*******************************************************************************/
void TC1126_ChAdaptive_TransModeSetting(uint32_t TxorRxFlag)
{
    uint16_t RegValue;
    uint16_t FcapValue;
    uint16_t FcapValue1;

    RegValue = ((SPI_read_singleData(REFH_REG))&(0xcfff));
    RegValue |= REFH_SCALE_EN;                                // 打开COEF_SCALE_ENABLE
    RegValue |= REFH_SCALE_MODE(TxorRxFlag);                // 转换SCALE_MODE
    SPI_write_singleData(REFH_REG, RegValue);
    
    SPI_write_singleData(TFC0_REG, TFC0_T0R0_FCAP_COEF((bdt.RxFcapValue[0]<<2)) | TFC0_T0R1_FCAP_COEF((bdt.RxFcapValue[1]<<2))
                        | TFC0_T0R2_FCAP_COEF((bdt.RxFcapValue[2]<<2)) | TFC0_T0R3_FCAP_COEF((bdt.RxFcapValue[3]<<2)));  
    SPI_write_singleData(TFC1_REG, TFC1_T0R4_FCAP_COEF((bdt.RxFcapValue[4]<<2)) | TFC1_T0R5_FCAP_COEF((bdt.RxFcapValue[5]<<2))
                        | TFC1_T0R6_FCAP_COEF((bdt.RxFcapValue[6]<<2)) | TFC1_T0R7_FCAP_COEF((bdt.RxFcapValue[7]<<2)));
         
    if( TxorRxFlag == SCALE_MODE_2_SPRX)        //SCALE_MODE_2_SPRX
    {
        if((bdt.AbnormalTxChNum == 0)&&(bdt.AbnormalRxChNum == 1))
        {
            FcapValue = bdt.RxAbnormalCh[bdt.RealAbnormalRxCh[0]];
            SPI_write_singleData(TFC2_REG, TFC2_T0R8_FCAP_COEF((bdt.RxFcapValue[8]<<2)) | TFC2_T0R9_FCAP_COEF((bdt.RxFcapValue[9]<<2))
                        | TFC2_TNR0_FCAP_COEF((bdt.RxFcapValue[0]<<2)) | TFC2_SPECIAL_RXI_COORD(INVALID_CHORPOINT)); 
            SPI_write_singleData(DIAG_REG, DIAG_SEPCIAL_TXRXI_FCAP(FcapValue) 
                        | DIAG_SEPCIAL_TXI_COORD(INVALID_CHORPOINT)); 
            SPI_write_singleData(TXMAPTOTX16MORE_REG, TXMAPTO16M_TXJ_CORD(INVALID_CHORPOINT)
                        | TXMAPTO16M_RXJ_CORD(bdt.RealAbnormalRxCh[0]) | TXMAPTO16M_TXRXJ_FCAP(FcapValue));
        }
        else if((bdt.AbnormalTxChNum == 0)&&(bdt.AbnormalRxChNum == 2))
        {
            FcapValue = bdt.RxAbnormalCh[bdt.RealAbnormalRxCh[0]];
            FcapValue1 = bdt.RxAbnormalCh[bdt.RealAbnormalRxCh[1]];
            
            SPI_write_singleData(TFC2_REG, TFC2_T0R8_FCAP_COEF((bdt.RxFcapValue[8]<<2)) | TFC2_T0R9_FCAP_COEF((bdt.RxFcapValue[9]<<2))
                        | TFC2_TNR0_FCAP_COEF((bdt.RxFcapValue[0]<<2)) | TFC2_SPECIAL_RXI_COORD(bdt.RealAbnormalRxCh[0])); 
            
            SPI_write_singleData(DIAG_REG, DIAG_SEPCIAL_TXRXI_FCAP(FcapValue) 
                        | DIAG_SEPCIAL_TXI_COORD(INVALID_CHORPOINT)); 
            SPI_write_singleData(TXMAPTOTX16MORE_REG, TXMAPTO16M_TXJ_CORD(INVALID_CHORPOINT)
                        | TXMAPTO16M_RXJ_CORD(bdt.RealAbnormalRxCh[1]) | TXMAPTO16M_TXRXJ_FCAP(FcapValue1)); 
        }
    }
    else if(TxorRxFlag == SCALE_MODE_2_SPTX)    //SCALE_MODE_2_SPTX
    {
        if((bdt.AbnormalTxChNum == 1)&&(bdt.AbnormalRxChNum == 0))
        {
            FcapValue = bdt.TxAbnormalCh[bdt.RealAbnormalTxCh[0]];
            SPI_write_singleData(DIAG_REG, DIAG_SEPCIAL_TXRXI_FCAP(FcapValue) 
                        | DIAG_SEPCIAL_TXI_COORD(bdt.RealAbnormalTxCh[0]));
            SPI_write_singleData(TXMAPTOTX16MORE_REG, TXMAPTO16M_TXJ_CORD(bdt.RealAbnormalTxCh[0])
                        | TXMAPTO16M_RXJ_CORD(INVALID_CHORPOINT) | TXMAPTO16M_TXRXJ_FCAP(FcapValue));
        }
        else if((bdt.AbnormalTxChNum == 2)&&(bdt.AbnormalRxChNum == 0))
        {
            FcapValue = bdt.TxAbnormalCh[bdt.RealAbnormalTxCh[0]];
            FcapValue1 = bdt.TxAbnormalCh[bdt.RealAbnormalTxCh[1]];
            SPI_write_singleData(DIAG_REG, DIAG_SEPCIAL_TXRXI_FCAP(FcapValue) 
                        | DIAG_SEPCIAL_TXI_COORD(bdt.RealAbnormalTxCh[0])); 
            SPI_write_singleData(TXMAPTOTX16MORE_REG, TXMAPTO16M_TXJ_CORD(bdt.RealAbnormalTxCh[1])
                        | TXMAPTO16M_RXJ_CORD(INVALID_CHORPOINT) | TXMAPTO16M_TXRXJ_FCAP(FcapValue1));
        }
    }
    else if(TxorRxFlag == SCALE_MODE_2_TXRX)    //SCALE_MODE_2_TXRX
    {
        FcapValue = bdt.TxAbnormalCh[bdt.RealAbnormalTxCh[0]];
        
        SPI_write_singleData(TFC2_REG, TFC2_T0R8_FCAP_COEF((bdt.RxFcapValue[8]<<2)) | TFC2_T0R9_FCAP_COEF((bdt.RxFcapValue[9]<<2))
                        | TFC2_TNR0_FCAP_COEF((bdt.RxFcapValue[0]<<2)) | TFC2_SPECIAL_RXI_COORD(bdt.RealAbnormalRxCh[0])); 
        
        SPI_write_singleData(DIAG_REG, DIAG_SEPCIAL_TXRXI_FCAP(FcapValue) 
                        | DIAG_SEPCIAL_TXI_COORD(INVALID_CHORPOINT)); 
        SPI_write_singleData(TXMAPTOTX16MORE_REG, TXMAPTO16M_TXJ_CORD(bdt.RealAbnormalTxCh[0])
                        | TXMAPTO16M_RXJ_CORD(INVALID_CHORPOINT) | TXMAPTO16M_TXRXJ_FCAP(FcapValue));
    }
    if(TxorRxFlag == SCALE_MODE_2_POINTS)    //SCALE_MODE_2_POINTS
    {
        if(bdt.AbnormalPointNum == 1)
        {
            SPI_write_singleData(TFC2_REG, TFC2_T0R8_FCAP_COEF((bdt.RxFcapValue[8]<<2)) | TFC2_T0R9_FCAP_COEF((bdt.RxFcapValue[9]<<2))
                        | TFC2_TNR0_FCAP_COEF((bdt.RxFcapValue[0]<<2)) | TFC2_SPECIAL_RXI_COORD(INVALID_CHORPOINT)); 
            
            SPI_write_singleData(DIAG_REG, DIAG_SEPCIAL_TXRXI_FCAP(bdt.TxRxiFCAP) 
                        | DIAG_SEPCIAL_TXI_COORD(INVALID_CHORPOINT)); 
            SPI_write_singleData(TXMAPTOTX16MORE_REG, TXMAPTO16M_TXJ_CORD(bdt.AbPointTxiCoord)
                        | TXMAPTO16M_RXJ_CORD(bdt.AbPointRxiCoord) | TXMAPTO16M_TXRXJ_FCAP(bdt.TxRxiFCAP));
        }
        else if(bdt.AbnormalPointNum == 2)
        {
            SPI_write_singleData(TFC2_REG, TFC2_T0R8_FCAP_COEF((bdt.RxFcapValue[8]<<2)) | TFC2_T0R9_FCAP_COEF((bdt.RxFcapValue[9]<<2))
                        | TFC2_TNR0_FCAP_COEF((bdt.RxFcapValue[0]<<2)) | TFC2_SPECIAL_RXI_COORD(bdt.AbPointRxiCoord)); 
            
            SPI_write_singleData(DIAG_REG, DIAG_SEPCIAL_TXRXI_FCAP(bdt.TxRxiFCAP) 
                        | DIAG_SEPCIAL_TXI_COORD(bdt.AbPointTxiCoord)); 
            SPI_write_singleData(TXMAPTOTX16MORE_REG, TXMAPTO16M_TXJ_CORD(bdt.AbPointTxjCoord)
                        | TXMAPTO16M_RXJ_CORD(bdt.AbPointRxjCoord) | TXMAPTO16M_TXRXJ_FCAP(bdt.TxRxiFCAP));
        }   
    }
}
#endif


/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/

void TC1126_Init_ADCI_REG0X22(void)
{
    uint16_t temp_reg = 0;

    temp_reg = ADCI_TEMP_ABSO|ADCI_TEMP_BCNT|ADCI_TEMP_REST;

    #ifdef PREQY_CHIRP_ONLY 
    SPI_write_singleData(ADCI_REG, temp_reg|ADCI_STRETCH_END(2));
    #else
    SPI_write_singleData(ADCI_REG, temp_reg);
    #endif
    
}




/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/

void TC1126_Init_FCTL_REG0X3F(void)
{
    uint16_t temp_reg = 0;
    switch(bdt.ModeSelect)
    {
        
        case iAUTOSCAN_MODE:
        {
            temp_reg  = FCTL_SCAN_MODE(2);
            #ifdef ONE_MORE_LINE_SCAN
            temp_reg  |= FCTL_TXCH_NUM(XMTR_NUM+1);
            #else
            temp_reg  |= FCTL_TXCH_NUM(XMTR_NUM);
            #endif
            break;
        }
        case DOZE_MODE:
        case SLEEP_MODE:
        {
            temp_reg  = FCTL_SCAN_MODE(3);                 /*  SET SLEEP_SCAN_MODE */
            temp_reg  |= FCTL_TXCH_NUM(DOZE_TXREADNUM);
            break;
        }
        default:
            break;
    }
    SPI_write_singleData(FCTL_REG, temp_reg);
}




/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/

void TC1126_Init_REVM_REG0X27(void)
{
    switch(bdt.ModeSelect)
    {
        case iAUTOSCAN_MODE:
        {
            SPI_write_singleData(RCVM_REG, RCVM_RJCT_EN |RCVM_RCVR_TURBO_EN1| RCVM_CHAN_RST_EN |RCVM_RCVR_FCAP(bdt.PCBA.RcvmRcvrFcapSet)| RCVM_FILT_BW(3)); 
            break;
        }
        case DOZE_MODE:
        case SLEEP_MODE:
        {
            SPI_write_singleData(RCVM_REG, RCVM_RCVR_ABS_EN|RCVM_CHAN_RST_EN|RCVM_RCVR_FCAP(bdt.PCBA.RcvmRcvrFcapSet));
            break;
        }
        default:
            break;
    }
}




/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
void TC1126_Init_RXEN_Field4Noise(void)
{
    /* uint16_t temp_reg; */
    bdt.REG3E_Value  = RXCN_RX_CH_SIZE(RECV_NUM);
    #ifdef PHASE_HOP_ONLY
    bdt.REG3E_Value |= RXCN_PHASE_HOP | RXCN_PHASE_MODE(2) | RXCN_CLUSTER_SIZE(1);
    #endif
    #ifdef FREQY_HOP_ONLY
    bdt.REG3E_Value |= RXCN_FREQ_HOP(1) | RXCN_CLUSTER_SIZE(2);
    #endif
    #ifdef PREQY_CHIRP_ONLY
    bdt.REG3E_Value |= RXCN_FREQ_CHIRP;
    #endif
    SPI_write_singleData(RXCN_REG, bdt.REG3E_Value); /*  REG3E */
}

/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
void TC1126_Init_iAutoScanModeSetting(void)
{
    TC1126_Init_CapHighLowSetting();
    TC1126_Init_TransModeSetting();
    TC1126_Init_TxMappingRegisters();
    
    /*  Set the Internal SCAN Mode Period */
#ifdef COMMUNICATION_WITH_PC
  #ifdef SHOW_EVERY_FRAME_DATA
    SPI_write_singleData(PERD_REG, 0xFFF);
    SPI_write_singleData(PROB_REG, PROB_INTR_MODE(3)); /*  Choose .... */
  #else
    if(dbg.DebugInfoLevel == DEBUG_INFO_NONE)
        SPI_write_singleData(PERD_REG, ISCANMODE_PERD_REG_VALUE);
    else if(dbg.DebugInfoLevel == DEBUG_INFO_FIGLOC)
        SPI_write_singleData(PERD_REG, ISCANMODE_PERD_REG_VALUE_FINGER_ONLY);
    else
        SPI_write_singleData(PERD_REG, ISCANMODE_PERD_REG_VALUE_NORMAL);
  #endif
#else
    SPI_write_singleData(PERD_REG, ISCANMODE_PERD_REG_VALUE); /*  cfg_reg39 */
#endif
    SPI_write_singleData(PROB_REG, PROB_INTR_MODE(0)); /*  Choose .... */
     
    TC1126_Init_ADCI_REG0X22();
    TC1126_Init_REVM_REG0X27();
    TC1126_Init_FCTL_REG0X3F();
}



/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/

void TC1126_Init_SleepModeSetting(uint32_t SLEEP_TIME)
{
    TC1126_Init_AbsModeSetting();
    TC1126_Init_CapHighLowSetting4SLEEP();
    TC1126_Init_TxMappingRegisters4SLEEP();
    
    /*  Set the Sleeping Period */
    SPI_write_singleData(PERD_REG, (uint16_t)(SLEEP_TIME&0xfff) );       /*  cfg_reg39 */
    SPI_write_singleData(PROB_REG, (uint16_t)((SLEEP_TIME>>12)&0xfff) ); /*  Change the sleeping time */
    
    /*  Setup the working mode as SCAN+SLEEP mode inside of the chip; */
    TC1126_Init_ADCI_REG0X22();
    TC1126_Init_REVM_REG0X27(); /*  SET ABS MODE over here */
    TC1126_Init_FCTL_REG0X3F(); /*  TX NUMBER = ABS TX SCAN NUM */
}



/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/

void TC1126_Init_AllRegisters(void)
{
    uint16_t temp_reg;
    uint32_t temp32;
    
    /*************************************************
    * Reset Slow Clock and Related Module
    ************************************************* */
    SPI_write_singleData(FLAG_REG, FLAG_SCLK_RST);
    
    /*************************************************
    * ADCM_REG is set, which is Useless Here Actually
    * Since we set it again @ Start_ADC_At_Begining
    **************************************************/
    temp_reg  =  ADCM_ADC_SPEED(ADC_SPEED_SET) | ADCM_ACS(ACS_SPEED_SET) | ADCM_ACTV_CONF | ADCM_MB_EN;
    SPI_write_singleData(ADCM_REG, temp_reg);
    
    /**************************************************
    * OSCC_REG is set, which is essential for working
    **************************************************/
    temp_reg  = OSCC_FAST_MODE(0) | OSCC_OSD_TRIM(0x3C) | OSCC_OSD_MODE(0);
    SPI_write_singleData(OSCC_REG, temp_reg); /*  0x3C0,0x580 */
    
    SPI_write_singleData(DURV_REG, DURV_RESET_DUR(bdt.PCBA.DurReset) | DURV_INTEG_DUR(bdt.PCBA.DurInteg)); /*  cfg_reg24, 12'h144 */
    SPI_write_singleData(DURS_REG, DURS_STRETCH_DUR(bdt.PCBA.DurStretch)|DURS_STRETCH_INC(STRETCH_INC_REG25));  /*  cfg_reg25, 12'h000 */
    SPI_write_singleData(BCNT_REG, BCNT_BURST_CNT(bdt.PCBA.BurstCnt)); /*  cfg_reg26, 12'h02f */
    
    /********************************************************
    * Enable or disable the RECV Sensor INPUT pin
    *********************************************************/
    temp32  = (1 << (RECV_STR)) - 1;   /*  RECV_STR, RECV_END */
    temp32  = ~temp32;
    temp32 &= (1 << RECV_END) - 1;   /*  RECV_STR, RECV_END */
    SPI_write_singleData(REN0_REG, 0x3ff); //(uint16_t)(temp32 & 0xfff));        /*  cfg_reg2a, 12'hfff,RX0-RX11 */
    SPI_write_singleData(RMP1_REG, RMP1_CH_MAP0(SCN_R0)|RMP1_CH_MAP1(SCN_R1)|RMP1_CH_MAP2(SCN_R2)|RMP1_CH_MAP3(SCN_R3));
    SPI_write_singleData(RMP2_REG, RMP2_CH_MAP4(SCN_R4)|RMP2_CH_MAP5(SCN_R5)|RMP2_CH_MAP6(SCN_R6)|RMP2_CH_MAP7(SCN_R7));
    SPI_write_singleData(RMP3_REG, RMP3_CH_MAP8(SCN_R8)|RMP3_CH_MAP9(SCN_R9));
    
    temp_reg =   TCRS_RCVR_FCAP_RST | TCRS_RCVR_FILT_RST | TCRS_RCVR_DEMOD_RST
                 | TCRS_RCVR_SNH_RST  | TCRS_RCVR_INPSWT_RST;
    SPI_write_singleData(TCRS_REG, temp_reg); /*  cfg_reg36, 12'hf80  */
   /************************************************************************
   * Delay for a while  After reset something
   ************************************************************************/
    #ifdef CN1100_LINUX
    mdelay(10);
    #else
    Tiny_Delay(100);
    #endif
    
    TC1126_Init_TxMappingRegisters();
    
    /********************************************************
    * Diagnostic and Test Control Register
    ******************************************************** */
    SPI_write_singleData(DIAG_REG, 0x0000); /*  Diagnostic Control; Some RGE_Fields are added  */
    SPI_write_singleData(TCRS_REG, 0x0000); /*  Test Control Reset and Enable */
    SPI_write_singleData(TCEN_REG, 0x0000); /*  Test Control Eanble and Reset */
    
    SPI_write_singleData(PERD_REG, PERD_PERIOD(0x40));                  /*  Period[11:0]; */
    SPI_write_singleData(PROB_REG, PROB_PERIOD(0)+PROB_INTR_MODE(0)); /*  Period[19:12],INTR_MOD[2:0],Frame_Repeat; */
    
    SPI_write_singleData(TTHR_REG, TTHR_TOUCH_TH0(0xb0)); /*  REG3C */
    
    TC1126_Init_RXEN_Field4Noise();
    
    switch(bdt.ModeSelect)
    {
        case iAUTOSCAN_MODE:
        {
            TC1126_Init_iAutoScanModeSetting();
            break;
        }
        case DOZE_MODE:
        {
            TC1126_Init_SleepModeSetting(DOZE_MODE_PERIOD); /*  Doze Mode (Not SLEEP Mode) */
            break;
        }
        case SLEEP_MODE:
        {
            TC1126_Init_SleepModeSetting(0x0);   /*  Sleep Mode (Not DOZE Mode) */
            break;
        }
        default:
            break;
    }
    CN1100_print("D.");
}







/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
void TC1126_iAutoMode_SubISR(void)
{
    if( SPI_read_singleData(DONE_REG) & DONE_FRM0_READABLE )
    {
        /*  CN1100_print("A."); */
        /*  Buffer A is ready in CN1100 */
        bdt.BSDSTS.iBuf_A_Fill = FRAME_FILLED;
        SPI_write_singleData(FLAG_REG, FLAG_FRM0_RDDONE);      /*  Clear the interrupt Bit4(Buffer B Just Filled) */
        #ifdef CN1100_STM32
        Tiny_Delay(1000);
        #endif
    }
    if( SPI_read_singleData(DONE_REG) & DONE_FRM1_READABLE )
    {
        /*  CN1100_print("B."); */
        /*  Buffer B is ready in CN1100 */
        bdt.BSDSTS.iBuf_B_Fill = FRAME_FILLED;
        SPI_write_singleData(FLAG_REG, FLAG_FRM1_RDDONE);      /*  Clear the interrupt Bit4(Buffer B Just Filled) */
        #ifdef CN1100_STM32
        Tiny_Delay(1000);
        #endif
    }
}




/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/

void TC1126_GotoDozeMode(void)
{
    #if defined(DOZE_ALLOWED)
        CN1100_print("==>DOZE_MODE\n");
    #endif
   
    SPI_write_singleData(ADCM_REG, ADCM_ADC_SPEED(ADC_SPEED_SET) | ADCM_ACS(ACS_SPEED_SET) | ADCM_MB_EN);
   
    TC1126_Init_SleepModeSetting(DOZE_MODE_PERIOD);
    /*set PERD_REG small to reduce the time of throwing useless frames*/
    SPI_write_singleData(PERD_REG, 0x200);
    SPI_write_singleData(FLAG_REG, FLAG_FRM0_RDDONE+FLAG_FRM1_RDDONE); // Clear the interrupt Bit3(Buffer A Just Filled)
    bdt.ModeSelect            = DOZE_MODE;
    bdt.MTD.mtdc.Doze_FirstIn = 0;
    bdt.MTD.mtdc.Doze_OddNum  = 0;
   
    #ifdef FINGER_HWDET4DOZE
    SPI_write_singleData(TTHR_REG, (TTHR_TOUCH_DETECT | TTHR_TOUCH_TH0(0x30)) );
    SPI_write_singleData(FLEN_REG, FLEN_FRAME_LEN(2*RECV_NUM) | FLEN_TOUCH_TH1(2) );
    #endif
   
    TC1126_Init_StartADCByReg21();
}


/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/

void TC1126_GotoAutoScanMode(uint16_t auto_mode_sel)
{
    if(iAUTOSCAN_MODE == auto_mode_sel)
    {
        #if defined(DOZE_ALLOWED)
            CN1100_print("==>iAUTOSCAN_MODE\n");
        #endif
            TC1126_Init_iAutoScanModeSetting();
        bdt.ModeSelect           = iAUTOSCAN_MODE;
    } 
    
    #ifdef FINGER_HWDET4DOZE
    SPI_write_singleData(TTHR_REG, (TTHR_TOUCH_TH0(0x30)) );

    #ifdef ONE_MORE_LINE_SCAN
      SPI_write_singleData(FLEN_REG, FLEN_FRAME_LEN(XMTR_NUM*RECV_NUM + RECV_NUM) | FLEN_TOUCH_TH1(2) );
    #else
      SPI_write_singleData(FLEN_REG, FLEN_FRAME_LEN(XMTR_NUM*RECV_NUM) | FLEN_TOUCH_TH1(2) );
    #endif

    #endif
    
    bdt.BSDSTS.iBuf_A_Fill   = FRAME_UNFILLED;
    bdt.BSDSTS.iBuf_B_Fill   = FRAME_UNFILLED;
    bdt.MTD.NoFingerCnt4Doze = 0;
    TC1126_Init_StartADCByReg21();
}



/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/

void TC1126_GotoSleepMode(void)
{
    if(DOZE_MODE == bdt.ModeSelect)
    {
    
      /*****************************************************************************
              * Set the Sleeping Period when previous mode is DOZE mode
              *****************************************************************************/
      
        SPI_write_singleData(PERD_REG, 0); // cfg_reg39
        SPI_write_singleData(PROB_REG, 0); // Change the sleeping time
    }
    else
    {
        TC1126_Init_SleepModeSetting(0);
    }
    bdt.ModeSelect = SLEEP_MODE;
    
    #ifdef FINGER_HWDET4DOZE
    SPI_write_singleData(TTHR_REG, (TTHR_TOUCH_TH0(0x30)) );
    SPI_write_singleData(FLEN_REG, FLEN_FRAME_LEN(2*RECV_NUM) | FLEN_TOUCH_TH1(2) );
    #endif
    SPI_write_singleData(FLAG_REG, 0x001f);
}



/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
#ifdef FINGER_HWDET4DOZE
/*********************************************************
* AutoDetection funcation is enabled @ Doze Mode
*********************************************************/
uint16_t TC1126_DozeModeDataHandling(uint16_t BufferID)
{
    uint16_t i, j, rtnValue = 0;
    int16_t  tempint16, tempMax = 0;
    
    #ifdef CN1100_iSCANMODE
    #define  SkipFrameNUM 2
    #endif
    
    /*********************************************
    * When Interrupt is coming, we skip the first 
    * "SkipFrameNUM" frame, then treat others;
    *********************************************/
    SPI_read_DATAs(0x400, DOZE_TXREADNUM*RECV_NUM, (uint16_t *)(bdt.FrameDatLoadA));
    SPI_read_DATAs(0x400+DOZE_TXREADNUM*RECV_NUM, DOZE_TXREADNUM*RECV_NUM, (uint16_t *)(bdt.FrameDatLoadB));
    if(bdt.MTD.mtdc.Doze_FirstIn<SkipFrameNUM)
    {
        bdt.MTD.mtdc.Doze_FirstIn++;
    }
    else
    {
    
        for(j=0; j<DOZE_TXREADNUM; j++)
        {
            for(i=0; i<RECV_NUM; i++) 
            {
                tempint16 = bdt.FrameDatLoadB[j][i] - bdt.FrameDatLoadA[j][i];
                if(tempint16 < 0) 
                {
                    tempint16 = 0 - tempint16;
                }
                if(tempint16 > tempMax) 
                {
                    tempMax = tempint16;
                }    
            }
        }
    }
    
    if(tempMax > DOZE_MODE_FINGER_THR) 
    {
        #ifdef CN1100_iSCANMODE
            TC1126_GotoAutoScanMode(iAUTOSCAN_MODE);
        #endif
        
        rtnValue = 1;
    }
    
    return rtnValue;

}
#else



/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
/*********************************************************
* AutoDetection funcation is disabled @ Doze Mode
*********************************************************/
uint16_t TC1126_DozeModeDataHandling(uint16_t BufferID)
{
    uint16_t i, j, rtnValue = 0;
    int16_t  tempint16, tempMax = 0;
    #define  SkipFrameNUM 3
    
 /*********************************************
    *Read data from Buffer A
    *********************************************/
    switch(BufferID)
    {
        case BUFFER_A:
        {
            SPI_read_DATAs(0x400, DOZE_TXREADNUM*RECV_NUM, (uint16_t *)(bdt.FrameDatLoadA));
            break;
        }
        case BUFFER_B:
        {
            SPI_read_DATAs(0x400+DOZE_TXREADNUM*RECV_NUM, DOZE_TXREADNUM*RECV_NUM, (uint16_t *)(bdt.FrameDatLoadB));
            break;
        }
    }
    
    #if 1
    if(bdt.MTD.mtdc.Doze_FirstIn<SkipFrameNUM)
    {
        bdt.MTD.mtdc.Doze_FirstIn++;
        if(SkipFrameNUM == bdt.MTD.mtdc.Doze_FirstIn)
        {
            /*set PERD_REG as DOZE_MODE_PERIOD when we will compute delta value */
            SPI_write_singleData(PERD_REG, DOZE_MODE_PERIOD);
        }
    }
    else
    {
        /* CN1100_print("DeltaBuf:\n"); */
        for(j=0; j<DOZE_TXREADNUM; j++)
        {
            for(i=0; i<RECV_NUM; i++) 
            {
                switch(BufferID)
                {
                    case BUFFER_A:
                    {
                    tempint16 = bdt.FrameDatLoadA[j][i] - bdt.FrameDatLoadB[j][i];
                    break;
                    }
                    case BUFFER_B:
                    {
                    tempint16 = bdt.FrameDatLoadB[j][i] - bdt.FrameDatLoadA[j][i];
                    break;
                    }
                }
                if(tempint16 < 0)
                {
                    tempint16 = 0 - tempint16;
                }
                if(tempint16 > tempMax)
                {
                    tempMax = tempint16;
                }

                #if 1
                /* CN1100_print("%4d ", tempint16); */
                #else
                if(BUFFER_A == BufferID)
                {
                    tempint16 = bdt.FrameDatLoadA[j][i];
                }
                else
                {
                    tempint16 = bdt.FrameDatLoadB[j][i];
                }
                CN1100_print("%4x ", tempint16);
                #endif
            }
            /* CN1100_print("\n"); */
        }
        /* CN1100_print("tempMax: %4d\n", tempMax); */
        /* CN1100_print("\n"); */
        
        if(tempMax > DOZE_MODE_FINGER_THR) 
        {
            #ifdef CN1100_iSCANMODE
                TC1126_GotoAutoScanMode(iAUTOSCAN_MODE);
            #endif
            
            rtnValue = 1;
        }
    }
    #endif
    
    return rtnValue;

}
#endif


#ifdef SLEEP_EVENT_SIM
/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
uint16_t Glitch_Protect    = 0;
void CN1100_SLEEP_EVENT_SIM_ISR()
{
    if(0 == Glitch_Protect)
    {
        Glitch_Protect = 1000;
        switch(bdt.ModeSelect)
        {
            case DOZE_MODE:
            {
                CN1100_print("BP-Doze\n");
                #ifdef FINGER_HWDET4DOZE
                STM32_WakeupToucIC();
                #endif
                bdt.Prepare2SleepMode = 1;
                break;
            }
            
            case iAUTOSCAN_MODE:
            {
                CN1100_print("BP-iScan\n");
                bdt.Prepare2SleepMode = 1;
                break;
            }

            case SLEEP_MODE:
            {
                CN1100_print("BP-Sleep\n");
                STM32_WakeupToucIC();
                #ifdef CN1100_iSCANMODE
                    TC1126_GotoAutoScanMode(iAUTOSCAN_MODE);
                #endif
                STM32_ExtiIRQControl(ENABLE);
                CN1100_print("Scan.Ready\n");
                break;
            }
        }
    }
    else
    {
        CN1100_print("Invalid Button Pressed\n");
    }
}
#endif


/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
#ifdef CN1100_LINUX
    enum hrtimer_restart CN1100_SysTick_ISR(struct hrtimer *timer)
#else
void CN1100_SysTick_ISR(void)
#endif
{  

    #ifdef CN1100_STM32
        #ifdef SLEEP_EVENT_SIM
            if(Glitch_Protect>0) 
            {
                Glitch_Protect--;
            }
        #endif
    
        #ifdef STM32VC_LCD
            if(bdt.Button_Glitch_Protect>0)
            {
                bdt.Button_Glitch_Protect--;
            }
        #endif
    #endif
    
    #ifdef CN1100_LINUX
    if(!(spidev->mode & CN1100_IS_DOZE)){
        if(!(spidev->mode & CN1100_IS_SUSPENDED))
            spidev->ticks++;
    }    
    #if defined(CN1100_RESET_ENABLE)
        if((spidev->ticks>5000)){
                printk("%d\n",spidev->ticks);
                spidev->ticks = 0; 
                queue_work(spidev->workqueue,&spidev->reset_work);
        }    
    #endif
    hrtimer_start(&spidev->systic, ktime_set(0, 1000000), HRTIMER_MODE_REL);
    #endif
    
    switch(bdt.ModeSelect)
    {
        case iAUTOSCAN_MODE:
        {
            if(bdt.MTD.NoFingerCnt4Doze > WORK_MODE_NOFING_MAXPERD)
            {
                bdt.MTD.NoFingerCnt4Doze = 0;
                #ifdef DOZE_ALLOWED
                    #ifdef CN1100_STM32
                        TC1126_GotoDozeMode();
                    #else
                        spidev->mode |= CN1100_IS_DOZE;
                    #endif
                #endif
            } 
            break;
        }
        case SLEEP_MODE:
        {
            break;
        }
        case DOZE_MODE:
        {
            break;
        }
    }
    
    #ifdef CN1100_LINUX
        return HRTIMER_NORESTART;
    #endif
}




/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/

#if defined(CN1100_LINUX)&&!defined(CN1100_MTK)
void CN1100_FrameScanDoneInt_ISR(struct work_struct *work)
#else
void CN1100_FrameScanDoneInt_ISR()
#endif
{
    uint16_t DONE_VALUE;
    
    #if defined(CN1100_LINUX) && !defined(SLEEP_EVENT_SIM)
        if(spidev->mode & CN1100_IS_SUSPENDED)
        {
	    #ifndef CN1100_MTK
            enable_irq(spidev->irq);
	    #endif
            return;
        }
    #elif !defined(CN1100_LINUX)
        STM32_LEDx_TURN_ONorOFF(LED2, LED_ON);
    #endif
    
    switch(bdt.ModeSelect)
    {
        case DOZE_MODE:
        {
            #ifdef SLEEP_EVENT_SIM
            if(1 == bdt.Prepare2SleepMode)
            {
                bdt.Prepare2SleepMode = 0;
                TC1126_GotoSleepMode();
                #ifdef CN1100_LINUX
                spidev->mode |= CN1100_IS_SUSPENDED;
                wake_up(&spidev->waitq);
                #ifdef CN1100_S5PV210
                msleep(10);
                #endif
                #endif
            }
            else
            #endif
            {
                bdt.BFD.bbdc.NoFingerCnt4Base = 0;
                bdt.BFD.FingerLeftProtectTime = 0;
                /***************************************************************************
                * set BaseUpdateCase as BASE_FRAME_DISCARD so we can discard the first frame 
                * when come back to normal mode
                ****************************************************************************/
                bdt.BFD.bbdc.BaseUpdateCase   = BASE_FRAME_DISCARD;

                DONE_VALUE = SPI_read_singleData(DONE_REG);
                if( DONE_VALUE & DONE_FRM0_READABLE )
                {
                    /* Buffer A is ready in CN1100*/
                    if(0 == TC1126_DozeModeDataHandling(BUFFER_A))
                    {
                        SPI_write_singleData(FLAG_REG, FLAG_FRM0_RDDONE);
                    }
                    #ifdef CN1100_LINUX
                        spidev->irq_count = 0;
                    #endif
                }
                else 
                {
                    if( DONE_VALUE & DONE_FRM1_READABLE )
                    {
                        /* Buffer B is ready in CN1100*/
                        if(0 == TC1126_DozeModeDataHandling(BUFFER_B))
                        {
                            SPI_write_singleData(FLAG_REG, FLAG_FRM1_RDDONE);
                        }
                        #ifdef CN1100_LINUX
                           spidev->irq_count = 0;
                        #endif
                    }
                }

                #ifdef CN1100_STM32
                Tiny_Delay(2000);
                #endif
                #ifdef CN1100_LINUX
		#ifndef CN1100_MTK
                enable_irq(spidev->irq);
		#endif
                #endif
            }
            break;
        }
        
        case iAUTOSCAN_MODE:
        {
            #ifdef CN1100_LINUX
            spidev->ticks = 0;
            if(spidev->mode & CN1100_IS_DOZE)
            {
                spidev->mode &= ~(CN1100_IS_DOZE);
                spidev->irq_count = 0;
                TC1126_GotoDozeMode();
                msleep(10);
		#ifndef CN1100_MTK
                enable_irq(spidev->irq);
		#endif
                break;
            }
            #endif
            
            #ifdef SLEEP_EVENT_SIM
            if(1 == bdt.Prepare2SleepMode)
            {
                bdt.Prepare2SleepMode = 0;
                TC1126_GotoSleepMode();
                #ifdef CN1100_LINUX
                spidev->mode |= CN1100_IS_SUSPENDED;
                wake_up(&spidev->waitq);
                #ifdef CN1100_S5PV210
                msleep(10);
                #endif
                #endif
            }
            else
            #endif
            {   
                #ifdef SCREEN_SIMPLE_ADAPTIVE
                if(bdt.ScreenAdaptiveFlag)
                {
                    TC1126_Init_RefHLRegWRITE();
                    bdt.ScreenAdaptiveFlag=0;
                }
                #endif
                #ifdef SCREEN_FULL_ADAPTIVE
                // 轮流设置每次的参考电压
                if(bdt.PCBA.RefHLSetCount <= 16)
                {
                    TC1126_Init_RefHLRegWRITE();
                }
                // 最终设置参考电压
                if(bdt.PCBA.RefHLSetEndFlag != 0)
                {
                    TC1126_ChAdaptive_RefHLRegWRITE();
                }
                #endif
                
                #ifdef CHANNEL_ADAPTIVE
                if(bdt.AdjustRxChFlag == 1)
                {
                    TC1126_RxChAdaptive_TransModeSetting();
                }
                if((bdt.FindOkFlag == 1)&&(bdt.AbnormalPointNum != 0))
                {
                    TC1126_ChAdaptive_TransModeSetting(SCALE_MODE_2_POINTS);
                    bdt.FindOkFlag = 0;
                }
                if(bdt.FindOkFlag == 1)
                {
                    if((bdt.AbnormalTxChNum != 0)&&(bdt.AbnormalRxChNum == 0))
                    {
                        TC1126_ChAdaptive_TransModeSetting(SCALE_MODE_2_SPTX);
                    }
                    else if((bdt.AbnormalTxChNum == 0)&&(bdt.AbnormalRxChNum != 0))
                    {
                        TC1126_ChAdaptive_TransModeSetting(SCALE_MODE_2_SPRX);
                    }
                    else if((bdt.AbnormalTxChNum == 1)&&(bdt.AbnormalRxChNum == 1))
                    {
                        TC1126_ChAdaptive_TransModeSetting(SCALE_MODE_2_TXRX);
                    }
                    else
                    {
                        //do nothing
                    }
                bdt.FindOkFlag = 0;
                }
                #endif
           
                TC1126_iAutoMode_SubISR();
         
                
                #ifdef CN1100_LINUX
                if(FRAME_FILLED == bdt.BSDSTS.iBuf_A_Fill)
                {    
                    bd->BufferID = 0;
                    BufferAHandle();
                    /*Clear the interrupt Bit4(Buffer B Just Filled)*/
                }
                
                if(FRAME_FILLED == bdt.BSDSTS.iBuf_B_Fill)
                {   
                    bd->BufferID = 1;
                    BufferBHandle();
                    /*Clear the interrupt Bit4(Buffer B Just Filled)*/
                }
		#ifndef CN1100_MTK
                enable_irq(spidev->irq);
		#endif
                #endif
            }
            break;
        }
        case SLEEP_MODE:
        {
            CN1100_print("s1..");
            /***************************************
            * Just Disable TIMING_EN (bit4)
            ***************************************/
            SPI_write_singleData(ADCM_REG, ADCM_XMTR_STR(XMTR_STRENGTH_SET) | ADCM_ADC_SPEED(ADC_SPEED_SET) | ADCM_ACS(ACS_SPEED_SET) | ADCM_TIMING_EN | ADCM_ACTV_CONF);
            bdt.BFD.bbdc.NoFingerCnt4Base = 0;
            bdt.BFD.FingerLeftProtectTime = 0;

            /***************************************************************************
            * set BaseUpdateCase as BASE_FRAME_DISCARD so we can discard the first frame 
            * when come back to normal mode
            ****************************************************************************/
            bdt.BFD.bbdc.BaseUpdateCase   = BASE_FRAME_DISCARD;
            CN1100_print("s2..");
            
            DONE_VALUE = SPI_read_singleData(DONE_REG);
            CN1100_print("s3:DONE_REG=%x ", DONE_VALUE);
            if( DONE_VALUE & DONE_FRM0_READABLE )
            {
                /*Buffer A is ready in CN1100*/
                SPI_write_singleData(FLAG_REG, FLAG_FRM0_RDDONE); /*Clear the interrupt Bit3(Buffer A Just Filled)*/
            }
            else 
            {
                if( DONE_VALUE & DONE_FRM1_READABLE )
                {
                /* Buffer A is ready in CN1100*/
                SPI_write_singleData(FLAG_REG, FLAG_FRM1_RDDONE); /* Clear the interrupt Bit3(Buffer A Just Filled)*/
                }
            }
            
            CN1100_print("s4..");
            #ifdef CN1100_STM32
            STM32_ExtiIRQControl(DISABLE);
            #endif
            CN1100_print("Sleep ISR Done\n");
            
            #ifdef CN1100_LINUX
            #ifdef CN1100_S5PV210
            msleep(10);
            #endif
            spidev->mode |= CN1100_IS_SUSPENDED;
            wake_up(&spidev->waitq);
            #endif
            break;
        }
        default:
        {
            break;
        }
    }
    
    #ifdef CN1100_STM32
    STM32_LEDx_TURN_ONorOFF(LED2, LED_OFF);
    #endif
}

#endif
