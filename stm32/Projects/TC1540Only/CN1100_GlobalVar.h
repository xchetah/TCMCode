#ifndef GLOBSLVAR_H
#define  GLOBSLVAR_H

typedef struct DeltaPData {

    uint32_t   BitMapDat[XMTR_NUM];                    // Array for Delta Data transfering

    uint16_t   Finger_X_XMTR;                          // Finger Location X, Q8
    uint16_t   Finger_Y_RECV;                          // Finger Location Y, Q8
    uint16_t   Prev_Finger_X[FINGER_INFO_SAVE_NUM];    // Previous Finger Location X for sortfingers, 4-Tap Back Filter
    uint16_t   Prev_Finger_Y[FINGER_INFO_SAVE_NUM];    // Previous Finger Location Y for sortfingers, 4-Tap Back Filter

    uint8_t    Point_Hold;                             // Point ID should hold if it is not zero
    uint8_t    LifeNumber;
    uint16_t   Finger_X_Reported;                      // Final report finger point X, Q8
    uint16_t   Finger_Y_Reported;                      // Final report finger point Y, Q8
    #ifdef FROMOUT2IN_INORDER
    uint16_t   Finger_X_Erpt[3];                      // Final report finger point X, Q8
    uint16_t   Finger_Y_Erpt[3];
    uint16_t   Finger_X_Erpt1[3];                      // Final report finger point X, Q8
    uint16_t   Finger_Y_Erpt1[3];
    #endif
    uint16_t   Finger_X_S[SL_SAVE_FINGER_NUM];         // 2-Tap Forward Filter
    uint16_t   Finger_Y_S[SL_SAVE_FINGER_NUM];         // 2-Tap Forward Filter
    uint16_t   FF_LastDist;

    #ifdef RAWPOINT4FINGMATCH
    uint16_t   Finger_XRAW[3];         // 
    uint16_t   Finger_YRAW[3];         // 
    #endif

    #ifdef STM32VC_LCD
    uint16_t   Finger_X_LCD;                           // Finger point based on LCD resolution, e.g. 800 X 480
    uint16_t   Finger_Y_LCD;                           // Finger point based on LCD resolution, e.g. 800 X 480
    #endif

    uint16_t   Finger_X_Raw;                           // Just Previous Finger Location X, Backup only
    uint16_t   Finger_Y_Raw;                           // Just Previous Finger Location Y, Backup only
    uint16_t   FingMovingSpeed;                        // Dist Threshold, If it is longer than the value, we think it is different finger point 

    uint32_t   colLSB;                                 // Column MSB
    uint16_t   start_x;                                // Start X of the valid Area
    uint16_t   start_y;                                //
    uint16_t   len_x;                                  //
    uint16_t   len_y;                                  //
    int16_t    pBuffer_X[XMTR_NUM]; 
    int16_t    pBuffer_Y[RECV_NUM];  
    uint8_t    FingerStateFlag;
    uint8_t    FingerStateFlagEdge;
    uint8_t    JustPassStateFlag4;

    uint16_t   AdjustDistance;
    uint8_t    AdjustState;
    uint8_t    AdjStickCounter;
    uint16_t   AdjustOrigin_x;	
    uint16_t   AdjustOrigin_y;	

    uint16_t   EdgeShift_L;
    uint16_t   EdgeOffset_L;
    uint16_t   EdgeShift_R;
    uint16_t   EdgeOffset_R;
    uint16_t   EdgeShift_T;
    uint16_t   EdgeOffset_T;
    uint16_t   EdgeShift_B;
    uint16_t   EdgeOffset_B;
    uint8_t    FingerRealNum1_X;
    uint8_t    FingerRealNum2_X;
    uint8_t    FingerRealNum2R_X;
    uint8_t    FingerRealNum1_Y;
    uint8_t    FingerRealNum2_Y;

    uint8_t    ExtremumCount;                          // How many local peak in the finger area?
    uint8_t    DataInvalidFlag;                        // =1: Invalid; =0: Data of Valid Finger Point
    uint8_t    Extremum_Pos_X[FINGER_NUM_MAX];
    uint8_t    Extremum_Pos_Y[FINGER_NUM_MAX];
    uint32_t   BitmapOrig[XMTR_NUM];
} DeltaPData_t;

typedef struct
{
    uint16_t First_Burst : 1; // Bit0  // Useless now
    uint16_t Buff_Select : 1; // Bit1  // Useless now
    uint16_t Buf_A_Ready : 1; // Bit2  // Useless now
    uint16_t Buf_B_Ready : 1; // Bit3  // Useless now
    uint16_t ADC_Release : 1; // Bit4  // Useless now
    uint16_t iBuf_A_Fill : 1; // Bit5  // Useful
    uint16_t iBuf_B_Fill : 1; // Bit6  // Useful
    uint16_t Buf_A_Pring : 1; // Bit7  // Useless now
    uint16_t Buf_B_Pring : 1; // Bit8  // Useless now
} BurstStateDef;

typedef struct
{
    uint16_t Doze_FirstIn      : 2; // Bit1:0   Useful
    uint16_t Doze_OddNum       : 1; // Bit2     Useless now
} MTDCtrl;	

typedef struct
{
    uint16_t Flag       : 1; // Bit0     
    uint16_t WFNum      : 7; // Bit7:1   
    uint16_t BigNum     : 7; // Bit14:8  
} FaceDetCtrl;	

typedef struct
{
    uint16_t       NoFingerCnt4Doze;
    MTDCtrl        mtdc;                       // 32bits
} ModeTransferDef;

typedef struct
{
    uint32_t FingerExist       : 2; // Bit1:0
    uint32_t PreFingerExt      : 2; // Bit3:2
    uint32_t NoFingerCnt4Base  : 8; // Bit11:3
    uint32_t BaseUpdateCase    : 2; // Bit13:12
} BBDCtrl;	

typedef struct
{
    uint8_t MenuKey		:1;	//bit1 
    uint8_t	HomeKey		:1;	//bit2
    uint8_t	ReturnKey	:1;	//bit3 
} PressKeyStruct;

typedef struct
{

    uint16_t       BaseDat[XMTR_NUM][RECV_NUM];      // Buffer for Basement
    uint16_t       BaseDatSaved[XMTR_NUM][RECV_NUM];
    uint16_t       SumBaseDat[XMTR_NUM][RECV_NUM];   // Buffer for Basement Sum Data
    uint16_t       InitCount;
    BBDCtrl        bbdc;                       // 32bits
    uint32_t       SettledPointExistTime;      //
    uint32_t       FingerLeftProtectTime;      //
    uint32_t       TooLongTime4BaseUpdate;
    uint32_t       AfterBaseUpdatedTime;
    uint8_t        AdjustCount;
    uint8_t        AbnormalUpdateDelay;
    int16_t        DeltaSumMaxThreshold;
    int32_t        DeltaSum;	
} BaseBufferDef;

//typedef struct
//{
// int16_t    SimDeldaData[XMTR_NUM][RECV_NUM];
// int16_t    DrawCount;
// int16_t    LineCount;
// int16_t    LineNumMax;
//} SimDeltaData_t;

typedef struct
{
    uint32_t    ProtectTime;
    uint32_t    FrameMaxSample;
    uint32_t    AbnormalMaxDiff;
    uint32_t    AbnormalNumDiff;
    uint32_t    FixPointMaxTime;
    uint32_t    MaxUpdateTime;
    uint32_t    HighRefSet;    // 0,1,2,3
    uint32_t    HighRefPlSet;  // 0,1
    uint32_t    LowRefSet;     // 0,1,2,3
    uint32_t    LowRefPlSet;   // 0,1,2,3
    uint32_t    HighRefGainSet;// 0,1,2,3
    uint32_t    LowRefGainSet; // 0,1,2,3
    uint32_t    TxPolarity;
    uint32_t    FinAdjDisMin;
    uint32_t    FinAdjDisMax;
    uint32_t    MaxValueNoFinger;
    uint32_t    FinThrMin;
    uint32_t    FinAdjDisX;
    uint32_t    RcvmRcvrFcapSet;
    uint32_t    DurReset;
    uint32_t    DurInteg;
    uint32_t    DurStretch;
    uint32_t    BurstCnt;
} ParaCanBeAdjusted_t;

typedef struct {
    uint16_t    Noise_Sum;
    uint8_t     Noise_Count;
    uint8_t     Battery_Level;
    uint16_t    Frame_Count;
} NoiseDataDef;

typedef struct BasData {

    //***********************************************************************
    // Frame and Delta data for data handling
    //***********************************************************************
    uint16_t       FrameDatLoadA[XMTR_NUM][RECV_NUM]; // Buffer A
    uint16_t       FrameDatLoadB[XMTR_NUM][RECV_NUM]; // Buffer B
    uint16_t       FrameDataSave[XMTR_NUM][RECV_NUM];

    BaseBufferDef    BFD;
    ModeTransferDef  MTD;
    NoiseDataDef     NDD;
      int16_t        DeltaDat16A[XMTR_NUM][RECV_NUM];  // Buffer for Delta Data

#ifdef PRESS_KEY_DETECT
    int16_t        DeltaDat_kp[RECV_NUM];            // Buffer for Delta Data
#endif

    uint32_t       DeltaBitDat[XMTR_NUM];            // Array for Delta Data transfering
    DeltaPData_t   DPD[FINGER_NUM_MAX];

    //***********************************************************************
    // XMTR drive control and data extract procedure control
    //***********************************************************************
    //uint32_t       XmtrDriveOut;                     // Drive Signal Out of XMTR (1^XmtrDriveCount)
    //uint16_t       XmtrDriveCount;                   // XMTR Drive Counting (0 ... XMTR_NUM)
    //uint16_t       int_wait;                         // Several Interrupts to Output one data

    //***********************************************************************
    // BSDSTS : Bit0, Initial State/Working State for first burst;
    //          Bit1, Indicate BufferA or BufferB for saving data;
    //          Bit2, BufferA could be handled or handled already;
    //          Bit3, BufferB could be handled or handled already;
    //          Bit4, NewAdcStartLock, after we start an ADC in SysTick
    //***********************************************************************
    BurstStateDef   BSDSTS;
    uint16_t        ModeSelect;

    uint16_t        MEM_MAX_X_XMTR;        // Memory MAX location X
    uint16_t        MEM_MAX_Y_RECV;        // Memory Max Location Y
    uint16_t        MEM_MAX_XY_Count;      // MAX Point Record time

    uint16_t        MEM_MIN_X_XMTR;        // Memory MIN location X
    uint16_t        MEM_MIN_Y_RECV;        // Memory MIN Location Y
    uint16_t        MEM_MIN_XY_Count;      // MIN Point Record time

    uint16_t        FRM_MIN_X_XMTR;        // Frame MIN Location X
    uint16_t        FRM_MIN_Y_RECV;        // Frame MIN Location Y

    uint16_t        FRM_MAX_X_XMTR;        // Frame MAX Location X
    uint16_t        FRM_MAX_Y_RECV;        // Frame MAX Location Y

    int16_t         MaxValueInFrame;       // Should be Positive Value
    int16_t         MinValueInFrame;       // Maybe Negative Value

    int16_t         ThresholdInFrame;      // Dynamic Threshold for the frame

    uint16_t        FingerInvalidPNum;     // Used for Baseline Updating

#ifdef AUTO_FACEDETECTION
    FaceDetCtrl     FDC;                   // 
    uint16_t        FaceDetectDelay;       // 
#endif
    uint16_t        FingerDetectNum;       // 
    uint16_t        PrevFingerDetectNum;   // used for sortfingers
    uint16_t        PrevFingerDetectNum2;  // used for sortfingers

    uint16_t        FingerDetectBitFlag;   // 
    uint8_t         PressKeyFlag1;
    PressKeyStruct  PressKeyFlag;

    uint8_t         BaseChangeFlag;
    uint8_t	        PowerOnWithFinger;     //
    uint8_t         ThresholdPara;

#ifdef	SCREEN_ADAPTIVE
   uint8_t          ScreenAdaptiveFlag;
#endif

   uint8_t          FingerRealNum;
   uint8_t          FingerRealNum1;

   uint16_t         ThrLow4DistanceFilter;
   uint16_t         ThrHigh4DistanceFilter;

   uint16_t         REG3E_Value;

   uint16_t         LFrame_NUM;
     int8_t         LFrame_X_XMTR[FINGER_NUM_MAX];
     int8_t         LFrame_Y_RECV[FINGER_NUM_MAX];

#ifdef FREQHOP_BYSTRETCH
    #if 1
    uint32_t        NoiseDataFrFrame;      // Sum of ABS(Delta) of every Frame
    uint32_t        SumNoiseDataFrFrame;   // Sum of absSum (Total: 16 Frames)
    uint16_t        TxScanNoiseCount;      // Every Freq, we calculate 16 Times most likely
    uint16_t        StretchValue;          // Current Debug Value for Stretch
    uint16_t        StretchInReg;          // Current Best Value for Stretch
    uint16_t        NoiseDataTable[STRETCH_STNUM]; // All Value of SumAbsSum for every Stretch (Total: STRETCH_STNUM)
    uint16_t        FingerDelay;          // After a finger appearing, we should not scan before delay is met
    uint16_t        TxScanValid;          // TxScan should be kept as a valid status for a while
    uint16_t        CurNoiseThrHi;        // The Current Noise Threshold
    uint16_t        CurNoiseThrLo;        // The Current Noise Threshold
    uint16_t        CurNoiseSum;          // The Current Noise Threshold
    #else
    uint16_t        FreqHopState;
    uint16_t        CurrentNoise[3]; // 0:HF;1:MF;2:SF
    uint8_t         CalibCount;
    uint8_t         BNoiseCount;
    uint8_t         SNoiseCount;
    uint16_t        CurNoiseThrd;
    uint16_t        FreqHopRestPerd;	
    #endif
#endif

    ParaCanBeAdjusted_t PCBA;
    uint16_t        Prepare2SleepMode;
    int16_t         AveDeltaXmtr[XMTR_NUM];
    uint32_t        ClockCount4ANY;
    int16_t         SigDeltaDat[XMTR_NUM][RECV_NUM];  // Single Finger Delta Data
#ifdef STM32VC_LCD
    uint16_t   LcdIsrFlag;
    uint16_t   LcdShowDataType;
    uint16_t   LcdShowDataIndex;
    uint16_t   Button_Glitch_Protect;
    int16_t   LcdShowData[XMTR_NUM][RECV_NUM];
#endif

#ifdef SHOW_H_DATA
    uint16_t  Left_h;
    uint16_t  Right_h;
    uint16_t  Top_h;
    uint16_t  Bottom_h;
#endif
  
} BasData_t;

#define VarSL(x,v,m)  do {                                 \
                      x = 1;                               \
                      if ((v) & 1) x <<= 1;                \
                      if ((v) & 2) x <<= 2;                \
                      if (((m) > 3) && ((v) & 4)) x <<= 4; \
                      if (((m) > 7) && ((v) & 8)) x <<= 8; \
                      } while (0)

#ifdef COMMUNICATION_WITH_PC
typedef struct DbgData {
    uint16_t   DebugInfoLevel;
    uint16_t   RegisterAddr;
    uint16_t   RegisterVal;
    uint8_t    ReceiveBuf[100];
    uint8_t    validCommand; // 0:Receive Command; 1: Received Para; 2: Execute Command
    uint8_t    UartParaNum;
    uint8_t    debug_value_num;   //send debug_value to PC
    int16_t    debug_value[DEBUG_VALUE_NUM_MAX]; //send debug_value to PC
    #ifdef USB_COMMUNICATION
    uint8_t    Transi_Buffer[USB_REPORT_LEN*USB_ROW_MAX];
    uint8_t    USB_ReceiveFlg;
    uint8_t    TimeCount;
    uint8_t    USB_dataflag;
    uint8_t    TbufCount;
    uint8_t    RowCount;
    #endif
} DbgData_t;
#endif

#ifdef CN1100_LINUX
typedef struct BufferData{
    int BufferID;
    BasData_t *bdt;
    int used_time;
} bd_t;
#endif

#ifdef CN1100_MAIN_C
    BasData_t  bdt;
    #ifdef COMMUNICATION_WITH_PC
    DbgData_t  dbg;
    #endif
    #ifdef CN1100_LINUX
    bd_t *bd;
    #endif
#else
    extern BasData_t  bdt;
    #ifdef COMMUNICATION_WITH_PC
    extern DbgData_t  dbg;
    #endif
    #ifdef CN1100_LINUX
    extern bd_t *bd;
    #endif
#endif

#endif

