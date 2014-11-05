/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
* File Name          : usb_endp.c
* Author             : MCD Application Team
* Version            : V2.2.1
* Date               : 09/22/2008
* Description        : Endpoint routines
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "CN1100_common.h"
#include "CN1100_STM32.h"
#include "usb_lib.h"
#include "usb_istr.h"
#include "CN1100_Function.h"


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* Function Name  : EP1_OUT_Callback.
* Description    : EP1 OUT Callback Routine.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
#ifdef USB_COMMUNICATION
void EP1_OUT_Callback(void)
{
     dbg.USB_ReceiveFlg = TRUE;
     PMAToUserBufferCopy(dbg.ReceiveBuf, ENDP1_RXADDR,USB_REPORT_LEN);	
     SetEPRxStatus(ENDP1, EP_RX_VALID);	 
	  Usb_IRQHandler();
}



void EP2_IN_Callback(void)
{
     u8 ii;
     for (ii=0;ii<USB_REPORT_LEN;ii++) dbg.Transi_Buffer[ii] = 0x00;
     //for LED test
     //if (GPIOA->ODR & 0x0c )  GPIOA->ODR &= (~0x0c);
     //else GPIOA->ODR |= 0x0c;
}

#endif
/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/

