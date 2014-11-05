#ifndef TC2681_SPEC_H
#define  TC2681_SPEC_H

//**************************************************************

//#define ABSO_MODE_ACTIVE
#define SHOW_USEFUL4DEBUG_ONLY
//#define TC6206_USEDAS_TC1680
#define SCAN_MODE_TYPE 		2     // iAutoScanMode
//#define SCAN_MODE_TYPE 	3     // Doze/Sleep Mode

//**************************************************************

#if 1
  #define FREQ_HOPP_SWITCH FUNCATION_DISABLE
  #define CLUSTER_SIZE     0
  #define PHASEENAB_SWITCH FUNCATION_DISABLE
  #define PHASEHOPP_TYPE   0
#else
 #ifdef FREQ_HOPP
  #define FREQ_HOPP_SWITCH FUNCATION_ENABLE
  #ifdef PHASE_HOPP
  #define CLUSTER_SIZE     3
  #define PHASEENAB_SWITCH FUNCATION_ENABLE
  #define PHASEHOPP_TYPE   3
  #else
  #define CLUSTER_SIZE     2
  #define PHASEENAB_SWITCH FUNCATION_DISABLE
  #define PHASEHOPP_TYPE   2
  #endif
 #else
  #define FREQ_HOPP_SWITCH FUNCATION_DISABLE
  #ifdef PHASE_HOPP
  #define CLUSTER_SIZE     1
  #define PHASEENAB_SWITCH FUNCATION_ENABLE
  #define PHASEHOPP_TYPE   2
  #else
  #define CLUSTER_SIZE     0
  #define PHASEENAB_SWITCH FUNCATION_DISABLE
  #define PHASEHOPP_TYPE   0
  #endif
 #endif
#endif

#ifdef TC6206_USEDAS_TC1680
#define TXCH_SWITCHL      0x7FFF0000          // S14(N), 16-29
#define TXCH_SWITCHH      0x00
#define TXCH_ENA0V       (0xffff & (TXCH_SWITCHL>>0))    // TX Switch for every TX CH
#define TXCH_ENA1V       (0xffff & (TXCH_SWITCHL>>16))   // TX Switch for every TX CH
#define TXCH_ENA2V       (0x00ff & (TXCH_SWITCHH))       // TX Switch for every TX CH

#define RXCH_VALIDL       0x000001FF          // S0-9 as RX0 - RX9
#define RXCH_VALIDH       0x0
#define RXCH_VLD0V       (0xffff & (RXCH_VALIDL>>0))   // ABS CAP (RX SWITCH)
#define RXCH_VLD1V       (0xffff & (RXCH_VALIDL>>16))  // ABS CAP (RX SWITCH) 
#define RXCH_VLD2V       (0x00ff & (RXCH_VALIDH))      // ABS CAP (RX SWITCH)

#define TXCHAN_SIZE       15                             // TX_NUM = 15
#define RXCHAN_SSIZE      10                             // RECV_NUM
#define RXCHAN_GSIZE      9                             // RECV_NUM
#define RXGROUP_SIZE      1                              // RX Group 1 and 2;(S10 - S21)
#define TXGRPCH_SIZE     (TXCHAN_SIZE * RXGROUP_SIZE)    // Real Scan Times
#define FRAMEBUF_SIZE    (TXCHAN_SIZE * 10) //RXCHAN_GSIZE)    // 
#else
#define TXCH_SWITCHL      0xffc003ff          // S0 - S9, S22-S31 as TX(0-19)
#define TXCH_SWITCHH      0x00
#define TXCH_ENA0V       (0xffff & (TXCH_SWITCHL>>0))    // TX Switch for every TX CH
#define TXCH_ENA1V       (0xffff & (TXCH_SWITCHL>>16))   // TX Switch for every TX CH
#define TXCH_ENA2V       (0x00ff & (TXCH_SWITCHH))       // TX Switch for every TX CH

#define RXCH_VALIDL       0x3FF00C00          // S10 - S21 as RX0 - RX11
#define RXCH_VALIDH       0x0
#define RXCH_VLD0V       (0xffff & (RXCH_VALIDL>>0))   // ABS CAP (RX SWITCH)
#define RXCH_VLD1V       (0xffff & (RXCH_VALIDL>>16))  // ABS CAP (RX SWITCH) 
#define RXCH_VLD2V       (0x00ff & (RXCH_VALIDH))      // ABS CAP (RX SWITCH)

#define TXCHAN_SIZE       20                             // TX_NUM
#define RXCHAN_SSIZE      10                             // RECV_NUM
#define RXCHAN_GSIZE      12                             // RECV_NUM
#define RXGROUP_SIZE      2                              // RX Group 1 and 2;(S10 - S21)
#define TXGRPCH_SIZE     (TXCHAN_SIZE * RXGROUP_SIZE)    // Real Scan Times
#define FRAMEBUF_SIZE    (TXCHAN_SIZE * RXCHAN_GSIZE)    // 
#endif

#define RXGP_SIZEV       (RXGROUP_SIZE-1)                // 2-1, write to REG
#define RXCH_SIZEV       (RXCHAN_SSIZE-1)                // 10-1, write to REG
#define TXCH_SIZEV       (TXGRPCH_SIZE-1)                // 40-1, write to REG
#define FRAM_SIZEV       (FRAMEBUF_SIZE-1)               // 240-1, write to REG

#define DSCALE_SIZE      (FRAMEBUF_SIZE>>1)
#define ASCALE_SIZE      (TXGRPCH_SIZE<<2)

#define CODEMEM_SIZE     (FRAMEBUF_SIZE>>1)

#define FRAMEBUF_ADDR     0x1000
#define DSCALE_ADDR      (FRAMEBUF_ADDR + (FRAMEBUF_SIZE<<1))
#define ASCALE_ADDR       0x2000
#define FRAME0_ADDRESS    FRAMEBUF_ADDR
#define FRAME1_ADDRESS   (FRAMEBUF_ADDR + FRAMEBUF_SIZE)
#define CODE0_ADDRESS    (DSCALE_ADDR + DSCALE_SIZE)
#define CODE1_ADDRESS    (CODE0_ADDRESS + DSCALE_SIZE)


#define ASCA_GAIN_L       0xffff // 0xaaaa
#define ASCA_GAIN_H       0xf    // 0xa

#define PULL_SWITCH       FUNCATION_DISABLE

#define RCCK_SPEDV 3
#define RCCK_ADJUV 0
#define RCCK_STEPV 0x18
#define RCCK_MODEV 3
#define RCCH_HIGHV 0

#define ADCK_SPEDV 3
#define HVCK_SPEDV 2
#define HVCP_LEVLV 3
#define HVCP_ENABV 1
#define GURD_RINGV 0
#define CPIO_MODEV 1
#define ENDI_MODEV 0
#define INTR_MODEV 0

#define REFH_ENABV 1
//#define REFH_POLAV 1  // POS(1)
#define REFH_DRIVV 0
#define REFH_TCAPV 2  // 6PF(3)
#define REFH_FCAPV 3  // 16PF(3)

#define REFL_ENABV 1
//#define REFL_POLAV 0  // NEG(0)
#define REFL_DRIVV 0
#define REFL_TCAPV 3  // 3PF(3)
#define REFL_FCAPV 3  // 16PF(3)

#define BRST_DURAV 0x030
#define SCAN_MODIV SCAN_MODE_TYPE                 // 2: iAutoScan, 3: Doze Mode
#define RXMB_ENAIV 1
#define CHIP_ENAIV 1

#define INTE_DURAV 0x02F                          // 5
#define REST_DURAV 4
#define STRC_INCRV 0
#define STRC_ENDSV 0xa
#define STRC_DURIV 1
#if 0
#define SHDN_ENABV 0
#define MCFG_ENABV 0
#define XCLK_ENABV 0
#define XSEN_ENABV 0
#define SENS_ENABV 0
#define RJCT_ENABV 0
#define ADCS_ENABV 0
#define SPHD_ENABV 0
#define SHAR_ENABV 0
#define FILT_ENABV 0
#define DEMO_ENABV 0
#define FBSW_ENABV 0
#define PUUP_ENABV 0
#define PUDN_ENABV 0
#define INSW_ENABV 0
#define TEMP_BWATV 0
#define TEMP_BCNTV 0
#define TEMP_ABSOV 0
#define TEMP_SPMBV 0
#define TEMP_STARV 0
#define TEMP_RESTV 0
#define SPHD_RESTV 0
#define SHAR_RESTV 0
#define FILT_RESTV 0
#define DEMO_RESTV 0
#define FBSW_RESTV 0
#define PUUP_RESTV 0
#define PUDN_RESTV 0
#define INSW_RESTV 0
#endif
#define CODE_ENABV 1
#define CODE_MODEV 2
#define ASCA_ENABV 1
#define DSCA_ENABV 1
#define HALF_FILTV 0
#define PHAS_HOPPV PHASEENAB_SWITCH   // PHASE ENABLE
#define PHAS_TYPEV PHASEHOPP_TYPE     // PHASE HOPP TYPE
#define FREQ_TYPEV FREQ_HOPP_SWITCH   // FREQ HOPP SWITCH
#define CLUS_SIZEV CLUSTER_SIZE       // CLUSTER SIZE
#define PULL_ENABV PULL_SWITCH        // Used for ABS CAP Drive
#define PULL_POLAV 0
#define RXCH_FLOAV 0
#define TXCH_FLOAV 1
#if 0
#define RJCT_CLRSV 0
#define FILT_CLRSV 0
#define RXCH_BIASV 0
#endif
#define FILT_BDWTV 0
#define RXCH_FCAPV 3

#define RXCH_MAP9V 9
#define RXCH_MAP8V 8
#define RXCH_MAP7V 7
#define RXCH_MAP6V 6
#define RXCH_MAP5V 5
#define RXCH_MAP4V 4
#define RXCH_MAP3V 3
#define RXCH_MAP2V 2
#define RXCH_MAP1V 1
#define RXCH_MAP0V 0

#define RXGP_MAP3V 3
#define RXGP_MAP2V 2
#define RXGP_MAP1V 1
#define RXGP_MAP0V 0


//#define RXCH_ENABV 1
#define RXCH_ENABV 0x3ff

#define GEST_SIZEV 3
#define TXCH_STRNV 0
#define GEST_ENABV FUNCATION_DISABLE     // Gesture Fucntion switch

#if 0
#define TXCH_POLAV 0
#define REFH_ACAPV 1 // 0
#define REFL_ACAPV 1
#define ABSO_JPENV 1
#define ABSO_HIGHV 1
#define AREF_BIASV 6
#define ABSO_BIASV 5
#define PROB_CKENV 0
#define PROB_IOENV 0
#define PROB_ENABV 0
#define PROB_SELEV 0
#define CALI_ALLSV 0
#define CALI_POLAV 0
#define CALI_DRIVV 0
#define CALI_SELEV 0
#define DDAC_VREFV 0
#define DDAC_HOLDV 0
#define DDAC_DRIVV 0
#define DDAC_DATAV 0
#define TUCH_BUFFV 0
#define TUCH_SIZEV 1
#define TUCH_LEVLV 0x3f
#define GEST_WINDV 6
#define BRST_CLRSV 1
#define ADCS_CLRSV 1
#define MIXS_CLRSV 1
#define FRMA_CLRSV 1
#define FRMB_CLRSV 1
#define CPSW_RESTV 1
#define PBSW_RESTV 1
#define SBSW_RESTV 1
#define CHIP_STARV 1
#endif
#include "Screen_TC2681_DITO_20X12.h"

#endif

