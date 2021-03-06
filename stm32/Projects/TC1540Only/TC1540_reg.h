#ifndef TC1540_REG_H
#define  TC1540_REG_H

#define HIGH_REF_1PF        0
#define HIGH_REF_2PF        1
#define HIGH_REF_4PF        2
#define HIGH_REF_6PF        3
#define HIGH_REF_POS        1
#define HIGH_REF_NEG        0

#define LOW_REF_0PF         0
#define LOW_REF_1PF         1
#define LOW_REF_2PF         2
#define LOW_REF_3PF         3
#define LOW_REF_POS         1
#define LOW_REF_NEG         0

#define REF_GAIN_4PF        0
#define REF_GAIN_8PF        1
#define REF_GAIN_12PF       2
#define REF_GAIN_16PF       3

#define RXF_GAIN_4PF        0
#define RXF_GAIN_8PF        1
#define RXF_GAIN_12PF       2
#define RXF_GAIN_16PF       3

#define TX_DRIVE_NORM       0
#define TX_DRIVE_INVS       1

#define XMTR_STRENGTH_MIN  	0
#define XMTR_STRENGTH_MID1 	1
#define XMTR_STRENGTH_MID2 	2
#define XMTR_STRENGTH_MAX  	3

//*****************************************************************
//
// The following Reg is operated for both Reading and Writing
//
//*****************************************************************
#define RCCK_CTRL    			0x20      // RC Clock Divider
#define RCCK_SPED(x)            (x<<13)   // Divider: 1,2,3,4,6,8,12,20, Bit15:13
#define RCCK_ADJU(x)            (x<<9)    // in the step 1/16th of rcck_step
#define RCCK_STEP(x)            (x<<3)    // in the step 1/64th of rcck_mode
#define RCCK_MODE(x)            (x<<1)    // RCCK: 8/16/24/32 MHz
#define RCCK_HIGH               (1)       // Highest 40MHz

#define ADCK_HVCK    			0x21      // ADC Management
#define ADCK_SPED(x)            (x<<14)   // Divider: 1,2,4,6 
#define HVCK_SPED(x)            (x<<10)   // High Voltage Pump working in the step 1/16th of rcck_step
#define HVCP_LEVL(x)            (x<<8)    // Output Level as: 4,  4.5,  5,  5.5
#define HVCP_ENAB               (1<<7)    // Enable High Voltage Pumper
#define GURD_RING               (1<<6)    // Enable Drive Guard Ring
#define CPIO_MODE               (1<<5)    // Define Chip IO Power Level: 1.8V or 3.3V
#define ENDI_MODE(x)            (x<<3)    // I2C Comm Endian Mode (BB,BL,LB,LL)
#define INTR_MODE(x)            (x<<0)    // Interrupt Mode

#define REFH_REFL    			0x22      // ADC Interrupt Type selection
#define REFH_ENAB               (1<<14)   // Enable REFH Channel
#define REFH_POLA               (1<<13)   // REFH Channel Polarity
#define REFH_DRIV               (1<<12)   // Enable Drive the REFH Channel
#define REFH_TCAP(x)            (x<<10)   // REFH Channel: Trans_CAP: 1,2,4,6 PF
#define REFH_FCAP(x)            (x<<8)    // REFH Channel: FeedBack_CAP: 10,12,14,16 PF
#define REFL_ENAB               (1<<6)    // Enable REFL Channel
#define REFL_POLA               (1<<5)    // REFL Channel Polarity
#define REFL_DRIV               (1<<4)    // Enable Drive the REFL Channel
#define REFL_TCAP(x)            (x<<2)    // REFL Channel: Trans_CAP: 0,1,2,3 PF
#define REFL_FCAP(x)            (x<<0)    // REFL Channel: FeedBack_CAP: 10,12,14,16 PF

#define BRST_MODE    			0x23      // Burst Mode Register
#define BRST_DURA(x)    		(x<<5)    // Burst Number Setting
#define SCAN_MODE(x)    		(x<<3)    // Scan Mode(0:Line Mode, 1: Manual Scan, 2: Auto Scan, 3 Doze)
#define ABSO_MODE    		    (1<<2)    // ABS CAP Mode
#define RXMB_ENAB        		(1<<1)    // Receiver Main Bias
#define CHIP_ENAB        		(1<<0)    // Enable the Whole Chip

#define TIME_DURA    			0x24       // Duration for both Reset and Integration
#define TIME_DURL(x) 			(x<<0)     // Time_Dura[15:0]

#define INTE_REST    			0x25       // Durtion of Stretch
#define TIME_DURH(x) 			(x<<12)    // Time_Dura[19:16]
#define INTE_DURL(x) 			(x<<6)     // INTEG_Dura[5:0]
#define REST_DURL(x) 			(x<<0)     // RESET_Dura[5:0]

#define STRC_DURA   			0x26       // Stretch 
#define STRC_INCR(x) 			(x<<12)    // INC Step of Stretch Duration
#define STRC_ENDS(x) 			(x<<6)     // Stretch MAX Duration
#define STRC_DURV(x) 			(x<<0)     // Start Stretch Duration

#define FUNC_ENAB   			0x27       // Kinds of Funcation Switch
#define SHDN_ENAB               (1<<15)    // Shutdown Enable
#define MCFG_ENAB               (1<<14)    // enables and sync all the previous config settings to active
#define XCLK_ENAB               (1<<13)    // external clock and disables the internal rc clock
#define XSEN_ENAB               (1<<12)    // external sensing control
#define SENS_ENAB               (1<<11)    // enables sensor
#define RJCT_ENAB               (1<<10)    // enables reject feature to ignore the sample while...
#define SLEW_ENAB               (1<<9)     // enables the slew detection function of the front amplifier
#define ADCS_ENAB               (1<<8)     // enables ADC
#define SPHD_ENAB               (1<<7)     // Sample/Hold enable
#define SHAR_ENAB               (1<<6)     // Share Feature Enable
#define FILT_ENAB               (1<<5)     // Filter enable
#define DEMO_ENAB               (1<<4)     // demodulator Enable
#define FBSW_ENAB               (1<<3)     // feedback switch Enable
#define PUUP_ENAB               (1<<2)     // Pullup??? Enable
#define PUDN_ENAB               (1<<1)     // Pulldown??? Enable
#define INSW_ENAB               (1<<0)     // receiver input switch Enable

#define TEMP_REST    			0x28       // Temp bit and Reset enables bit
#define TEMP_BWAT               (1<<15)    // temporary for burst wait testing
#define TEMP_BCNT               (1<<14)    // temporary for burst count testing
#define TEMP_ABSO               (1<<13)    // temporary for absolute capacitance testing
#define TEMP_SPMB               (1<<12)    // temporary for main bias testing
#define TPRT_REST               (1<<10)    // temporary for enabling the sensing reset
#define SPHD_REST               (1<<7)     // forces the S/H reset
#define SHAR_REST               (1<<6)     // forces the samp share reset
#define FILT_REST               (1<<5)     // forces the Filter reset
#define DEMO_REST               (1<<4)     // forces the demodulator reset
#define FBSW_REST               (1<<3)     // forces the feedback capacitor switch reset
#define PUUP_REST               (1<<2)     // forces the Pull Up reset
#define PUDN_REST               (1<<1)     // forces the Pull Down reset
#define INSW_REST               (1<<0)     // forces the input switch reset

#define FILT_TYPE    			0x29       // Filter Type and Others
#define CODE_MODE(x)            (x<<12)    // 0: loseless; 1: 1bit precision lost; 2: 2 bits..; 3: 3bits...
#define ASCA_ENAB               (1<<11)    // enables the analog scaling function
#define DSCA_ENAB               (1<<10)    // enables the digital scaling function
#define HALF_FILT               (1<<9)     // enables the half burst filter 
#define BASE_FILT               (1<<8)     // enables the baseline filter 
#define PHAS_HOPP               (1<<7)     // enables the phase hopping mode
#define PHAS_TYPE(x)            (x<<5)     // phase hopping types:0(None),1(invert@Middle),...
#define FREQ_CHRP               (1<<4)     // enables the chirp scanning mode
#define FREQ_TYPE(x)            (x<<2)     // frequency scan filtering types: 0(None),1(Middle@3Freq), ...
#define CLUS_SIZE(x)            (x<<0)     // defines the burst numbers in one cluster

#define GEST_RXCH    			0x2A       // Gesture ...
#define GEST_SIZE(x)            (x<<12)    // defines the running window of threshold the gesture detection
#define PULL_ENAB               (1<<11)    // enables the pull up and pull down function
#define PULL_POLA               (1<<10)    // defines the pull up and pull down order
#define RXCH_FLOA               (1<<9)    // forces the receiver input in floating status during...
#define TXCH_FLOA               (1<<8)    // forces the transmitter output in floating status during...
#define RJCT_CLRS               (1<<7)    // resets the reject counter in the beginning of every burst
#define FILT_CLRS               (1<<6)    // resets the filter in the beginning of every burst
#define FILT_BDWT(x)            (x<<4)    // defines the sensor channel filter bandwidth (160Hz to 3.55KHz)
#define RXCH_FCAP(x)            (x<<2)    // defines the receiver feedback capacitance(10,12,14,16)
#define RXCH_BIAS(x)            (x<<0)    // defines the receiver bias types (1, 1.25, 1.5, 1.75uA)

#define RXCH_MAPA   			0x2B       // Each RX mapping relation with screen RX
#define RXCH_MAP3(x)            (x<<12)    // RX3 to Screen RX(x)
#define RXCH_MAP2(x)            (x<<8)     // RX2 to Screen RX(x)
#define RXCH_MAP1(x)            (x<<4)     // RX1 to Screen RX(x)
#define RXCH_MAP0(x)            (x<<0)     // RX0 to Screen RX(x)

#define RXCH_MAPB    			0x2C       // Each RX mapping relation with screen RX
#define RXCH_MAP7(x)            (x<<12)    // RX7 to Screen RX(x)
#define RXCH_MAP6(x)            (x<<8)     // RX6 to Screen RX(x)
#define RXCH_MAP5(x)            (x<<4)     // RX5 to Screen RX(x)
#define RXCH_MAP4(x)            (x<<0)     // RX4 to Screen RX(x)

#define RXGP_MAPC    			0x2D       // Each SCN RX group round robin mapping relation with internal RX
#define RXGP_MAP3(x)            (x<<14)    // SCN RX Group3(30-39) to IC RX(0-9)
#define RXGP_MAP2(x)            (x<<12)    // SCN RX Group3(30-39) to IC RX(0-9)
#define RXGP_MAP1(x)            (x<<10)    // SCN RX Group3(30-39) to IC RX(0-9)
#define RXGP_MAP0(x)            (x<<8)     // SCN RX Group3(30-39) to IC RX(0-9)
#define RXCH_MAP9(x)            (x<<4)     // RX9 to Screen RX(x)
#define RXCH_MAP8(x)            (x<<0)     // RX8 to Screen RX(x)

#define RXCH_SIZE    			0x2E       // RX Size
#define RXGP_SIZE(x)            (x<<14)    // screen receiver group numbers to be used
#define RXCH_SIZV(x)            (x<<10)    // the internal receiver channel number of each group to be used
#define RXCH_ENAB(x)            (x<<0)     // enables the individual receiver channel

#define TXCH_SIZE    			0x2F       // Tansmitter Enable [27:24]
#define TXCH_SIZV(x)            (x<<10)    // transmitter channel number
#define FRAM_SIZE(x)            (x<<0 )    // the frame size which should be set as RXGP_SIZE*RXCH_SIZE*TXCH_SIZE

#define TXCH_ENAA    			0x30       // TX ENABLE [15:0]
#define TXCH_ENAB    			0x31       // TX ENABLE [31:16]

#define TXCH_ENAC    			0x32       // Last TX Enable
#define TXCH_STRN(x)            (x<<14)    // the driver strengths of transmitter carrier
#define TXCH_POLA(x)            (1<<13)    // the transmitter carrier polarity
#define TXCH_ENAH(x)            (x<<0)     // TX ENABLE [39:32] // Bit7:0

#define ABSO_CTRL   			0x33      // ABS CAP Setting
#define REFH_ACAP(x)            (x<<13)   // the ABS CAP of REFH(0-7/8, step:1/8)
#define REFL_ACAP(x)            (x<<10)   // the ABS CAP of REFL(0-7/8, step:1/8)
#define ABSO_JPEN               (1<<9)    // enables the guard ring with jump balanced driving 
#define ABSO_HIGH               (1<<8)    // enables the guard ring with high / power level driving 
#define AREF_BIAS(x)            (x<<4)    // enables the guard ring with jump balanced driving 
#define ABSO_BIAS(x)            (x<<0)    // enables the guard ring with jump balanced driving 

#define PROB_CALI 			    0x34      // Probe control and Calibration
#define PROB_CKEN               (1<<15)   // enables the internal rc clock output to chip pin
#define PROB_IOEN               (1<<14)   // enables one io pin as internal probing output
#define PROB_ENAB               (1<<13)   // enables the probe testing
#define PROB_SELE(x)            (x<<8)    // Internal Sig Selection to PIN
#define CALI_ALLS               (1<<7)    // enables the all RX connecting with internal 1pF CAP
#define CALI_POLA               (1<<6)    // defines the carrier polarity in calibration mode
#define CALI_DRIV               (1<<5)    // enables the carrier in calibration mode
#define CALI_SELE(x)            (x<<0)    // selects one of RX to connect with the internal 1pF 

#define DDAC_CTRL  			    0x35      // DDAC Control
#define DDAC_VREF               (1<<14)   // Define REF Voltage of internal Diag DAC
#define DDAC_HOLD               (1<<13)   // Define DDAC input mode
#define DDAC_DRIV               (1<<12)   // Define DDAC operation mode
#define DDAC_DATA(x)            (x<<0)    // Set data for DDAC input comparing

#define GEST_TUCH    			0x36      // Gesture and Touch
#define GEST_WIND(x)            (x<<11)   // the running window size for gesture detection
#define GEST_ENAB               (1<<10)   // enables the gesture detection function
#define TUCH_BUFF               (1<<9)    // defines frame buffer mode during touch detection
#define TUCH_ENAB               (1<<8)    // enables the touch detection function for doze wakeup mode 
#define TUCH_SIZE(x)            (x<<6)    // defines touch trig threshold while the numbers of ...
#define TUCH_LEVL(x)            (x<<0)    // defines touch level threshold

//*****************************************************************
//
// The following Reg is speically DIFF @ Read and Write operation
//
//*****************************************************************
#define FLAG_CLRS    			0x37      // Write Only
#define CHIP_STAR               (1<<8)    // Enable Sensor to start sensoring
#define SBSW_REST               (1<<7)    // resets chip second bus related blocks running on all mode
#define PBSW_REST               (1<<6)    // resets chip primary bus related blocks running in normal working mode
#define CPSW_REST               (1<<5)    // Reset the whole chip
#define FRMB_CLRS               (1<<4)    // clears the frame buffer B interrupt
#define FRMA_CLRS               (1<<3)    // clears the frame buffer A interrupt
#define MIXD_CLRS               (1<<2)    // clears the adc done flag and burst done flag
#define ADCS_CLRS               (1<<1)    // clears the adc done flag
#define BRST_CLRS               (1<<0)    // clears the burst done flag

#define RXCH_VLDA    			0x38      // Write Only, defines the valid receiver channels in 00 to 15 
#define RXCH_VLDB    			0x39      // Write Only, defines the valid receiver channels in 16 to 31 
#define RXCH_VLDC    			0x3A      // Write Only, defines the valid receiver channels in 32 to 39 

#define CPID_FLAG    			0x37      // Read Only
#define DDAC_RSLT               (1<<12)   // keeps the DDAC comparator result
#define CPID_NUMB(x)            (x<<8)    // keeps chip id numbers
#define TPID_NUMB(x)            (x<<5)    // keeps tp id numbers
#define FRMB_FULL               (1<<4)    // keeps frame buffer B full flag
#define FRMA_FULL               (1<<3)    // keeps frame buffer A full flag
#define MIXD_DONE               (1<<2)    // keeps adc done or burst done flag
#define ADCS_DONE               (1<<1)    // keeps ADC done flag
#define BRST_DONE               (1<<0)    // keeps Burst Done flag

#define TUCH_ADRA    			0x38      // Read Only, Touch Point A address, Bit11:0 is effective
#define TUCH_VALA    			0x39      // Read Only, Touch Point A value,  Bit11:0 is effective
#define TUCH_ADRB    			0x3A      // Read Only, Touch Point B address, Bit11:0 is effective
#define TUCH_VALB    			0x3B      // Read Only, Touch Point B value,  Bit11:0 is effective

#define RJCT_CONT    			0x3C      // Read Only, keeps reject sample numbers while reject detection is on, Bt11:0

#define CODE_SIZA   			0x3D      // Read Only, Frame A Coding Size, Bit9:0
#define CODE_SIZB   			0x3E      // Read Only, Frame B Coding Size, Bit9:0

//************************************************************
// Buffer Allocation
//************************************************************
#define FRMA_BUFF    			0x1000    // TX CH Control, FRMB_BUFF is from (0x400+FRAME_SIZE)


//****************************************************************************************************
// Gain Value for Different TX/RX/Group Allocation
// DST0_G0R1:	This field defines the digital scale coefficient for receiver channel 1 of group 0 
//              while transmitter is scanning channel 0 
// DST0_G0R0:	This field defines the digital scale coefficient for receiver channel 0 of group 0 
//              while transmitter is scanning channel 0
//****************************************************************************************************
#define DST0_G010    			0x1800             // Digital Scale for T0
#define DST0_G032    			(1+DST0_G010)     // 
#define DST0_G054    			(1+DST0_G032)     // 
#define DST0_G076    			(1+DST0_G054)     // 
#define DST0_G098    			(1+DST0_G076)     // 
#define DST0_G110    			(1+DST0_G098)     // 
#define DST0_G132    			(1+DST0_G110)     // 
#define DST0_G154    			(1+DST0_G132)     // 
#define DST0_G176    			(1+DST0_G154)     // 
#define DST0_G198    			(1+DST0_G176)     // 
#define DST0_G210    			(1+DST0_G198)     // 
#define DST0_G232    			(1+DST0_G210)     // 
#define DST0_G254    			(1+DST0_G232)     // 
#define DST0_G276    			(1+DST0_G254)     // 
#define DST0_G298    			(1+DST0_G276)     // 
#define DST0_G310    			(1+DST0_G298)     // 
#define DST0_G332    			(1+DST0_G310)     // 
#define DST0_G354    			(1+DST0_G332)     // 
#define DST0_G376    			(1+DST0_G354)     // 
#define DST0_G398    			(1+DST0_G376)     // Total 20 registers

#define DST1_G010    			0x1814             // Digital Scale for T1
#define DST1_G032    			(1+DST1_G010)     // 
#define DST1_G054    			(1+DST1_G032)     // 
#define DST1_G076    			(1+DST1_G054)     // 
#define DST1_G098    			(1+DST1_G076)     // 
#define DST1_G110    			(1+DST1_G098)     // 
#define DST1_G132    			(1+DST1_G110)     // 
#define DST1_G154    			(1+DST1_G132)     // 
#define DST1_G176    			(1+DST1_G154)     // 
#define DST1_G198    			(1+DST1_G176)     // 
#define DST1_G210    			(1+DST1_G198)     // 
#define DST1_G232    			(1+DST1_G210)     // 
#define DST1_G254    			(1+DST1_G232)     // 
#define DST1_G276    			(1+DST1_G254)     // 
#define DST1_G298    			(1+DST1_G276)     // 
#define DST1_G310    			(1+DST1_G298)     // 
#define DST1_G332    			(1+DST1_G310)     // 
#define DST1_G354    			(1+DST1_G332)     // 
#define DST1_G376    			(1+DST1_G354)     // 
#define DST1_G398    			(1+DST1_G376)     // Total 20 registers

#define DST2_G010    			0x1828             // Digital Scale for T1
#define DST3_G010    			0x183C             // Digital Scale for T1
#define DST4_G010    			0x1850             // Digital Scale for T1
#define DST5_G010    			0x1864             // Digital Scale for T1
#define DST6_G010    			0x1878             // Digital Scale for T1
#define DST7_G010    			0x188C             // Digital Scale for T1
#define DST8_G010    			0x18A0             // Digital Scale for T1
#define DST9_G010    			0x18B4             // Digital Scale for T1
#define DSTA_G010    			0x18C8             // Digital Scale for T1
#define DSTB_G010    			0x18DC             // Digital Scale for T1
#define DSTC_G010    			0x18F0             // Digital Scale for T1
#define DSTD_G010    			0x1904             // Digital Scale for T1
#define DSTE_G010    			0x1918             // Digital Scale for T1

#define DSTF_G010    			0x192C     // Digital Scale for TF
#define DSTF_G032    			(1+DSTF_G010)     // 
#define DSTF_G054    			(1+DSTF_G032)     // 
#define DSTF_G076    			(1+DSTF_G054)     // 
#define DSTF_G098    			(1+DSTF_G076)     // 
#define DSTF_G110    			(1+DSTF_G098)     // 
#define DSTF_G132    			(1+DSTF_G110)     // 
#define DSTF_G154    			(1+DSTF_G132)     // 
#define DSTF_G176    			(1+DSTF_G154)     // 
#define DSTF_G198    			(1+DSTF_G176)     // 
#define DSTF_G210    			(1+DSTF_G198)     // 
#define DSTF_G232    			(1+DSTF_G210)     // 
#define DSTF_G254    			(1+DSTF_G232)     // 
#define DSTF_G276    			(1+DSTF_G254)     // 
#define DSTF_G298    			(1+DSTF_G276)     // 
#define DSTF_G310    			(1+DSTF_G298)     // 
#define DSTF_G332    			(1+DSTF_G310)     // 
#define DSTF_G354    			(1+DSTF_G332)     // 
#define DSTF_G376    			(1+DSTF_G354)     // 
#define DSTF_G398    			(1+DSTF_G376)     // Total 20 registers

//*****************************************************************************************
//                              Analog Scaling Coef.
//  This field defines the analog scale with two bits feedback capacitance switch control
//  in each receiver channel of group 0 while transmitter is scanning channel 0 as below
//*****************************************************************************************
#define ASCA_T0G0    			0x2000             // t0g0[15:0]
#define TXCH_M00A    			0x2001             // map0[11:0], t0g0[19:0]
#define TXCH_M00B    			0x2002             // map0[27:12]
#define TXCH_M00C    			0x2003             // map0[39:28]

#define ASCA_T0G1    			0x2004             // 
#define TXCH_M01A    			0x2005             // 
#define TXCH_M01B    			0x2006             // 
#define TXCH_M01C    			0x2007             // 

#define ASCA_T0G2    			0x2008             // 
#define TXCH_M02A    			0x2009             // 
#define TXCH_M02B    			0x200A             // 
#define TXCH_M02C    			0x200B             // 

#define ASCA_T0G3    			0x200C             // 
#define TXCH_M03A    			0x200D             // 
#define TXCH_M03B    			0x200E             // 
#define TXCH_M03C    			0x200F             // 


#define ASCA_T1G0    			0x2010             // 
#define TXCH_M10A    			0x2011             // 
#define TXCH_M10B    			0x2012             // 
#define TXCH_M10C    			0x2013             // 

#define ASCA_T1G1    			0x2014             // 
#define TXCH_M11A    			0x2015             // 
#define TXCH_M11B    			0x2016             // 
#define TXCH_M11C    			0x2017             // 

#define ASCA_T1G2    			0x2018             // 
#define TXCH_M12A    			0x2019             // 
#define TXCH_M12B    			0x201A             // 
#define TXCH_M12C    			0x201B             // 

#define ASCA_T1G3    			0x201C             // 
#define TXCH_M13A    			0x201D             // 
#define TXCH_M13B    			0x201E             // 
#define TXCH_M13C    			0x201F             // 

#define ASCA_T2G0    			0x2020             // 
#define ASCA_T3G0    			0x2030             // 
#define ASCA_T4G0    			0x2040             // 
#define ASCA_T5G0    			0x2050             // 
#define ASCA_T6G0    			0x2060             // 
#define ASCA_T7G0    			0x2070             // 
#define ASCA_T8G0    			0x2080             // 
#define ASCA_T9G0    			0x2090             // 
#define ASCA_TAG0    			0x20A0             // 
#define ASCA_TBG0    			0x20B0             // 
#define ASCA_TCG0    			0x20C0             // 
#define ASCA_TDG0    			0x20D0             // 
#define ASCA_TEG0    			0x20E0             // 

#define ASCA_TFG0    			0x20F0             // 
#define TXCH_MF0A    			0x20F1             // 
#define TXCH_MF0B    			0x20F2             // 
#define TXCH_MF0C    			0x20F3             // 

#define ASCA_TFG1    			0x20F4             // 
#define TXCH_MF1A    			0x20F5             // 
#define TXCH_MF1B    			0x20F6             // 
#define TXCH_MF1C    			0x20F7             // 

#define ASCA_TFG2    			0x20F8             // 
#define TXCH_MF2A    			0x20F9             // 
#define TXCH_MF2B    			0x20FA             // 
#define TXCH_MF2C    			0x20FB             // 

#define ASCA_TFG3    			0x20FC             // 
#define TXCH_MF3A    			0x20FD             // 
#define TXCH_MF3B    			0x20FE             // 
#define TXCH_MF3C    			0x20FF             // 

#endif
