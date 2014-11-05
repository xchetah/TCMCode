#ifndef TC1126S_TMAP_TC1680SB_H
#define  TC1126S_TMAP_TC1680SB_H

#if defined(CN1100_S5PV210) || defined(CN1100_STM32)

#ifdef SH_FEATUREPHONE_13X9
#define SCN_T0   8  // Feature Phone (TC1688) for SH
#define SCN_T1   9
#define SCN_T2   10
#define SCN_T3   4
#define SCN_T4   11
#define SCN_T5   3
#define SCN_T6   12
#define SCN_T7   15
#define SCN_T8   0
#define SCN_T9   14
#define SCN_T10  1
#define SCN_T11  13
#define SCN_T12  2
#else
#define SCN_T0  15   // The first TX in Screen to T15 in Chip
#define SCN_T1  14
#define SCN_T2  13
#define SCN_T3  12
#define SCN_T4  11
#define SCN_T5  10
#define SCN_T6  9
#define SCN_T7  8
#define SCN_T8  7
#define SCN_T9  6
#define SCN_T10 5
#define SCN_T11 4
#define SCN_T12 3
#define SCN_T13 2
#define SCN_T14 1
#endif

#else
#define SCN_T0  7   // The first TX in Screen to T15 in Chip
#define SCN_T1  6
#define SCN_T2  5
#define SCN_T3  4
#define SCN_T4  3
#define SCN_T5  2
#define SCN_T6  1
#define SCN_T7  0
#define SCN_T8  8
#define SCN_T9  9
#define SCN_T10 10
#define SCN_T11 11
#define SCN_T12 12
#define SCN_T13 13
#define SCN_T14 14
#endif

//#define _SCN_ORDER(x)  #x
//#define SCN_ORDER(x)  _SCN_ORDER(x)
//#define SCN_TO_IC(x)  SCN_T##_SCN_ORDER(x)

#ifdef SH_FEATUREPHONE_13X9
#define SCN_R0  9  // The first RX in Screen to R0 in Chip
#define SCN_R1  8
#define SCN_R2  7
#define SCN_R3  6
#define SCN_R4  5
#define SCN_R5  4
#define SCN_R6  3
#define SCN_R7  2
#define SCN_R8  1
#define SCN_R9  0    // Useless RX(0) in the phone actually
#else
#define SCN_R0  0    // The first RX in Screen to R0 in Chip
#define SCN_R1  1
#define SCN_R2  2
#define SCN_R3  3
#define SCN_R4  4
#define SCN_R5  5
#define SCN_R6  6
#define SCN_R7  7
#define SCN_R8  8
#define SCN_R9  9
#endif

#endif
