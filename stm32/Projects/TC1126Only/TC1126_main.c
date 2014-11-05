/******************************************************************************
 * 版权所有(C) TRUECOREING
 * DEPARTMENT:
 * MANUAL_PERCENT:
 * 文件名称: CN1100_main.c 
 * 文件标识:    
 * 内容摘要: main program body.
 * 其它说明:
 * 当前版本: V1.0.0
 * 作    者: Qiuliang Fu
 * 完成日期:
 * 当前责任人: 
 *
 * 修改记录1: 代码合规
 * 修改日期: 2014-09-15
 * 版 本 号: 
 * 修 改 人: Wangpc 
 * 修改内容: 
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

