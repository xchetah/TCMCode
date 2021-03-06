#ifndef ITO_BORDERLINE_15X10_H
#define  ITO_BORDERLINE_15X10_H

#if 1
#define HIREF_SETTING   HIGH_REF_2PF
#define LOREF_SETTING   LOW_REF_0PF
#define HIREF_PL_SET    HIGH_REF_POS
#define LOREF_PL_SET    LOW_REF_POS
#define HIREF_GAIN_SET  REF_GAIN_16PF
#define LOREF_GAIN_SET  REF_GAIN_16PF
#define TX_DRIVE_PL     TX_DRIVE_INVS
#else
#define HIREF_SETTING   HIGH_REF_4PF
#define LOREF_SETTING   LOW_REF_1PF
#define HIREF_PL_SET    HIGH_REF_POS
#define LOREF_PL_SET    LOW_REF_POS
#define HIREF_GAIN_SET  REF_GAIN_16PF
#define LOREF_GAIN_SET  REF_GAIN_16PF
#define TX_DRIVE_PL     TX_DRIVE_INVS
#endif
#define RCVM_RCVR_FCAP_SET  3

#define DOZE_HIREF_SETTING   HIGH_REF_1PF
#define DOZE_LOREF_SETTING   LOW_REF_3PF
#define DOZE_HIREF_PL_SET    HIGH_REF_NEG
#define DOZE_LOREF_PL_SET    LOW_REF_NEG
#define DOZE_HIREF_GAIN_SET  REF_GAIN_16PF
#define DOZE_LOREF_GAIN_SET  REF_GAIN_12PF
#define DOZE_TX_DRIVE_PL     TX_DRIVE_NORM

#define DUR_RESET       16
#define DUR_INTEG       48
#define DUR_STRETCH     0
#define BURST_CNT       0x82F


#define MIN_VALUE_POINT 0

#define XMTR_STR        0
#define SXMTR_NUM       15 // Screen XMTR
#define KXMTR_NUM       0  // Key XMTR
#define XMTR_NUM        (SXMTR_NUM + KXMTR_NUM)
#define RECV_STR        0
#define SRECV_NUM       10
#define KRECV_NUM       0
#define RECV_NUM        (SRECV_NUM + KRECV_NUM)

#define ROUGH_FINGER_THR      50
#define MAX_VAL_NON_FINGER    100
#define BE_PERCENT_RATIO      BE25PERCENT
#define XMTR_STRENGTH_SET     XMTR_STRENGTH_MAX

#endif
