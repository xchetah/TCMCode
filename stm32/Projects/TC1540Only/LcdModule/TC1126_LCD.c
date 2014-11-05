/******************************************************************************
 * ��Ȩ����(C) TRUECOREING
 * DEPARTMENT:
 * MANUAL_PERCENT:
 * �ļ�����: TC1126_LCD.c 
 * �ļ���ʶ:    
 * ����ժҪ:
 * ����˵��:
 * ��ǰ�汾: 
 * ��    ��: 
 * �������:
 * ��ǰ������: 
 *
 * �޸ļ�¼1: ����Ϲ�
 * �޸�����:  2014-09-15
 * �� �� ��:
 * �� �� ��: Wangpc
 * �޸�����: 
 *****************************************************************************/


#ifndef CN1100_SIMDELDA_C
#define  CN1100_SIMDELDA_C

#include "CN1100_common.h"
#ifdef CN1100_STM32
#include "CN1100_STM32.h"
#endif
#ifdef CN1100_LINUX
#include "CN1100_linux.h"
#endif
#include "CN1100_Function.h"

#ifdef STM32VC_LCD

#ifdef LCD_LOGO_INFO_SHOW
#include "logo.h"
#include "inc_name.h"
#include "tel_and_website.h"
#endif

#include "ASC2_1608.h"  /* �ַ��ֿ� */
#include "GB1616.h"  /* 16*16�����ֿ� */
#include "GB3232.h"  /* 32*32���ֿ� */


const int16_t SINDATQ8[24] = {0, 66, 128, 181, 222, 247, 256, 247, 222, 181, 128, 66, 
                              0,-66,-128,-181,-222,-247,-256,-247,-222,-181,-128,-66};
const int16_t COSDATQ8[24] = {256, 247, 222, 181, 128, 66, 0, -66, -128, -181, -222, -247,
                              -256, -247, -222, -181, -128, -66, 0, 66, 128, 181, 222, 247};


/*******************************************************************************
* Function Name  : LCD_RCC_Configuration
* Description    : Configures the different system clocks.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_RCC_Configuration(void)
{
    ErrorStatus HSEStartUpStatus;
    /* RCC system reset(for debug purpose) */
    RCC_DeInit();/* ʱ�ӿ��ƼĴ���ȫ���ָ�Ĭ��ֵ */
    /* Enable HSE */
    RCC_HSEConfig(RCC_HSE_ON);/* �ⲿ����ʱ��Դ������8M���� */
    /* Wait till HSE is ready */
    HSEStartUpStatus = RCC_WaitForHSEStartUp();/* �ȴ��ⲿʱ�Ӿ��� */
    if(SUCCESS == HSEStartUpStatus)/* ���ʱ�������ɹ� */
    {
         /* HCLK = SYSCLK */
         RCC_HCLKConfig(RCC_SYSCLK_Div1);/* ����AHB�豸ʱ��Ϊϵͳʱ��1��Ƶ */
         /* PCLK2 = HCLK */
         RCC_PCLK2Config(RCC_HCLK_Div1);/* ����APB2�豸ʱ��ΪHCLKʱ��1��Ƶ */
         /* PCLK1 = HCLK/2 */
         RCC_PCLK1Config(RCC_HCLK_Div2);/* ����APB1�豸ʱ��ΪHCLKʱ��2��Ƶ */
         /* Flash 2 wait state */
         FLASH_SetLatency(FLASH_Latency_2);/* �趨�ڲ�FLASH�ĵ���ʱ����Ϊ2���� */
         /* Enable Prefetch Buffer */
         FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);/* ʹ��FLASHԤ��ȡ������ */
        /* PLLCLK = 8MHz * 9 = 72 MHz */
         RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);/* ����PLLʱ��Ϊ�ⲿ����ʱ�ӵ�9��Ƶ��8MHz * 9 = 72 MHz */
         /* Enable PLL */
         RCC_PLLCmd(ENABLE);/* ʹ��PLLʱ�� */
         /* Wait till PLL is ready */
         while(RESET == RCC_GetFlagStatus(RCC_FLAG_PLLRDY))/* �ȴ�PLLʱ���������׼������ */
         {
         }
         /* Select PLL as system clock source */
         RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);/* ʹ��PLLʱ����Ϊϵͳʱ��Դ */
         /* Wait till PLL is used as system clock source */
         while(RCC_GetSYSCLKSource() != 0x08)/* ����ϵͳ����ʱ��Դȷ��Ϊ�ⲿ���پ���8M���� */
         {
         }
     }
     /* �豸ʱ�ӿ��� */
     RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOD|RCC_APB2Periph_GPIOE , ENABLE); 
     RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC, ENABLE);/* ��FSMCʱ�� */

}

/*******************************************************************************
* Function Name  : LCD_GPIO_Configuration
* Description    : Configures the different GPIO ports.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_GPIO_Configuration(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
  
    /* Һ����λRESET */
    #ifdef RESET_LCD_PE0
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
    GPIO_Init(GPIOE, &GPIO_InitStructure);
    #endif
  
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOE, &GPIO_InitStructure);
  
    /* ������ƽ� */
    #ifdef  LCD_SYSTEM_20140804
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable,ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    #else
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;   
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    #endif
    /* ��д����RD��WR */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5; 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    /* Һ��ƬѡCS */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;   
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    /* Һ��ָ�����ѡ���RS */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 ;  
    GPIO_Init(GPIOD, &GPIO_InitStructure); 
    /* 16λ�����ߣ�D0-D15 */
    /*  DB0-DB07: PD14,PD15,PD0, PD1, PE7, PE8,PE9,PE10 */
    /*  DB8-DB15: PE11,PE12,PE13,PE14,PE15,PD8,PD9,PD10 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_8 |
                                  GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_14 | 
                                  GPIO_Pin_15;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 |
                                  GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 |
                                  GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_Init(GPIOE, &GPIO_InitStructure); 
  
    /* IO�ڳ�ֵ���������ã�������ܻ�����ʾ */
    /*   GPIO_SetBits(GPIOD, GPIO_Pin_7); CS=1  */
    /*   GPIO_SetBits(GPIOD, GPIO_Pin_4);  RD=1 */
    /*   GPIO_SetBits(GPIOD, GPIO_Pin_5); WR=1 */
    #ifdef  LCD_SYSTEM_20140804
    GPIO_SetBits(GPIOB, GPIO_Pin_4 );/*  ������ */
    #else
    GPIO_SetBits(GPIOC, GPIO_Pin_9 );/*  ������ */
    #endif
}

/*******************************************************************************
* Function Name  : FSMC_LCD_Init
* Description    : FSMC��ʼ��
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void FSMC_LCD_Init(void)
{
    FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
    FSMC_NORSRAMTimingInitTypeDef  FSMC_Timing;
  
    FSMC_Timing.FSMC_AddressSetupTime      = 0x01;   /* ��ַ����ʱ�� */
    FSMC_Timing.FSMC_AddressHoldTime       = 0x02;   /* ��ַ����ʱ�� */
    FSMC_Timing.FSMC_DataSetupTime         = 0x02;   /* ��������ʱ�� */
    FSMC_Timing.FSMC_BusTurnAroundDuration = 0x01;   /* ���߻�תʱ�� */
    FSMC_Timing.FSMC_CLKDivision           = 0x01;   /* ʱ�ӷָ��Ƶ */
    FSMC_Timing.FSMC_DataLatency           = 0x02;   /* ���ݶ�ȡ��ʱ */
    FSMC_Timing.FSMC_AccessMode            = FSMC_AccessMode_D; /* �첽��ַ����ģʽ */
  
    FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM1;  /* Bank1��ַ0x60000000 */
    FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;  /* �رյ�ַ�������߻��ģʽ */
    FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_NOR;  /* ѡ��洢��ģʽ��ѡ����Һ�����������Ƶ�ģʽ */
    FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;  /* ���ݿ��16λ */
    FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;  /* �ر�������ַģʽ���Զ�����ַ */
    FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;  /* ����ģʽ�е�ַ�ߵ�״̬��ƽ */
    FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;  /* �ر�����ģʽ�еİ����� */
    FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;/* ��������ģʽ���ڵȴ�״̬ǰ���ȴ��ź� */
    FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable; /* ʹ��FSMCд���� */
    FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;  /* �ر�����ģʽ�еĵȴ��ź� */
    FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Disable;  /* �ر�ʱ����չģʽ */
    FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;  /* �رղ���д��ģʽ */
    FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &FSMC_Timing; /* ��FSMC_Timing�ṹ���趨��дʱ�� */
    
    FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure);   /* ���ϲ�����ʼ��FSMC */
  
    /* Enable FSMC Bank1_SRAM Bank */
    FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM1, ENABLE); /* ʹ�� */
}
/*******************************************************************************
* Function Name  : LCD_WR_REGn
* Description    : FSMC�Ĵ�����ַ����
* Input          : index:�Ĵ�����ַ
* Output         : None
* Return         : None
*******************************************************************************/
//void LCD_WR(unsigned int index)		     //дָ���Ӻ���
//{												 
//	*(__IO  uint16_t *) (Bank1_LCD_C)= index;//<<8;	     //ָ��д�붨��ĵ�ַ
//}

/*******************************************************************************
* Function Name  : LCD_WR_CMD
* Description    : FSMCдָ���Ĵ���
* Input          : index:�Ĵ�����ַ; val:��д�������
* Output         : None
* Return         : None
*******************************************************************************/
//void LCD_WR_CMD(unsigned int index,unsigned int val) //д���Ĵ����Ӻ���
//{	
//	*(__IO uint16_t *) (Bank1_LCD_C)= index;		 //ָ��д�붨��ĵ�ַ
//	*(__IO uint16_t *) (Bank1_LCD_D)= val;			 //����д�붨��ĵ�ַ
//}

/*******************************************************************************
* Function Name  : LCD_WD
* Description    : FSMCд16λ����
* Input          : val:��д�������
* Output         : None
* Return         : None
*******************************************************************************/
//void    LCD_WD(unsigned int val)				 //д�����Ӻ���
//{   
//	*(__IO uint16_t *) (Bank1_LCD_D)= val;//<<8; 			 //����д�붨��ĵ�ַ
//}

/*******************************************************************************
* Function Name  : lcd_rst
* Description    : TFT ��λ����
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void lcd_rst(void)
{
    #ifdef RESET_LCD_PE0
    GPIO_ResetBits(GPIOE, GPIO_Pin_0);
    Tiny_Delay(0xffff);
    GPIO_SetBits(GPIOE, GPIO_Pin_0 );
    Tiny_Delay(0xffff);
    #endif
    GPIO_ResetBits(GPIOE, GPIO_Pin_1);
    Tiny_Delay(0xffff);
    GPIO_SetBits(GPIOE, GPIO_Pin_1 );
    Tiny_Delay(0xffff);
}

#ifndef LCD_OnePixelIsWorking


/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
void LCD_Init_ILI9806_800x480(void)
{
    //
    //LCD_WR(0xFF); // EXTC Command Set enable register 
    //LCD_WD(0xFF); 
    
    //LCD_WR(0x28);// Display OFF
    LCD_WR(0xFF);//EXTC command Password Set Enable Register
    LCD_WD(0xFF);
    LCD_WD(0x98);
    LCD_WD(0x06);
    
    LCD_WR(0xBA);  // SPI Interface Setting
    LCD_WD(0xE0); // SPI8Color = 0;
    
    LCD_WR(0xBC);  // GIP 1 
    LCD_WD(0x03); 
    LCD_WD(0x0F); 
    LCD_WD(0x63); 
    LCD_WD(0x33); 
    LCD_WD(0x01); 
    LCD_WD(0x01); 
    LCD_WD(0x1B); 
    LCD_WD(0x11); 
    LCD_WD(0x38); 
    LCD_WD(0x73); 
    LCD_WD(0xFF); 
    LCD_WD(0xFF); 
    LCD_WD(0x0A); 
    LCD_WD(0x0A); 
    LCD_WD(0x05); 
    LCD_WD(0x00); 
    LCD_WD(0xFF); 
    LCD_WD(0xE2); 
    LCD_WD(0x01); 
    LCD_WD(0x00); 
    LCD_WD(0xC1); 
    
    
    LCD_WR(0xBD);
    LCD_WD(0x01);
    LCD_WD(0x23);
    LCD_WD(0x45);
    LCD_WD(0x67);
    LCD_WD(0x01);
    LCD_WD(0x23);
    LCD_WD(0x45);
    LCD_WD(0x67);
    
    LCD_WR(0xBE);
    LCD_WD(0x00);
    LCD_WD(0x22);
    LCD_WD(0x27);
    LCD_WD(0x6A);
    LCD_WD(0xBC);
    LCD_WD(0xD8);
    LCD_WD(0x92);
    LCD_WD(0x22);
    LCD_WD(0x22);
    
    LCD_WR(0xC7);// VCOM Cpontrol
    LCD_WD(0x7B);
    
    LCD_WR(0xED);
    LCD_WD(0x7F);
    LCD_WD(0x0F);
    LCD_WD(0x00);
    
    LCD_WR(0xC0);// Power Control 1
    LCD_WD(0x63);
    LCD_WD(0x0B);
    LCD_WD(0x02);
    
    LCD_WR(0xFC);
    LCD_WD(0x08);
    
    LCD_WR(0xDF);
    LCD_WD(0x00);
    LCD_WD(0x00);
    LCD_WD(0x00);
    LCD_WD(0x00);
    LCD_WD(0x00);
    LCD_WD(0x20);
    
    LCD_WR(0xF3);
    LCD_WD(0x74);
    
    LCD_WR(0xB4);// Display Iversion Control
    LCD_WD(0x00);
    LCD_WD(0x00);
    LCD_WD(0x00);
    
    LCD_WR(0xF7);// Resolution Control
    LCD_WD(0x82); // 480(RGB)X800(Gate Line)
    
    LCD_WR(0xB1);// Frame Rate Control
    LCD_WD(0x00);
    LCD_WD(0x12);
    LCD_WD(0x15);
    
    LCD_WR(0xF1);  // Panel Timing Control1
    LCD_WD(0x29); 
    LCD_WD(0x8A); 
    LCD_WD(0x07); 
    
    LCD_WR(0xF2);// Panel Timing Control2
    LCD_WD(0x40);
    LCD_WD(0xd0);
    LCD_WD(0x50);
    LCD_WD(0x28);
    
    LCD_WR(0xC1);// Power Control 2
    LCD_WD(0x17);
    LCD_WD(0x60);
    LCD_WD(0x60);
    LCD_WD(0x20);
    
    LCD_WR(0xB5);// Blank Porch Control
    LCD_WD(0x15);
    LCD_WD(0x13);
    LCD_WD(0x10);
    LCD_WD(0x00);
    
    LCD_WR(0xE0);// Positive Gamma Control
    LCD_WD(0x00);
    LCD_WD(0x16);
    LCD_WD(0x21);
    LCD_WD(0x0F);
    LCD_WD(0x10);
    LCD_WD(0x15);
    LCD_WD(0x07);
    LCD_WD(0x06);
    LCD_WD(0x05);
    LCD_WD(0x09);
    LCD_WD(0x07);
    LCD_WD(0x0F);
    LCD_WD(0x0F);
    LCD_WD(0x35);
    LCD_WD(0x31);
    LCD_WD(0x00);
    
    LCD_WR(0xE1);// Negative Gamma Control
    LCD_WD(0x00);
    LCD_WD(0x16);
    LCD_WD(0x19);
    LCD_WD(0x0C);
    LCD_WD(0x0F);
    LCD_WD(0x10);
    LCD_WD(0x06);
    LCD_WD(0x07);
    LCD_WD(0x04);
    LCD_WD(0x08);
    LCD_WD(0x08);
    LCD_WD(0x0A);
    LCD_WD(0x0B);
    LCD_WD(0x24);
    LCD_WD(0x20);
    LCD_WD(0x00);
    
    
    LCD_WR(0x36); // Memory Access Control (MADCTRL)
    LCD_WD(0x60);  //
    
    LCD_WR(0x35);// Tearing Effect Line ON
    LCD_WD(0x00);
    
    LCD_WR(0x3A);// Interface Pixel Format
    LCD_WD(0x55);
    
    LCD_WR(0x11);// Sleep Out
    Tiny_Delay(1200000);
    LCD_WR(0x29);// Display ON
}

#else
void LCD_Init_ILI9806_800x480(void)//�ڶ��� LCM
{
    LCD_WR(0xFF);
    LCD_WD(0xFF);
    LCD_WD(0x98);
    LCD_WD(0x26);
    
    LCD_WR(0xBA);  // SPI Interface Setting
    LCD_WD(0xE0); // SPI8Color = 0;
    
    LCD_WR(0xBC);
    LCD_WD(0x3D);
    LCD_WD(0x06);
    LCD_WD(0x00);
    LCD_WD(0x00);
    LCD_WD(0x0D);
    LCD_WD(0x0D);
    LCD_WD(0x80);
    LCD_WD(0x04);
    LCD_WD(0x03);
    LCD_WD(0x00);
    LCD_WD(0x00);
    LCD_WD(0x00);
    LCD_WD(0x0D);
    LCD_WD(0x0D);
    LCD_WD(0x50);
    LCD_WD(0xD0);
    LCD_WD(0x00);
    LCD_WD(0x00);
    LCD_WD(0x00);
    LCD_WD(0xC0);
    LCD_WD(0x08);
    LCD_WD(0x00);
    LCD_WD(0x00);
    LCD_WD(0x00);
    LCD_WD(0x00);
    LCD_WD(0x00);
    
    LCD_WR(0xBD);
    LCD_WD(0x01);
    LCD_WD(0x23);
    LCD_WD(0x45);
    LCD_WD(0x67);
    LCD_WD(0x01);
    LCD_WD(0x23);
    LCD_WD(0x45);
    LCD_WD(0x67);
    
    LCD_WR(0xBE);
    LCD_WD(0x13);
    LCD_WD(0x22);
    LCD_WD(0xDD);
    LCD_WD(0xCC);
    LCD_WD(0xBB);
    LCD_WD(0xAA);
    LCD_WD(0x22);
    LCD_WD(0x26);
    LCD_WD(0x72);
    LCD_WD(0xFF);
    LCD_WD(0x22);
    LCD_WD(0xEE);
    LCD_WD(0x22);
    LCD_WD(0x22);
    LCD_WD(0x22);
    LCD_WD(0x22);
    LCD_WD(0x22);
    
    LCD_WR(0xB1);
    LCD_WD(0x00);
    LCD_WD(0x58);
    LCD_WD(0x03);
    
    LCD_WR(0xB4);
    LCD_WD(0x02);
    
    
    LCD_WR(0xC0);
    LCD_WD(0x0A);
    
    LCD_WR(0xC1);
    LCD_WD(0x10);
    LCD_WD(0x78);
    LCD_WD(0x78);
    
    LCD_WR(0xC7);
    LCD_WD(0x59);
    
    LCD_WR(0xD7);
    LCD_WD(0x10);
    LCD_WD(0x84);
    LCD_WD(0xB4);
    LCD_WD(0x83);
    
    LCD_WR(0xED);
    LCD_WD(0x7F);
    LCD_WD(0x0F);
    
    //LCD_WR(0xF7);
    //LCD_WD(0x02);
    
    
    LCD_WR(0xF7);// Resolution Control
    LCD_WD(0x82); // 480(RGB)X800(Gate Line)
    
    LCD_WR(0xF1);  // Panel Timing Control1
    LCD_WD(0x29); 
    LCD_WD(0x8A); 
    LCD_WD(0x07); 
    
    
    
    LCD_WR(0xF2);
    LCD_WD(0x00);
    LCD_WD(0x07);
    LCD_WD(0x09);
    LCD_WD(0x83);
    LCD_WD(0x06);
    LCD_WD(0x06);
    
    LCD_WR(0xFC);
    LCD_WD(0x08);
    
    LCD_WR(0xDF);
    LCD_WD(0x00);
    LCD_WD(0x00);
    LCD_WD(0x00);
    LCD_WD(0x00);
    LCD_WD(0x00);
    LCD_WD(0x20);
    
    LCD_WR(0xF3);
    LCD_WD(0x74);
    
    
    LCD_WR(0xE0);
    LCD_WD(0x00);
    LCD_WD(0x07);
    LCD_WD(0x0F);
    LCD_WD(0x10);
    LCD_WD(0x12);
    LCD_WD(0x1B);
    LCD_WD(0xC9);
    LCD_WD(0x09);
    LCD_WD(0x02);
    LCD_WD(0x06);
    LCD_WD(0x05);
    LCD_WD(0x0D);
    LCD_WD(0x0D);
    LCD_WD(0x33);
    LCD_WD(0x33);
    LCD_WD(0x00);
    
    LCD_WR(0xE1);
    LCD_WD(0x00);
    LCD_WD(0x05);
    LCD_WD(0x0C);
    LCD_WD(0x0E);
    LCD_WD(0x10);
    LCD_WD(0x17);
    LCD_WD(0x78);
    LCD_WD(0x08);
    LCD_WD(0x04);
    LCD_WD(0x07);
    LCD_WD(0x07);
    LCD_WD(0x0B);
    LCD_WD(0x0E);
    LCD_WD(0x2B);
    LCD_WD(0x29);
    LCD_WD(0x00);
    
    //�ָ���������
    LCD_WR(0x36);
    LCD_WD(0x60);
    
    LCD_WR(0x3A);
    LCD_WD(0x55);
    
    LCD_WR(0x35);
    LCD_WD(0x00);
    
    LCD_WR(0x11); //Exit Sleep
    Tiny_Delay(1200000);
    LCD_WR(0x29); // Display On

}
#endif



/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
void SetXYStartAddrEndAddr(uint16_t x1,uint16_t x2,uint16_t y1,uint16_t y2)
{

    LCD_WR(0x2a);
    LCD_WD(x1>>8); // GRAM������ʼ��ַ0
    LCD_WD(x1&0xff); 
    LCD_WD(x2>>8);// GRAM���������ַ480-1
    LCD_WD(x2&0xff);  
    
    LCD_WR(0x2b);
    LCD_WD(y1>>8); // GRAM������ʼ��ַ0
    LCD_WD(y1&0xff); 
    LCD_WD(y2>>8);// GRAM���������ַ 800-1
    LCD_WD(y2&0xff);
    
    LCD_WR(0x2c);

}

/*******************************************************************************
* Function Name  : D8toD16
* Description    : �������ֽ���ϳ�һ����
* Input          : *Dl:��8λ*; Dh:��8λ
* Output         : һ��int��
* Return         : None
*******************************************************************************/
unsigned int D8toD16(unsigned char Dh,unsigned char Dl)       
{
   unsigned int tmp=0;
   tmp=Dh;  //������ֽ�
   tmp<<=8;  //����8λ
   tmp=tmp+Dl;//������ֽ�
   return tmp;  //���غϲ���16λ��
}

/*******************************************************************************
* Function Name  : LCD_fill
* Description    : ȫ�����
* Input          : color:�����ɫ
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_fill(uint16_t color)
{
  unsigned int n=0;
  SetXYStartAddrEndAddr(0, 800-1, 0, 480-1);
  
  while(n<(384000)) // ȫ�����
  {
      LCD_WD(color);
      n++;
  }   
}




/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
void LCD_Init(void)
{

    FSMC_LCD_Init();// ����FSMCҺ���ӿڲ���

    lcd_rst();   // Һ������λ

    LCD_Init_ILI9806_800x480();
    
    LCD_fill(0xf800); // RED


//LCD_PointFill(100,100,0xffff);

//	while(1);




}


/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
void CN1100_LCD_init(void)
{
    /* ϵͳʱ�ӳ�ʼ�� */
    LCD_RCC_Configuration();   
  
    /*GPIO�˿�����*/
    LCD_GPIO_Configuration();
  
    /*Һ����ʼ��*/
    LCD_Init();
}


/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
void LCD_AreaFill(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, unsigned int color)
{
    uint16_t n=0;
    uint16_t temp,xd,yd;
  
    if(x1>x2) 
    {
        temp = x1;
        x1   = x2;
        x2   = temp;
    }
    if(y1>y2) 
    {
       temp = y1;
       y1   = y2;
       y2   = temp;
    }
    xd = x2 - x1 + 1;
    yd = y2 - y1 + 1;
    temp = xd * yd;
  
    SetXYStartAddrEndAddr(x1,x2,y1,y2);
  
      
    //LCD_WR_CMD(0x0050, x1);   // GRAM������ʼ��ַ
    //LCD_WR_CMD(0x0051, x2);   // GRAM���������ַ
    //LCD_WR_CMD(0x0052, y1);   // GRAM������ʼ��ַ
    //LCD_WR_CMD(0x0053, y2);   // GRAM���������ַ	 
    //LCD_WR_CMD(0x0020, x1);	 // ���GRAM�������
    //LCD_WR_CMD(0x0021, y1);	 // ���GRAM�������
     while(n < temp)      // ȫ�����
    {
         LCD_WD(color);
         n++;
    }   
}


/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
void LCD_PointFill(uint16_t x1, uint16_t y1, uint16_t color)
{
  //LCD_WR_CMD(0x0050, x1);   // GRAM������ʼ��ַ
  //LCD_WR_CMD(0x0051, x1);   // GRAM���������ַ
  //LCD_WR_CMD(0x0052, y1);   // GRAM������ʼ��ַ
  //LCD_WR_CMD(0x0053, y1);   // GRAM���������ַ	 
  //LCD_WR_CMD(0x0020, x1);	 // ���GRAM�������
  //LCD_WR_CMD(0x0021, y1);	 // ���GRAM�������
  //LCD_WR(0x0022);		 // ����GRAM������䣬��ַ����
    SetXYStartAddrEndAddr(x1, x1+1, y1, y1+1);
    LCD_WD(color);
    LCD_WD(color);
    LCD_WD(color);
    LCD_WD(color);
}


/*******************************************************************************
* Function Name  : LCD_show_pic
* Description    : ָ��λ����ʾͼƬ
* Input          : postion_x:0~maxX of screen; postion_y:0~maxY of screen;
 				   pic_x:0~sizeX; pic_y:0~sizeY;
	               p: ָ��ͼƬ���ݻ�ַ
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_show_pic(unsigned int postion_x, unsigned int postion_y,unsigned int pic_x, unsigned int pic_y, unsigned char *p)
{
unsigned int num,num2;


    SetXYStartAddrEndAddr(postion_x, postion_x+pic_x-1, postion_y, postion_y+pic_y);
    num2=pic_y*pic_x;// ����������
    for(num=0;num<num2;num++) LCD_WD(D8toD16(*p++,*p++));
    // �������
}


/*******************************************************************************
* Function Name  : DisplayLine
* Description    : ָ��λ����ʾһ����
* Input          : x1/2, 0-239 y1/2: 0-319 (start point and end point)
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_DisplayLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t Color)
{
    int16_t  xi, yi;
    int16_t  xd, yd, ratioQ8;
    int16_t  uxd, uyd;
    
    xd = (int16_t)x2 - (int16_t)x1;
    yd = (int16_t)y2 - (int16_t)y1;
    uxd = (xd>=0)? xd: (0-xd);
    uyd = (yd>=0)? yd: (0-yd);

//    LCD_PointFill(x1, y1, LCD_COLOR_MAGENTA);
//    LCD_PointFill(x1+1, y1-1, LCD_COLOR_MAGENTA);
//    LCD_PointFill(x1-1, y1+1, LCD_COLOR_MAGENTA);
//    LCD_PointFill(x1+1, y1+1, LCD_COLOR_MAGENTA);
//    LCD_PointFill(x1-1, y1-1, LCD_COLOR_MAGENTA);
    if(uxd > uyd)
    {
     /****************************************************************
     * XD is bigger than YD, so we draw the line based on XD
     ****************************************************************/
        if(0 == yd)
        {
            if(x1 >= x2)
            for (xi = x2+1; xi <= x1; xi++)
            {
                LCD_PointFill(xi, y1, Color);
            }
            else
            {
                for (xi = x1+1; xi <= x2; xi++) 
                { 
                    LCD_PointFill(xi, y1, Color);
                }
            }
        }
        else if(xd > 0)
        {
            ratioQ8 = (yd<<8)/xd;
            for (xi = 1; xi <= xd; xi++)
            {
                yi = (ratioQ8*xi+128)>>8;
                LCD_PointFill(x1+xi, y1+yi, Color);
            }
        }
        else
        {
            xd = 0 - xd;
            ratioQ8 = (yd<<8)/xd;
            for (xi = 1; xi <= xd; xi++)
            {
                yi = (ratioQ8*xi+128)>>8;
                LCD_PointFill(x1-xi, y1+yi, Color);
            }
        }
    }
    else
    {
       /****************************************************************
        * YD is bigger than XD, so we draw the line based on YD
        *****************************************************************/
        if(0 == xd)
        {
            if(y1 >= y2)
            for (yi = y2+1; yi <= y1; yi++) 
            {
                LCD_PointFill(x1, yi, Color);
            }
            else
            {    for (yi = y1+1; yi <= y2; yi++) 
                 {
                     LCD_PointFill(x1, yi, Color);
                 }
            }
        }
        else if(yd > 0)
        {
            ratioQ8 = (xd<<8)/yd;
            for (yi = 1; yi <= yd; yi++)
            {
                xi = (ratioQ8*yi+128)>>8;
                LCD_PointFill(x1+xi, y1+yi, Color);
            }
        }
        else
        {
            yd = 0 - yd;
            ratioQ8 = (xd<<8)/yd;
            for (yi = 1; yi <= yd; yi++)
            {
                xi = (ratioQ8*yi+128)>>8;
                LCD_PointFill(x1+xi, y1-yi, Color);
            }
        }
    }
}

/*******************************************************************************
* Function Name  : DisplayGB3232
* Description    : ָ��λ����ʾһ��32*32���֣�һ������ռ128�ֽ���ģ����
* Input          : x:0~7; y:0~10; Code:Ҫ��ʾ�ĺ���
*  fColor:�ַ�ɫ  bColor:����ɫ
* Output         : None
* Return         : None
*******************************************************************************/
void DisplayGB3232(unsigned char postion_x, unsigned char  postion_y, unsigned char Code[2], unsigned int fColor,unsigned int bColor)
{
    unsigned int i,j,k;
 
    SetXYStartAddrEndAddr(postion_x*32, postion_x*32+31, postion_y*32, postion_y*32+31);

    /* д���� */
    for (k=0;k<8;k++)    /* k��ʾ�Խ����ֿ��еĸ�����ѭ����ѯ���� */
    {                             
        if ((codeGB_32[k].Index[0]==Code[0])&&(codeGB_32[k].Index[1]==Code[1]))
        {                 /* �����ǰ�������ֿ��е�������� */
            for (i=0;i<128;i++)         /* ˳���ȡ��ģ�� */
            {
                unsigned char m=codeGB_32[k].Msk[i];             /* ��ȡ��i�� */
                for (j=0;j<8;j++)           /* �Ӹ�λ����λ */
                {
                    if(0x80 == (m&0x80))
                    {
                        LCD_WD(fColor);/* �����ǰλΪ1���ڶ�Ӧ���������ַ�ɫ */
                    }
                    else
                    {
                        LCD_WD(bColor);/* �����ǰλΪ0���ڶ�Ӧ�������뱳��ɫ */
                    }
                    m<<=1;/* ��ģ������һλ׼����һ���ж� */
                } 
            }
        }  
    }
}

/*******************************************************************************
* Function Name  : DisplayGB1616
* Description    : ָ��λ����ʾһ��16*16���֣�һ������ռ64�ֽ���ģ����
* Input          : x:0~15; y:0~20; Code:Ҫ��ʾ�ĺ���
* fColor:�ַ�ɫ  bColor:����ɫ
* Output         : None
* Return         : None
*******************************************************************************/
void DisplayGB1616(unsigned char postion_x, unsigned char  postion_y, unsigned char Code[2], unsigned int fColor,unsigned int bColor)
{
    unsigned int i,j,k;
 
    SetXYStartAddrEndAddr(postion_x*16, postion_x*16+15, postion_y*16, postion_y*16+15);

    /* д���� */
    for (k=0;k<64;k++)  /* k��ʾ�Խ����ֿ��еĸ�����ѭ����ѯ���� */
    { 
        if ((codeGB_16[k].Index[0]==Code[0])&&(codeGB_16[k].Index[1]==Code[1]))
        {   /* �����ǰ�������ֿ��е�������� */
            for (i=0;i<32;i++) /* ˳���ȡ��ģ�� */
            {
                unsigned char m=codeGB_16[k].Msk[i];/* ��ȡ��i�� */
                for (j=0;j<8;j++)  /* �Ӹ�λ����λ */
                {
                    if(0x80 == (m&0x80)) 
                    {
                        LCD_WD(fColor);/* �����ǰλΪ1���ڶ�Ӧ���������ַ�ɫ */
                    }
                    else
                    {
                        LCD_WD(bColor);               /* �����ǰλΪ0���ڶ�Ӧ�������뱳��ɫ */
                    }
                    m<<=1; /* ��ģ������һλ׼����һ���ж� */
                } 
            }
        }  
    }
}

/*******************************************************************************
* Function Name  : DisplayGB1616_Direction
* Description    : ��ָ��λ����ʾһ��������ĺ��֣�һ������ռ64�ֽ���ģ����
* Description    : ָ��λ����ʾһ��16*16���ִ�
* Input          : x:0~15; y:0~20; Code:Ҫ��ʾ�ĺ���
* direction����ʾ����U��ʾ���ϣ�D��ʾ���£�L��ʾ����R��ʾ����
* fColor:�ַ�ɫ  bColor:����ɫ
* Output         : None
* Return         : None
*******************************************************************************/
void DisplayGB1616_Direction(unsigned char postion_x, unsigned char  postion_y, unsigned char direction, unsigned char Code[2], unsigned int fColor,unsigned int bColor)
{
    unsigned int i,j,k;
    /* ����GRAM��������䷽�� */
    if('U' == direction)
    {
        LCD_WR(0x36);
        LCD_WD(0x60);
        SetXYStartAddrEndAddr(postion_x*16, postion_x*16+15, postion_y*16, postion_y*16+15);
    }
    if('D' == direction)
    {
        LCD_WR(0x36);
        LCD_WD(0xE0);
        SetXYStartAddrEndAddr(800-(postion_x*16+15), 800-postion_x*16, postion_y*16, postion_y*16+15);
    }
    if('L' == direction)
    {
        LCD_WR(0x36);
        LCD_WD(0x40);
        SetXYStartAddrEndAddr(postion_y*16, postion_y*16+15, postion_x*16, postion_x*16+15);
        //SetXYStartAddrEndAddr(postion_x*16, (postion_x*16+15), 480-(postion_y*16+15), 480-postion_y*16);
    }
    if('R' == direction)
    {
        LCD_WR(0x36);
        LCD_WD(0x60);
        SetXYStartAddrEndAddr(postion_x*16, postion_x*16+15, postion_y*16, postion_y*16+15);
    }
    
    
    for (k=0;k<64;k++) 
    { 
        if ((codeGB_16[k].Index[0]==Code[0])&&(codeGB_16[k].Index[1]==Code[1]))
        { 
            for (i=0;i<32;i++) 
            {
                unsigned char m=codeGB_16[k].Msk[i];
                for (j=0;j<8;j++) 
                {
                    if(0x80 == (m&0x80))
                    {
                        LCD_WD(fColor);
                    }
                    else
                    {
                        LCD_WD(bColor);
                    }
                    m<<=1;
                } 
            }
        }  
    }
        LCD_WR(0x36);
        LCD_WD(0x60);
}


/*******************************************************************************
* Function Name  : LCD_DisplayChinese
* Description    : ָ��λ����ʾһ��16*16���ִ�
* Input          : x:0~15; y:0~20; *s:Ҫ��ʾ�ĺ��ִ�
* fColor:�ַ�ɫ  bColor:����ɫ
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_DisplayChinese(unsigned char x, unsigned char y, unsigned char *s, unsigned int fColor, unsigned int bColor) 
{
    unsigned char L=0;
    while(*s) 
    {
        DisplayGB1616(x+L,y,(unsigned char*)s,fColor,bColor);
        s+=2;L++;
    }
}

/*******************************************************************************
* Function Name  : TFT_ShowChar
* Description    : ָ��λ����ʾһ��8*16�ַ���һ���ַ�ռ16����ģ����
* Input          : x:0~234; y:0~308; num:Ҫ��ʾ���ַ�
* fColor:�ַ�ɫ  bColor:����ɫ
* Output         : None
* Return         : None
*******************************************************************************/
void TFT_ShowChar(unsigned int postion_x,unsigned int postion_y,unsigned char num,unsigned int fColor, unsigned int bColor)
{       
    unsigned char temp;
    unsigned char pos,t;      

   
    SetXYStartAddrEndAddr(postion_x*8, postion_x*8+7, postion_y*16, postion_y*16+15);

  

    num=num-' ';/* ���㵱ǰ�ַ��Կ��ַ�ƫ�ƺ��ֵ */
    for(pos=0;pos<16;pos++)
    {
        temp=asc2_1608[num][pos];
        for(t=0;t<8;t++)
        {                 
            if((temp&0x80) != 0)
            {
                LCD_WD(fColor);
            }
            else
            { 
                LCD_WD(bColor);/* ��ɫ     */
            } 
            temp<<=1; 
        }
    }
}

/*******************************************************************************
* Function Name  : mypow
* Description    : m^n������m��n�η�
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
unsigned int mypow(unsigned char m,unsigned char n)
{
    u32 result=1;	 
    while(n--)result*=m;    
    return result;
}

/*******************************************************************************
* Function Name  : TFT_ShowNum
* Description    : ��ʾ����
* Input          : x,y:�������;num:��ֵ
* fColor:�ַ�ɫ  bColor:����ɫ
* Output         : None
* Return         : None
*******************************************************************************/
void TFT_ShowNum(unsigned int x,unsigned int y,unsigned long num,uint16_t fColor, uint16_t bColor)
{      
    unsigned long res;   	   
    unsigned char t=0,t1=0;   
    res=num;
    
    if(!num) 
    {
        TFT_ShowChar(x,y,'0',fColor,bColor);/* ���NUM����Ϊ0����ʾ0 */
        for (t=1; t<5; t++) 
        {
            TFT_ShowChar(x+t,y,' ',fColor,bColor);
        }
        return;
    }
    
    while(res)  /* ѭ����10�õ����ֳ���t */
    {
        res/=10;
        t++;
    }
    t1=t;
    while(t)   /* ��ʾ���� */
    {
        res=mypow(10,t-1);/* 10��t-1�η� */
        TFT_ShowChar(x+(t1-t)*1,y,(num/res)%10+'0',fColor,bColor);	 /* �������ݵ�tλ����ֵ����ʾ */
        t--;
    }
    for (t=t1; t<5; t++) 
    {
        TFT_ShowChar(x+t,y,' ',fColor,bColor);
    }
}

/*******************************************************************************
* Function Name  : TFT_ShowString
* Description    : ��ʾ�ַ���
* Input          : x,y:�������; *p:�ַ�����ʼ��ַ
* fColor:�ַ�ɫ  bColor:����ɫ
* Output         : None
* Return         : None
*******************************************************************************/
void TFT_ShowString(unsigned char x,unsigned int y,const unsigned char *p,unsigned int fColor, unsigned int bColor)
{         
    while(*p!='\0')
    {       
        if(x>30){x=0;y++;}
        if(y>20){y=x=0;LCD_fill(0xffff);}
        TFT_ShowChar(x,y,*p,fColor,bColor);
        x++;
        p++;
    }  
}

/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
void Draw_EndlessCircle(void)
{
    int16_t x1, x2, y1, y2;
    int16_t index, r = 10;
    
    x1 = 120 + ((r * SINDATQ8[0])>>8);
    y1 = 160 + ((r * COSDATQ8[0])>>8);
    
    for (r = 11; r<=100; r++)
    {
        index = (r - 10)%24;
        x2    = 120 + ((128 + r * SINDATQ8[index])>>8);
        y2    = 160 + ((128 + r * COSDATQ8[index])>>8);
        LCD_DisplayLine(x1, y1, x2, y2, 0x001f);
        x1    = x2;
        y1    = y2;
    }

}

/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
void CN1100_LCD_test(void)
{
    TFT_ShowChar(1,0,'A',0xf800,0xffff);  /* ���ַ���ʾ */
    TFT_ShowNum(1,1,12345,0x001f,0xffff);/* ���ִ���ʾ,��֧��С�����ַ���� */
    
    TFT_ShowString(1,2,"DQ.elc",0x07e0,0xffff);/* �ַ�����ʾ */
    TFT_ShowString(1,3,"HTTP://dqelc.taobao.com",0x0000,0xffff);
    
    DisplayGB1616(1,4,"��",0x0000,0xffff);   /* ��������ʾ��16*16�� */
    DisplayGB1616(2,4,"��",0x0000,0xffff);
    DisplayGB1616(3,4,"��",0x0000,0xffff);
    DisplayGB1616(4,4,"ʾ",0x0000,0xffff);
    
    
    LCD_DisplayChinese(1, 5, "����", 0x0000,0xffff);/* ���ִ���ʾ����֧����Ӣ��� */
    TFT_ShowString(6,5,"DQ",0x0000,0xffff);/* �ַ�����ʾ */
    LCD_DisplayChinese(4, 5, "����", 0x0000,0xffff);
    
    /* �ַ���Ҫ��ķ�����ʾ��U��ʾ���ϣ�D��ʾ���£�L��ʾ����R��ʾ���� */
    DisplayGB1616_Direction(1,6,'U',"��",0x0000,0xffff);/* �ַ�����ѡ����ʾ */
    DisplayGB1616_Direction(2,6,'D',"��",0x0000,0xffff);
    DisplayGB1616_Direction(3,6,'L',"��",0x0000,0xffff);
    DisplayGB1616_Direction(4,6,'R',"��",0x0000,0xffff);
    
    DisplayGB3232(1, 4, "��", 0x0000,0xffff);/* ������ʾ��32*32 */
    DisplayGB3232(2, 4, "��", 0x0000,0xffff);
    DisplayGB3232(3, 4, "��", 0x0000,0xffff);
    
    /* ��ʾͼƬ */
    /* LCD_pic(60,180,120,100);  */
    
    /*Display a Line in the screen*/ 
    LCD_DisplayLine(0, 0, 799, 479, 0x001f);
    /* LCD_DisplayLine(0, 319, 239, 0, 0x001f); */
    LCD_DisplayLine(799, 0, 0, 479, 0x001f);
    
    LCD_AreaFill(10, 290, 230, 300, 0x001f);
    
    Draw_EndlessCircle();
    
    
      
}


/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
void CleanTheScreen(void)
{
    /*clean the screen by fill green*/
    LCD_fill(0x7e0);  /*  GREEN */
    Stupid_Delay(5);
}


/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
uint16_t LCD_GetNumLength(uint16_t num)
{
    if(num >= 1000)
    {
        return 4;
    }
    else if(num >= 100)
    {
        return 3;
    }
    else if(num >= 10)
    {
        return 2;
    }
    else
    {
        return 1;
    }
}


/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
void LCD_Show_Coordinate(void)
{
    uint16_t i;
    uint16_t posX = 0;
    
    uint32_t fcolor = 0xf800;
    uint32_t bcolor = LCD_COLOR_GREEN;
    uint32_t posY   = 0;
    
    for (i = 0;i < FINGER_NUM_MAX;i++)
    {
        if(bdt.DPD[i].Finger_X_LCD || bdt.DPD[i].Finger_Y_LCD)
        {
            TFT_ShowChar(posX,posY,'(',fcolor,bcolor);
            posX++;
            TFT_ShowNum(posX,posY,bdt.DPD[i].Finger_X_LCD,fcolor,bcolor); 
            posX += LCD_GetNumLength(bdt.DPD[i].Finger_X_LCD);
            TFT_ShowChar(posX,posY,',',fcolor,bcolor);
            posX++;
            TFT_ShowNum(posX,posY,bdt.DPD[i].Finger_Y_LCD,fcolor,bcolor); 
            posX += LCD_GetNumLength(bdt.DPD[i].Finger_Y_LCD);
            TFT_ShowChar(posX,posY,')',fcolor,bcolor);
            posX++;
            TFT_ShowChar(posX++,posY,' ',fcolor,bcolor);
            posX++;
        }
    }
    
    while(posX < 60)
    {
        TFT_ShowChar(posX++,posY,' ',fcolor,bcolor);
    }
}


/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
void LCD_Show_Data(uint16_t *buffer)
{
    uint16_t i, j, temp, k;
    int16_t *bufs, *bufd;
    uint16_t strp, step, XP, YP;
    uint32_t fcolor = LCD_COLOR_MAGENTA;
    uint32_t bcolor = LCD_COLOR_GREEN;
    int16_t val,sign;
    
    if(DEBUG_INFO_NONE == bdt.LcdShowDataType) 
    {
        return;
    }
    
    bufd = (int16_t *)bdt.LcdShowData;
    step = (800>>3) / XMTR_NUM;
    strp = (100 - step * XMTR_NUM)>>1;
	
    /*save buffer every XMTR_NUM frames one time*/
    if(0 == bdt.LcdShowDataIndex)
    {
    
        if(DEBUG_INFO_DELTA == bdt.LcdShowDataType)
        {
            bufs = (int16_t *)(bdt.DeltaDat16A);
        }
        else if(DEBUG_INFO_DBG == bdt.LcdShowDataType)   
        {   
            bufs = (int16_t *)(bdt.SigDeltaDat);
        }
        else if(DEBUG_INFO_BASE == bdt.LcdShowDataType)
        {   
            bufs = (int16_t *)(bdt.BFD.BaseDat);
        }
        else if(DEBUG_INFO_ORG == bdt.LcdShowDataType)
        { 
            bufs = (int16_t *)buffer;
        }
        for (i = 0;i < XMTR_NUM;i++)
        for (j = 0;j < RECV_NUM;j++)
        {
            bufd[i*RECV_NUM+j] = bufs[i*RECV_NUM+j];
        }
    }
    
    i  = bdt.LcdShowDataIndex;
    XP = strp + step * i;
    for (j = 0; j < RECV_NUM; j++)
    {
        if(DEBUG_INFO_DELTA == bdt.LcdShowDataType)
        {
            val  = bufd[i*RECV_NUM+j];
            sign = 0;
            if(val < 0) 
            {
                val = 0 - val;
                sign = 1;
            }
            temp = LCD_GetNumLength(val);
            YP = (j<<1) + 4;
            for (k = 0; k <(4-temp); k++)
            {
                TFT_ShowChar(XP + k, YP, ' ', fcolor, bcolor);
            }
            if(1 == sign)
            {
                TFT_ShowChar(XP + k, YP, '-', fcolor, bcolor);
            }
            else
            {
                TFT_ShowChar(XP + k, YP, ' ', fcolor, bcolor);
            }
            TFT_ShowNum(XP + k+1, YP, val, fcolor, bcolor); 
        }
        else
        {
        temp = LCD_GetNumLength(bufd[i*RECV_NUM+j]);
        
        YP = (j<<1) + 4;
        for(k = 0; k <(5-temp); k++)
        {
        TFT_ShowChar(XP + k, YP, ' ', fcolor, bcolor);
        }
        TFT_ShowNum(XP + k, YP, bufd[i*RECV_NUM+j], fcolor, bcolor); 
        }
    }
    
    bdt.LcdShowDataIndex++;
    if(XMTR_NUM == bdt.LcdShowDataIndex)
    {
    bdt.LcdShowDataIndex = 0;
    }
}


/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
void LCD_Show_Line(void)
{
    uint16_t i,X[FINGER_NUM_MAX],Y[FINGER_NUM_MAX];
    uint16_t cnt = 0;
    unsigned int LineColor[FINGER_NUM_MAX] = {LCD_COLOR_RED, LCD_COLOR_BLUE, LCD_COLOR_BLACK, LCD_COLOR_YELLOW, LCD_COLOR_GREY};

    if(0 == bdt.LcdIsrFlag)
    for (i=0; i<FINGER_NUM_MAX; i++)
    {
        X[i]  = bdt.DPD[i].Finger_X_Reported; /*  X -> XTMR (800) */
        Y[i]  = bdt.DPD[i].Finger_Y_Reported; /*  Y -> RECV (480) */
        
        /*get the coordinate in the screen*/
        X[i]  = (uint16_t)(( ((uint32_t)X[i]) * XMTR_SCALE )>>16);
        Y[i]  = (uint16_t)(( ((uint32_t)Y[i]) * RECV_SCALE )>>16);
        
        if(X[i] || Y[i])
        {
            if(SCREEN_HIGH  <= X[i])
            {   
                X[i] = SCREEN_HIGH - 1;
            }
            if(SCREEN_WIDTH <= Y[i])
            {
                Y[i] = SCREEN_WIDTH - 1;
            }
            cnt++;
            if(bdt.DPD[i].Finger_X_LCD || bdt.DPD[i].Finger_Y_LCD)
            {
                if(bdt.DPD[i].FingerStateFlag == STATE_NONE_FINGER)
                LCD_DisplayLine(bdt.DPD[i].Finger_X_LCD, bdt.DPD[i].Finger_Y_LCD, X[i], Y[i], LCD_COLOR_WHITE);
                else if(bdt.DPD[i].FingerStateFlag == STATE_LOST_2_FINGER)
                LCD_DisplayLine(bdt.DPD[i].Finger_X_LCD, bdt.DPD[i].Finger_Y_LCD, X[i], Y[i], LCD_COLOR_MAGENTA);
                else
                LCD_DisplayLine(bdt.DPD[i].Finger_X_LCD, bdt.DPD[i].Finger_Y_LCD, X[i], Y[i], LineColor[i]);
            }
        }
        
        /*save the coordinate*/
        bdt.DPD[i].Finger_X_LCD = X[i];
        bdt.DPD[i].Finger_Y_LCD = Y[i];
        
    }
    
}



/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
void LCD_Show_STM32(uint16_t *buffer)
{
    LCD_Show_Line();
    
    if(1 == bdt.LcdIsrFlag)
    {
        CleanTheScreen();
        bdt.LcdIsrFlag = 0;
        TC1126_Init_HardwareRegs();
        bdt.BFD.InitCount = 0;
    }
    else
    {
        LCD_Show_Coordinate();
        LCD_Show_Data(buffer);
    }

}

/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
void TC1126_Key1EventModeSelect_ISR(void)
{
    if(0 == bdt.Button_Glitch_Protect)
    {
        bdt.Button_Glitch_Protect = 500;
        bdt.LcdIsrFlag = 1;
        bdt.LcdShowDataIndex = 0;
    }
}

/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
void TC1126_Key2EventModeSelect_ISR(void)
{
    if(bdt.Button_Glitch_Protect==0)
    {
        bdt.Button_Glitch_Protect = 500;
        bdt.LcdIsrFlag = 1;
        bdt.LcdShowDataIndex = 0;
        bdt.LcdShowDataType += 1;
        if(DEBUG_INFO_DBG == bdt.LcdShowDataType)
        {
            bdt.LcdShowDataType = DEBUG_INFO_NONE;
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
void LCD_Show_Inc_info(void)
{
    #ifdef LCD_LOGO_INFO_SHOW
    uint8_t  *p;
    uint16_t i;
    
    /*show logo*/
    p = (uint8_t*)gImage_logo;
    LCD_show_pic(30, 100, 123,98, p);
    /*show information*/
    p = (uint8_t*)gImage_inc_name;
    LCD_show_pic(153, 100, 609,98, p);
    p = (uint8_t*)gImage_tel_and_website;
    LCD_show_pic(220, 350, 314,53, p);
    
    for (i=0;i<2500;i++) 
    {
        Tiny_Delay(10000);
    }
    #endif
    LCD_fill(0x7e0);  /*  GREEN */
}

#endif

#endif

