#ifndef CN1000_COMMON_H
#define  CN1000_COMMON_H

#define UINT8               uint8_t   // Unsigned Char
#define UINT16              uint16_t  // Unsigned Short
#define UINT32              uint32_t  // Unsigned Int

#define BE20PERCENT         1         // Threshold for Valid Finger Signal
#define BE25PERCENT         2         // Threshold for Valid Finger Signal
#define BE30PERCENT         3         // Threshold for Valid Finger Signal
#define BE40PERCENT         4         // Threshold for Valid Finger Signal

#define TRINGLE_LEFT        0
#define TRINGLE_RIGHT       1

#include "TC1540_reg.h"

//#define TC1126              // The First SPEC MPW
//#define TC1126A             // The First MASK, TX/RX mapping is added etc.

#define TC1540
//#define TC6336_CHIP  // FocalTech
#define TC6206_CHIP  // FocalTech
//#define TC2133_CHIP  // Mstar
//#define TC2138_CHIP  // Mstar
//#define TC3670_CHIP  // Silead
//#define TC2681_CHIP  // Silead

#define FUNCATION_ENABLE  1
#define FUNCATION_DISABLE 0

#ifdef TC6336_CHIP
#include "TC6336_Spec.h"
#endif

#ifdef TC6206_CHIP
#include "TC6206_Spec.h"
#endif

#ifdef TC2133_CHIP
#include "TC2133_Spec.h"
#endif

#ifdef TC2138_CHIP
#include "TC2138_Spec.h"
#endif

#ifdef TC3670_CHIP
#include "TC3670_Spec.h"
#endif

#ifdef TC2681_CHIP
#include "TC2681_Spec.h"
#endif

/************************************************************************
*******1111111111111111111111111111111111111111111111111111111111********
*******1111111111111111111111111111111111111111111111111111111111********
*************************************************************************
*
*
*      Switch Setting (it is a parameter for sure)
*      (系统功能的有关开关)
*
*
*************************************************************************
*******1111111111111111111111111111111111111111111111111111111111********
*******1111111111111111111111111111111111111111111111111111111111********
************************************************************************/

#define CN1100_STM32
//#define CN1100_LINUX

/**********************************************************************/
/**** 1.1    *********  CN1100_LINUX SETTING  *************************/
/*                      IRQ, RESET Pin Location                       */
/**********************************************************************/
#ifdef CN1100_LINUX

  #ifdef CN1100_RK3026
  #define CN1100_INT_PIN          RK30_PIN1_PB0
  #define CN1100_RESET_PIN        RK30_PIN3_PC1 
  #define CN1100_RK3026_1024X600
  #endif

  #ifdef CN1100_ATM
  #define CN1100_RESET_PIN        ASOC_GPIO_PORTB(3)
  #endif

  #ifdef CN1100_S5PV210
  #define CN1100_INT_PIN          S5PV210_GPH1(6)
  #define CN1100_RESET_PIN        S5PV210_GPD0(3)
  #endif

  #define CN1100_print(fmt, ...) printk(fmt,##__VA_ARGS__)
#endif

/**********************************************************************/
/**** 1.2    *********  CN1100_STM32 SETTING  *************************/
/*                      USB, UART, LCD                                */
/**********************************************************************/
#ifdef CN1100_STM32
  #define CN1100_print(fmt, ...) printf(fmt,##__VA_ARGS__)
  #define inline __INLINE

  #define COMMUNICATION_WITH_PC
  #ifdef COMMUNICATION_WITH_PC
    //#define USB_COMMUNICATION
    #define SHOW_EVERY_FRAME_DATA
    #define REG_MEM_16BITS_ADDR
  #endif

  //#define LCD_LOGO_INFO_SHOW
  //#define LCD_LINE_ENDPOINT_SHOW
  //#define RESET_LCD_PE0              // BEEP now
  #define LCD_OnePixelIsWorking        // LCD Screen Selection
  #define LCD_SYSTEM_20140804          // PCB Type Selection
#endif

/**********************************************************************/
/**** 1.3    *********   System Working Mode  *************************/
/**********************************************************************/
//#define CN1100_SLEEPMODE
#define CN1100_iSCANMODE
//#define CN1100_DOZEMODE
//#define SLEEP_EVENT_SIM    // Simulate the Sleep Command with STM32's GPIO
//#define DOZE_ALLOWED       // Allow to Doze mode if finger unexist for long time
//#define FINGER_HWDET4DOZE    // @ Doze, enable Finger auto detection
//#define SHUT_DOWN_TEST     // Working for TC1126A only


/**********************************************************************/
/**** 1.4    *********  SCALE ENABLE SETTING  *************************/
/**********************************************************************/
//#define COEF_SCALE_ENABLE
#define SCALE_MODE_SELECT   SCALE_MODE_2_SPTX

/**********************************************************************/
/**** 1.5    *********    Anti-Noise  Setting *************************/
/**********************************************************************/
//#define PHASE_HOP_ONLY        // Hardware Switch
//#define FREQY_HOP_ONLY        // Hardware Switch
//#define PREQY_CHIRP_ONLY      // Hardware Switch
//#define DISBALE_HWFH_INSWFH   // Hardware Switch

//#define FREQHOP_BYSTRETCH     // Search a goof TX Freq by Stretch Setting

/**********************************************************************/
/**** 1.6    *********  Feature and Funcation *************************/
/**********************************************************************/
//#define ONE_MORE_LINE_SCAN           // Scan 2 times just for the first TX in screen
#define RAWPOINT4FINGMATCH           // Finger Match is operated with RAW Poing Data, not from the Output of MultiFilters
#define FOURPOINTS4STICKMOVING       // Four Ponts is needed for Stick Finger moving
//#define SCREEN_ADAPTIVE              // Adaptive parameter for different screen

//#define AUTO_FACEDETECTION           // For CELLPHONE Talking

//#define SUPER_FILTER4EDGE
//#define FROMOUT2IN_INORDER
//#define SUPFIL_RANGE          256
//#define BORDER_ADJUSTABLE          // Try to use general method to do edge mapping, which is fail actually, we are not going to use it
//#define SHOW_H_DATA                // Test and show h value when we set the mapping array of h2H

/**********************************************************************/
/**** 1.7    *********  I2C Data Format & Set *************************/
/**********************************************************************/
#define COMM_I2C_INTERFACE      // Undef it, SPI will be used
#ifdef COMM_I2C_INTERFACE
  //#define REGISTER_RWTEST
  #define CN1100_I2C_COMM
  #define I2C_DEVICE_ADDR       0x40      // 0xBA 
  #define I2C_WRITE             0
  #define I2C_READ              1
  #define I2C_DEVICE_ID_DONE    0
  #define I2C_SUB_ADDR_DONE     1
  #define I2C_IDLE_STATUS       0
  #define I2C_STOP_STATUS       0xf0
  #define ARC_I2C_IRQ
#endif

#ifndef TC1540  // The ctp file is included in another SPEC file
/**********************************************************************/
/**** 1.8    *********  关于 触摸屏 类型  *****************************/
/**********************************************************************/

#define WHITESCREEN_15X10               // White or Black screen DITO, 15X10
//#define SITO_SCREEN_10X15             // Black Screen SITO, 15X10
//#define ITO_BORDERLINE_15X10          // ITO走线 DITO 屏

//#define PHONESCREEN_13X10               // 手机屏(单层ITO), GT960
//#define SH_FEATUREPHONE_13X9            // 手机屏(单层ITO), GT1688 ( //// FREQY_HOP_ONLY has to be disabled ////)

//#define PCB_DITO_SCREEN_15X10         // PCB 标准 DITO 图案
//#define PCB_RHOMBUS_15X10             // PCB - 菱形图案

/**********************************************************************/
/**** 1.9    *********  关于 触摸屏 头文件  ***************************/
/**********************************************************************/

#ifdef WHITESCREEN_15X10
#include "ctp_WHITESCREEN_15X10.h"
#endif

#ifdef SITO_SCREEN_10X15
#include "ctp_SITO_SCREEN_10X15.h"
#endif

#ifdef ITO_BORDERLINE_15X10
#include "ctp_ITO_BORDERLINE_15X10.h"
#endif

#ifdef PHONESCREEN_13X10
#include "ctp_PHONESCREEN_13X10.h"
#endif

#ifdef SH_FEATUREPHONE_13X9
#include "ctp_SH_FEATUREPHONE_13X9.h"
#endif

#ifdef PCB_DITO_SCREEN_15X10
#include "ctp_PCB_SCREEN_15X10.h"
#endif

#ifdef PCB_RHOMBUS_15X10
#include "ctp_PCB_RHOMBUS_15X10.h"
#endif

/**********************************************************************/
/**** 1.10   *********  关于 IC类型 及 TX/RX 头文件 *******************/
/**********************************************************************/
#ifdef PHONESCREEN_13X10
#define BOARD_TC960                     // For TC960/TC968
#else 
#define BOARD_TC1680                    // For TC1680/TC1688
#endif

#ifdef BOARD_TC1680
#include "TxRxMap_TC1126s_TC1680SB.h"
#endif

#ifdef BOARD_TC960
#include "TxRxMap_TC1126s_TC960SB.h"
#endif
#endif

/************************************************************************
******2222222222222222222222222222222222222222222222222222222222222******
******2222222222222222222222222222222222222222222222222222222222222******
*************************************************************************
*
*
*      Parameter Setting (it is a parameter for sure)
*
*
*************************************************************************
******2222222222222222222222222222222222222222222222222222222222222******
******2222222222222222222222222222222222222222222222222222222222222******
************************************************************************/
#define OSCC_VAL               0x3C0
#define ADC_SPEED_SET          ADCM_ADC_SPEED_DIV4
#define ACS_SPEED_SET          ADCM_ACS_DIV01

#define FINGER_NUM_MAX         5
#define DEBUG_VALUE_NUM_MAX    10

#define CALINUM_EVERYSTEP      5  // FrequencyHopByStretch
#define BASE_FRAME_SHIFT       2  // Baseline Proc
#define BASE_COUNT_MAX         4  // Baseline Proc

//*********************************************************
// Used for function of MACRO "FREQHOP_BYSTRETCH"
//*********************************************************
#ifdef FREQHOP_BYSTRETCH
#define NOISECAL_SHIFT       3                   // Based on INTEG_DUR 
#define NOISECAL_NUM         (1<<NOISECAL_SHIFT) // Based on INTEG_DUR
#define STRETCH_SHIFT        0                   // Based on INTEG_DUR 
#define STRETCH_STEP         (1<<STRETCH_SHIFT)  // Based on INTEG_DUR
#define STRETCH_STNUM        15                  // Based on INTEG_DUR 
#define TXSCAN_FINGERDELAY   100
#define TXSCAN_VALIDPERIOD   4800
#define TXSCAN_ENABLE        1                   // Based on INTEG_DUR 
#define TXSCAN_DISABLE       0                   // Based on INTEG_DUR 
#define STRETCH_INC_REG25    0
//#undef  FREQY_HOP_ONLY          // Hardware Switch
#else
#define STRETCH_INC_REG25    3
#endif

//*********************************************************
// Used for function of MACRO "AUTO_FACEDETECTION"
//*********************************************************
#define MIN_VALIDDATA4FACEDET  20   // Step1
#define MIN_CONNECTNUM4FACEDET 18   // Step2
#define MIN_FACEDET_LOSSPERD   100  // DELAY setting

//*********************************************************
// Used for Baseline_BaseBufferHandled()
//*********************************************************
#define PROTECT_TIME                    50    // 200

#define ABNORMAL_MAX_DIFF               15
#define ABNORMAL_NUM_DIFF               37    // ((XMTR_NUM*RECV_NUM)>>2)
#define ABNORMAL_HOLD_TIME              5

#define FRAME_MAX_SAMPLE                100

#define MAX_MUST_UPDATE_PERIOD          4000  // 10000
#define MAX_HOLDTIME_AFTERUPDATE        2000
#define DELTA_SUM_INIT                  600

//************************************************************
// These threshold should be different on different screens,
// But should be the same by length actually
//************************************************************
#define FINGER_UNITE_THRESHOLD          326        //6mm
#define FINGER_SEPATATE_THRESHOLD       396        //12mm
#define FINGER_FINGER_SPEED_LOW         1100       //37mm
#define FINGER_FINGER_SPEED_MIDDLE      (FINGER_FINGER_SPEED_LOW+50)       //39mm
#define FINGER_FINGER_SPEED_FAST        (FINGER_FINGER_SPEED_LOW+51)       //39mm
#define FINGER_MOVING_SPEED_THRD_LOW    200        
#define FINGER_MOVING_SPEED_THRD_MIDDLE 400        
#define FINGER_ID_HOLD_TIME             4

#define THRESHOLD_PARA_MAX              51
#define THRESHOLD_PARA_MIN              26
#define HAVE_FINGER_MAX                 10

#define THR144                          48     // DistanceFilter0
#define THR072                          36     // DistanceFilter0
#define THR024                          24     // DistanceFilter0
#define THR108                          108    // DistanceFilter0
#define THR048                          48     // DistanceFilter0

#define FINGER_INFO_SAVE_NUM            8      // 4 Points TAP filter; 4 Point Delay Report @ Begining
#define SL_SAVE_FINGER_NUM              3      // used for SMOOTH_LINES_FORWARD

//used for DataProc_ExpandBorder1Bit
#define BITMAP_MAX                      ((1<<RECV_NUM) - 1)

//**********************************************************
// Working Mode Parameter
//**********************************************************
#define DOZE_MODE_PERIOD                0x1000  // 160ms
#define DOZE_MODE_FINGER_THR            200     // The MAX of the Frame Diff is bigger than 200, finger is coming or lost
#define WORK_MODE_NOFING_MAXPERD        1000    // After 10 Seconds, NO finger appeared, then Doze Mode

#define FINGER_ADJUST_DISTANCE_MIN      24
#define FINGER_ADJUST_DISTANCE_MAX      72

#ifdef CN1100_STM32
  #ifdef STM32VC_LCD
  #define ISCANMODE_PERD_REG_VALUE             0x240   //20ms
  #else
  #define ISCANMODE_PERD_REG_VALUE             0x180   //15ms
  #endif

  #ifdef USB_COMMUNICATION
  #define ISCANMODE_PERD_REG_VALUE_NORMAL      0x233   //22ms
  #define ISCANMODE_PERD_REG_VALUE_FINGER_ONLY 0x1cc   //18ms
  #else
  #define ISCANMODE_PERD_REG_VALUE_NORMAL      0x780   //75ms
  #define ISCANMODE_PERD_REG_VALUE_FINGER_ONLY 0x1e6   //19ms
  #endif

  #define RAW_DATA_DIFF_THR                    800

  #define SCREEN_HIGH                          800
  #define SCREEN_WIDTH                         480
#else // CN1100_STM32
  #define ISCANMODE_PERD_REG_VALUE             0x140   //12.5ms      

  #define RAW_DATA_DIFF_THR                    1100

  extern int SCREEN_HIGH;
  extern int SCREEN_WIDTH;
#endif

#define XMTR_SCALE      ((SCREEN_HIGH<<8)/SXMTR_NUM)   // Q8
#define RECV_SCALE      ((SCREEN_WIDTH<<8)/RECV_NUM)   // Q8

#define XMTR_END        (XMTR_STR+XMTR_NUM)
#define RECV_END        (RECV_STR+RECV_NUM)
#define SAMP_NUM        (XMTR_NUM*RECV_NUM)



/************************************************************************
******333333333333333333333333333333333333333333333333333333333333*******
******333333333333333333333333333333333333333333333333333333333333*******
*************************************************************************
*
*
*      Const Setting (it is just a name somehow)
*
*
*************************************************************************
******333333333333333333333333333333333333333333333333333333333333*******
******333333333333333333333333333333333333333333333333333333333333*******
************************************************************************/
#define LED1                0         // PA1, PB0
#define LED2                1         // PA2, PB1

#define LED_ON              0
#define LED_OFF             1

#define HIGH_LEVEL          1
#define LOW_LEVEL           0

//*********************************************
// SPI, UART and USB Communication Command
//*********************************************
#define READ_CHIP_ID         0x9F    // SPI Read 0xc000 back;
#define FAST_WRITE_DATA      0x02    // SPI Following by 24 bits address and write any # of data
#define FAST_READ_DATA       0x03    // SPI Following by 24 bits address and read any # of data
#define WRITE_STATUS         0x01    // SPI Following by 2 bytes status writing to the regsiter;
#define READ_STATUS          0x05    // SPI Following by 2 bytes status reading to the regsiter;

#define USART_COMM_START     0
#define USART_COMM_WR_REG    1
#define USART_COMM_RD_REG    2
#define USART_COMM_WR_VAL    3

#define STOP_DEBUG_INFO      0xf0
#define START_DEBUG_INFO     0xf1
#define WRITE_REG_VALUE      0xf2
#define READ_REG_VALUE       0xf3
#define SHOW_DIFF_VALUE      0xf4
#define SHOW_ORIG_VALUE      0xf5
#define SHOW_BASE_VALUE      0xf6
#define SHOW_DEBUG_VALUE     0xf7
#define SHOW_FIG_LOC_ONLY    0xf8
#define SHOW_STM32_PARA      0xf9
#define SET_STM32_PARA       0xfa

#define USB_DATA_UNREADY     0
#define USB_DATA_BEGIN       1
#define USB_DATA_END         2


#define USART_RECEIVED_CMD   0
#define USART_RECEIVED_PARA  1 
#define USART_EXECUTED_CMD   2

//*********************************************
// DEBUG LEVEL set by UART Communication
//*********************************************
#define DEBUG_INFO_NONE      0
#define DEBUG_INFO_DELTA     1
#define DEBUG_INFO_ORG       2
#define DEBUG_INFO_BASE      3
#define DEBUG_INFO_DBG       4
#define DEBUG_INFO_FIGLOC    5

#define MAX_INT16            0x7FFF
#define MAX_UINT16           0xFFFF
#define MAX_MAP_VALUE        256

#define BUFFER_A             0
#define BUFFER_B             1

#define DATA_UNREADY      0
#define DATA_READY        1

#define FRAME_UNFILLED    0
#define FRAME_FILLED      1

#define ADC_RELEASED      0
#define ADC_LOCKED        1

#define SLEEP_MODE        3
#define iAUTOSCAN_MODE    4
#define DOZE_MODE         5

#define BATTERY_GOOD      0 // 0  - 24
#define BATTERY_BAD       1 // 20 - 32
#define BATTERY_WORSE     2 // 28 - 40
#define BATTERY_WORST     3 // 36 - XX

//*********************************************
// Threshold Setting for Data Handling
//*********************************************
#define NO_FINGER            0
#define FINGER_SHOW          1

#define TX_SPEED_TUNED_DONE  0    // FrequencyHopByStretch
#define TX_SPEED_TUNING      1    // FrequencyHopByStretch

#define TX_FAST_SPEED          10   // FrequencyHopByStretch
#define TX_MIDDLE_SPEED        11   // FrequencyHopByStretch
#define TX_SLOW_SPEED          12   // FrequencyHopByStretch

#define BASE_FRAME_HOLD        0  // Baseline Proc
#define BASE_FRAME_UPDATE      1  // Baseline Proc
#define BASE_FRAME_DISCARD     2  // Baseline Proc

#define BASELINE_UPDATING_CASE 0  // Baseline Proc
#define BASELINE_HOLDING_CASE  1  // Baseline Proc

//*********************************************
// 0-9,   Waiting for 10 frame to get stability
// 10-29, DUR Calibration
// 30-49, Baseline Initialization
//*********************************************
#define FRAME_0000            0
#define FRAME_0001            1
#define FRAME_0002            2
#define FRAME_0003            3
#define FRAME_0004            4
#define FRAME_0005            5
#define FRAME_0006            6
#define FRAME_0007            7
#define BASEFRAME_INITCNT     (FRAME_0007+1)

#define NORMAL_POSITION 	 0
#define BORDER_POSITION 	 1

#define STATE_NONE_FINGER    0         // No Finger Before
#define STATE_FIRST_FINGER   1         // One Finger has been come
#define STATE_SECOND_FINGER  2         // Two Fingers have been come
#define STATE_THIRD_FINGER   3         // Three Fingers have been come
#define STATE_FORTH_FINGER   4         // Four Fingers have been come
#define STATE_SERIAL_FINGER  10        // Four Fingers have been appeared, Oh, Yeah!

#define STATE_STAYAT_FINGER  15        // Stick State (NO MOVING ...)
#define STATE_S_OUT1_FINGER  16        // Stick State to Moving Out Finter 1
#define STATE_S_OUT2_FINGER  17        // Stick State to Moving Out Finter 2
#define STATE_S_OUT3_FINGER  18        // Stick State to Moving Out Finter 3

#define STATE_LOST_1_FINGER  20        // One NO_FINGER has been appeared
#define STATE_LOST_2_FINGER  21        // Two NO_FINGER have been appeared

#define STATE_STICK_START    0         // DistanceFilter0
#define STATE_STICK_HERE     1         // DistanceFilter0
#define STATE_PREPARE_MOVE   2         // DistanceFilter0
#define STATE_MOVING         3         // DistanceFilter0
#define STATE_PREPARE_STICK  4         // DistanceFilter0


//used for COMMUNICATION_WITH_PC
#define SIMULATE_DELTA_DISABLE  0
#define SIMULATE_DELTA_ENABLE   1


#define EXPAND_NEED             1
#define EXPAND_DONE             0

#define USB_REPORT_LEN          64
#define USB_ROW_MAX             12

#define MIDDLE_OF_FIVE          0
#define FRONT_OF_FOUR           1
#define BACK_OF_FOUR            2

//*******************************************************************************
// Press Key Flag from "Para.h" actually
//*******************************************************************************
#define NO_KEY_PRESSED          0
#define MENU_KEY_PRESSED        1
#define HOME_KEY_PRESSED        2
#define RETURN_KEY_PRESSED      3


//*******************************************************************************
// Face Detection State (Flag)
//*******************************************************************************
#define FACE_DETECT_FAR         0
#define FACE_DETECT_NEAR        1


//***********************************
// Used for BORDER_ADJUSTABLE    need open "#define BORDER_ADJUSTABLE" in CN1100_FuncSwitch.h; 
//***********************************
#define BORDER_TOMAXVALUE_L  125 
#define BORDER_TOMAXVALUE_R  125
#define BORDER_TOMAXVALUE_T  125
#define BORDER_TOMAXVALUE_B  125

#define BORDERDIS_L  25 
#define BORDERDIS_R  25 
#define BORDERDIS_T  25 
#define BORDERDIS_B  25 

#define BORDERZONE_L   0
#define BORDERZONE_R   0
#define BORDERZONE_T   0
#define BORDERZONE_B   0

#endif
