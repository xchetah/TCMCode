#ifndef BOARD_FPCSCREEN_17X10_H
#define  BOARD_FPCSCREEN_17X10_H

//*******************************************************************************
// 本来需要放在 FuncSwitch.h 里，但是考虑到只有手机屏才有的东西，遂放在此处
//*******************************************************************************
#define PRESS_KEY_DETECT        

#define KEY_TX_LOC              12
#define KEY_HOME_RXLOC          5
#define KEY_RET_RXLOC           9
#define KEY_MENU_RXLOC          0

#define KEY_HOME_RXLOCATION     500 // Threshold Value for valid pressing
#define KEY_RET_RXLOCATION      500
#define KEY_MENU_RXLOCATION     500

#define HIREF_SETTING        HIGH_REF_4PF
#define LOREF_SETTING        LOW_REF_1PF
#define HIREF_PL_SET         HIGH_REF_POS
#define LOREF_PL_SET         LOW_REF_POS
#define HIREF_GAIN_SET       REF_GAIN_16PF
#define LOREF_GAIN_SET       REF_GAIN_12PF
#define TX_DRIVE_PL          TX_DRIVE_INVS

#define RCVM_RCVR_FCAP_SET   RXF_GAIN_12PF

#define DUR_RESET            8
#define DUR_INTEG            36
#define DUR_STRETCH          1      // TX Freq: 150K
#define BURST_CNT            0x81F

#define DOZE_HIREF_SETTING   HIGH_REF_4PF
#define DOZE_LOREF_SETTING   LOW_REF_1PF
#define DOZE_HIREF_PL_SET    HIGH_REF_POS
#define DOZE_LOREF_PL_SET    LOW_REF_NEG
#define DOZE_HIREF_GAIN_SET  REF_GAIN_16PF
#define DOZE_LOREF_GAIN_SET  REF_GAIN_16PF
#define DOZE_TX_DRIVE_PL     TX_DRIVE_NORM

#define DOZE_TXREADNUM       2


#define XMTR_STR             0
#define SXMTR_NUM            17 // Screen XMTR
#define KXMTR_NUM            0  // Key XMTR
#define XMTR_NUM             (SXMTR_NUM + KXMTR_NUM)
#define RECV_STR             0
#define SRECV_NUM            9
#define KRECV_NUM            1
#define RECV_NUM             (SRECV_NUM + KRECV_NUM)

#define MIN_VALUE_POINT      0
#define ROUGH_FINGER_THR     50     // Minimum Value for Valid Finger
#define MAX_VAL_NON_FINGER   200
#define BE_PERCENT_RATIO     BE25PERCENT
#define XMTR_STRENGTH_SET    XMTR_STRENGTH_MAX

#endif
