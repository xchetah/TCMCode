/******************************************************************************
 * ��Ȩ����(C) TRUECOREING
 * DEPARTMENT:
 * MANUAL_PERCENT:
 * �ļ�����: CN1100_main.c 
 * �ļ���ʶ:    
 * ����ժҪ: main program body.
 * ����˵��:
 * ��ǰ�汾: V1.0.0
 * ��    ��: Qiuliang Fu
 * �������:
 * ��ǰ������: 
 *
 * �޸ļ�¼1: ����Ϲ�
 * �޸�����: 2014-09-15
 * �� �� ��: 
 * �� �� ��: Wangpc 
 * �޸�����: 
 *****************************************************************************/


#ifndef CN1100_MAIN_C
#define  CN1100_MAIN_C

#include "CN1100_common.h"
#ifdef CN1100_STM32
#include "CN1100_STM32.h"
#endif
#ifdef CN1100_LINUX
#include "CN1100_linux.h"
#endif
#include "CN1100_Function.h"


#ifdef CN1100_STM32
int main(void)
{
    #ifdef STM32VC_LCD
    CN1100_LCD_init();
    LCD_Show_Inc_info();
    #endif

    /***************************************************************
    * CN1100 Initialization
    ***************************************************************/
    TC1126_Init_GlobalVariables();
    TC1126_Init_HardwareRegs();

    do
    {
	    switch(bdt.ModeSelect)
	    {
	        case iAUTOSCAN_MODE:
	        {
	            if(FRAME_FILLED == bdt.BSDSTS.iBuf_A_Fill) 
	            {
	                BufferAHandle();
	            }
	            if(FRAME_FILLED == bdt.BSDSTS.iBuf_B_Fill) 
	            {
	                BufferBHandle();
	            }
	           
	            #ifdef COMMUNICATION_WITH_PC
	            if(USART_EXECUTED_CMD == dbg.validCommand)
	            {
	                USART_COMM_Handle();
	                dbg.validCommand = USART_RECEIVED_CMD;
	            }
	            #endif
	            break;
	         }
	    }
    } while(1);
}
#endif /*If not linux*/
#endif

