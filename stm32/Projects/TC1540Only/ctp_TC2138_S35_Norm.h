#ifndef BOARD_WHITESCREEN_15X10_H
#define  BOARD_WHITESCREEN_15X10_H

#define DITO_SCREEN_TYPE1
//#define DITO_SCREEN_TYPE2

#ifdef DITO_SCREEN_TYPE2
//****************************************************
// This TYPE of the screen is the first TP we used
//                            (From 2013-12-1)
//****************************************************
#define HIREF_SETTING        HIGH_REF_1PF
#define LOREF_SETTING        LOW_REF_2PF
#define HIREF_PL_SET         HIGH_REF_NEG
#define LOREF_PL_SET         LOW_REF_NEG
#define HIREF_GAIN_SET       REF_GAIN_16PF
#define LOREF_GAIN_SET       REF_GAIN_16PF
#define TX_DRIVE_PL          TX_DRIVE_NORM

#define RCVM_RCVR_FCAP_SET   RXF_GAIN_12PF

#define DUR_RESET            4  // 4
#define DUR_INTEG            48 // 6
#define DUR_STRETCH          0
#define BURST_CNT            0x828

#define DOZE_HIREF_SETTING   HIGH_REF_1PF
#define DOZE_LOREF_SETTING   LOW_REF_3PF
#define DOZE_HIREF_PL_SET    HIGH_REF_NEG
#define DOZE_LOREF_PL_SET    LOW_REF_NEG
#define DOZE_HIREF_GAIN_SET  REF_GAIN_16PF
#define DOZE_LOREF_GAIN_SET  REF_GAIN_12PF
#define DOZE_TX_DRIVE_PL     TX_DRIVE_NORM
#endif

#ifdef DITO_SCREEN_TYPE1 //TYPE1
//****************************************************
// This TYPE of the screen is the first TP we used
//                            (From 2013-5-1)
//****************************************************
#define HIREF_SETTING        HIGH_REF_4PF
#define LOREF_SETTING        LOW_REF_1PF
#define HIREF_PL_SET         HIGH_REF_POS
#define LOREF_PL_SET         LOW_REF_POS
#define HIREF_GAIN_SET       REF_GAIN_16PF
#define LOREF_GAIN_SET       REF_GAIN_12PF
#define TX_DRIVE_PL          TX_DRIVE_INVS

#define RCVM_RCVR_FCAP_SET   RXF_GAIN_12PF

#define DUR_RESET            8

#ifdef CN1100_WINNER  
#define DUR_INTEG            63
#else
#define DUR_INTEG            48
#endif

#define DUR_STRETCH          1      // TX Freq: 150K
#define BURST_CNT            0x81F

#define DOZE_HIREF_SETTING   HIGH_REF_4PF
#define DOZE_LOREF_SETTING   LOW_REF_0PF
#define DOZE_HIREF_PL_SET    HIGH_REF_POS
#define DOZE_LOREF_PL_SET    LOW_REF_POS
#define DOZE_HIREF_GAIN_SET  REF_GAIN_12PF
#define DOZE_LOREF_GAIN_SET  REF_GAIN_12PF
#define DOZE_TX_DRIVE_PL     TX_DRIVE_NORM

#define DOZE_TXREADNUM       2

#endif //TYPE1

#define XMTR_NUM   (FRAMEBUF_SIZE>>1)    // XMTR_NUM TC6336 G0
#define RECV_NUM   2                   // RECV_NUM TC6336 Group

#define XMTR_STR             0
#define SXMTR_NUM            15 // Screen XMTR
#define KXMTR_NUM            0  // Key XMTR
//#define XMTR_NUM             (SXMTR_NUM + KXMTR_NUM)
#define RECV_STR             0
//#define RECV_NUM             10

#define MIN_VALUE_POINT      0
#define ROUGH_FINGER_THR     50     // Minimum Value for Valid Finger
#define MAX_VAL_NON_FINGER   200
#define BE_PERCENT_RATIO     BE25PERCENT
#define XMTR_STRENGTH_SET    XMTR_STRENGTH_MAX

#endif
