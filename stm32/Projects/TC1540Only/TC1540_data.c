/******************************************************************************
 * 版权所有(C) TRUECOREING
 * DEPARTMENT:
 * MANUAL_PERCENT:
 * 文件名称: CN1000_data.c 
 * 文件标识:    
 * 内容摘要: 
 * 其它说明:
 * 当前版本: 
 * 作    者: 
 * 完成日期:
 * 当前责任人: 
 *
 * 修改记录1: 代码合规
 * 修改日期: 2014-09-15
 * 版 本 号:
 * 修 改 人: Wangpc
 * 修改内容: 
 *****************************************************************************/

#ifndef CN1540_DATA_C
#define CN1540_DATA_C

#include "CN1100_common.h"
#ifdef CN1100_STM32
#include "CN1100_STM32.h"
#endif
#ifdef CN1100_LINUX
#include "CN1100_linux.h"
#endif
#include "CN1100_Function.h"

inline int16_t abs16(int16_t x) { return (x < 0) ? -x : x; }
inline uint32_t multiply16by16(uint16_t x, uint16_t y) {  return (uint32_t) x * y; }


/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
/*this function is used for reducing the time of  divide*/
int16_t divideINT16byUINT8(int16_t num, uint8_t den)
{
    uint8_t para_div[13] ={85,64,51,42,36,32,28,25,23,21,19,18,17};
    
    if(0 == den)
    {
        return MAX_INT16;
    }
    else if(den >= 1 && den <= 2)
    {
    
        return num>>(den-1);
    }
    else if(den >= 3 && den <= 15)
    {
    
        return (num * para_div[den - 3])>>8;
    }
    else
    {
        return num/den;
    }
}

/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
uint16_t msbPos(uint32_t val) 
{
    uint16_t pos = 0;
    
    if ((val & 0xFFFF0000) != 0)
    {
        pos += 16;
        val >>= 16;
    }
    if ((val & 0xFFFFFF00) != 0) 
    {
        pos += 8;
        val >>= 8;
    }
    if ((val & 0xFFFFFFF0) != 0) 
    {
        pos += 4;
        val >>= 4;
    }
    if ((val & 0xFFFFFFFC) != 0)
    {
        pos += 2;
        val >>= 2;
    }
    if ((val & 0xFFFFFFFE) != 0)
    {
        pos += 1;
    }

    return (pos);
}



/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
uint16_t lsbPos(uint32_t val) 
{

    uint16_t pos = 0;
    
    if ((val & 0x0000FFFF) == 0)
    {
        pos += 16;
        val >>= 16;
    }
    if ((val & 0x000000FF) == 0) 
    {
        pos += 8;
        val >>= 8;
    }
    if ((val & 0x0000000F) == 0)
    {
        pos += 4;
        val >>= 4;
    }
    if ((val & 0x00000003) == 0)
    {
        pos += 2;
        val >>= 2;
    }
    if ((val & 0x00000001) == 0)
    {
        pos += 1;
    }
    
    return (pos);
}



/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
void clearArray(uint16_t* p, uint16_t size)
{
    uint16_t i;
    for (i = 0;i< size;i++)
    {
        *p = 0;
        p++;
    }
}


/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/

void Baseline_FingerExistedHandled(uint16_t *buffer)
{
    uint16_t Min_num,Max_num,i,j;
    int16_t Min_sum,Max_sum;
    Min_num = 0;
    Min_sum = 0;
    Max_num = 0;
    Max_sum = 0;
    if(bdt.MinValueInFrame < (0-(bdt.PCBA.FrameMaxSample<<1)))
    {
        if(bdt.MEM_MIN_XY_Count > ABNORMAL_HOLD_TIME)
        {
            if((ABNORMAL_HOLD_TIME+1) == bdt.MEM_MIN_XY_Count)
            {
                bdt.PowerOnWithFinger = 1;
                return ;
            }
        
        }
        
       /*****************************************************************************
        * 完全相等，似乎是有些太强的条件约束了
        *****************************************************************************/
        if((bdt.MEM_MIN_X_XMTR == bdt.FRM_MIN_X_XMTR) && (bdt.MEM_MIN_Y_RECV == bdt.FRM_MIN_Y_RECV))
        {
            for(i=0;i<XMTR_NUM;i++)
            for(j=0;j<RECV_NUM;j++)
            {
                if(bdt.DeltaDat16A[i][j] >= bdt.PCBA.FrameMaxSample)
                {
                    Max_num++;
                    Max_sum += bdt.DeltaDat16A[i][j];
                }else if(bdt.DeltaDat16A[i][j] <= (0 - bdt.PCBA.FrameMaxSample))
                {
                    Min_num++;
                    Min_sum += bdt.DeltaDat16A[i][j];
                }
            }
            
            if(0 ==  Max_num)
            {
                bdt.MEM_MIN_XY_Count++;
            }
            else if(Min_num != 0)
            {
                if((Max_sum/Max_num) + (Min_sum/Min_num) < 5)
                {
                    bdt.MEM_MIN_XY_Count++;
                }
            }
        }
        else
        {
            bdt.MEM_MIN_XY_Count = 0;
        }
    }
    bdt.PowerOnWithFinger  = 0;
}



/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
uint8_t Baseline_HugeInvalidDatainDeltaFrame(void)
{
    uint8_t result = BASE_FRAME_HOLD;
    uint16_t Min_num,Max_num,i,j;
    int16_t Min_sum,Max_sum;
    Min_num = 0;
    Min_sum = 0;
    Max_num = 0;
    Max_sum = 0;

   /*********************************************************************************
   * If we have large value ,update base data, +100/-100
   *********************************************************************************/
    if(bdt.MaxValueInFrame > bdt.PCBA.FrameMaxSample)
    {
        if(bdt.MEM_MAX_XY_Count > ABNORMAL_HOLD_TIME)
        {
           /*bdt.MEM_MAX_XY_Count = 0;*/
            result = BASE_FRAME_UPDATE;
        }
        
       /*****************************************************************************
       * 完全相等，似乎是有些太强的条件约束了
       *****************************************************************************/
        if((bdt.MEM_MAX_X_XMTR == bdt.FRM_MAX_X_XMTR) && (bdt.MEM_MAX_Y_RECV == bdt.FRM_MAX_Y_RECV))
        {
            bdt.MEM_MAX_XY_Count++;
        }
        else
        {
            bdt.MEM_MAX_XY_Count = 0;
        }
    }
    else 
    {
        bdt.MEM_MAX_XY_Count = 0;
    }
    
    if(bdt.MinValueInFrame<(0-bdt.PCBA.FrameMaxSample))
    {
       /*CN1100_print("2."); */
        if(bdt.MEM_MIN_XY_Count > ABNORMAL_HOLD_TIME)
        {
           /*bdt.MEM_MIN_XY_Count = 0; */
            result = BASE_FRAME_UPDATE;
        }
        
       /*****************************************************************************
       * 完全相等，似乎是有些太强的条件约束了
       *****************************************************************************/
       /*CN1100_print("X0,X1,Y0,Y1=%d,%d,%d,%d\n",bdt.MEM_MIN_X_XMTR,bdt.FRM_MIN_X_XMTR,bdt.MEM_MIN_Y_RECV,bdt.FRM_MIN_Y_RECV);*/
        
        if((bdt.MEM_MIN_X_XMTR == bdt.FRM_MIN_X_XMTR) && (bdt.MEM_MIN_Y_RECV == bdt.FRM_MIN_Y_RECV))
        {
            for(i=0;i<XMTR_NUM;i++)
            for(j=0;j<RECV_NUM;j++)
            {
                if(bdt.DeltaDat16A[i][j] >= bdt.PCBA.FrameMaxSample)
                {
                    Max_num++;
                    Max_sum += bdt.DeltaDat16A[i][j];
                }else if(bdt.DeltaDat16A[i][j] <= (0 - bdt.PCBA.FrameMaxSample))
                {
                    Min_num++;
                    Min_sum += bdt.DeltaDat16A[i][j];
                }
            }
            if(0 ==  Max_num)
            {
                bdt.MEM_MIN_XY_Count++;
            }
            else if(Min_num != 0)
            {
                if((Max_sum/Max_num) + (Min_sum/Min_num) < 5)
                {
                    bdt.MEM_MIN_XY_Count++;
                }
            }
        }
        else
        {
            bdt.MEM_MIN_XY_Count = 0;
        }
    }
    else
    {
        bdt.MEM_MIN_XY_Count = 0;
    }
    return result;
}



/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
uint8_t Baseline_ManyBigInvalidSampleinDeltaFrame(void)
{
    uint16_t i, j;
    uint32_t temp_count = 0;
    
  /***********************************************************************
       * 如果一个差分帧内:
       *    数值超过"bdt.PCBA.AbnormalMaxDiff = ABNORMAL_MAX_DIFF(15)"
       *    个数超过"bdt.PCBA.AbnormalNumDiff = ABNORMAL_NUM_DIFF(1/4帧)"
       *    次数超过 ABNORMAL_HOLD_TIME(5)
       ***********************************************************************/
    for (i=0;i<XMTR_NUM;i++)
    for (j=0;j<RECV_NUM;j++)
    {
        if(abs16(bdt.DeltaDat16A[i][j]) <= bdt.ThresholdInFrame)
        if(abs16(bdt.DeltaDat16A[i][j]) >= bdt.PCBA.AbnormalMaxDiff)
            temp_count++;
    }
    
    if(temp_count > bdt.PCBA.AbnormalNumDiff)
    {
        bdt.BFD.AbnormalUpdateDelay++;
        if(bdt.BFD.AbnormalUpdateDelay > ABNORMAL_HOLD_TIME)
        {
            return BASE_FRAME_UPDATE;
        }
    }
    else
    {
        bdt.BFD.AbnormalUpdateDelay = 0;
    }
    return BASE_FRAME_HOLD;
}


/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/

uint8_t Baseline_IsNeedUpdateBaseBuffer(void)
{
    uint16_t i, j;
    uint8_t  result = BASE_FRAME_HOLD;
    
   /*********************************************************************************
        * Step 1: Compute the sum of delta data
        *********************************************************************************/
    bdt.BFD.DeltaSum = 0;
    for (i=0;i<XMTR_NUM;i++)
    for (j=0;j<RECV_NUM;j++)
    {
        bdt.BFD.DeltaSum += abs16(bdt.DeltaDat16A[i][j]);
    }

    
   /*********************************************************************************
        * Step 2: We have two cases to handle
        *      Case 1: It's time to update, we should make sure that we do it or not.
        *      Case 2: It's Time to hold, but already updated base for a while
        *********************************************************************************/
    switch(bdt.BFD.bbdc.BaseUpdateCase)
    {
        case BASELINE_UPDATING_CASE:
        {
           /*************************************************************************
                        * 这是主动要Update Baseline的时刻
                        *************************************************************************/
            if(bdt.BFD.AfterBaseUpdatedTime < MAX_HOLDTIME_AFTERUPDATE)
            {
                return BASE_FRAME_HOLD;
            }
            
            if(bdt.BFD.FingerLeftProtectTime < bdt.PCBA.ProtectTime) 
            {
                return BASE_FRAME_HOLD;
            }
            
            if(bdt.BFD.TooLongTime4BaseUpdate >= bdt.PCBA.MaxUpdateTime)
            {
               /**********************************************************
                                * 主动要Update的时刻，差分值一直都够小，很久了...
                                **********************************************************/
                bdt.BFD.bbdc.BaseUpdateCase = BASELINE_HOLDING_CASE;
                bdt.BFD.TooLongTime4BaseUpdate = 0;
            }
            else
            {
               /**********************************************************
                                * 主动要Update的时刻，必须差分值够小才可以做这件事
                                **********************************************************/
                if(bdt.BFD.DeltaSum < (bdt.BFD.DeltaSumMaxThreshold) ) 
                {
                    return BASE_FRAME_UPDATE;
                }
                else
                {
                    return BASE_FRAME_HOLD;
                }
            }
            break;
        }
        case BASELINE_HOLDING_CASE:
        {
            if(BASE_FRAME_UPDATE == Baseline_HugeInvalidDatainDeltaFrame())
            {
                return BASE_FRAME_UPDATE;
            }
            
            if(BASE_FRAME_UPDATE == Baseline_ManyBigInvalidSampleinDeltaFrame())
            {
                return BASE_FRAME_UPDATE;
            }
            
           /*********************************************************************************
           *  If Update base data is not happened for more than MaxUpdateTime,
           *  update base data shall be happened 
                        *********************************************************************************/
            if(bdt.BFD.TooLongTime4BaseUpdate >= bdt.PCBA.MaxUpdateTime)
            {
               /*****************************************************************************
               * It is too loog without update, we have to update base now;
               *****************************************************************************/
                bdt.BFD.bbdc.BaseUpdateCase = BASELINE_UPDATING_CASE;
                bdt.BFD.TooLongTime4BaseUpdate = 0;
            }
            break;
        }
        default:
            break;
    }
    
    return result;
}



/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
void Baseline_BaseBufferHandled(uint16_t *buffer)
{
    int i, j;
    
   /************************************************************************
   * After finger is just left, Hold for Protection Time
   ************************************************************************/
    if(bdt.BFD.FingerLeftProtectTime  < bdt.PCBA.ProtectTime)
    {    
        bdt.BFD.FingerLeftProtectTime++;
    }
   /************************************************************************
   * Too Long for us not to Update Baseline: MAX_MUST_UPDATE_PERIOD(4000)
   ************************************************************************/
    if(bdt.BFD.TooLongTime4BaseUpdate < bdt.PCBA.MaxUpdateTime)
    {
        bdt.BFD.TooLongTime4BaseUpdate++;
    }    
   /************************************************************************
   * After Updating Baseline, Hold for: MAX_HOLDTIME_AFTERUPDATE (2000)
   ************************************************************************/
    if(bdt.BFD.AfterBaseUpdatedTime < MAX_HOLDTIME_AFTERUPDATE) 
    {
        bdt.BFD.AfterBaseUpdatedTime++;
    }

    #ifdef AUTO_FACEDETECTION

    if(FACE_DETECT_NEAR == bdt.FDC.Flag) return;

    #endif
	
    if(bdt.BFD.bbdc.FingerExist == NO_FINGER)
    {
        #ifdef DOZE_ALLOWED
       /***********************************************************
       *        This will be useless actually (QFU)
       * Rewrite PERD_REG when the mode will change to DOZE_MODE
       ***********************************************************/
        if(WORK_MODE_NOFING_MAXPERD == bdt.MTD.NoFingerCnt4Doze)
        if(iAUTOSCAN_MODE == bdt.ModeSelect)
        {
            #ifdef COMMUNICATION_WITH_PC
            SPI_write_singleData(PERD_REG, ISCANMODE_PERD_REG_VALUE_NORMAL);
            #else
            SPI_write_singleData(PERD_REG, 0x200);
            #endif
        }
        #endif
        
       /*******************************************************
       * Record how many time for the case of NO_FINGER
       *******************************************************/
        bdt.MTD.NoFingerCnt4Doze++;
        
        if(BASE_FRAME_HOLD == Baseline_IsNeedUpdateBaseBuffer())
        {
            /***************************************************
            * 基准值不需要更新, 于此故意设为有手指
            ***************************************************/
            bdt.BFD.bbdc.PreFingerExt = FINGER_SHOW;
            return;
        }
        
        if(bdt.BFD.bbdc.PreFingerExt != bdt.BFD.bbdc.FingerExist)
        {
           /*******************************************************
            * 更新基准数据，于此是第一帧，总共要4帧才得完成
            *******************************************************/
            bdt.BFD.bbdc.NoFingerCnt4Base = 0;
            for (i=0;i<XMTR_NUM;i++)
            for (j=0;j<RECV_NUM;j++)
            {
                bdt.BFD.SumBaseDat[i][j] = buffer[ (i*RECV_NUM) + j];
            }
        }
        else
        {
            for (i=0;i<XMTR_NUM;i++)
            for (j=0;j<RECV_NUM;j++)
            {
                bdt.BFD.SumBaseDat[i][j] += buffer[ (i*RECV_NUM) + j];
            }
        }
        
        bdt.BFD.bbdc.NoFingerCnt4Base++;
        
        if(BASE_COUNT_MAX == bdt.BFD.bbdc.NoFingerCnt4Base)
        {
          /****************************************************************
          * 四帧无手指原始数据求和，得到基准数据
          ****************************************************************/
            bdt.BaseChangeFlag++;               /* Counting 1 for baseline updating */
            bdt.BFD.TooLongTime4BaseUpdate = 0; /* Reset the Timing Count */
            bdt.BFD.AfterBaseUpdatedTime   = 0; /* Reset the Timing Count */
            bdt.MEM_MAX_XY_Count = 0;
            bdt.BFD.bbdc.BaseUpdateCase    = BASELINE_HOLDING_CASE;
            
            for(i=0;i<XMTR_NUM;i++)
            for(j=0;j<RECV_NUM;j++)
            {
                  /************************************************
                  * 1. 利用上次得到的基准数据
                  * 2. 记录这次得到的基准数据，以备下次使用
                  ************************************************/
                bdt.BFD.BaseDat[i][j]      = bdt.BFD.BaseDatSaved[i][j]; 
                bdt.BFD.BaseDatSaved[i][j] = (bdt.BFD.SumBaseDat[i][j])>>BASE_FRAME_SHIFT;
                bdt.BFD.SumBaseDat[i][j]   = 0;
            }
            
           /*****************************************************************
           * Reset the Base Data Frame Integration
           *****************************************************************/
            bdt.BFD.bbdc.NoFingerCnt4Base = 0;
        }
    }
    else
    {
        bdt.BFD.bbdc.NoFingerCnt4Base = 0;
        bdt.BFD.FingerLeftProtectTime = 0;
        
        Baseline_FingerExistedHandled(buffer);
        
        if(0 == bdt.PrevFingerDetectNum2)
        {
            for (i=0;i<XMTR_NUM;i++)
            for (j=0;j<RECV_NUM;j++)
            {
                bdt.BFD.BaseDatSaved[i][j] = bdt.BFD.BaseDat[i][j];
            }
            #ifdef DOZE_ALLOWED
           /***********************************************************
                        *        This will be useless actually (QFU)
                        * Set PERD_REG to normal
                        ***********************************************************/
            if(bdt.ModeSelect == iAUTOSCAN_MODE && bdt.MTD.NoFingerCnt4Doze == WORK_MODE_NOFING_MAXPERD)
            {
                #ifdef COMMUNICATION_WITH_PC
                if(DEBUG_INFO_NONE == dbg.DebugInfoLevel)
                {
                    SPI_write_singleData(PERD_REG, ISCANMODE_PERD_REG_VALUE);
                }
                else if(DEBUG_INFO_FIGLOC == dbg.DebugInfoLevel)
                {
                    SPI_write_singleData(PERD_REG, ISCANMODE_PERD_REG_VALUE_FINGER_ONLY);
                }
                else
                {
                    SPI_write_singleData(PERD_REG, ISCANMODE_PERD_REG_VALUE_NORMAL);
                }
                #else
                {
                    SPI_write_singleData(PERD_REG, ISCANMODE_PERD_REG_VALUE);
                }
                #endif
            }
            #endif
        }
        
       /*******************************************************
                * Reset the Record = 0 when any finger is coming
                *******************************************************/
        bdt.MTD.NoFingerCnt4Doze = 0;
    }
    
   /****************************************
        * Save the Finger existing status
        ****************************************/
    
    bdt.BFD.bbdc.PreFingerExt = bdt.BFD.bbdc.FingerExist;
}



/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
#ifdef BORDER_ADJUSTABLE
uint16_t FingProc_NolinearMapping(int8_t border,uint16_t h)
{
    uint16_t  sub_1=0,sub_2=0,sub_3=0,sub_4=0,sub_5=0,sub_6=0,sub_7=0,result;
    uint16_t  temp_1,temp_2,temp_3,temp_4,temp_5,temp_6,temp_7,temp_h;
    int16_t border_zone;
    temp_h = h;  
    if(1==border)
    {    
        border_zone = BORDERZONE_L;
    }
    else if (2==border) 
    {
        border_zone = BORDERZONE_R;
    }
    else if (3==border)
    {
        border_zone = BORDERZONE_T;
    }
    else
    {
        border_zone = BORDERZONE_B;
    }
    
    if(border_zone>-14) 
    {
        sub_1 = 14+border_zone;
        sub_2 = 28+border_zone;
        sub_3 = 42+border_zone;
        sub_4 = 56+border_zone;
        sub_5 = 70+border_zone;
        sub_6 = 84+border_zone;
        sub_7 = 98+border_zone;
    }
    else if(border_zone>-28) 
    {
        sub_2 = 28+border_zone;
        sub_3 = 42+border_zone;
        sub_4 = 56+border_zone;
        sub_5 = 70+border_zone;
        sub_6 = 84+border_zone;
        sub_7 = 98+border_zone;
    }
    else if(border_zone>-42) 
    {
        sub_3=42+border_zone;
        sub_4=56+border_zone;
        sub_5=70+border_zone;
        sub_6=84+border_zone;
        sub_7=98+border_zone;
    }
    else if(border_zone>-56) 
    {
        sub_4=56+border_zone;
        sub_5=70+border_zone;
        sub_6=84+border_zone;
        sub_7=98+border_zone;
    }
    else if(border_zone>-70) 
    {
        sub_5=70+border_zone;
        sub_6=84+border_zone;
        sub_7=98+border_zone;
    }
    else if(border_zone>-84) 
    {
        sub_6=84+border_zone;
        sub_7=98+border_zone;
    }
    else if(border_zone>-98)
    {
        sub_7=98+border_zone;
    }
    
    temp_1=sub_1+((sub_2-sub_1)>>3);                                      /*1.125*/
    temp_2=temp_1+((sub_3-sub_2))+((sub_3-sub_2)>>2);                     /*1.25 */
    temp_3=temp_2+(sub_4-sub_3)+((sub_4-sub_3)>>1);                       /*1.5*/
    temp_4=temp_3+(sub_5-sub_4)+((sub_5-sub_4)>>1)+((sub_5-sub_4)>>2);    /*1.75*/
    temp_5=temp_4+((sub_6-sub_5)<<1);                                     /*2.00 */
    temp_6=temp_5+((sub_7-sub_6)<<1)+(sub_7-sub_6);                       /*3.00*/
    temp_7=temp_6+((h-sub_7)<<1)+((h-sub_7)<<1)+((h-sub_7)>>1);           /*4.50 */
    
    if(h<sub_1)
    {
        result = h + (h>>3);
    }                           /* 1.125*/
    else if(h<sub_2)
    {
        h -= sub_1; 
        result = temp_1 + h + (h>>2);
    }                  /*1.25*/
    else if(h<sub_3)
    {
        h -= sub_2;
        result = temp_2 + h + (h>>1);
    }               /*1.5*/
    else if(h<sub_4) 
    {
        h -= sub_3; 
        result = temp_3 + h + (h>>1) + (h>>2);
    }         /*1.75*/
    else if(h<sub_5) 
    {
        h -= sub_4; 
        result = temp_4 + (h<<1);
    }                       /* 2.00*/
    else if(h<sub_6)
    {
        h -= sub_5; 
        result = temp_5 + (h<<1) + h;
    }               /*3.00*/
    else if(h<sub_7)
    {
        h -= sub_6;
        result = temp_6 + (h<<1) + (h<<1) + (h>>1);
    }      /*4.50  211(14) */
    else 
    {
        h -= sub_7; 
        result = temp_7 + (h<<1) + (h<<1) + h;
    }                  /* 5.00  */ 
    
    if(1 == border)  
    {
        if(result >= (MAX_MAP_VALUE - BORDERDIS_L))
        {
            result = MAX_MAP_VALUE - BORDERDIS_L;
        }
        if(temp_h>BORDER_TOMAXVALUE_L)
        { 
            result = MAX_MAP_VALUE- BORDERDIS_L;
        }    
    }
    else if(2 == border)    
    {
        if(result >= (MAX_MAP_VALUE - BORDERDIS_R))
        {
            result = MAX_MAP_VALUE - BORDERDIS_R;
        }
        if(temp_h>BORDER_TOMAXVALUE_R)
        {
            result = MAX_MAP_VALUE- BORDERDIS_R;
        }
    }
    else if(3 == border)    
    {
        if(result >= (MAX_MAP_VALUE - BORDERDIS_T)) 
        {
            result = MAX_MAP_VALUE - BORDERDIS_T;
        }
        if(temp_h>BORDER_TOMAXVALUE_T)
        {
            result = MAX_MAP_VALUE- BORDERDIS_T;
        }
    }
    else      
    {
        if(result >= (MAX_MAP_VALUE - BORDERDIS_B)) 
        {
            result = MAX_MAP_VALUE - BORDERDIS_B;
        }
        if(temp_h>BORDER_TOMAXVALUE_B)
        {
            result = MAX_MAP_VALUE- BORDERDIS_B;
        }
    }
                        
    return result;

}
#endif


/*******************************************************************************
* Function Name  : FingProc_Dist2PMeasure
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
uint16_t FingProc_Dist2PMeasure(int16_t x1, int16_t y1, int16_t x2, int16_t y2)
{
    int16_t x, y, t; 
    int16_t distance;
    
    x = x1 - x2;
    y = y1 - y2;
    if(x<0) 
    {
        x = 0 - x;
    }
    if(y<0)
    {
        y = 0 - y;
    }
    t = y - x;
    if(t>=0)
    {
        y -= t; 
        x += t;
    }
    distance = x + (y>>3) + (y>>2);
    return ((uint16_t)distance);
}


/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
uint16_t FingProc_Dist4Uint16Var(uint16_t x0, uint16_t x1)
{
    if(x0>x1) 
    {
        return (x0-x1);
    }
    else
    { 
        return (x1-x0);
    }
}


/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/

void FingProc_DistanceFilter(uint16_t i, uint16_t x1, uint16_t y1, uint16_t *pX, uint16_t *pY)
{
}


/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/

void FingProc_DistanceFilter0(uint16_t i, uint16_t x1, uint16_t y1, uint16_t *pX, uint16_t *pY)
{
    #if 1
    uint16_t distance;

    #define EDGE_NUMR 8
    if((0 == bdt.DPD[i].AdjustOrigin_x) && (0 == bdt.DPD[i].AdjustOrigin_y))
    {
        bdt.DPD[i].AdjustOrigin_x = x1;
        bdt.DPD[i].AdjustOrigin_y = y1;
    }
    
    distance = FingProc_Dist2PMeasure((int16_t)bdt.DPD[i].AdjustOrigin_x, (int16_t)bdt.DPD[i].AdjustOrigin_y, (int16_t)(*pX), (int16_t)(*pY));
   
    
    switch(bdt.DPD[i].AdjustState)
    {
     /*************************************************************
     * 分成两种CASE, 是为了 在中途case "STATE_STICK_HERE" 可以更好
     * 地安排 手指报告点 接近 手指触摸物理点的 真实位置。这还没做!
     **************************************************************/
        case STATE_STICK_START:
        case STATE_STICK_HERE:
        {
            bdt.DPD[i].AdjustDistance = bdt.ThrLow4DistanceFilter;
            if(distance <= bdt.DPD[i].AdjustDistance)
            {                
                *pX = bdt.DPD[i].AdjustOrigin_x; /* 步伐太小, 屹然不动*/
                *pY = bdt.DPD[i].AdjustOrigin_y; /* 步伐太小, 屹然不动*/
            }
            else if(distance <= bdt.ThrHigh4DistanceFilter) 
            {
                *pX = bdt.DPD[i].AdjustOrigin_x; /* 步伐明显, 可能要动*/
                *pY = bdt.DPD[i].AdjustOrigin_y; /* 步伐明显, 可能要动*/
                bdt.DPD[i].AdjustState = STATE_PREPARE_MOVE;
            }
            else 
            {
                bdt.DPD[i].AdjustOrigin_x = *pX; /* 步伐很大, 突然要动*/
                bdt.DPD[i].AdjustOrigin_y = *pY; /* 步伐很大, 突然要动*/
                bdt.DPD[i].AdjustState = STATE_MOVING;
            }
            break;
        }
        case STATE_PREPARE_MOVE:
        {
            if(distance <= bdt.DPD[i].AdjustDistance)
            {
                *pX = bdt.DPD[i].AdjustOrigin_x; /* 貌似阻断, 恍然不动*/
                *pY = bdt.DPD[i].AdjustOrigin_y; /* 貌似阻断, 恍然不动*/ 
                bdt.DPD[i].AdjustState = STATE_STICK_HERE;
            }
            else if(distance <= bdt.ThrHigh4DistanceFilter) 
            {
                if((FingProc_Dist4Uint16Var(*pX,x1) > bdt.DPD[i].AdjustDistance) && (FingProc_Dist4Uint16Var(*pY,y1) < THR024))
                {
                    *pX = bdt.DPD[i].AdjustOrigin_x; /* 有点蹊跷, 暂时不动*/
                    *pY = bdt.DPD[i].AdjustOrigin_y; /* 有点蹊跷, 暂时不动*/
                }
                else 
                {
                    *pX = bdt.DPD[i].AdjustOrigin_x; /* 名副其实, 确实要动*/
                    *pY = bdt.DPD[i].AdjustOrigin_y; /* 名副其实, 确实要动*/
                    bdt.DPD[i].AdjustState = STATE_MOVING;
                }
            }
            else
            {
                bdt.DPD[i].AdjustState = STATE_MOVING;
                bdt.DPD[i].AdjustOrigin_x = *pX; /* 快马加鞭, 立即行动*/
                bdt.DPD[i].AdjustOrigin_y = *pY; /* 快马加鞭, 立即行动*/
            }
            break;
        }
        case STATE_MOVING:
        {
            bdt.DPD[i].AdjustDistance = THR024;
            if(distance <= 12)
            {
                *pX = bdt.DPD[i].AdjustOrigin_x;  /*动中有静, 考虑不动*/
                *pY = bdt.DPD[i].AdjustOrigin_y;  /* 动中有静, 考虑不动*/
                bdt.DPD[i].AdjStickCounter = 2;   
                bdt.DPD[i].AdjustState = STATE_PREPARE_STICK;
                
            }
            if(distance <= bdt.DPD[i].AdjustDistance)
            {
                *pX = bdt.DPD[i].AdjustOrigin_x; /* 动中有静, 考虑不动*/
                *pY = bdt.DPD[i].AdjustOrigin_y; /* 动中有静, 考虑不动*/
                bdt.DPD[i].AdjStickCounter = 0;  
                bdt.DPD[i].AdjustState = STATE_PREPARE_STICK;
               
            }
            else 
            {
                bdt.DPD[i].AdjustOrigin_x = *pX; /* 持续行动, 记录轨迹*/
                bdt.DPD[i].AdjustOrigin_y = *pY; /* 持续行动, 记录轨迹*/
            }
            break ;
        }
        case STATE_PREPARE_STICK:
        {
            bdt.DPD[i].AdjustDistance = bdt.ThrLow4DistanceFilter;
            if(distance <= bdt.DPD[i].AdjustDistance)
            {
                *pX = bdt.DPD[i].AdjustOrigin_x; /* 先行静谧, 再看趋势*/
                *pY = bdt.DPD[i].AdjustOrigin_y; /* 先行静谧, 再看趋势*/
                if(bdt.DPD[i].AdjStickCounter >= 3)
                {
                    bdt.DPD[i].AdjStickCounter = 0;          /* 静止有时, 可以换挡*/
                    bdt.DPD[i].AdjustState = STATE_STICK_HERE;
                }
                else
                {
                    bdt.DPD[i].AdjStickCounter++;            /* 静止进行, 持续加码*/
                }
            }
            else if(distance <= THR144)
            {
                if((FingProc_Dist4Uint16Var(*pX,x1) > THR048) && (FingProc_Dist4Uint16Var(*pY,y1) < THR024))
                {
                    *pX = bdt.DPD[i].AdjustOrigin_x; /*静中假动, 认真考量*/
                    *pY = bdt.DPD[i].AdjustOrigin_y; /* 静中假动, 认真考量*/
                }
                else 
                {
                    bdt.DPD[i].AdjStickCounter = 0;
                    *pX = bdt.DPD[i].AdjustOrigin_x; /* 静中有动, 还得行动*/
                    *pY = bdt.DPD[i].AdjustOrigin_y; /* 静中有动, 还得行动*/
                    bdt.DPD[i].AdjustState = STATE_MOVING;
                }
            }
            else
            {
                bdt.DPD[i].AdjStickCounter = 0;
                bdt.DPD[i].AdjustOrigin_x = *pX;    /* 静是假静, 动是真动*/
                bdt.DPD[i].AdjustOrigin_y = *pY;    /* 静是假静, 动是真动*/
                bdt.DPD[i].AdjustState = STATE_MOVING;
            }
            break;
        }
    }
    #endif
}

/*************************************************************
* i: 手指 ID
* x1,y1: 手指在前一帧的坐标
* pX,pY: 手指在本帧的坐标的数据的变量指针
**************************************************************/
/*******************************************************************************
 * Function Name  : 
 * Description    : 
 * Input          : 
 * Output         : 
 * Return         : 
 *******************************************************************************/

void FingProc_DistanceFilter1(uint16_t i, uint16_t x1, uint16_t y1, uint16_t *pX, uint16_t *pY)
{
}


/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/

uint16_t FingProc_SingleForwardFilter(uint16_t i, uint16_t xoldest, uint16_t xolder, uint16_t curx, uint16_t MAX)
{
    int16_t px;
    
    if(bdt.DPD[i].FingerStateFlag >= STATE_LOST_1_FINGER)
    {
        px = (int16_t)xolder;
    }
    else
    {
        if(FINGER_FINGER_SPEED_FAST == bdt.DPD[i].FingMovingSpeed)
        {
            px = (int16_t)curx;
        }
        else 
        {
            px   = (int16_t)(xolder<<1) - (int16_t)xoldest; 
            px   = ((int16_t)curx + px)>>1;
        }
    }
    if(px <= 0)   
    {
        px = 1;
    }
    if(px >= MAX) 
    {
        px = MAX - 1;
    }
    return (uint16_t)px;
}


/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
void FingProc_ForwardSmoothLine(uint16_t i, uint16_t *xs, uint16_t *ys)
{
    int16_t x1, y1, x2, y2;
    int16_t curx, cury, dist;
    int16_t x4, y4, x5, y5;
    
    x1   = bdt.DPD[i].Finger_X_S[1]; /* Oldest*/
    y1   = bdt.DPD[i].Finger_Y_S[1]; /* Oldest*/
    x2   = bdt.DPD[i].Finger_X_S[0]; /* Older*/
    y2   = bdt.DPD[i].Finger_Y_S[0]; /* Older*/
    curx = bdt.DPD[i].Finger_X_XMTR; /* Current*/
    cury = bdt.DPD[i].Finger_Y_RECV; /* Current*/
    dist = FingProc_Dist2PMeasure(x2, y2, curx, cury);
    
    if(bdt.DPD[i].FingerStateFlag >= STATE_LOST_1_FINGER) /* LOST Finger Point*/
    {
        x5 = x2;
        y5 = y2;
    }
    else if(FINGER_FINGER_SPEED_FAST == bdt.DPD[i].FingMovingSpeed) /* Too Fast Finger Moving*/
    {
        x5 = curx;
        y5 = cury;
    }
    else if( (dist>>2) > bdt.DPD[i].FF_LastDist) /* Too High Speed for time being*/
    {
        x5 = curx; 
        y5 = cury;
    }
    else
    {
        x4 = (x2<<1) - x1;
        y4 = (y2<<1) - y1;
        x5 = (curx + x4)>>1;
        y5 = (cury + y4)>>1;
        if(x5 < 0)
        { 
            x5 = 1;
        }
        if(y5 < 0)
        { 
            y5 = 1;
        }
        if(x5 >= (XMTR_NUM<<8)) 
        { 
            x5 = (XMTR_NUM<<8) - 1;
        }
        if(y5 >= (RECV_NUM<<8))
        { 
        y5 = (RECV_NUM<<8) - 1;
        }
    }
    
    bdt.DPD[i].Finger_X_S[0] = x5;
    bdt.DPD[i].Finger_Y_S[0] = y5;
    bdt.DPD[i].Finger_X_S[1] = x2;
    bdt.DPD[i].Finger_Y_S[1] = y2;
    bdt.DPD[i].FF_LastDist   = FingProc_Dist2PMeasure(x2,y2,x5,y5);
    
    bdt.DPD[i].Finger_X_XMTR = x5;
    bdt.DPD[i].Finger_Y_RECV = y5;
}


/*******************************************************************************
* Function Name  : FingProc_TapFilterEdgeProcess
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
void FingProc_TapFilterEdgeProcess(uint16_t i, uint16_t curx, uint16_t cury, uint16_t *x, uint16_t *y)
{
    bdt.DPD[i].Finger_X_XMTR = (x[6]>>7) + (x[5]>>7) + (x[4]>>6) 
                               + (x[3]>>5) + (x[2]>>4) + (x[1]>>3)  + (x[0]>>2) +(curx>>1);  /*7阶 IIR 滤波*/
                               
    bdt.DPD[i].Finger_Y_RECV = (y[6]>>7) + (y[5]>>7) + (y[4]>>6) 
                               + (y[3]>>5) + (y[2]>>4) + (y[1]>>3)  + (y[0]>>2) +(cury>>1);  /* 7阶 IIR 滤波*/
}


/*******************************************************************************
* Function Name  : FingProc_TapFilterProcess
* Description    :  3阶 IIR 滤波
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
void FingProc_TapFilterProcess(uint16_t i, uint16_t curx, uint16_t cury, uint16_t *x, uint16_t *y)
{
#if 1
    bdt.DPD[i].Finger_X_XMTR = (x[2]>>3) + (x[1]>>3) + (x[0]>>2) + (curx>>1);  // 3阶 IIR 滤波
    bdt.DPD[i].Finger_Y_RECV = (y[2]>>3) + (y[1]>>3) + (y[0]>>2) + (cury>>1);  // 3阶 IIR 滤波
#else
    bdt.DPD[i].Finger_X_XMTR = (curx>>3) + (x[1]>>3) + (x[0]>>2) + (curx>>1);  // 2阶 IIR 滤波
    bdt.DPD[i].Finger_Y_RECV = (cury>>3) + (y[1]>>3) + (y[0]>>2) + (cury>>1);  // 2阶 IIR 滤波
#endif
}


/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
void FingProc_TapFilterStateReset(uint16_t index, uint8_t FilterState)
{
    uint16_t  i;
    bdt.DPD[index].FingerStateFlag   = FilterState;
    for (i = 0; i < FINGER_INFO_SAVE_NUM; i++)
    {
        bdt.DPD[index].Prev_Finger_X[i]  = 0;
        bdt.DPD[index].Prev_Finger_Y[i]  = 0;
    }
    bdt.DPD[index].Finger_X_Reported = 0;
    bdt.DPD[index].Finger_Y_Reported = 0;
    bdt.DPD[index].LifeNumber        = 0;
    bdt.DPD[index].FingMovingSpeed   = FINGER_FINGER_SPEED_LOW;
    bdt.DPD[index].AdjustState       = STATE_STICK_START;
}


/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
void FingProc_TapFilterStateUpdate(uint16_t i)
{
    uint16_t  j;
    for (j = (FINGER_INFO_SAVE_NUM-1); j>0; j--)
    {
        bdt.DPD[i].Prev_Finger_X[j] = bdt.DPD[i].Prev_Finger_X[j-1];
        bdt.DPD[i].Prev_Finger_Y[j] = bdt.DPD[i].Prev_Finger_Y[j-1];
    }
    bdt.DPD[i].Prev_Finger_X[0] = bdt.DPD[i].Finger_X_XMTR;
    bdt.DPD[i].Prev_Finger_Y[0] = bdt.DPD[i].Finger_Y_RECV;
}


/*******************************************************************************
* Function Name  : FingProc_MultiFilterProcess
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
void FingProc_MultiFilterProcess(uint16_t i, uint16_t curx, uint16_t cury, uint16_t *x, uint16_t *y)
{
    FingProc_ForwardSmoothLine(i,x,y);

    curx = bdt.DPD[i].Finger_X_XMTR;
    cury = bdt.DPD[i].Finger_Y_RECV;
    FingProc_TapFilterProcess(i, curx,cury,x,y);

    FingProc_DistanceFilter0(i, x[0], y[0], &bdt.DPD[i].Finger_X_XMTR, &bdt.DPD[i].Finger_Y_RECV);

    FingProc_TapFilterStateUpdate(i);
	
    bdt.DPD[i].Finger_X_Reported = bdt.DPD[i].Finger_X_XMTR;
    bdt.DPD[i].Finger_Y_Reported = bdt.DPD[i].Finger_Y_RECV;
    #if 1
    {
        uint16_t max=0, mini=0xffff, cury;
        uint16_t d01;
		
        if(bdt.FingerDetectNum>2)
        {
            for (i=0;i<bdt.FingerDetectNum;i++)
            {
                cury = bdt.DPD[i].Finger_Y_Reported;    /* Just calculated from raw data*/
                if(cury>max)
                {
                    max = cury;
                }
                if(cury<mini)
                {
                    mini = cury;
                }
            }
            max = max - mini;
            if(max<128)
            {
                bdt.FingerRealNum1=7;
                d01  = FingProc_Dist4Uint16Var(x[0], x[1]);
                if(d01 > 64)
                {    
                    FingProc_TapFilterStateReset(i, STATE_NONE_FINGER);
                }
            }
        }
        else
        {
            bdt.FingerRealNum1=2;
        }
    }
    #endif
    
}


/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/

void FingProc_LCD_Show_Line(uint16_t i, uint16_t xs, uint16_t ys, uint16_t xe, uint16_t ye)
{
    #ifdef STM32VC_LCD
    uint16_t xslcd;
    uint16_t yslcd;
    uint16_t xelcd;
    uint16_t yelcd;
    
    /*get the coordinate in the screen*/
    xslcd  = (uint16_t)(( ((uint32_t)xs) * XMTR_SCALE )>>16);
    yslcd  = (uint16_t)(( ((uint32_t)ys) * RECV_SCALE )>>16);
    xelcd  = (uint16_t)(( ((uint32_t)xe) * XMTR_SCALE )>>16);
    yelcd  = (uint16_t)(( ((uint32_t)ye) * RECV_SCALE )>>16);
    if(SCREEN_HIGH  <= xs)
    {
        xs = SCREEN_HIGH - 1;
    }
    if(SCREEN_WIDTH <= ys)
    { 
        ys = SCREEN_WIDTH - 1;
    }
    if(SCREEN_HIGH  <= xe)
    { 
        xe = SCREEN_HIGH - 1;
    }
    if(SCREEN_WIDTH <= ye)
    {
        ye = SCREEN_WIDTH - 1;
    }
    
    if(0 == bdt.LcdIsrFlag)
    {
        LCD_DisplayLine(xslcd, yslcd, xelcd, yelcd, LCD_COLOR_WHITE);
        bdt.DPD[i].Finger_X_LCD = xelcd;
        bdt.DPD[i].Finger_Y_LCD = yelcd;
    }
    #endif
}


/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/

void FingProc_First4PointsFilter(uint16_t *x, uint16_t *y, uint16_t curx, uint16_t cury)
{
    // x[3] = x[3]; // The first point should not be changed
    // y[3] = y[3]; // The first point should not be changed
    
    x[2] = (x[2]>>1) + (x[2]>>3) + (x[3]>>2) + (x[3]>>3); /*(5/8 , 3/8)*/
    y[2] = (y[2]>>1) + (y[2]>>3) + (y[3]>>2) + (y[3]>>3); /* (5/8 , 3/8)*/

    x[1] = (x[1]>>1) + (x[1]>>3) + (x[2]>>2) + (x[3]>>3); /* (5/8 , 1/4, 1/8)*/
    y[1] = (y[1]>>1) + (y[1]>>3) + (y[2]>>2) + (y[3]>>3); /* (5/8 , 1/4, 1/8)*/

#if 1
    x[0] = (x[0]>>1) + (x[1]>>2) + (x[2]>>3) + (x[0]>>3); // (5/8 , 1/4, 1/8)
    y[0] = (y[0]>>1) + (y[1]>>2) + (y[2]>>3) + (y[0]>>3); // (5/8 , 1/4, 1/8)
#else
    x[0] = (x[0]>>1) + (x[1]>>2) + (x[2]>>3) + (x[3]>>3); // (1/2 , 1/4, 1/8, 1/8)
    y[0] = (y[0]>>1) + (y[1]>>2) + (y[2]>>3) + (y[3]>>3); // (1/2 , 1/4, 1/8, 1/8)
#endif

}


/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/

void  FingProc_TringleProcessFilter(uint16_t x0, uint16_t y0, uint16_t *x1, uint16_t *y1, uint16_t x2, uint16_t y2)
{
}


/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/

int16_t FingProc_BackForthLineJudge(uint16_t x2, uint16_t y2, uint16_t x1, uint16_t y1, uint16_t x0, uint16_t y0)
{
    int16_t  result = 0; 
    uint16_t d01, d12, d02, d_2Side, d_1Side;

    d01  = FingProc_Dist2PMeasure(x2, y2, x1, y1);
    d12  = FingProc_Dist2PMeasure(x1, y1, x0, y0);
    /******************************************************
          * Make sure d01 is longer than d12
          *******************************************************/
    if(d01<d12) 
    {
        d02=d01;
        d01=d12;
        d12=d02;
    }
    d02 = FingProc_Dist2PMeasure(x2, y2, x0, y0);
    d_2Side = d12 + d02;       /*(A + B)*/
    d_1Side = d01 + (d01>>2);  /* C * 1.0625; Since 0.0625 = COS(20);*/
    
    if(d_1Side > d_2Side)   /* Special Tringle  */
    if(d12 > (FINGER_ADJUST_DISTANCE_MAX<<2))       /* Short Side is long enough*/
    {
        result = 1;
    }
    return result;
}

/*******************************************************************************
* Function Name  : FingProc_JustBeginingProcess
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
void FingProc_JustBeginingProcess(uint16_t i, uint16_t FF, uint8_t StateFlagWithFinger, uint8_t StateFlagWithoutFinger)
{
    uint16_t *x, *y, j;

   /**********************************
   * P0 is appeared, no reported
   ***********************************/
    if(FF != 0) 
    {
        bdt.DPD[i].FingerStateFlag = StateFlagWithFinger;        /*First Finger Point is found   */
        /*******************************************************
        * State0 save the current Piont P0, 常规处理
        ********************************************************/
        FingProc_TapFilterStateUpdate(i);
        bdt.DPD[i].Finger_X_Reported = 0;
        bdt.DPD[i].Finger_Y_Reported = 0;
        bdt.DPD[i].LifeNumber        = 1;
        
        /*******************************************************
        * Only One Finger, Just for this Finger "i", 特别处理
        ********************************************************/
        
        if(1 == bdt.FingerRealNum)
        {
            x = bdt.DPD[i].Prev_Finger_X;
            y = bdt.DPD[i].Prev_Finger_Y;
            bdt.DPD[i].FingerStateFlag   = STATE_SERIAL_FINGER; /* STATE_FORTH_FINGER;*/
            for (j = 1; j<FINGER_INFO_SAVE_NUM; j++)
            {
                x[j] = bdt.DPD[i].Finger_X_XMTR;
                y[j] = bdt.DPD[i].Finger_Y_RECV;
            }
            bdt.DPD[i].Finger_X_S[0] = x[0];
            bdt.DPD[i].Finger_Y_S[0] = y[0];
            bdt.DPD[i].Finger_X_S[1] = x[1];
            bdt.DPD[i].Finger_Y_S[1] = y[1];
            bdt.DPD[i].FF_LastDist   = 0;
            bdt.DPD[i].Finger_X_Reported = x[0]; /*对于孤独第一点, 报告之*/
            bdt.DPD[i].Finger_Y_Reported = y[0]; /* 对于孤独第一点, 报告之*/
            #ifdef CN1100_STM32
            FingProc_LCD_Show_Line(i, x[1], y[1], x[0], y[0]); /* Report X/Y[1]*/
            #endif
            return;
        }
    }
    else 
    {
        FingProc_TapFilterStateReset(i, StateFlagWithoutFinger);  /* P1 is lost again*/
    }
}



/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/

void FingProc_SmallAngleTringleProcess(uint16_t i, uint16_t *x, uint16_t *y, uint8_t SFWithoutF)
{
    uint16_t d01, d12, d02, d_2Side, d_1Side, j;

    d01  = FingProc_Dist2PMeasure(x[2], y[2], x[1], y[1]); /* 第0点 到 第1点的距离*/
    d12  = FingProc_Dist2PMeasure(x[1], y[1], x[0], y[0]); /* 第1点 到 第2点的距离*/

    if((d12>256) && (d12 > (d01<<2)))
    {
        /*****************************************************************
        *  第1点 到 第2点的距离 太大 (画线太快了) 且 大过 前一段线距 4倍
        *  这种情况，被认为是 有问题的
        *******************************************************************/
        if(bdt.DPD[i].FingerStateFlag < STATE_SERIAL_FINGER)
        FingProc_TapFilterStateReset(i, SFWithoutF);  /* Px is lost again*/
        else
        {
            for (j = 0; j < FINGER_INFO_SAVE_NUM; j++)
            {
                bdt.DPD[i].Prev_Finger_X[j] = bdt.DPD[i].AdjustOrigin_x;
                bdt.DPD[i].Prev_Finger_Y[j] = bdt.DPD[i].AdjustOrigin_y;
                bdt.DPD[i].FingerStateFlag  = SFWithoutF;
            }
        }
    }
    else
    {
       /*****************************************************************
       * 前两段线距，安排 d01 放置 长线段; d12 放置 短线段;
       *****************************************************************/
        if(d01<d12) 
        {
            d02=d01;
            d01=d12; 
            d12=d02;
        }
        
        
       /*****************************************************************
       *  虚拟的当前的 线距 d02 : 第0点 到 第2点的距离
       ******************************************************************/
        d02  = FingProc_Dist2PMeasure(x[0], y[0], x[2], y[2]);
        
       /*****************************************************************
       *  虚拟的当前的 线距 d02 + 短线距 ===> d_2side
       *  长线距 * (1.25)                ===> d_1side
       ******************************************************************/
        d_2Side = d12 + d02;       // 三角形两边之和(A + B)
        d_1Side = d01 + (d01>>2);  // C * 1.0625; Since 0.0625 = COS(20);
        
       /*****************************************************************
       *  若长线距 * (1.25) 大过 双线距离, 且短边大过 256, 则抛弃该手指
       ******************************************************************/
        if((d_1Side > d_2Side) && (d12 > THR024))
        {
           /******************************************************************
           * 这是一个奇怪的来回三角形, 禁止出现这种连线，这里其被认为是飞点
           ******************************************************************/
            if(bdt.DPD[i].FingerStateFlag < STATE_SERIAL_FINGER)
            {    
                FingProc_TapFilterStateReset(i, SFWithoutF);  // Px is lost again
            }
            else
            {
            
                for (j = 0; j < FINGER_INFO_SAVE_NUM; j++)
                {
                    bdt.DPD[i].Prev_Finger_X[j] = bdt.DPD[i].AdjustOrigin_x;
                    bdt.DPD[i].Prev_Finger_Y[j] = bdt.DPD[i].AdjustOrigin_y;
                    bdt.DPD[i].FingerStateFlag  = SFWithoutF;
                }
            }
        }
    }
}


/*******************************************************************************
* Function Name  : FingProc_unreasonblespeedProcess
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/

void FingProc_unreasonblespeedProcess(uint16_t i, uint16_t *x, uint16_t *y, uint8_t SFWithoutF)
{
    uint16_t d01, d12, d23, j;
    
    d01  = FingProc_Dist2PMeasure(x[3], y[3], x[2], y[2]); 
    d12  = FingProc_Dist2PMeasure(x[2], y[2], x[1], y[1]); 
    d23  = FingProc_Dist2PMeasure(x[1], y[1], x[0], y[0]);  
    
    if(d01>128)
    if(d23>128)
    {
        d01=d01<<1;
        d23=d23<<1;
        if((d12>d23)&&(d12>d01))
        { 
            if(bdt.DPD[i].FingerStateFlag < STATE_SERIAL_FINGER)
            {
                FingProc_TapFilterStateReset(i, SFWithoutF);  /*Px is lost again*/
            }
            else
            {
                for (j = 0; j < FINGER_INFO_SAVE_NUM; j++)
                {
                    bdt.DPD[i].Prev_Finger_X[j] = bdt.DPD[i].AdjustOrigin_x;
                    bdt.DPD[i].Prev_Finger_Y[j] = bdt.DPD[i].AdjustOrigin_y;
                    bdt.DPD[i].FingerStateFlag  = SFWithoutF;
                }
            }
        }
    }
    if(d12>128)
    {
        d01=d01>>1;
        d23=d23>>1;
        d12=d12<<1;
        if((d12<d01)&&(d12<d23))
        { 
            if(bdt.DPD[i].FingerStateFlag < STATE_SERIAL_FINGER)
            {
                FingProc_TapFilterStateReset(i, SFWithoutF);  /* Px is lost again*/
            }
            else
            {
                for (j = 0; j < FINGER_INFO_SAVE_NUM; j++)
                {
                    bdt.DPD[i].Prev_Finger_X[j] = bdt.DPD[i].AdjustOrigin_x;
                    bdt.DPD[i].Prev_Finger_Y[j] = bdt.DPD[i].AdjustOrigin_y;
                    bdt.DPD[i].FingerStateFlag  = SFWithoutF;
                }
            }
        }
    }
}


/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/

void FingProc_AlreadyBeginingProcess(uint16_t i, uint16_t FF, uint8_t StateFlagWithFinger, uint8_t StateFlagWithoutFinger)
{
    uint16_t *x, *y;
    uint16_t d01;

    if(FF != 0) 
    {
        bdt.DPD[i].FingerStateFlag = StateFlagWithFinger;
        FingProc_TapFilterStateUpdate(i); 
        bdt.DPD[i].Finger_X_Reported = 0;
        bdt.DPD[i].Finger_Y_Reported = 0;
        bdt.DPD[i].LifeNumber++;

        x = bdt.DPD[i].Prev_Finger_X;
        y = bdt.DPD[i].Prev_Finger_Y;
        if((x[2]!=0) || (y[2]!=0))
        {     
            FingProc_SmallAngleTringleProcess(i, x, y, StateFlagWithoutFinger);
            FingProc_unreasonblespeedProcess(i, x, y, StateFlagWithoutFinger);
        }
        else
        {
            d01  = FingProc_Dist2PMeasure(x[0], y[0], x[1], y[1]);
             /*****************************************************************
                            * 这是一个太快速的开始点，这里其被认为是飞点
                            ******************************************************************/
            if(d01 > 256)
            {
                FingProc_TapFilterStateReset(i, StateFlagWithoutFinger);  /* Px is lost again*/
            }
        }
    }
    else 
    {
        FingProc_TapFilterStateReset(i, StateFlagWithoutFinger);  /* P1 is lost again*/
    }
}



/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/

void FingProc_FromStickCaseProcess(uint16_t i, uint16_t FF, uint16_t curx, uint16_t cury, uint8_t NextFingerState)
{
    uint16_t j, *x, *y;

    x = bdt.DPD[i].Prev_Finger_X;
    y = bdt.DPD[i].Prev_Finger_Y;
    
    if(0 == FF) 
    {
        bdt.DPD[i].FingerStateFlag = STATE_LOST_1_FINGER;
        curx = bdt.DPD[i].Finger_X_Raw; /* Load the previous RAW data X*/
        cury = bdt.DPD[i].Finger_Y_Raw; /* Load the previous RAW data Y*/
        if((i + 1) > bdt.FingerDetectNum)
        {
            bdt.FingerDetectNum = i + 1;
        }
        #ifdef RAWPOINT4FINGMATCH
        bdt.DPD[i].Point_Hold     = FINGER_ID_HOLD_TIME;
        bdt.DPD[i].Finger_XRAW[0] = curx;
        bdt.DPD[i].Finger_YRAW[0] = cury;
        #endif
        FingProc_MultiFilterProcess(i, curx, cury, x, y);
    }
    else 
    {
        if(bdt.DPD[i].LifeNumber < 100)
        {
            bdt.DPD[i].LifeNumber++;
        }
        /*****************************************************************
                  * 正常计算 手指点滤波过程
                  ******************************************************************/
        FingProc_MultiFilterProcess(i, curx, cury, x, y);
        
        /*****************************************************************
                  * 但是不报告 到HOST系统 目前的手指点，报告到HOST的依旧是以前的点
                  ******************************************************************/
        bdt.DPD[i].Finger_X_Reported = x[bdt.DPD[i].FingerStateFlag - STATE_S_OUT1_FINGER+2]; 
        bdt.DPD[i].Finger_Y_Reported = y[bdt.DPD[i].FingerStateFlag - STATE_S_OUT1_FINGER+2]; 
        
        /*****************************************************************
                  * 首先判断 最新手指点 到 基地手指点之间的距离 是否太近了?
                  ******************************************************************/ 
        if(FingProc_Dist2PMeasure(curx, cury, bdt.DPD[i].Finger_X_Reported, bdt.DPD[i].Finger_Y_Reported) < bdt.ThrLow4DistanceFilter)
        {
            /********************************************
            * 发现手指点又回到基地
            *********************************************/
                       
            bdt.DPD[i].FingerStateFlag  = STATE_STAYAT_FINGER; /* 回到STICK点，此前手指点是飞点耳*/
            for(j = 0; j<FINGER_INFO_SAVE_NUM; j++)
            {
                bdt.DPD[i].Prev_Finger_X[j] = bdt.DPD[i].Finger_X_Reported; /*清除飞点 对 状态变量 造成的 不好的影响*/
                bdt.DPD[i].Prev_Finger_Y[j] = bdt.DPD[i].Finger_Y_Reported; /* 清除飞点 对 状态变量 造成的 不好的影响*/
            }
        }
        else
        {
         /********************************************
         * 手指点依旧离基地有距离
         **********************************************/
            bdt.DPD[i].FingerStateFlag  = NextFingerState;
            FingProc_SmallAngleTringleProcess(i, x, y, STATE_STAYAT_FINGER);
            FingProc_unreasonblespeedProcess(i, x, y, STATE_STAYAT_FINGER);
        }
    }
}


/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
void FingProc_ImproveByMultiFilters(void)
{
    uint16_t *x, *y, curx, cury;
    uint16_t i;
    uint16_t FINGER_FLAG = 0;
    
    for (i = 0;i < FINGER_NUM_MAX;i++)
    {
        x    = bdt.DPD[i].Prev_Finger_X;    /* Point to the saving array*/
        y    = bdt.DPD[i].Prev_Finger_Y;
        curx = bdt.DPD[i].Finger_X_XMTR;    /* Just calculated from raw data*/
        cury = bdt.DPD[i].Finger_Y_RECV;	/* Just calculated from raw data */
        if(curx || cury)
        {
            FINGER_FLAG = 1;   /* Make sure there is finger @ i (index)*/
        }
        else
        {    
            FINGER_FLAG = 0;
        }
        
        switch(bdt.DPD[i].FingerStateFlag)
        {
            case STATE_NONE_FINGER:
            {
                bdt.DPD[i].AdjustOrigin_x = 0;
                bdt.DPD[i].AdjustOrigin_y = 0;
                                       
                FingProc_JustBeginingProcess(i, FINGER_FLAG, STATE_FIRST_FINGER, STATE_NONE_FINGER);
                                       
                break;
            }
            
            case STATE_FIRST_FINGER:
            {
                FingProc_AlreadyBeginingProcess(i, FINGER_FLAG, STATE_SECOND_FINGER, STATE_NONE_FINGER);
                break;
            }
            
            case STATE_SECOND_FINGER:
            {
                FingProc_AlreadyBeginingProcess(i, FINGER_FLAG, STATE_THIRD_FINGER, STATE_NONE_FINGER);
                break;
            }
            
            case STATE_THIRD_FINGER:
            {
                FingProc_AlreadyBeginingProcess(i, FINGER_FLAG, STATE_FORTH_FINGER, STATE_NONE_FINGER);
                bdt.DPD[i].JustPassStateFlag4 = 0;
                break;
            }
            
            case STATE_FORTH_FINGER:
            {
                /*************************************************************
                * When P4 is appeared, reported all (P0, P1, P2, P3, P4 ....)
                **************************************************************/
                if(FINGER_FLAG != 0) 
                {
                    bdt.DPD[i].JustPassStateFlag4 = 1;
                    bdt.DPD[i].FingerStateFlag = STATE_SERIAL_FINGER;
                    
                    bdt.DPD[i].Finger_X_S[0]     = x[0];
                    bdt.DPD[i].Finger_Y_S[0]     = y[0];
                    bdt.DPD[i].Finger_X_S[1]     = x[1];
                    bdt.DPD[i].Finger_Y_S[1]     = y[1];
                    bdt.DPD[i].FF_LastDist       = 0;
                    /*******************************************************
                                          * Process the four point and Cur Point
                                          ********************************************************/
                    FingProc_First4PointsFilter(x, y, curx, cury);
                    
                    #ifdef CN1100_STM32
                    FingProc_LCD_Show_Line(i, x[3], y[3], x[2], y[2]); /* Report X/Y[3]*/
                    FingProc_LCD_Show_Line(i, x[2], y[2], x[1], y[1]); /* Report X/Y[2]*/
                    FingProc_LCD_Show_Line(i, x[1], y[1], x[0], y[0]); /* Report X/Y[1]*/
                    #endif
                    
                    FingProc_MultiFilterProcess(i, curx, cury, x, y); 
                    
                    #ifdef CN1100_STM32
                    FingProc_LCD_Show_Line(i, x[1], y[1], x[0], y[0]); /*Report X/Y[0]*/
                    #endif
                } 
                else
                {
                    FingProc_TapFilterStateReset(i, STATE_NONE_FINGER);  /* P1 is lost again*/
                }
                break;
            }
            
            case STATE_SERIAL_FINGER:
            {
                /******************************************
                * P3 is appeared, reported p3', so on;
                *******************************************/
                bdt.DPD[i].JustPassStateFlag4 = 0;
                if(0 == FINGER_FLAG) 
                {
                    if(bdt.DPD[i].LifeNumber < 2)
                    {
                        FingProc_TapFilterStateReset(i, STATE_NONE_FINGER);  /*P1 is lost again*/
                        break;
                    }
                    else
                    {
                        bdt.DPD[i].FingerStateFlag = STATE_LOST_1_FINGER;
                        curx = bdt.DPD[i].Finger_X_Raw; /* Load the previous RAW data X*/
                        cury = bdt.DPD[i].Finger_Y_Raw; /* Load the previous RAW data Y*/
                        if((i + 1) > bdt.FingerDetectNum)	bdt.FingerDetectNum = i + 1;
                       
                    }
                    #ifdef RAWPOINT4FINGMATCH
                    bdt.DPD[i].Point_Hold     = FINGER_ID_HOLD_TIME;
                    bdt.DPD[i].Finger_XRAW[0] = curx;
                    bdt.DPD[i].Finger_YRAW[0] = cury;
                    #endif
                }
                else 
                {
                    
                    if(bdt.DPD[i].LifeNumber < 100) 
                    {
                        bdt.DPD[i].LifeNumber++;
                    }
                    
                    #ifdef FOURPOINTS4STICKMOVING
                    if(STATE_STICK_HERE == bdt.DPD[i].AdjustState) 
                    { 
                        bdt.DPD[i].FingerStateFlag  = STATE_STAYAT_FINGER;
                    }
                    else if((bdt.DPD[i].LifeNumber > 8) && (STATE_STICK_START == bdt.DPD[i].AdjustState))
                    {
                        bdt.DPD[i].FingerStateFlag  = STATE_STAYAT_FINGER;
                    }
                    #endif
                }
                FingProc_MultiFilterProcess(i, curx, cury, x, y);
                break;
            }
            
            case STATE_STAYAT_FINGER:
            {
                if(0 == FINGER_FLAG) 
                {
                    bdt.DPD[i].FingerStateFlag = STATE_LOST_1_FINGER;
                    curx = bdt.DPD[i].Finger_X_Raw; /* Load the previous RAW data X*/
                    cury = bdt.DPD[i].Finger_Y_Raw; /* Load the previous RAW data Y*/
                    
                    if((i + 1) > bdt.FingerDetectNum)
                    {
                        bdt.FingerDetectNum = i + 1;
                    }
                    #ifdef RAWPOINT4FINGMATCH
                    bdt.DPD[i].Point_Hold     = FINGER_ID_HOLD_TIME;
                    bdt.DPD[i].Finger_XRAW[0] = curx;
                    bdt.DPD[i].Finger_YRAW[0] = cury;
                    #endif
                    FingProc_MultiFilterProcess(i, curx, cury, x, y);
                }
                else 
                {
                    if(bdt.DPD[i].LifeNumber < 100) 
                    {    
                        bdt.DPD[i].LifeNumber++;
                    }
                    FingProc_MultiFilterProcess(i, curx, cury, x, y);
                    if(bdt.DPD[i].AdjustState > STATE_STICK_HERE)
                    {
                        /**********************************************************************
                        * 检测到手指可能要"大"动作，注意，"小动作"外部应该看不出来, STICK嘛
                        ***********************************************************************/
                        bdt.DPD[i].Finger_X_Reported = x[1]; /* 强行报告老的手指点数据*/
                        bdt.DPD[i].Finger_Y_Reported = y[1]; /* 强行报告老的手指点数据*/
                        bdt.DPD[i].FingerStateFlag = STATE_S_OUT1_FINGER;
                    }
                }
                break;
            }
            
            case STATE_S_OUT1_FINGER:
            {
                /**************************************************************************
                * 这个状态时过渡状态，会回到 (1)STATE_STAYAT_FINGER (2)STATE_LOST_1_FINGER
                * 或者 (3)STATE_S_OUT2_FINGER 各类状态，依不同的条件
                ***************************************************************************/
                FingProc_FromStickCaseProcess(i, FINGER_FLAG, curx, cury, STATE_S_OUT2_FINGER);
                break;
            }
            case STATE_S_OUT2_FINGER:
            {
                /**************************************************************************
                * 这个状态时过渡状态，会回到 (1)STATE_STAYAT_FINGER (2)STATE_LOST_1_FINGER
                * 或者 (3)STATE_S_OUT3_FINGER 各类状态，依不同的条件
                ***************************************************************************/
                FingProc_FromStickCaseProcess(i, FINGER_FLAG, curx, cury, STATE_S_OUT3_FINGER);
                bdt.DPD[i].JustPassStateFlag4 = 0;
                break;
            }
            case STATE_S_OUT3_FINGER:
            {
                /**********************************************************************
                * So Far, 我们已经发现了3个手指点都是正常的偏离基地点而去的手指点,所以
                * 如果这次(第四点)依旧是一个这样的手指点，那么就认为这是一次正常的手指
                * 移动了, 并且把之前的有效移动的手指点补偿性地重现出来
                ***********************************************************************/
                if(0 == FINGER_FLAG) 
                {
                    bdt.DPD[i].FingerStateFlag = STATE_LOST_1_FINGER;
                    curx = bdt.DPD[i].Finger_X_Raw; /* Load the previous RAW data X*/
                    cury = bdt.DPD[i].Finger_Y_Raw; /* Load the previous RAW data Y*/
                    if((i + 1) > bdt.FingerDetectNum)
                    {
                        bdt.FingerDetectNum = i + 1;
                    }
                    
                    #ifdef RAWPOINT4FINGMATCH
                    bdt.DPD[i].Point_Hold     = FINGER_ID_HOLD_TIME;
                    bdt.DPD[i].Finger_XRAW[0] = curx;
                    bdt.DPD[i].Finger_YRAW[0] = cury;
                    #endif
                    FingProc_MultiFilterProcess(i, curx, cury, x, y);
                }
                else 
                {
                     if(bdt.DPD[i].LifeNumber < 100)
                     {
                         bdt.DPD[i].LifeNumber++;
                     }
                     
                     if(FingProc_Dist2PMeasure(curx, cury, bdt.DPD[i].Finger_X_Reported, bdt.DPD[i].Finger_Y_Reported) < bdt.ThrLow4DistanceFilter)
                     {
                         /********************************************
                         * 发现手指点又回到基地
                         *********************************************/
                         uint16_t j;
                         bdt.DPD[i].FingerStateFlag  = STATE_STAYAT_FINGER; /* 回到STICK点，此前手指点是飞点耳*/
                         for (j = 0; j<FINGER_INFO_SAVE_NUM; j++)
                         {
                             bdt.DPD[i].Prev_Finger_X[j] = bdt.DPD[i].Finger_X_Reported; /* 清除飞点 对 状态变量 造成的 不好的影响*/
                             bdt.DPD[i].Prev_Finger_Y[j] = bdt.DPD[i].Finger_Y_Reported; /* 清除飞点 对 状态变量 造成的 不好的影响*/
                         }
                         break;
                     }
                     else
                     {
                         /********************************************
                         * 手指点依旧离基地有距离
                         *********************************************/
                         bdt.DPD[i].FingerStateFlag  = STATE_SERIAL_FINGER;
                         FingProc_SmallAngleTringleProcess(i, x, y, STATE_STAYAT_FINGER);
                         FingProc_unreasonblespeedProcess(i, x, y, STATE_STAYAT_FINGER);
                     }
                     
                     if(STATE_SERIAL_FINGER == bdt.DPD[i].FingerStateFlag)
                     {   
                         bdt.DPD[i].JustPassStateFlag4 = 1;
                     }
                     else 
                     {
                         break;
                     }
                     
                     #ifdef CN1100_STM32
                     FingProc_LCD_Show_Line(i, x[3], y[3], x[2], y[2]); /*Report X/Y[3]*/
                     FingProc_LCD_Show_Line(i, x[2], y[2], x[1], y[1]); /* Report X/Y[2]*/
                     FingProc_LCD_Show_Line(i, x[1], y[1], x[0], y[0]); /* Report X/Y[1]*/
                     #endif
                     FingProc_MultiFilterProcess(i, curx, cury, x, y);
                     #ifdef CN1100_STM32
                     FingProc_LCD_Show_Line(i, x[1], y[1], x[0], y[0]); /* Report X/Y[1]*/
                     #endif
                 
                }
                break;
            }
            case STATE_LOST_1_FINGER:
            {
                if(FINGER_FLAG) 
                {
                    bdt.DPD[i].FingerStateFlag  = STATE_SERIAL_FINGER;
                    FingProc_MultiFilterProcess(i, curx, cury, x, y); 
                }
                else
                {       
                    bdt.DPD[i].FingerStateFlag = STATE_LOST_2_FINGER;
                    
                    if((STATE_STICK_HERE == bdt.DPD[i].AdjustState) || (STATE_STICK_START == bdt.DPD[i].AdjustState)) 
                    {    
                        break;
                    }
                    
                    curx = bdt.DPD[i].Finger_X_Raw;
                    cury = bdt.DPD[i].Finger_Y_Raw;
                    if((i + 1) > bdt.FingerDetectNum)
                    {
                        bdt.FingerDetectNum = i + 1;
                    }
                    
                    FingProc_MultiFilterProcess(i, curx, cury, x, y); 
                    bdt.DPD[i].Finger_X_Reported  += bdt.DPD[i].Finger_X_Raw;
                    bdt.DPD[i].Finger_Y_Reported  += bdt.DPD[i].Finger_Y_Raw;
                    bdt.DPD[i].Finger_X_Reported >>= 1;
                    bdt.DPD[i].Finger_Y_Reported >>= 1;
                    x[0] = bdt.DPD[i].Finger_X_Reported;
                    y[0] = bdt.DPD[i].Finger_Y_Reported;
                    
                    
                    #ifdef RAWPOINT4FINGMATCH
                    bdt.DPD[i].Point_Hold     = FINGER_ID_HOLD_TIME;
                    bdt.DPD[i].Finger_XRAW[0] = curx;
                    bdt.DPD[i].Finger_YRAW[0] = cury;
                    #endif
                }
                break;
            }
            case STATE_LOST_2_FINGER:
            {
                if(FINGER_FLAG != 0) 
                {
                    bdt.DPD[i].FingerStateFlag   = STATE_SERIAL_FINGER;
                    FingProc_MultiFilterProcess(i, curx, cury, x, y);
                }
                else
                {
                    FingProc_TapFilterStateReset(i, STATE_NONE_FINGER);
                    
                    if((STATE_STICK_HERE == bdt.DPD[i].AdjustState) || (STATE_STICK_START == bdt.DPD[i].AdjustState)) 
                    { 
                        break;
                    }
                    bdt.DPD[i].Finger_X_Reported = bdt.DPD[i].Finger_X_Raw;
                    bdt.DPD[i].Finger_Y_Reported = bdt.DPD[i].Finger_Y_Raw;
                    bdt.DPD[i].Point_Hold        = FINGER_ID_HOLD_TIME;
                    bdt.DPD[i].AdjustDistance    = FINGER_ADJUST_DISTANCE_MAX;
                    if((i + 1) > bdt.FingerDetectNum)
                    { 
                        bdt.FingerDetectNum = i + 1;
                    }
                    
                
                    
                    #ifdef RAWPOINT4FINGMATCH
                    bdt.DPD[i].Finger_XRAW[0] = 0;
                    bdt.DPD[i].Finger_YRAW[0] = 0;
                    bdt.DPD[i].Finger_XRAW[1] = 0;
                    bdt.DPD[i].Finger_YRAW[1] = 0;
                    bdt.DPD[i].Finger_XRAW[2] = 0;
                    bdt.DPD[i].Finger_YRAW[2] = 0;
                    #endif
                }
                break;
            }
            default:
            {
                FingProc_TapFilterStateReset(i, STATE_NONE_FINGER);
                break;
            }
        }
        
        /*******************************************************
        * Save the raw data (Point - Axial data) for the next
        ********************************************************/
        if(FINGER_FLAG != 0)
        {
            bdt.DPD[i].Finger_X_Raw     = curx;
            bdt.DPD[i].Finger_Y_Raw     = cury;
        }
    }
    
    
}



/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
uint16_t FingProc_measurestep(uint16_t *p, uint16_t Rpt)
{
    int16_t i, dx;

    dx = (int16_t)Rpt - (int16_t)p[0];
    i  = 0;
    if(dx > 0)
    {
        dx = (int16_t)p[0] - (int16_t)p[1];
        if(dx > 0)
        {
            i  = 1;
            dx = (int16_t)p[1] - (int16_t)p[2];
            if(dx > 0)
            {
                i  = 2;
                dx = (int16_t)p[2] - (int16_t)p[3];
                if(dx > 0)
                {
                    i = 3;
                }
            }
        }
    }
    return i;
}


/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
uint16_t FingProc_measurestep1(uint16_t *p, uint16_t Rpt)
{
    int16_t i, dx;
    dx = (int16_t)Rpt - (int16_t)p[0];
    i = 0;
    if(dx < 0)
    {
        dx = (int16_t)p[0] - (int16_t)p[1];
        if(dx < 0)
        {   
            i=1;
            dx = (int16_t)p[1] - (int16_t)p[2];
            if(dx < 0)
            {
                i = 2;
                dx = (int16_t)p[2] - (int16_t)p[3];
                if(dx < 0)
                {    
                    i = 3;
                }
            }
        }
    }
    return i;
}



/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/

void FingProc_ImproveEdgeLinearity_L(void)
{
#ifdef SUPER_FILTER4EDGE
    uint16_t *x, *y, xRpt, yRpt;
    uint16_t i, dx, dy;

    for (i = 0;i < FINGER_NUM_MAX;i++)
    {
        if(bdt.DPD[i].FingerStateFlag < STATE_SERIAL_FINGER)
        {
            continue;
        }

        #ifdef FROMOUT2IN_INORDER
        x    =  bdt.DPD[i].Finger_X_Erpt;    /* Point to the saving array*/
        y    =  bdt.DPD[i].Finger_Y_Erpt;
        #else 
        x    = bdt.DPD[i].Prev_Finger_X;    /*Point to the saving array*/
        y    = bdt.DPD[i].Prev_Finger_Y;
        #endif

        xRpt = bdt.DPD[i].Finger_X_XMTR;    /* Just calculated from raw data*/
        yRpt = bdt.DPD[i].Finger_Y_RECV;    /* Just calculated from raw data */
        if( xRpt || yRpt) /* Finger Point*/
        {
            if(xRpt < SUPFIL_RANGE && x[0] < SUPFIL_RANGE && x[1] <SUPFIL_RANGE && x[2] < SUPFIL_RANGE) /* Left Edg*/
            {
                 dx = FingProc_Dist4Uint16Var(xRpt, x[0]);
                 dy = FingProc_Dist4Uint16Var(yRpt, y[0]);
                 if((dx > (dy>>2)) && (dy < 256))
                 {
                     if(0 == FingProc_measurestep(x,xRpt)) 
                     {
                         if(bdt.DPD[i].EdgeShift_L < 3)
                         {  
                              bdt.DPD[i].EdgeShift_L += 1;
                              bdt.DPD[i].EdgeOffset_L >>= 1;
                          }
                      }
                      else
                      {
                          if(bdt.DPD[i].EdgeShift_L > 0)
                          {  
                              bdt.DPD[i].EdgeShift_L -= 1;
                               bdt.DPD[i].EdgeOffset_L <<= 1;
                          }
        
                      }
                                
                      if(dx<128) 
                      {
                          dx = dx>>(bdt.DPD[i].EdgeShift_L);
                      }
                      else  
                      {
                          dx = bdt.DPD[i].EdgeOffset_L + (dx>>(bdt.DPD[i].EdgeShift_L+1));
                      }
                 }
        
                 if(xRpt>x[0])
                 {
                     xRpt = x[0]+dx;
                 }
                 else
                 { 
                     xRpt = x[0]-dx;
                 }

                  bdt.DPD[i].Finger_X_XMTR = xRpt;
             }
         }
         
        #ifdef FROMOUT2IN_INORDER
        x[2]=x[1]; x[1]=x[0]; x[0]=xRpt;
        y[2]=y[1]; y[1]=y[0]; y[0]=yRpt;
        #endif
    }
#endif
}


/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/

void FingProc_ImproveEdgeLinearity_R(void)
{
#ifdef SUPER_FILTER4EDGE
    uint16_t *x, *y, xRpt, yRpt;
    uint16_t i, dx, dy;

    for (i = 0;i < FINGER_NUM_MAX;i++)
    {
        if(bdt.DPD[i].FingerStateFlag < STATE_SERIAL_FINGER) 
        {
            continue;
        }
       
        #ifdef FROMOUT2IN_INORDER
        x    =  bdt.DPD[i].Finger_X_Erpt1;  /* Point to the saving array*/
        y    =  bdt.DPD[i].Finger_Y_Erpt1;
        #else
        x    = bdt.DPD[i].Prev_Finger_X;    /* Point to the saving array*/
        y    = bdt.DPD[i].Prev_Finger_Y;
        #endif  
        xRpt = bdt.DPD[i].Finger_X_XMTR;    /* Just calculated from raw data*/
        yRpt = bdt.DPD[i].Finger_Y_RECV;    /* Just calculated from raw data */
    
        if( xRpt || yRpt) /* Finger Point*/
        { 
            if((xRpt >(XMTR_NUM<<8)-SUPFIL_RANGE && x[0] >(XMTR_NUM<<8)-SUPFIL_RANGE && x[1] > (XMTR_NUM<<8)-SUPFIL_RANGE && x[2] > (XMTR_NUM<<8)-SUPFIL_RANGE)) /* Left Edg*/
            {
                dx =FingProc_Dist4Uint16Var(xRpt, x[0]);
                dy =FingProc_Dist4Uint16Var(yRpt, y[0]);
        
                if((dx > (dy>>2)) && (dy < 256))
                { 
                
                    if(0 == FingProc_measurestep1(x,xRpt)) 
                    {
                        if(bdt.DPD[i].EdgeShift_R < 3)
                        {  
                            bdt.DPD[i].EdgeShift_R += 1;
                         
                            bdt.DPD[i].EdgeOffset_R >>= 1;
                        }
                    }
                    else
                    {
                        if(bdt.DPD[i].EdgeShift_R > 0)
                        {  
                            bdt.DPD[i].EdgeShift_R -= 1;
                        
                            bdt.DPD[i].EdgeOffset_R <<= 1;
                        }
                    }
                    if(dx<128)  
                    {    
                        dx = dx>>(bdt.DPD[i].EdgeShift_R);
                    }
                    else  
                    {
                        dx = bdt.DPD[i].EdgeOffset_R + (dx>>(bdt.DPD[i].EdgeShift_R+1));
                    }
                } 
                if(xRpt>x[0]) 
                {
                    xRpt = x[0]+dx;
                }
                else 
                {    
                    xRpt = x[0]-dx;
                }
                bdt.DPD[i].Finger_X_XMTR = xRpt;
               
            }                  
        }
        
        #ifdef FROMOUT2IN_INORDER
        x[2]=x[1]; x[1]=x[0]; x[0]=xRpt;
        y[2]=y[1]; y[1]=y[0]; y[0]=yRpt;
        #endif
    }
#endif
}


/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
void FingProc_ImproveEdgeLinearity_T(void)
{
#ifdef SUPER_FILTER4EDGE
    uint16_t *x, *y, xRpt, yRpt;
    uint16_t i, dx, dy;

    for (i = 0;i < FINGER_NUM_MAX;i++)
    {
        if(bdt.DPD[i].FingerStateFlag < STATE_SERIAL_FINGER) 
        {    
            continue;
        }
        #ifdef FROMOUT2IN_INORDER
        x    =  bdt.DPD[i].Finger_X_Erpt;    /* Point to the saving array*/
        y    =  bdt.DPD[i].Finger_Y_Erpt;
        #else
        x    = bdt.DPD[i].Prev_Finger_X;    /* Point to the saving array*/
        y    = bdt.DPD[i].Prev_Finger_Y;
        #endif
                
        xRpt = bdt.DPD[i].Finger_X_XMTR;    /* Just calculated from raw data*/
        yRpt = bdt.DPD[i].Finger_Y_RECV;    /* Just calculated from raw data */
        if(xRpt || yRpt) /* Finger Point*/
        {
            if((yRpt < SUPFIL_RANGE && y[0] < SUPFIL_RANGE && y[1] <SUPFIL_RANGE && y[2] < SUPFIL_RANGE)) /* Left Edg*/
            {
           
            
                  dx =FingProc_Dist4Uint16Var(xRpt, x[0]);
                  dy =FingProc_Dist4Uint16Var(yRpt, y[0]);
                  if((dy > (dx>>2)) && (dx < 256))
                  {
                       if(0 == FingProc_measurestep(y,yRpt)) 
                       {
                           if(bdt.DPD[i].EdgeShift_T < 3)
                           {  
                               bdt.DPD[i].EdgeShift_T += 1;
                               bdt.DPD[i].EdgeOffset_T >>= 1;
                           }
                       }
                        else
                       {
                           if(bdt.DPD[i].EdgeShift_T > 0)
                           {  
                               bdt.DPD[i].EdgeShift_T -= 1;
                      
                               bdt.DPD[i].EdgeOffset_T <<= 1;
                           }
                
                       }
                                         
                       if(dy<128) 
                       { 
                           dy = dy>>(bdt.DPD[i].EdgeShift_T);
                       }
                       else  
                       { 
                           dy = bdt.DPD[i].EdgeOffset_T + (dy>>(bdt.DPD[i].EdgeShift_T+1));
                       }
                 }
                  
                 if(yRpt>y[0])
                 {
                     yRpt = y[0]+dy ;
                 }
                 else
                 {
                     yRpt = y[0]-dy;
                 }
                               
                bdt.DPD[i].Finger_Y_RECV = yRpt;
                
            }
        }
        #ifdef FROMOUT2IN_INORDER
         x[2]=x[1]; x[1]=x[0]; x[0]=xRpt;
         y[2]=y[1]; y[1]=y[0]; y[0]=yRpt;
        #endif
    }
#endif
}



/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
void FingProc_ImproveEdgeLinearity_B(void)
{
#ifdef SUPER_FILTER4EDGE
    uint16_t *x, *y, xRpt, yRpt;
    uint16_t i, dx, dy;

    for (i = 0;i < FINGER_NUM_MAX;i++)
    {
        if(bdt.DPD[i].FingerStateFlag < STATE_SERIAL_FINGER) 
        {    
            continue;
        }
        #ifdef FROMOUT2IN_INORDER
        x    =  bdt.DPD[i].Finger_X_Erpt1;  /* Point to the saving array*/
        y    =  bdt.DPD[i].Finger_Y_Erpt1;
        #else
        x    = bdt.DPD[i].Prev_Finger_X;    /* Point to the saving array*/
        y    = bdt.DPD[i].Prev_Finger_Y;
        #endif
            
        xRpt = bdt.DPD[i].Finger_X_XMTR;    /* Just calculated from raw data*/
        yRpt = bdt.DPD[i].Finger_Y_RECV;    /* Just calculated from raw data */
        if( xRpt || yRpt)                   /* Finger Point */
        {
            if((yRpt >(RECV_NUM<<8)-SUPFIL_RANGE && y[0] >(RECV_NUM<<8)-SUPFIL_RANGE && y[1] > (RECV_NUM<<8)-SUPFIL_RANGE && y[2] > (RECV_NUM<<8)-SUPFIL_RANGE)) /* Left Edg*/
            {
               
                dx =FingProc_Dist4Uint16Var(xRpt, x[0]);
                dy =FingProc_Dist4Uint16Var(yRpt, y[0]);
                #if 1
                if((dy > (dx>>2)) && (dx < 256))
                {

                    if(0 == FingProc_measurestep1(y,yRpt)) 
                    {
                        if(bdt.DPD[i].EdgeShift_B < 3)
                        {  
                            bdt.DPD[i].EdgeShift_B += 1;
                            bdt.DPD[i].EdgeOffset_B >>= 1;
                        }
                    }
                    else
                    {
                        if(bdt.DPD[i].EdgeShift_B > 0)
                        {  
                            bdt.DPD[i].EdgeShift_B -= 1;
                            bdt.DPD[i].EdgeOffset_B <<= 1;
                        }

                    }
                        
                    if(dy<128)
                    {
                        dy = dy>>(bdt.DPD[i].EdgeShift_B);
                    }
                    else
                    {
                        dy = bdt.DPD[i].EdgeOffset_B + (dy>>(bdt.DPD[i].EdgeShift_B+1));
                    }
                }
                #endif
                if(yRpt>y[0])
                {    
                    yRpt = y[0]+dy ;
                }
                else
                {    
                    yRpt = y[0]-dy;
                }  
                bdt.DPD[i].Finger_Y_RECV = yRpt;
              
            }
        }
        #ifdef FROMOUT2IN_INORDER
        x[2]=x[1]; x[1]=x[0]; x[0]=xRpt;
        y[2]=y[1]; y[1]=y[0]; y[0]=yRpt;
        #endif
    }
#endif
}



/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
void FingProc_SuperFilter4Edge(void)
{
#ifdef SUPER_FILTER4EDGE
    uint16_t i;

    for (i=0; i<bdt.FingerDetectNum; i++)
    {
        #if 1
        if(bdt.DPD[i].Finger_X_XMTR > 256 && bdt.DPD[i].Finger_X_XMTR < (XMTR_NUM<<8)-256 )
        { 
            bdt.DPD[i].FingerRealNum1_X++;
            if(bdt.DPD[i].FingerRealNum1_X>200)
            {
                bdt.DPD[i].FingerRealNum1_X=200;
            }
            bdt.DPD[i].FingerRealNum2_X = 0;
            bdt.DPD[i].FingerRealNum2R_X=0;
        }
        else
        {
            if(bdt.DPD[i].Finger_X_XMTR != 0)
            {    
                if(bdt.DPD[i].Finger_Y_RECV != 0)
                { 
                    bdt.DPD[i].FingerRealNum2_X++;
                    bdt.DPD[i].FingerRealNum2R_X++;  
                    if(bdt.DPD[i].FingerRealNum2_X>200)
                    {
                        bdt.DPD[i].FingerRealNum2_X=200;
                    }
                    if(bdt.DPD[i].FingerRealNum2R_X>200) 
                    { 
                        bdt.DPD[i].FingerRealNum2R_X=200; 
                    }
                    bdt.DPD[i].FingerRealNum1_X = 0;
                
                }
            }
        }

        if(bdt.DPD[i].FingerRealNum1_X != 0)
        {
            if(bdt.DPD[i].FingerRealNum1_X<10) 
            { 
                FingProc_ImproveEdgeLinearity_L();
                FingProc_ImproveEdgeLinearity_R();
            }  
        }
        if(bdt.DPD[i].FingerRealNum2_X > 20)  
        { 
                FingProc_ImproveEdgeLinearity_L();
        }
        if(bdt.DPD[i].FingerRealNum2R_X > 35 )  
        {
                FingProc_ImproveEdgeLinearity_R();
        }
        #endif

        #if 1
        if(bdt.DPD[i].Finger_Y_RECV > 256 && bdt.DPD[i].Finger_Y_RECV < (RECV_NUM<<8)-256)
        { 
            bdt.DPD[i].FingerRealNum1_Y++;
            if(bdt.DPD[i].FingerRealNum1_Y>200) bdt.DPD[i].FingerRealNum1_Y=200;
            bdt.DPD[i].FingerRealNum2_Y = 0;
        }
        else
        {
            if(bdt.DPD[i].Finger_X_XMTR != 0)
            {
                if(bdt.DPD[i].Finger_Y_RECV != 0)
                {
                    bdt.DPD[i].FingerRealNum2_Y++;
                    if(bdt.DPD[i].FingerRealNum2_Y>200)
                    {
                        bdt.DPD[i].FingerRealNum2_Y=200;
                    }
                    bdt.DPD[i].FingerRealNum1_Y = 0;
                }
            }
        }
        if(bdt.DPD[i].FingerRealNum1_Y != 0)   
        {
            if(bdt.DPD[i].FingerRealNum1_Y<10) 
            { 
                FingProc_ImproveEdgeLinearity_T();
                FingProc_ImproveEdgeLinearity_B();
            }
        }
        
        if(bdt.DPD[i].FingerRealNum2_Y>20) 
        { 
            FingProc_ImproveEdgeLinearity_T();
            FingProc_ImproveEdgeLinearity_B();
        }
        #endif
    }
#endif
}


/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
#ifdef PRESS_KEY_DETECT
/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
void DataProc_PressKeyDetect()
{
    bdt.PressKeyFlag1          = NO_KEY_PRESSED;
    bdt.PressKeyFlag.MenuKey   = NO_KEY_PRESSED;
    bdt.PressKeyFlag.HomeKey   = NO_KEY_PRESSED;
    bdt.PressKeyFlag.ReturnKey = NO_KEY_PRESSED;
    
    if(bdt.FingerDetectNum)
    {
        if(bdt.DeltaDat16A[KEY_TX_LOC][KEY_MENU_RXLOC] > KEY_MENU_RXLOCATION) 
        {
            bdt.PressKeyFlag.MenuKey   = 1;
        }
        if(bdt.DeltaDat16A[KEY_TX_LOC][KEY_HOME_RXLOC] > KEY_HOME_RXLOCATION)
        {   
            bdt.PressKeyFlag.HomeKey   = 1;
        }
        if(bdt.DeltaDat16A[KEY_TX_LOC][KEY_RET_RXLOC]  > KEY_RET_RXLOCATION) 
        {
            bdt.PressKeyFlag.ReturnKey = 1;
        }
        
        if(1 == bdt.PressKeyFlag.MenuKey)
        { 
            bdt.PressKeyFlag1 = MENU_KEY_PRESSED;
        }
        else if(1 == bdt.PressKeyFlag.HomeKey)
        {
            bdt.PressKeyFlag1 = HOME_KEY_PRESSED;
        }
        else if(1 == bdt.PressKeyFlag.ReturnKey)
        {
            bdt.PressKeyFlag1 = RETURN_KEY_PRESSED;
        }
    }
}


#endif


/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/

void FingProc_CheckCrossedLines(void)
{
}


/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/

void FingProc_FigureOutLeastFromMatrix(uint16_t dist[FINGER_NUM_MAX][FINGER_NUM_MAX], uint16_t Dim2idx[2])
{
    uint16_t i, j;
    uint16_t distmin = MAX_UINT16;

    Dim2idx[0] = FINGER_NUM_MAX;    /* Previous Point*/
    Dim2idx[1] = FINGER_NUM_MAX;    /* Current Point*/
    for (j = 0; j < bdt.PrevFingerDetectNum; j++)
        for (i = 0; i < bdt.FingerDetectNum; i++)
        {
            if(distmin > dist[j][i])
            {
                Dim2idx[0] = j;
                Dim2idx[1] = i;
                distmin = dist[j][i];
            }
        }
    /**************************************
    * Erase the Matrix 
    ***************************************/
}


/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/

uint16_t FingProc_TriangleDist3P(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3)
{
    int16_t  mx, my;
#ifdef RAWPOINT4FINGMATCH
    mx = (int16_t)((x1+x3)>>1);
    my = (int16_t)((y1+y3)>>1);
#else
    /*************************************
    * x1, y1 is the oldest point
    * x3, y3 i sthe newest point
    * "1/3" is about equals to "5/16"
    **************************************/
    mx = (int16_t)x3 - (int16_t)x1;
    my = (int16_t)y3 - (int16_t)y1;
    mx = (mx>>2) + (mx>>4) + x1;
    my = (my>>2) + (my>>4) + y1;
#endif
    return FingProc_Dist2PMeasure((int16_t)x2, (int16_t)y2, mx, my);
}



/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/

void FingProc_SortFingersWithMatrix(void)
{
    uint16_t i,  j;
    uint16_t *x, *y;
    uint16_t dist[FINGER_NUM_MAX][FINGER_NUM_MAX], Dim2idx[2];
    uint16_t Pos_X[FINGER_NUM_MAX];      
    uint16_t Pos_Y[FINGER_NUM_MAX];      
    uint16_t Prev2CurID[FINGER_NUM_MAX]; 
    uint16_t Cur2PrevID[FINGER_NUM_MAX]; 
    uint16_t flag    = 0, tdist,tdist2,tdist1;
    uint16_t MaxPos  = 0, minNum;
    
    /**************************************************
    * Step 1: Initialization of VARs
    ***************************************************/
    for (i = 0; i < bdt.FingerDetectNum; i++)
    {
        Pos_X[i] = bdt.DPD[i].Finger_X_XMTR;
        Pos_Y[i] = bdt.DPD[i].Finger_Y_RECV;
    }
    for (i = 0; i < FINGER_NUM_MAX; i++)    
    {
        for (j = 0; j < FINGER_NUM_MAX; j++)
        {
            dist[i][j]   = MAX_UINT16; 
        }
        Prev2CurID[i] = FINGER_NUM_MAX;
        Cur2PrevID[i] = FINGER_NUM_MAX;
        bdt.DPD[i].Finger_X_XMTR = 0;  /* SAVE 配对最后的结果*/
        bdt.DPD[i].Finger_Y_RECV = 0;  /* SAVE 配对最后的结果*/
    }
    for (i = bdt.FingerDetectNum; i < FINGER_NUM_MAX; i++)
    {
        Pos_X[i] = 0;
        Pos_Y[i] = 0;
    }
    
    if(0 == bdt.FingerDetectNum)
    {
        return;
    }
    
    /**************************************************
    * Step 2A: Triangle Algorithm [1], [0], CUR
    ***************************************************/
    for (j = 0; j < bdt.PrevFingerDetectNum; j++)
    {
        #ifdef RAWPOINT4FINGMATCH
        x     = bdt.DPD[j].Finger_XRAW;
        y     = bdt.DPD[j].Finger_YRAW;
        #else
        x     = bdt.DPD[j].Prev_Finger_X;
        y     = bdt.DPD[j].Prev_Finger_Y;
        #endif
        
        if(x[1] || y[1])                 /* 前帧是有手指的*/
        {
            for (i = 0; i < bdt.FingerDetectNum; i++)
            {
                dist[j][i] = FingProc_TriangleDist3P(x[1], y[1], x[0], y[0], Pos_X[i], Pos_Y[i]);
            }
        }
    }
    
    minNum = bdt.PrevFingerDetectNum;
    if(minNum > bdt.FingerDetectNum) 
    {
        minNum = bdt.FingerDetectNum; 
    }

    for (j = 0; j < minNum; j++)
    {
        /*****************************************************
        * Extract the possible MAPPING
        ******************************************************/
        FingProc_FigureOutLeastFromMatrix(dist, Dim2idx);
        
        if(Dim2idx[0] < FINGER_NUM_MAX)
        {
            #if 1
            flag   = 1;
            
            #ifdef RAWPOINT4FINGMATCH
            x = bdt.DPD[Dim2idx[0]].Finger_XRAW;
            y = bdt.DPD[Dim2idx[0]].Finger_YRAW;
            #else
            x = bdt.DPD[Dim2idx[0]].Prev_Finger_X;
            y = bdt.DPD[Dim2idx[0]].Prev_Finger_Y;
            #endif
            tdist1  = FingProc_Dist2PMeasure(x[0], y[0], Pos_X[Dim2idx[1]], Pos_Y[Dim2idx[1]]); /* 当前距离*/
            tdist2 = FingProc_Dist2PMeasure(x[0], y[0], x[1], y[1]);
            tdist  = (tdist1 + tdist2)>>1;     /* 两条三角边长的平均值*/
            if(tdist > FINGER_ADJUST_DISTANCE_MIN)
            {
               
                tdist = (tdist>>1);                 /* 两条三角边长的平均值 的 0.50(60度)*/
                /**********************************************************************************
                * 如果 三角距离 大于 两条三角边长的平均值 的 3/4, 夹角近弱90°，则趋势判断不好用
                ***********************************************************************************/
                if(dist[Dim2idx[0]][Dim2idx[1]] > tdist) 
                {
                    flag = 0;
                   
                }
            }
            
            /**********************************************************************************
            * 如果 X 方向位移 突然过大 也不行
            ***********************************************************************************/
                        
            if(flag != 0)
            #endif
            {
                           
                /***************************************
                * 手指配对成功
                ****************************************/
                Prev2CurID[Dim2idx[0]] = Dim2idx[1];  /* From Previous to Current*/
                Cur2PrevID[Dim2idx[1]] = Dim2idx[0];
                bdt.DPD[Dim2idx[0]].Finger_X_XMTR = Pos_X[Dim2idx[1]];
                bdt.DPD[Dim2idx[0]].Finger_Y_RECV = Pos_Y[Dim2idx[1]];
                if(MaxPos < Dim2idx[0]) 
                {
                    MaxPos = Dim2idx[0];
                }
            }
            
            for (i = 0; i < bdt.PrevFingerDetectNum; i++) 
            {
                dist[i][Dim2idx[1]] = MAX_UINT16; /* Based on Previous Line*/
            }
            for (i = 0; i < bdt.FingerDetectNum; i++)    
            { 
                dist[Dim2idx[0]][i] = MAX_UINT16; /* Based on Current Line*/
            }
        }
    }
    
    /**********************************************************
    * Step 2B: Distance Algorithm      [0], CUR
    ***********************************************************/
    for (j = 0; j < bdt.PrevFingerDetectNum; j++)
        for (i = 0; i < bdt.FingerDetectNum; i++)
        {
            dist[j][i] = MAX_UINT16;
        }
        
    for (j = 0; j < bdt.PrevFingerDetectNum; j++)
    {   /* Distance Algorithm*/
        #ifdef RAWPOINT4FINGMATCH
        x = bdt.DPD[j].Finger_XRAW;
        y = bdt.DPD[j].Finger_YRAW;
        #else
        x = bdt.DPD[j].Prev_Finger_X;
        y = bdt.DPD[j].Prev_Finger_Y;
        #endif
        if(x[0] || y[0])   
        {    /* Finger Point*/
            if(FINGER_NUM_MAX == Prev2CurID[j]) /* Unmatched Point*/
            {
                for (i = 0; i < bdt.FingerDetectNum; i++)
                {
                    if(FINGER_NUM_MAX == Cur2PrevID[i]) /* Unmatched Point*/
                    {
                    dist[j][i] = FingProc_Dist2PMeasure(x[0], y[0], Pos_X[i], Pos_Y[i]);
                    }
                }
            }
        }    
    }
    
    for (j = 0; j < bdt.PrevFingerDetectNum; j++)
    {
        FingProc_FigureOutLeastFromMatrix(dist, Dim2idx);
        
        if(Dim2idx[0] < FINGER_NUM_MAX)
        {
            #if 1
            flag   = 0;
            #ifdef RAWPOINT4FINGMATCH
            x = bdt.DPD[Dim2idx[0]].Finger_XRAW;
            y = bdt.DPD[Dim2idx[0]].Finger_YRAW;
            #else
            x = bdt.DPD[Dim2idx[0]].Prev_Finger_X;
            y = bdt.DPD[Dim2idx[0]].Prev_Finger_Y;
            #endif
            
            if(x[1] || y[1])
            {
                /****************************************************
                * 如果 有 来回过速 的嫌疑，取消 手指匹配
                *****************************************************/
                if(FingProc_BackForthLineJudge(x[1],y[1],x[0],y[0],Pos_X[Dim2idx[1]],Pos_Y[Dim2idx[1]]))
                {
                    flag  = 1;
                }
                else if(dist[Dim2idx[0]][Dim2idx[1]] > FINGER_ADJUST_DISTANCE_MAX) 
                {
                    uint16_t lost;
                    
                    tdist2 = FingProc_Dist2PMeasure(x[0], y[0], x[1], y[1]);
                    if(x[2] || y[2])
                    {   /*lost变量在这里被临时借用(QFU）*/
                        lost = FingProc_Dist2PMeasure(x[1], y[1], x[2], y[2]);
                        if(tdist2 < lost) 
                        { 
                            tdist2 = lost;
                        }
                    }
                    
                    lost= 0;
                    
                    if((x[0]==x[1]) && (y[0]==y[1])) 
                    { 
                        lost = 1;
                    }
                    /****************************************************
                    * 如果直线距离 大于 原先距离5倍，则距离判断要谨慎
                    * 类似在 AvoidLongLine() 函数里的处理过程
                    *****************************************************/
                    #ifdef RAWPOINT4FINGMATCH
                    if(bdt.DPD[Dim2idx[0]].LifeNumber < 4) 
                    {
                        tdist2 = (tdist2<<2) + tdist2; /* 5倍*/
                        if(tdist2 < 384) 
                        {
                            tdist2 = 384;
                        }
                    }
                    else 
                    { 
                        tdist2 = (tdist2<<1) + tdist2; /* 3倍*/
                        if(tdist2 < 256)
                        {
                            tdist2 = 256;
                        }
                    }
                    #else
                    tdist2 = (tdist2<<2) + tdist2; /* 5倍*/
                    if(tdist2 < 256)
                    {
                        tdist2 = 256;
                    }
                    #endif
                    
                    if(lost != 0)
                    {
                        tdist2 = tdist2<<1;
                    }
                    if(dist[Dim2idx[0]][Dim2idx[1]] > tdist2)
                    {  
                        flag  = 2;
                    }
                    else
                    {
                        /*****************************************************
                        * 如果 X 方向位移 突然过大 也不行
                        *****************************************************/
                        tdist  = FingProc_Dist4Uint16Var(x[0], Pos_X[Dim2idx[1]]); /* 当前X方向距离*/
                        tdist2 = FingProc_Dist4Uint16Var(x[0], x[1]);
                        #ifdef RAWPOINT4FINGMATCH
                        tdist2 = tdist2<<2;
                        if(lost != 0)
                        {
                            tdist2 = tdist2<<1;
                        }
                        #else
                        if(x[2] || y[2]) 
                        {
                            tdist2 += FingProc_Dist4Uint16Var(x[2], x[1]);
                        }
                        tdist2 = tdist2<<3;
                        #endif
                        
                        if(bdt.DPD[i].LifeNumber > 6)
                        {
                            if((tdist > tdist2) && (tdist>256)) 
                            {
                                flag = 3;
                            }
                        }
                    }
                }
            }
            
            if(flag == 0)
            #endif
            {
                Prev2CurID[Dim2idx[0]] = Dim2idx[1];  /* Whatever, we found one match point*/
                Cur2PrevID[Dim2idx[1]] = Dim2idx[0];  /* Whatever, we found one match point*/
                bdt.DPD[Dim2idx[0]].Finger_X_XMTR = Pos_X[Dim2idx[1]];
                bdt.DPD[Dim2idx[0]].Finger_Y_RECV = Pos_Y[Dim2idx[1]];
                if(MaxPos < Dim2idx[0])
                {
                    MaxPos = Dim2idx[0];
                }
            } 
            for (i = 0; i < bdt.PrevFingerDetectNum; i++)
            {
                dist[i][Dim2idx[1]] = MAX_UINT16; /* Based on Previous Line*/
            }
            for (i = 0; i < bdt.FingerDetectNum; i++)   
            {   
                dist[Dim2idx[0]][i] = MAX_UINT16; /* Based on Current Line*/
            }
        }
    }
    
    /*************************************************
    * Step 3: Give a index to the unmatched Point
    **************************************************/
    for (i = 0;i < bdt.FingerDetectNum;i++)
    {
        /*********************************************
        * 如果 flag = 1, 目前手指已经找到匹配
        * 其中配对的"i"是 "手指的ID"
        **********************************************/
        flag = 0;
        for (j = 0; j < bdt.PrevFingerDetectNum; j++)
        {
            if(i == Prev2CurID[j]) 
            {
                flag = 1;
            }
        }
        
        /*********************************************
        * 对未配对的 当前帧 手指 分配一个自由的 ID
        **********************************************/
        if(0 == flag)
        for (j = 0; j < FINGER_NUM_MAX; j++)
        {
            if(FINGER_NUM_MAX == Prev2CurID[j])
            #ifdef RAWPOINT4FINGMATCH
            if(!(bdt.DPD[j].Finger_XRAW[0]||bdt.DPD[j].Finger_YRAW[0]))
            #else
            if(!(bdt.DPD[j].Prev_Finger_X[0]||bdt.DPD[j].Prev_Finger_Y[0]))
            #endif
            if(0 == bdt.DPD[j].Point_Hold)
            {
                Prev2CurID[j] = i; /* j 为 手指 ID*/
                bdt.DPD[j].Finger_X_XMTR = Pos_X[i];
                bdt.DPD[j].Finger_Y_RECV = Pos_Y[i];
                if(MaxPos < j) MaxPos = j;
                break;
            }
        }
    }
    
    /****************************************************
    * Step 4: 得到当前帧的 最大ID,对应出 FingerDetectNum
    *****************************************************/
    if(bdt.FingerDetectNum != 0)
    {
        bdt.FingerDetectNum = MaxPos + 1;
    }
}


/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
void FingProc_SortFingers(void)
{
    uint16_t j,n,m,i;
    uint16_t  MaxDeltaDat16A;
    MaxDeltaDat16A=0;
    #ifdef ABSO_MODE_ACTIVE
    //***********************************
    // SKIP the finger match here
    //***********************************
    if(bdt.FingerDetectNum != 0)
    {
         for (i = 0; i < bdt.FingerDetectNum; i++)
         {
             if(bdt.DPD[i].Finger_X_XMTR || bdt.DPD[i].Finger_Y_RECV)
             {
                 bdt.DPD[0].Finger_X_XMTR = bdt.DPD[i].Finger_X_XMTR;
                 bdt.DPD[0].Finger_Y_RECV = bdt.DPD[i].Finger_Y_RECV;
             }
         }
    }
    #else
    FingProc_SortFingersWithMatrix();
    #endif
    /*************************************************************
    * 下述code, 是为了检验手指点有效性 而设置的 (QFU)
    **************************************************************/
    bdt.LFrame_NUM      = 0;
    for (i = 0; i < FINGER_NUM_MAX;i++)
    {
        bdt.LFrame_X_XMTR[i] = XMTR_NUM;
        bdt.LFrame_Y_RECV[i] = RECV_NUM;
    }
    for (i = 0;i < FINGER_NUM_MAX;i++)
    {
        if(bdt.DPD[i].Finger_X_XMTR || bdt.DPD[i].Finger_Y_RECV)
        {
            bdt.LFrame_X_XMTR[bdt.LFrame_NUM] = (bdt.DPD[i].Finger_X_XMTR)>>8;
            bdt.LFrame_Y_RECV[bdt.LFrame_NUM] = (bdt.DPD[i].Finger_Y_RECV)>>8;
            bdt.LFrame_NUM++;
        }
    }
    
    if(bdt.FingerDetectNum != 0)
    {
        int16_t temp;
        for (j = 0; j < bdt.FingerDetectNum; j++)
        {
            n=bdt.DPD[j].Finger_X_XMTR>>8;
            m=bdt.DPD[j].Finger_Y_RECV>>8;
            temp = (bdt.DeltaDat16A[n][m]>>1)+(bdt.DeltaDat16A[n][m]>>3);
            if(temp > MaxDeltaDat16A)
            {
                MaxDeltaDat16A = temp;
            }
        }
        if(MaxDeltaDat16A>200) 
        {
            bdt.PCBA.MaxValueNoFinger=200;
        }
        else if(MaxDeltaDat16A< 120) 
        {  
            bdt.PCBA.MaxValueNoFinger= 120;
        }
        else 
        {
            bdt.PCBA.MaxValueNoFinger = MaxDeltaDat16A;
        }
    }
    else
    {
        bdt.PCBA.MaxValueNoFinger=200;
    }
    
    for(j = 0; j < FINGER_NUM_MAX; j++)
    {
        if(bdt.DPD[j].Point_Hold > 0)
        {
            bdt.DPD[j].Point_Hold--;
        }
        #ifdef RAWPOINT4FINGMATCH
        bdt.DPD[j].Finger_XRAW[2] = bdt.DPD[j].Finger_XRAW[1];
        bdt.DPD[j].Finger_YRAW[2] = bdt.DPD[j].Finger_YRAW[1];
        bdt.DPD[j].Finger_XRAW[1] = bdt.DPD[j].Finger_XRAW[0];
        bdt.DPD[j].Finger_YRAW[1] = bdt.DPD[j].Finger_YRAW[0];
        bdt.DPD[j].Finger_XRAW[0] = bdt.DPD[j].Finger_X_XMTR;
        bdt.DPD[j].Finger_YRAW[0] = bdt.DPD[j].Finger_Y_RECV;
        #endif
    }
}


/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/

void FingProc_AvoidLongLine(void)
{
    uint16_t i, j, flag_dist; 
    uint16_t curx, cury, x2, y2;
    uint16_t *x, *y;
    uint16_t dist0, dist1;
        
    for (i = 0; i < FINGER_NUM_MAX; i++)
    {
        curx = bdt.DPD[i].Finger_X_XMTR;
        cury = bdt.DPD[i].Finger_Y_RECV;
        /*******************************************************
        * If it is not a FINGER POINT @ the index now
        ********************************************************/
        if((0 == curx) && (0 == cury)) 
        {
            continue;
        }
        x2 = bdt.DPD[i].Finger_X_Reported;
        y2 = bdt.DPD[i].Finger_Y_Reported;
        /*******************************************************
        * If it was not a FINGER POINT @ the index last time 
        ********************************************************/
        if((0 == x2) && (0 == y2) && (0 == bdt.DPD[i].FingerStateFlag))
        {
            continue;
        }
        #ifdef RAWPOINT4FINGMATCH
        x = bdt.DPD[i].Finger_XRAW;
        y = bdt.DPD[i].Finger_YRAW;
        #else
        x = bdt.DPD[i].Prev_Finger_X;
        y = bdt.DPD[i].Prev_Finger_Y;
        #endif
        
        /*******************************************************
        * Distance (Now_Raw_Position - Last_RAW_Position)
        * The distance of the match fingers is too long
        * Maybe it will bring ping-pong line up
        ********************************************************/
        flag_dist = 0;
        
        if(x[0] || y[0]) 
        {
            dist0 = FingProc_Dist2PMeasure(curx, cury, x[0], y[0]);
        }
        else
        { 
            dist0 = 0;
        }
        if(x[1] || y[1]) 
        {
            dist1 = FingProc_Dist2PMeasure(x[1], y[1], x[0], y[0]);
        }
        else
        {
            dist1 = 0;
        }
        
        dist1 = (dist1<<2) + dist1;
        if(dist1 < 512) 
        {
            dist1 = bdt.DPD[i].FingMovingSpeed;
        }
        
        if(dist0 > dist1) 
        { 
            flag_dist = 1;
        }
        
        #if 0
        if(bdt.DPD[i].FingerStateFlag>=STATE_SERIAL_FINGER)
                if(dist1 < bdt.DPD[i].AdjustDistance)
        if(dist0 > 256) flag_dist = 1;
        #endif
        /*******************************************************
        * Threshold Adaptive based on the moving speed
        ********************************************************/
        if(dist0 < FINGER_MOVING_SPEED_THRD_LOW)                      /* DIST = 0 - 200*/
        {
            bdt.DPD[i].FingMovingSpeed = FINGER_FINGER_SPEED_LOW;     /* Slow Speed*/
        }
        else if(dist0 < FINGER_MOVING_SPEED_THRD_MIDDLE)             /* DIST = 200 - 400*/
        {   
            bdt.DPD[i].FingMovingSpeed = FINGER_FINGER_SPEED_MIDDLE;  /* Middle Speed*/
        }
        else                                                         /* DIST = 200 - 400*/
        {
            bdt.DPD[i].FingMovingSpeed = FINGER_FINGER_SPEED_FAST;    /* Fast Speed*/
        }
        
        /**********************************************************************
        * The FINGER Point is moved too long @ the index
        * Then, it is moved to a starting point of another line
        ***********************************************************************/
        if(flag_dist != 0)
        {
            bdt.DPD[i].Finger_X_XMTR = 0;
            bdt.DPD[i].Finger_Y_RECV = 0;
            for (j = 0; j < FINGER_NUM_MAX; j++)
            {
                if(0 == bdt.DPD[j].Finger_X_Reported)
                if(0 == bdt.DPD[j].Finger_Y_Reported)
                if(0 == bdt.DPD[j].Finger_X_XMTR)
                if(0 == bdt.DPD[j].Finger_Y_RECV)
                if(bdt.DPD[j].FingerStateFlag == STATE_NONE_FINGER)
                {
                    bdt.DPD[j].Finger_X_XMTR = curx;
                    bdt.DPD[j].Finger_Y_RECV = cury;
                    if((j + 1) > bdt.FingerDetectNum) 
                    {
                        bdt.FingerDetectNum = j + 1;
                    }
                    break;
                }
            }
        }
    }
}


/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
uint16_t FingProc_XMTR_NolinearMapping_Left(uint16_t h)
{
    uint16_t result;
    #ifdef SHOW_H_DATA
    bdt.Left_h=h;
    #endif
    
    
    #ifdef CN1100_WINNER                    
    if(h < 16)    
    {
        result = h + (h>>2);
    }                           /* 1.25*/
    else if(h<32)
    {
        h -= 16;
        result = 20  + h + (h>>1);
    }                           /* 1.50*/
    else if(h<48)
    {
        h -= 32; 
        result = 44  + (h<<1);
    }                           /* 2.00*/
    else if(h<64)
    {
        h -= 48; 
        result = 76  + (h<<1) + (h>>1) + (h>>2);
    }                           /* 2.75*/
    else if(h<80)
    {
        h -= 64; 
        result = 120  + (h<<1) + h +(h>>1);
    }                           /* 3.50 */
    else         
    {
        h -= 80;
        result = 176  + (h<<2) + h;
    }                          /* 5.00*/
    
    if(result >= (MAX_MAP_VALUE-25)) 
    {
        result = MAX_MAP_VALUE - 25;/*使边界值保持在合理范围内*/
    }
    #else 
    
    #ifdef CN1100_MTK     
    
    if(h < 16)    
    {
        result = h + (h>>3);
    }                           /*  1.125 */
    else if(h<32) 
    {
        h -= 16; 
        result = 18  + h + (h>>2);
    }                           /*  1.25 */
    else if(h<48) 
    {
        h -= 32;
        result = 38  + h + (h>>1);
    }                           /*  1.50 */
    else if(h<64)
    {
        h -= 48; 
        result = 62  + h + (h>>1) + (h>>2);
    }                           /*  1.75 */
    else if(h<80) 
    {
        h -= 64;
        result = 90  + (h<<1);
    }                           /*  2.00 */
    else          
    {
        h -= 80; 
        result = 122  + (h<<1) + (h>>1);
    }                           /*  2.50 */
    
    if(result >= (MAX_MAP_VALUE-60))
    {
        result = MAX_MAP_VALUE - 60;/* 使边界值保持在合理范围内 */
    }
    #else
    #ifdef SH_FEATUREPHONE_13X9
    if(h < 16)   
    {
        result = h+(h>>1);
    }                      /*1.5 */
    else if(h<32) 
    {
        h -= 16; 
        result = 24 + (h<<1);
    }                      /*  2 */
    else if(h<48) 
    {
        h -= 32; 
        result = 56  + (h<<1)+(h>>1);
    }                      /* 2.5 */
    else if(h<64)
    {
        h -= 48;
        result = 96  + (h<<1) + h;
    }                      /*  3 */
    else if(h<80)
    {
        h -= 64;
        result = 144  + (h<<2) + h+(h>>1);
    }                      /*  3.5 */
    else         
    {
        h -= 80; 
        result = 200  + (h<<2);
    }                     /*  4 */
    if(result >= (MAX_MAP_VALUE-5)) 
    {
        result = MAX_MAP_VALUE - 5;/*使边界值保持在合理范围内   */
    }
    #else
    if(h < 16)   
    {
        result = h + (h>>2);
    }                  /*  1.25 */
    else if(h<32) 
    {
        h -= 16; 
        result = 20  + h + (h>>1) + (h>>2);
    }                  /*  1.75 */
    else if(h<48) 
    {
        h -= 32; 
        result = 48  + (h<<1) + (h>>2);
    }                  /*  2.25 */
    else if(h<64)
    {
        h -= 48;
        result = 84  + (h<<1) + h;
    }                  /*  3.00 */
    else if(h<80)
    {
        h -= 64;
        result = 132  + (h<<1) + h + (h>>1) + (h>>2);
    }                  /*  3.75 */
    else         
    {
        h -= 80; 
        result = 192  + (h<<2) + (h>>1);
    }                  /*  4.50 */
    //else          {h -= 96; result = 264  + (h<<2) + (h>>1);}                   /*  5.25 */
    //else          {h -= 112; result = 270 + (h<<2)  + (h>>1);}                  /*  4.50 */
    if(result >= (MAX_MAP_VALUE-25)) 
    {
        result = MAX_MAP_VALUE - 25;/*使边界值保持在合理范围内   */
    }
    #endif
    #endif
    #endif

    return result;
}


/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
uint16_t FingProc_XMTR_NolinearMapping_Right(uint16_t h)
{
    uint16_t result;
    #ifdef SHOW_H_DATA
    bdt.Right_h=h;
    #endif


    #ifdef CN1100_WINNER                    
    if(h < 16)
    {  
        result = h + (h>>2);
    }                           /*  1.25 */
    else if(h<32) 
    {  
        h -= 16;
        result = 20  + h + (h>>1);
    }                           /*  1.50 */
    else if(h<48)
    {   
        h -= 32;
        result = 48  + h + (h>>1) + (h>>2);
    }                           /*  1.75 */
    else if(h<64) 
    {   
        h -= 48; 
        result = 80  + (h<<1);
    }                           /*  2.00 */
    else if(h<80)
    { 
        h -= 64;
        result = 112  + (h<<1) + (h>>2);
    }                           /*  2.25 */
    else if(h<96)
    {
        h -= 80;
        result = 148  + (h<<1) +(h>>1) + (h>>2);
    }                           /*  2.75 */
    else       
    {
        h -= 96;
        result = 192  + h + (h<<1) + (h>>1) + (h>>2);
    }                           /*  3.75     h=110-> H=225.5 */
    
    if(result >= (MAX_MAP_VALUE-25)) 
    {
        result = MAX_MAP_VALUE - 25;/* 使边界值保持在合理范围内 */
    }
    
    #else
    
    #ifdef CN1100_MTK                    
    if(h < 16)   
    {
        result = h + (h>>3);
    }                           /*  1.125 */
    else if(h<32) 
    {
        h -= 16;
        result = 18  + h + (h>>2);
    }                           /*  1.25 */
    else if(h<48)
    { 
        h -= 32;
        result = 38  + h + (h>>1);
    }                           /*  1.50 */
    else if(h<64)
    {
        h -= 48; 
        result = 62  + h + (h>>1) + (h>>2);
    }                           /*  1.75 */
    else if(h<80)
    { 
        h -= 64;
        result = 90  + (h<<1);
    }                           /*  2.00 */
    else         
    {
        h -= 80;
        result = 122  + (h<<1) + (h>>1);
    }                           /*  2.50 */
    
    
    if(result >= (MAX_MAP_VALUE-60)) 
    {
        result = MAX_MAP_VALUE - 60;/* 使边界值保持在合理范围内 */
    }
    
    #else 
    #ifdef SH_FEATUREPHONE_13X9
    if(h < 16)   
    {
        result = h+(h>>1);
    }                 /*1.5 */
    else if(h<32) 
    {
        h -= 16; 
        result = 24 + (h<<1);
    }                  /*  2 */
    else if(h<48) 
    {
        h -= 32; 
        result = 56  + (h<<1)+(h>>1);
    }                  /* 2.5 */
    else if(h<64)
    {
        h -= 48;
        result = 96  + (h<<1) + h;
    }                  /*  3 */
    else if(h<80)
    {
        h -= 64;
        result = 144  + (h<<1) + h+(h>>1);
    }                  /*  3.5 */
    else         
    {
        h -= 80; 
        result = 200  + (h<<2);
    }                  /*  4 */
    if(result >= (MAX_MAP_VALUE-5)) 
    {
        result = MAX_MAP_VALUE - 5;/*使边界值保持在合理范围内   */
    }
    #else
    if(h < 16)  
    {
        result = h + (h>>3);
    }                   /*  1.125 */
    else if(h<32) 
    {
        h -= 16; 
        result = 18  + h + (h>>2)+ (h>>3);
    }                   /*  1.375 */
    else if(h<48)
    {
        h -= 32;
        result = 40  + h + (h>>1) + (h>>2);
    }                  /*  1.75 */
    else if(h<64) 
    {
        h -= 48; 
        result = 68  + (h<<1);
    }                  /*  2.00 */
    else if(h<80)
    {
        h -= 64; 
        result = 100  + (h<<1) + (h>>2);
    }                  /*  2.25 */
    else if(h<96)
    {
        h -= 80;
        result = 136  + (h<<1) + (h>>1);
    }                  /*  2.50 */
    else if(h<112)
    {
        h -= 96;
        result = 176  + h + (h<<1);
    }                  /*  3.00 */
    else        
    {
        h -= 112; 
        result = 224 + (h<<2) + h + (h>>1);
    }                  /*  5.50 */
    if(result >= (MAX_MAP_VALUE-25)) 
    {
        result = MAX_MAP_VALUE - 25;/* 使边界值保持在合理范围内   */
    }
    #endif
    #endif
    #endif
    
    return result;
}



/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
uint16_t FingProc_RECV_NolinearMapping_Top(uint16_t h)
{
    uint16_t result;
    #ifdef SHOW_H_DATA
    bdt.Top_h=h;  
    #endif

    #ifdef CN1100_WINNER                    
    if(h < 16)   
    {
        result = h + (h>>2);
    }                      /*  1.25 */
    else if(h<32)
    {
        h -= 16; 
        result = 20  + h + (h>>1);
    }                      /*  1.50 */
    else if(h<48)
    {
        h -= 32; 
        result = 44  + (h<<1);
    }                      /*  2.00 */
    else if(h<64)
    {
        h -= 48; 
        result = 76  + (h<<1) + (h>>1) + (h>>2);
    }                      /*  2.75 */
    else if(h<80)
    {
        h -= 64; 
        result = 120  + (h<<1) + h +(h>>1);
    }                      /*  3.50 */
    else         
    {
        h -= 80; 
        result = 176  + (h<<2) + h;
    }                      /*  5.00     */
    
    if(result >= (MAX_MAP_VALUE-25))
    { 
        result = MAX_MAP_VALUE - 25;/*  使边界值保持在合理范围内 */
    }
    
    #else
    #ifdef CN1100_MTK                   
    if(h < 16)   
    {
        result = h + (h>>3);
    }                      /*  1.125 */
    else if(h<32)
    {
        h -= 16; 
        result = 18  + h + (h>>2);
    }                      /*  1.25 */
    else if(h<48)
    {
        h -= 32; 
        result = 38  + h + (h>>1);
    }                      /*  1.50 */
    else if(h<64)
    {
        h -= 48; 
        result = 62  + h + (h>>1) + (h>>2);
    }                      /*  1.75 */
    else if(h<80)
    {
        h -= 64; 
        result = 90  + (h<<1);
    }                      /*  2.00 */
    else         
    {
        h -= 80;
        result = 122  + (h<<1) + (h>>1);
    }                      /*  2.50 */
    
    
    if(result >= (MAX_MAP_VALUE-60))
    {
        result = MAX_MAP_VALUE - 60;/*使边界值保持在合理范围内 */
    }
    #else  
    #ifdef SH_FEATUREPHONE_13X9
    if(h < 16)   
    {
        result = h+(h>>1);
    }                 /*1.5 */
    else if(h<32) 
    {
        h -= 16; 
        result = 24 + (h<<1);
    }                  /*  2 */
    else if(h<48) 
    {
        h -= 32; 
        result = 56  + (h<<1)+(h>>1);
    }                  /* 2.5 */
    else if(h<64)
    {
        h -= 48;
        result = 96  + (h<<1) + h;
    }                  /*  3 */
    else if(h<80)
    {
        h -= 64;
        result = 144  + (h<<1) + h+(h>>1);
    }                  /*  3.5 */
    else         
    {
        h -= 80; 
        result = 200  + (h<<2);
    }                  /*  4 */
    if(result >= (MAX_MAP_VALUE-25)) 
    {
        result = MAX_MAP_VALUE - 25;/*使边界值保持在合理范围内   */
    }
//  result = (h<<1)+(h>>1);
//  if(result >= (MAX_MAP_VALUE-25)) 
//  {
//      result = MAX_MAP_VALUE - 25;/*使边界值保持在合理范围内   */
//  }
    #else
    if(h < 16)   
    {
        result = h + (h>>2);
    }                   /*  1.25 */
    else if(h<32)
    {
        h -= 16; 
        result = 20  + h + (h>>1);
    }                   /*  1.5 */
    else if(h<48)
    {
        h -= 32;
        result = 44  + h + (h>>1) + (h>>2);
    }                   /*  1.75 */
    else if(h<64)
    {
        h -= 48;
        result = 72  + (h<<1) + (h>>1);
    }                   /*  2.5 */
    else if(h<80)
    {
        h -= 64; 
        result = 112  + (h<<1) + h;
    }                   /*  3.00 */
    else if(h<96)
    {
        h -= 80; 
        result = 160  + (h<<1) + h + (h>>1);
    }                   /*  3.50 */
    else         
    {
        h -= 96;
        result = 216  + (h<<2) + (h>>1);
    }                   /*  4.50 */
    if(result >= (MAX_MAP_VALUE-25))
    {
        result = MAX_MAP_VALUE - 25;/* 使边界值保持在合理范围内 */
    }
    #endif
    #endif
    #endif
    return result;
}


/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
uint16_t FingProc_RECV_NolinearMapping_Bottom(uint16_t h)
{
    uint16_t result;
    #ifdef SHOW_H_DATA
    bdt.Bottom_h=h;
    #endif


    #ifdef CN1100_WINNER                     
    if(h < 16)   
    {
        result = h + (h>>2);
    }                  /*  1.25 */
    else if(h<32) 
    {
        h -= 16;
        result = 20  + h + (h>>1);
    }                  /*  1.50 */
    else if(h<48)
    {
        h -= 32; 
        result = 48  + h + (h>>1) + (h>>2);
    }                  /*  1.75 */
    else if(h<64) 
    {
        h -= 48;
        result = 80  + (h<<1);
    }                  /*  2.00 */
    else if(h<80)
    {
        h -= 64;
        result = 112  + (h<<1) + (h>>1);
    }                  /*  2.50 */
    else if(h<96)
    {
        h -= 80;
        result = 152  + (h<<1) + h;
    }                  /*  3.00 */
    else         
    {
        h -= 96;
        result = 200  + h + (h<<1) + (h>>1) + (h>>2);
    }                  /*  3.75     h=105-> H=225.75 */
    
    if(result >= (MAX_MAP_VALUE-25))
    {
        result = MAX_MAP_VALUE - 25; /* 使边界值保持在合理范围内 */
    }
    
    #else
    #ifdef CN1100_MTK                    
    if(h < 16)   
    {
        result = h + (h>>3);
    }                  /*  1.125 */
    else if(h<32)
    {
         h -= 16; 
         result = 18  + h + (h>>2);
    }                  /*  1.25 */
    else if(h<48) 
    {
         h -= 32; 
         result = 38  + h + (h>>1);
    }                  /*  1.50 */
    else if(h<64) 
    {
         h -= 48; 
         result = 62  + h + (h>>1) + (h>>2);
    }                  /*  1.75 */
    else if(h<80)
    {     
        h -= 64;
        result = 90  + (h<<1);
    }                  /*  2.00 */
    else         
    {
        h -= 80;
        result = 122  + (h<<1) + (h>>2);
    }                  /*  2.25 */
    
    
    if(result >= (MAX_MAP_VALUE-60))
    {
        result = MAX_MAP_VALUE - 60;/* 使边界值保持在合理范围内 */
    }
    
    #else
    #ifdef SH_FEATUREPHONE_13X9
    if(h < 16)   
    {
        result = h+(h>>1);
    }                 /*1.5 */
    else if(h<32) 
    {
        h -= 16; 
        result = 24 + (h<<1);
    }                  /*  2 */
    else if(h<48) 
    {
        h -= 32; 
        result = 56  + (h<<1)+(h>>1);
    }                  /* 2.5 */
    else if(h<64)
    {
        h -= 48;
        result = 96  + (h<<1) + h;
    }                  /*  3 */
    else if(h<80)
    {
        h -= 64;
        result = 144  + (h<<1) + h+(h>>1);
    }                  /*  3.5 */
    else         
    {
        h -= 80; 
        result = 200  + (h<<2);
    }                  /*  4 */
    if(result >= (MAX_MAP_VALUE-5)) 
    {
        result = MAX_MAP_VALUE - 5;/*使边界值保持在合理范围内   */
    }
    #else
    if(h < 16)  
    {
        result = h + (h>>3);
    }                 /*  1.125 */
    else if(h<32)
    {
        h -= 16; 
        result = 18  + h + (h>>2);
    }                 /*  1.25 */
    else if(h<48)
    {
        h -= 32; 
        result = 38  + h + (h>>1);
    }                 /*  1.5 */
    else if(h<64)
    {
        h -= 48; 
        result = 62  + (h<<1) + (h>>1);
    }                 /*  2.5 */
    else if(h<80)
    {
        h -= 64; 
        result = 102  + (h<<1) + h;
    }                 /*  3.00   */
    else if(h<96)
    {
        h -= 80; 
        result = 150  + (h<<1) + h + (h>>1);
    }                 /*  3.50 */
    else         
    {
        h -= 96;
        result = 206  + (h<<2) + (h>>1);
    }                 /*  4.50 */
    if(result >= (MAX_MAP_VALUE-15))
    { 
        result = MAX_MAP_VALUE - 15; /* 使边界值保持在合理范围内 距离 */
    }
    #endif
    #endif
    #endif
    return result;
}



/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
void FingProc_ImproveEdgePoint(void)
{
    uint16_t index, h;
    #ifdef BORDER_ADJUSTABLE
    int8_t left,right,top,bottom;
    left=1; right=2; top=3; bottom=4;
    #endif
    
    for (index = 0; index < bdt.FingerDetectNum; index++)
    {
        /************************************************
        * Process XMTR direction
        ************************************************/
        if((bdt.DPD[index].Finger_X_XMTR < MAX_MAP_VALUE) && bdt.DPD[index].Finger_X_XMTR)
        {
            h = MAX_MAP_VALUE - bdt.DPD[index].Finger_X_XMTR;
            #ifdef SHOW_H_DATA
            bdt.Left_h=h;
            #endif
            #ifdef BORDER_ADJUSTABLE
            bdt.DPD[index].Finger_X_XMTR = MAX_MAP_VALUE -FingProc_NolinearMapping(left,h);
            #else
            bdt.DPD[index].Finger_X_XMTR = MAX_MAP_VALUE - FingProc_XMTR_NolinearMapping_Left(h);
            #endif
        }
        else if(bdt.DPD[index].Finger_X_XMTR > ((SXMTR_NUM<<8) - MAX_MAP_VALUE))
        {
            h = bdt.DPD[index].Finger_X_XMTR - ((SXMTR_NUM<<8) - MAX_MAP_VALUE);
            #ifdef SHOW_H_DATA
            bdt.Right_h=h;
            #endif
            #ifdef BORDER_ADJUSTABLE
            bdt.DPD[index].Finger_X_XMTR = ((SXMTR_NUM<<8) - MAX_MAP_VALUE) + FingProc_NolinearMapping(right,h);
            #else
            bdt.DPD[index].Finger_X_XMTR = ((SXMTR_NUM<<8) - MAX_MAP_VALUE) + FingProc_XMTR_NolinearMapping_Right(h);
            #endif
            
        }
        
        /******************************************************************************
        * Process RECV direction
        *******************************************************************************/
        if((bdt.DPD[index].Finger_Y_RECV < MAX_MAP_VALUE) && bdt.DPD[index].Finger_Y_RECV)
        {
            h = MAX_MAP_VALUE - bdt.DPD[index].Finger_Y_RECV;
            #ifdef SHOW_H_DATA
            bdt.Top_h=h;
            #endif
            #ifdef BORDER_ADJUSTABLE
            bdt.DPD[index].Finger_Y_RECV = MAX_MAP_VALUE -FingProc_NolinearMapping(top,h);
            #else 
            bdt.DPD[index].Finger_Y_RECV = MAX_MAP_VALUE - FingProc_RECV_NolinearMapping_Top(h);
            #endif
        }
        else if(bdt.DPD[index].Finger_Y_RECV > ((RECV_NUM<<8) - MAX_MAP_VALUE))
        {
            h = bdt.DPD[index].Finger_Y_RECV - ((RECV_NUM<<8) - MAX_MAP_VALUE);
            #ifdef SHOW_H_DATA
            bdt.Bottom_h=h;
            #endif
            #ifdef BORDER_ADJUSTABLE
            bdt.DPD[index].Finger_Y_RECV = ((RECV_NUM<<8) - MAX_MAP_VALUE)+FingProc_NolinearMapping(bottom,h);
            #else 
            bdt.DPD[index].Finger_Y_RECV = ((RECV_NUM<<8) - MAX_MAP_VALUE) + FingProc_RECV_NolinearMapping_Bottom(h);
            #endif 
        }
    }
}


/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/

void FingProc_MergeClosingPoints(void)
{
    uint16_t i, j, k;
    uint16_t x1, y1, x2, y2;
    uint16_t distance, distThr;
    uint16_t prevNum;
    uint16_t flag = 1;
    
    /********************************************************************
    *alculating total finger number in the last Frame (Previous Frame)
    *********************************************************************/
    prevNum = 0;
    for (k = 0; k < bdt.PrevFingerDetectNum; k++)
    {
        if(bdt.DPD[k].Prev_Finger_X[0] || bdt.DPD[k].Prev_Finger_Y[0]) 
        {
            prevNum++;
        }
    }
    
    /********************************************************************
    * The Processing for the mergement is step by step
    *********************************************************************/
    while(flag)
    {
        flag = 0;
        
        /****************************************************************
        * Threshold used in Judgement for finger is set. 
        * If more finger this time, threshold will be longer
        *****************************************************************/
        distThr = FINGER_UNITE_THRESHOLD;  /* = 192, about 6mm*/
        if(bdt.FingerDetectNum > prevNum)
        {
            distThr = FINGER_SEPATATE_THRESHOLD; /* 380-12mm*/
        }
        if(bdt.FingerDetectNum > 1)
        for (i = 0; i < bdt.FingerDetectNum;i++)
        for (j = i+1;j < bdt.FingerDetectNum;j++)
        {
            x1 = bdt.DPD[i].Finger_X_XMTR;
            y1 = bdt.DPD[i].Finger_Y_RECV;
            x2 = bdt.DPD[j].Finger_X_XMTR;
            y2 = bdt.DPD[j].Finger_Y_RECV;
            distance = FingProc_Dist2PMeasure(x1, y1, x2, y2);
           /****************************************************************
            * If DIST is too short, merge them; this will avoid some NOISE
            *****************************************************************/
            if(distance <= distThr)
            {
                flag = 1;
                bdt.FingerDetectNum--;
                
               /************************************************************
                * Reset the threshold based on the new condition
                *************************************************************/
                distThr = FINGER_UNITE_THRESHOLD;
                if(bdt.FingerDetectNum > prevNum) 
                {
                    distThr = FINGER_SEPATATE_THRESHOLD;
                }
                
               /************************************************************
                * Merge two points into one point
                ************************************************************/
                bdt.DPD[i].Finger_X_XMTR = (x1+x2)>>1;
                bdt.DPD[i].Finger_Y_RECV = (y1+y2)>>1;
                
               /************************************************************
                * Move the rest point to new position
                *************************************************************/
                for (k = j; k < bdt.FingerDetectNum; k++)
                {
                    bdt.DPD[k].Finger_X_XMTR = bdt.DPD[k + 1].Finger_X_XMTR;
                    bdt.DPD[k].Finger_Y_RECV = bdt.DPD[k + 1].Finger_Y_RECV;
                }
            }
        }
    }
    
    /********************************************************************
    * Set the remaining Finger Position as empty space
    *********************************************************************/
    for (i = bdt.FingerDetectNum; i < FINGER_NUM_MAX;i++)
    {
        bdt.DPD[i].Finger_X_XMTR = 0;
        bdt.DPD[i].Finger_Y_RECV = 0;
    }
}

/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
void FingProc_FingerInfoReorder(void)
{
    uint16_t i, FingNum;
 
    /* valid finger number */
    FingNum = 0;
    for (i = 0; i < bdt.FingerDetectNum;i++)
    {
        if(bdt.DPD[i].Finger_X_XMTR || bdt.DPD[i].Finger_Y_RECV)
        {
            bdt.DPD[FingNum].Finger_X_XMTR = bdt.DPD[i].Finger_X_XMTR;
            bdt.DPD[FingNum].Finger_Y_RECV = bdt.DPD[i].Finger_Y_RECV;
            FingNum++;
        }
    }
    bdt.FingerDetectNum = FingNum;
    for(i = FingNum; i < FINGER_NUM_MAX;i++)
    {
        bdt.DPD[i].Finger_X_XMTR = 0;
        bdt.DPD[i].Finger_Y_RECV = 0;
    } 
}

/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
void DataProc_HandleFingerInfo(void)
{
    uint16_t i;
    FingProc_FingerInfoReorder();
#ifndef ABSO_MODE_ACTIVE
    FingProc_MergeClosingPoints();  
    FingProc_ImproveEdgePoint();  /* optimize edge finger XY value */
#endif    
    FingProc_SortFingers();
     
#ifndef ABSO_MODE_ACTIVE
    FingProc_CheckCrossedLines();
    FingProc_AvoidLongLine();
#endif
    bdt.FingerRealNum = 0;
    for(i=0; i<bdt.FingerDetectNum; i++)
    {
        if(bdt.DPD[i].Finger_X_XMTR || bdt.DPD[i].Finger_Y_RECV)
        {
            bdt.FingerRealNum++;
        }
    }
#ifndef ABSO_MODE_ACTIVE
    FingProc_SuperFilter4Edge();
#endif
    FingProc_ImproveByMultiFilters();
    bdt.PrevFingerDetectNum2 = bdt.PrevFingerDetectNum;
    bdt.PrevFingerDetectNum  = bdt.FingerDetectNum;
    
}


/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
void DataProc_Screen_Adaptive(uint16_t max,uint16_t min)
{
#ifdef SCREEN_ADAPTIVE
    #ifdef SH_FEATUREPHONE_13X9
    if(max>4000)
    {
        if(min>2357)
        {
            bdt.ScreenAdaptiveFlag  = 1; 
            bdt.PCBA.HighRefSet     = HIGH_REF_2PF;
            bdt.PCBA.HighRefGainSet = REF_GAIN_12PF;
            bdt.PCBA.LowRefSet      = LOW_REF_1PF; 
            bdt.PCBA.LowRefGainSet  = REF_GAIN_12PF;
        }
    }
    else if((max<4000)&&(max>=2128))
    {
        if(min>=1690)
        {
            bdt.ScreenAdaptiveFlag  = 1; 
            bdt.PCBA.LowRefSet      = LOW_REF_1PF; 
            bdt.PCBA.LowRefGainSet  = REF_GAIN_12PF;
        }
    }
    else if((max<2128)&&(max>=1300))
    {
        if(min>=0)
        {
            bdt.ScreenAdaptiveFlag  = 1; 
            bdt.PCBA.HighRefSet     = HIGH_REF_1PF;
            bdt.PCBA.HighRefGainSet = REF_GAIN_12PF;
            bdt.PCBA.LowRefSet      = LOW_REF_0PF; 
            bdt.PCBA.LowRefGainSet  = REF_GAIN_16PF;
        }
        else if(min>=333)
        {
            bdt.ScreenAdaptiveFlag  = 1; 
            bdt.PCBA.HighRefSet     = HIGH_REF_1PF;
            bdt.PCBA.HighRefGainSet = REF_GAIN_12PF;
            bdt.PCBA.LowRefSet      = LOW_REF_1PF; 
            bdt.PCBA.LowRefGainSet  = REF_GAIN_16PF;
        }
    }
    else if(max<1300)
    {
        bdt.ScreenAdaptiveFlag  = 1; 
        bdt.PCBA.HighRefSet     = HIGH_REF_1PF;
        bdt.PCBA.HighRefGainSet = REF_GAIN_16PF;
        bdt.PCBA.LowRefSet      = LOW_REF_0PF; 
        bdt.PCBA.LowRefGainSet  = REF_GAIN_16PF;
    }
    if(bdt.ScreenAdaptiveFlag!=0)
    {
        bdt.BFD.InitCount++;
    }
    #else
    if(max>=4000)
    {
       
        if(min>2535)
        {
            bdt.ScreenAdaptiveFlag  = 1; 
            bdt.PCBA.HighRefSet     = HIGH_REF_4PF;
            bdt.PCBA.HighRefGainSet = REF_GAIN_12PF;
            bdt.PCBA.LowRefSet      = LOW_REF_2PF; 
            bdt.PCBA.LowRefGainSet  = REF_GAIN_12PF;
        }
    }
    else if((max<4000)&&(max>=1950))
         {
             if(min>1517)
             {
                 bdt.ScreenAdaptiveFlag	  = 1;
                 bdt.PCBA.LowRefSet			= LOW_REF_2PF; 
                 bdt.PCBA.LowRefGainSet		= REF_GAIN_16PF;
             }  
           
         }
    
    else if((max<1950)&&(max>=975))
         {
             bdt.ScreenAdaptiveFlag  = 1;
             bdt.PCBA.HighRefSet	  = HIGH_REF_2PF;
             bdt.PCBA.HighRefGainSet = REF_GAIN_12PF;
            
             bdt.PCBA.LowRefSet      = LOW_REF_0PF; 
             bdt.PCBA.LowRefGainSet  = REF_GAIN_12PF;
            
         }
  
    else if(max<975)
         { 
             bdt.ScreenAdaptiveFlag  = 1;
             bdt.PCBA.HighRefSet	  = HIGH_REF_2PF;
             bdt.PCBA.HighRefGainSet = REF_GAIN_16PF;
             bdt.PCBA.LowRefSet	  = LOW_REF_0PF; 
             bdt.PCBA.LowRefGainSet  = REF_GAIN_16PF;  
         }
  
    if(bdt.ScreenAdaptiveFlag!=0)
    {
        bdt.BFD.InitCount++;
    }
    #endif
#endif
}

/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
void DataProc_DepressRecvLineNoise0(uint16_t TXi)
{
    uint8_t  j;
    uint8_t  tempcount = 0;
    int16_t  aveval    = 0;
    int16_t  maxval    = 0;
    int16_t  sum       = 0;
    int16_t  minval    = MAX_INT16;
    uint16_t tempnum;
    
                 /********************************************************************************
                  *Compute MAX value and SUM if the sample is belower than threshold
                  ********************************************************************************/
    for (j = 0;j < RECV_NUM;j++)
    {
        if(abs16(bdt.DeltaDat16A[TXi][j]) < bdt.ThresholdInFrame)
        {
            tempcount++;
            sum += bdt.DeltaDat16A[TXi][j];
            if(maxval < bdt.DeltaDat16A[TXi][j]) 
            {
                maxval = bdt.DeltaDat16A[TXi][j]; /*get max value position*/
            }
            if(minval > bdt.DeltaDat16A[TXi][j]) 
            {
                minval = bdt.DeltaDat16A[TXi][j]; /*get min value position*/
            }
        }
    }
    
        /********************************************************************************
         *Compute Average value if the sample is belower than threshold
         ********************************************************************************/
    if(tempcount != 0)
    {
        aveval = divideINT16byUINT8(sum, tempcount);
    }
        /********************************************************************************
         * If MAX value is too big, Recompute average value
         ********************************************************************************/
    if(((aveval > 10) && (maxval > (aveval<<1)))||((aveval < 10) && (maxval > 100)))
    {
        tempcount = 0;
        sum       = 0;
        for (j = 0; j < RECV_NUM; j++)
        {   
            if((abs16(bdt.DeltaDat16A[TXi][j]) < aveval) && (abs16(bdt.DeltaDat16A[TXi][j]) < bdt.ThresholdInFrame))
            {
                tempcount++;
                sum += bdt.DeltaDat16A[TXi][j];
            }
        }
        /****************************************
         * Compute average value
         ****************************************/
        if(tempcount != 0) 
        {
            aveval = divideINT16byUINT8(sum, tempcount);
        }
        else
        {
            aveval = 0;
        }
    }
         /*****************************************************************************
          *if Min value is too big Recompute average value
          *****************************************************************************/
    if((aveval < -10 && minval < (aveval<<1))||((aveval > (-10)) && (minval < -100)))
    {
        tempcount = 0;
        sum       = 0;
        for (j = 0; j < RECV_NUM; j++)
        { 
            if((abs16(bdt.DeltaDat16A[TXi][j]) > aveval)&&(abs16(bdt.DeltaDat16A[TXi][j]) < bdt.ThresholdInFrame))
            {
                tempcount++;
                sum += bdt.DeltaDat16A[TXi][j];
            }
        }
                /****************************************
                 *Compute average value
                 ****************************************/
        if(tempcount != 0) 
        {
            aveval = divideINT16byUINT8(sum, tempcount);
        }
        else
        {
            aveval = 0;
        }
    }
    /* Reseting threshold value to calculate average value   */
    if(abs16(minval) > abs16(aveval<<3) || abs16(maxval) > abs16(aveval<<3))
    {
        sum = 0;
        tempcount = 0;
        if(abs16(minval) > abs16(maxval))
        {
            tempnum = abs16(minval);
        }
        else
        {
            tempnum = abs16(maxval);
        }
        for (j = 0; j < RECV_NUM; j++)
        {
            if(abs16(bdt.DeltaDat16A[TXi][j]) < (tempnum>>1))
            {
                tempcount++;
                sum += bdt.DeltaDat16A[TXi][j];
            }
        }    
        if(tempcount != 0) 
        {
            aveval = divideINT16byUINT8(sum, tempcount);
        }
        else
        {
            aveval = 0;
        }
    }
      /********************************************************************************
              * Adjust delta data by average value
              ********************************************************************************/
    for (j = 0; j < RECV_NUM; j++) 
    {
        bdt.DeltaDat16A[TXi][j] -= aveval;
    }
      /****************************************
              * Record All these average value
              ****************************************/
    bdt.AveDeltaXmtr[TXi] = aveval;
}


/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/

void DataProc_DepressNoise(void)
{
    int16_t  i;
    
    for (i = 0;i < XMTR_NUM;i++)
    {
        DataProc_DepressRecvLineNoise0(i);

    }
}


/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/

void DataProc_FindMaxAndMinValue(void)
{
    uint32_t i, j;
    int16_t  tempdat16;
    
    bdt.MaxValueInFrame = 0; /* Max Value*/
    bdt.MinValueInFrame = 0; /* Min Value*/
    bdt.FRM_MAX_X_XMTR  = 0; /* Max-X-Location*/
    bdt.FRM_MAX_Y_RECV  = 0; /* Max-Y-Location*/
    bdt.FRM_MIN_X_XMTR  = 0; /* Min-X-Location*/
    bdt.FRM_MIN_Y_RECV  = 0; /* Min-Y-Location*/
    
    for (i=0; i<XMTR_NUM; i++)
        for (j=0; j<RECV_NUM; j++)
        {
            tempdat16 = bdt.DeltaDat16A[i][j];
            /*******************************************************
                          * Extract the Max/Min Value of the Delta Frame data
                          *******************************************************/
            if(tempdat16 > bdt.MaxValueInFrame)
            {
                bdt.MaxValueInFrame   = tempdat16;
                bdt.FRM_MAX_X_XMTR    = i;
                bdt.FRM_MAX_Y_RECV    = j;
            }
            
            if(tempdat16 < bdt.MinValueInFrame)
            {
                bdt.MinValueInFrame = tempdat16;
                bdt.FRM_MIN_X_XMTR  = i;
                bdt.FRM_MIN_Y_RECV  = j;
            }
            
            /****************************************************************
                          * The following code is useful, it will make sure DEBUG value 
                          * shown in PC terminal will not keep the old debug value (QFU)
                          *****************************************************************/
            #ifdef COMMUNICATION_WITH_PC
            bdt.SigDeltaDat[i][j] = 0;
            #endif
            
            #ifdef CN1100_LINUX
            bdt.SigDeltaDat[i][j] = 0;
            #endif
        }
}


/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
void DataProc_CalculateDeltaData(uint16_t *buffer)
{
    uint32_t i, j;
    
    /*******************************************************
          * Get the Delta Value in a X/Y point
          *******************************************************/
    if(1 == bdt.PCBA.TxPolarity)
    {
        for (i=0; i<XMTR_NUM; i++)
            for (j=0; j<RECV_NUM; j++)
            {
                bdt.DeltaDat16A[i][j] = (int16_t)(bdt.BFD.BaseDat[i][j]) - (int16_t)(buffer[i*RECV_NUM+j]);
            }
    }
    else
    {
        for (i=0; i<XMTR_NUM; i++)
            for (j=0; j<RECV_NUM; j++)
            {
                bdt.DeltaDat16A[i][j] = (int16_t)(buffer[i*RECV_NUM+j]) - (int16_t)(bdt.BFD.BaseDat[i][j]);
            }
    }
}


/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
int16_t DataProc_Ratio2Threshold(int16_t MaxValue, uint16_t flag)
{
    int16_t value = 0;
    
    switch(flag)
    {
        case BE20PERCENT:
        {
            value  = MaxValue>>3;    /* 0.125*/
            value += MaxValue>>4;    /* 0.0625*/
            value += MaxValue>>6;    /* 0.015625, Total: 0.203125*/
            break;
        }
        case BE25PERCENT:
        {
            value  = MaxValue>>2;    /* 0.25*/
            break;
        }
        
        case BE30PERCENT:
        {
            value  = MaxValue>>2;    /* 0.25*/
            value += MaxValue>>4;    /* 0.0625, Total: 0.3125*/
            break;
        }
        case BE40PERCENT:
        {
            value  = MaxValue>>2;    /* 0.25*/
            value += MaxValue>>3;    /* 0.125 */
            value += MaxValue>>5;    /*0.03125 Total:0.40625*/
            break;
        }
    }
    
    /**********************************************************************
    * It is defined as 50 at the begining, but it could be tuned by TOOLs
    ***********************************************************************/
    if(value < bdt.PCBA.FinThrMin) 
    {
        value = bdt.PCBA.FinThrMin;
    }
    return value;
}



/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
void DataProc_DeltaMatrix2Array(uint32_t *BitDat, int16_t thr)
{
    uint32_t i, j;
    
    bdt.FingerDetectNum = 0;
    for (i=0; i<XMTR_NUM; i++)
    {
        BitDat[i] = 0;
        for (j=0; j<RECV_NUM; j++)
        {
            if((bdt.DeltaDat16A[i][j]) > thr)
            {
                BitDat[i]  |= (1<<j);    /* 逐行记录 */
                bdt.FingerDetectNum++;
            }
        }
    }
}


/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
uint16_t DataProc_PixelR2RConnected(uint32_t pg, uint32_t bits)
{

    if((pg & bits) || ((pg&(bits>>1))||(pg&(bits<<1))))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}


/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
void DataProc_MergePixelGroup(uint16_t curXMTR, uint32_t pg)
{
    uint16_t i, p;
    uint16_t FlagDoesOverlap = 0;
    uint32_t bits;
    
    
    if (bdt.FingerDetectNum && (bdt.FingerDetectNum <= FINGER_NUM_MAX) && curXMTR)
    {
        i = bdt.FingerDetectNum;
        p = 0;
        
        while(i--)
        {
            bits = bdt.DPD[p].BitMapDat[curXMTR - 1];  
            if (DataProc_PixelR2RConnected(pg, bits))
            {
                bdt.DPD[p].BitMapDat[curXMTR] |= pg;
                FlagDoesOverlap = 1;
            }
            p++;  /* Point to the next pixel group*/
        }
    }
    
    /**************************************************
          * If this pixel group is not connected, then we 
          * need to start a new segmented image.
          ***************************************************/
    if(FlagDoesOverlap == 0)
    {
        if(bdt.FingerDetectNum < FINGER_NUM_MAX)
        {
            bdt.DPD[bdt.FingerDetectNum].BitMapDat[curXMTR] = pg;
            bdt.FingerDetectNum += 1;
        }
    }
}


/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
uint16_t DataProc_bBitmapsIntersect(uint32_t *ar1, uint32_t *ar2)
{
    uint16_t i;
    for (i = 0;i < XMTR_NUM;i++)
    {
        if(ar1[i]&ar2[i]) 
        {
            return 1;
        }
    }
    return 0;
}


/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
uint16_t DataProc_CompareAndMerge(uint32_t *ar1, uint32_t *ar2, uint16_t m1, uint16_t m2)
{
    if((bdt.FingerDetectBitFlag & m1) && DataProc_bBitmapsIntersect(ar1,ar2))
    {
        /* Merge the bitmaps together*/
        uint16_t i,j;
        for (i = 0;i < XMTR_NUM;i++)
        {
            ar1[i] |= ar2[i];
        }
        /*delete Merged bitmaps*/
        for (i = m2;i < bdt.FingerDetectNum - 1;i++)
            for (j = 0;j < XMTR_NUM;j++)
            { 
                bdt.DPD[i].BitMapDat[j] = bdt.DPD[i + 1].BitMapDat[j];
            }
        
        
        return 1;
    }
    return 0;
}

/*****************************************************************************
* Purpose: Loop through all of the segmented bitmaps and determine if any
*          intersect. If they do, then merge them together.
******************************************************************************/
/*******************************************************************************
  * Function Name  : 
  * Description    : 
  * Input          : 
  * Output         : 
  * Return         : 
  *******************************************************************************/

void DataProc_MergeApartBitmaps(void)
{
    uint16_t aMergeFlag, nAS, i, j, m;
    uint32_t *pSBM1, *pSBM2;
    
    VarSL(bdt.FingerDetectBitFlag, bdt.FingerDetectNum, FINGER_NUM_MAX);
    bdt.FingerDetectBitFlag--;
    
    if(bdt.FingerDetectNum > 1)
    {
        do
        {
            aMergeFlag = 0;
            nAS        = bdt.FingerDetectNum;
            for (i=0,m=1; i<nAS-1; i++,m<<=1)
            {
                pSBM1 = bdt.DPD[i].BitMapDat;
                if(bdt.FingerDetectBitFlag & m)
                {
                    uint16_t n = m<<1;
                    pSBM2 = bdt.DPD[i + 1].BitMapDat;
                    for (j=i+1; j<nAS; j++,n<<=1,pSBM2 = bdt.DPD[j].BitMapDat)
                    {
                        if (DataProc_CompareAndMerge(pSBM1, pSBM2, n, j)!= 0) 
                        {
                            /* The two bit maps intersect. So, get rid of one.*/
                            aMergeFlag = 1;
                            bdt.FingerDetectBitFlag ^= (1<<bdt.FingerDetectNum);
                            bdt.FingerDetectNum--;
                        }
                    }
                }
            }
        }while(aMergeFlag);   /*if  merge one time,check  if need more */
    }
}



/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/

uint32_t DataProc_GetExtents(uint32_t *colExtent,uint32_t index)
{
    uint32_t rExtent=0;    /* Row extents*/
    uint32_t cExtent=0;    /* Column extents*/
    int16_t i;
    
    for (i=0; i<XMTR_NUM; i++)
    {
        uint32_t r;    /* Current row*/
        r = bdt.DPD[index].BitMapDat[i];
        rExtent <<= 1;
        if(r != 0) 
        {
            rExtent |= 1;
            cExtent |= r; 
        }
    }
    *colExtent = cExtent;
    return rExtent;
}


/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/

int16_t DataProc_computePosition(int16_t* buffer, uint16_t size,uint8_t Flag)
{
    uint16_t i;
    uint32_t iWeightedBufferSum=0;
    uint32_t iBufferSum = 0;
    int16_t  p = 0;
    
        /**********************************************
         * Length of Array is 1, return 1;
         * Length of Array is 0, return 0;
         **********************************************/
    if (size < 2) 
    {
        return size;
    }
    if(Flag == BORDER_POSITION)
    {
        for (i = 0;i < size;i++) 
            if (buffer[i] > 0) 
            {
                iWeightedBufferSum += multiply16by16((uint16_t)buffer[i], (uint16_t)buffer[i]) * (i + 1);
                iBufferSum += (uint32_t)(buffer[i] * buffer[i]);
            }
        if (iBufferSum != 0) 
        { 
            p = (int16_t)(((uint32_t)(iWeightedBufferSum<<8))/iBufferSum) - 128;
        }
    }
    else
    {
        for (i = 0;i < size;i++) 
            if (buffer[i] > 0) 
            {
                iWeightedBufferSum += multiply16by16(i + 1, (uint16_t)buffer[i]);
                iBufferSum += (uint32_t)buffer[i];
            }
        if (iBufferSum != 0)
        {
            p = (int16_t)(((uint32_t)(iWeightedBufferSum<<8))/iBufferSum) - 128;
        }
    }
    return p;
}

/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/

void DataProc_GetPoint(uint16_t index)
{
    int16_t *pX,*pY;
    uint8_t xFlag, yFlag;
    uint16_t maxX, maxY;
    
    xFlag = NORMAL_POSITION; /* BORDER_POSITION is no longer existed*/
    yFlag = NORMAL_POSITION; /* BORDER_POSITION is no longer existed*/
    
    pX = bdt.DPD[index].pBuffer_X; /* Data used for calculating the location X*/
    pY = bdt.DPD[index].pBuffer_Y; /* Data used for calculating the location Y*/
    
    maxX = bdt.DPD[index].Extremum_Pos_X[0];
    maxY = bdt.DPD[index].Extremum_Pos_Y[0];

    if(0 == maxX)
    {
        xFlag = BORDER_POSITION;
    }
    if(0 == maxY) 
    {
        yFlag = BORDER_POSITION;
    }
    if(maxX == bdt.DPD[index].start_x + bdt.DPD[index].len_x - 1)
    { 
        xFlag = BORDER_POSITION;
    }
    if(maxY == bdt.DPD[index].start_y + bdt.DPD[index].len_y - 1)
    { 
        yFlag = BORDER_POSITION;
    }
    
    #ifdef PRESS_KEY_DETECT
    if((KEY_TX_LOC-1) == maxX)
    {    
        xFlag = BORDER_POSITION;
    }
    #endif
    
    xFlag = NORMAL_POSITION; /* BORDER_POSITION is no longer existed*/
    yFlag = NORMAL_POSITION; /* BORDER_POSITION is no longer existed*/
    
    bdt.DPD[index].Finger_X_XMTR = (bdt.DPD[index].start_x << 8) + DataProc_computePosition(pX, bdt.DPD[index].len_x, xFlag);
    bdt.DPD[index].Finger_Y_RECV = (bdt.DPD[index].start_y << 8) + DataProc_computePosition(pY, bdt.DPD[index].len_y, yFlag);
}

/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/

void DataProc_ProjectBuffers (uint16_t index, uint16_t threshold, int16_t SingleDelta[XMTR_NUM][RECV_NUM])
{
    int16_t  *pXaxis, *pYaxis;
    int16_t  i, pVal, *pX, *pY;
    uint16_t colIterator, rowIterator;
    uint32_t bitMask, r;
    
    pXaxis = bdt.DPD[index].pBuffer_X;
    pYaxis = bdt.DPD[index].pBuffer_Y;
    
    clearArray((uint16_t *)pXaxis, XMTR_NUM);
    clearArray((uint16_t *)pYaxis, RECV_NUM);
    
    pX = pXaxis;
    pY = pYaxis;
    colIterator = bdt.DPD[index].start_y;
    rowIterator = bdt.DPD[index].start_x;
    
    for (i=0; i<(int16_t)bdt.DPD[index].len_x; i++)
    {
        /***************************************************************
                  *Grab one whole row of bits at a time and try to project them
                  ***************************************************************/
        r = bdt.DPD[index].BitMapDat[rowIterator];
        if (r != 0)
        {
            colIterator = bdt.DPD[index].start_y;
            bitMask     = bdt.DPD[index].colLSB;      /* start on the left side of the image*/
            pY          = pYaxis;
            while (r)
            {
                if (r & bitMask)
                {
                    pVal = SingleDelta[rowIterator][colIterator];
                    if (pVal > threshold) 
                    { 
                        *pY += pVal; 
                        *pX += pVal; 
                    }
                    r ^= bitMask;
                }
                pY++;
                colIterator++;
                bitMask <<= 1;
            }
        }
        /***************************************************
        *Advance to the next row
        ***************************************************/
        rowIterator++;
        pX++;
 
         
    }
}


/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/

void DataProc_PullBitmapApart(uint32_t *buf)
{
    uint16_t i, j;
    uint32_t r, lsp, pg;
    
    bdt.FingerDetectNum = 0;
    for (i=0; i<FINGER_NUM_MAX; i++)
    for (j=0; j<XMTR_NUM; j++) 
        bdt.DPD[i].BitMapDat[j] = 0;
    
    /********************************************************
    * Whole BitMap: bdt.DeltaBitDat[]
    *    |||||||||||||||||||||||
    *    VVVVVVVVVVVVVVVVVVVVVVV
    *   Sub BitMap: bdt.DPD[0~4].BitMapDat[]
    ********************************************************/
    for (i=0; i<XMTR_NUM; i++)
    {
        /********************************************************
        * Process one row in the bitmap. A pixel group is 1 or
        * more contiguous pixels, There may be multiple pixel
        * groups in a row. For each group, we check to see to 
        * which segmented image it belongs
        ********************************************************/
        r = buf[i];   
        while(r)
        {
            lsp = r & (~(r-1));    /* Extract the LSP*/
            pg  = r & (~(r+lsp));  /* Extract the group to which it belongs*/
            
            /************************************************************
            * If a pixel group was found, then merge it with the pixel
            * group(s) found thus far, then Remove this pixel group and 
            * loop again for the next.
            ************************************************************/
            if(pg != 0) 
            {   /* 逐列转换行列式，XMTR控制列 */
                DataProc_MergePixelGroup(i, pg); 
            }
            r ^= pg;
        }
    }
    
    /************************************************************
    * Merge together non-convex images that have been incorrectly
    * split into two (or more) segments.
    ************************************************************/
    DataProc_MergeApartBitmaps();  /* 逐行 向上合并矩阵抽象出finger point value */
}


/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/

void DataProc_ExpandBorder1Bit(uint32_t *buf)
{
    int       i;
    uint32_t  tempdat32, oldVal;
    
        /*****************************************************
                  * Process the first TX
                  *****************************************************/
    tempdat32  = buf[0];
    tempdat32 |= ((tempdat32<<1) | (tempdat32>>1));
    tempdat32 |= buf[1];
    oldVal     = buf[0];
    buf[0]     = tempdat32;
    
        /*****************************************************
                  * Process the Middle TX
                  *****************************************************/
    for(i=1; i<XMTR_NUM-1; i++)
    {
        tempdat32  = buf[i];
        tempdat32 |= ((tempdat32<<1) | (tempdat32>>1));
        tempdat32 |= oldVal;
        tempdat32 |= buf[i+1];
        oldVal     = buf[i];
        buf[i]     = tempdat32;
    }
    
        /*****************************************************
                  * Process the last TX
                  *****************************************************/
    tempdat32  = buf[XMTR_NUM-1];
    tempdat32 |= ((tempdat32<<1) | (tempdat32>>1));
    tempdat32 |= oldVal;
    buf[XMTR_NUM-1] = tempdat32;
    
    /*****************************************************
    *Make the expand bitmap VALID
    *****************************************************/
    for(i = 0; i < XMTR_NUM;i++)
    {
        buf[i] &= BITMAP_MAX;
    }    
}


/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
/*************************************************************
  * Make sure (x,y) is a peak point, actually just make sure it
  * is not smaller than its neigborhood, maybe equal to ...
  *************************************************************/
uint8_t DataProc_IsExtremum(uint8_t x,uint8_t y)
{
    uint16_t threshold = (bdt.ThresholdInFrame);
    if(bdt.DeltaDat16A[x][y] < threshold)
    {
        return 0; /* Make sure the peak value is big enough*/
    }
    if(x > 0 && bdt.DeltaDat16A[x][y] < bdt.DeltaDat16A[x - 1][y])
    {
        return 0; /* Not Located in the first XMTR line*/
    }
    if(x < XMTR_NUM - 1 && bdt.DeltaDat16A[x][y] < bdt.DeltaDat16A[x + 1][y])
    {
        return 0; /* Not Located in the last XMTR line*/
    }
    if(y > 0 && bdt.DeltaDat16A[x][y] < bdt.DeltaDat16A[x][y - 1])
    {
        return 0; /* Not Located in the first RECV line*/
    }
    if(y < RECV_NUM - 1 && bdt.DeltaDat16A[x][y] < bdt.DeltaDat16A[x][y + 1])
    {
        return 0; /* Not Located in the last RECV line*/
    }
    return 1;
}

/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/

uint8_t DataProc_IsAroundExtremum(uint8_t x,uint8_t y)
{
    if(x > 0 && DataProc_IsExtremum(x-1,y))    
    {    
        return 1;
    }
    if(y > 0 && DataProc_IsExtremum(x,y-1))     
    {
        return 1;
    }
    if(x < XMTR_NUM-1 && DataProc_IsExtremum(x+1,y)) 
    {
        return 1;
    }
    if(y < RECV_NUM-1 && DataProc_IsExtremum(x,y+1)) 
    {
        return 1;
    }
    return 0;
}


/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/

void DataProc_DelInvalidExtremum(void)
{
    uint16_t i, j, k, m, fnum;
    uint16_t X_dist, Y_dist;
    
    fnum = bdt.FingerDetectNum;
    for(i = 0; i < bdt.FingerDetectNum; i++)
    {
        if(bdt.DPD[i].ExtremumCount > 1)
        {
            /**********************************************************************
                          * "ExtremumCount > 1" shown there are more than 1 peaks in the BitMap
                          **********************************************************************/
            for(j = 0;   j < bdt.DPD[i].ExtremumCount; j++)
                for(k = j+1; k < bdt.DPD[i].ExtremumCount; k++)
                {
                    X_dist = (uint16_t)(abs16(bdt.DPD[i].Extremum_Pos_X[j] - bdt.DPD[i].Extremum_Pos_X[k]));
                    Y_dist = (uint16_t)(abs16(bdt.DPD[i].Extremum_Pos_Y[j] - bdt.DPD[i].Extremum_Pos_Y[k]));
                    if((X_dist + Y_dist) <= 1)
                    {
                        /**************************************************************
                        * The both peaks are located on the same RECV_line or XMTR_line
                        ***************************************************************/
                        bdt.DPD[i].ExtremumCount--;
                        for(m = k; m < bdt.DPD[i].ExtremumCount; m++)
                        {
                            bdt.DPD[i].Extremum_Pos_X[m] = bdt.DPD[i].Extremum_Pos_X[m+1];
                            bdt.DPD[i].Extremum_Pos_Y[m] = bdt.DPD[i].Extremum_Pos_Y[m+1];
                        }
                        k--;
                     }
                }
        }
        else if(0 == bdt.DPD[i].ExtremumCount)
        {
            /**********************************************************************
                          * Delete the finger without Extremum
                          ***********************************************************************/
            bdt.FingerDetectNum--;
            fnum--;
            for(j = i; j < bdt.FingerDetectNum; j++)
            {
                /******************************************************************
                * All related information should be shifted to make it tight
                *******************************************************************/
                for(k = 0; k < XMTR_NUM; k++)
                {
                    bdt.DPD[j].BitMapDat[k] = bdt.DPD[j+1].BitMapDat[k];
                }
                bdt.DPD[j].ExtremumCount = bdt.DPD[j+1].ExtremumCount;
                
                for(k = 0; k < FINGER_NUM_MAX; k++)
                {
                    bdt.DPD[j].Extremum_Pos_X[k] = bdt.DPD[j+1].Extremum_Pos_X[k];
                    bdt.DPD[j].Extremum_Pos_Y[k] = bdt.DPD[j+1].Extremum_Pos_Y[k];
                }
            }
            i--;
            continue;
        }
        
        /*****************************************************************
                  * Make sure all the peaks are not more than FINGER_NUM_MAX
                  ******************************************************************/
        if(FINGER_NUM_MAX == fnum)
        {
            /*************************************************************
                          * There is only one peak for this finger area
                          **************************************************************/
            bdt.DPD[i].ExtremumCount = 1;
        }
        else if(fnum + bdt.DPD[i].ExtremumCount - 1 > FINGER_NUM_MAX)
        {
            /*************************************************************
                          * Discard the unwanted peaks in this finger area
                          **************************************************************/
            bdt.DPD[i].ExtremumCount = FINGER_NUM_MAX - fnum + 1;
            fnum = FINGER_NUM_MAX;
        }
        else
        {
            /*************************************************************
                          * Accept all possible peaks in this finger area
                          **************************************************************/
            fnum += bdt.DPD[i].ExtremumCount - 1;
        }
    }
}


/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/

void DataProc_FindExtremum(void)
{
    uint16_t i, j, k;
    
    for(i = 0; i < bdt.FingerDetectNum; i++)
    {
        bdt.DPD[i].ExtremumCount = 0;
        
        /******************************************************
                  * Search all possible Peaks in the whole Frame
                  *If the Peak is satisfied , that is enough!
                  *******************************************************/
        for(j = 0; j < XMTR_NUM; j++)
            for(k = 0; k < RECV_NUM; k++)
            {
                if(bdt.DPD[i].BitMapDat[j] & (1<<k))          /* Valid Data*/
                if(DataProc_IsExtremum(j, k))                         /* Local Peak*/
                if(bdt.DPD[i].ExtremumCount < FINGER_NUM_MAX) /* Peak Allocation OK*/
                {
                    bdt.DPD[i].Extremum_Pos_X[bdt.DPD[i].ExtremumCount] = j;
                    bdt.DPD[i].Extremum_Pos_Y[bdt.DPD[i].ExtremumCount] = k;
                    bdt.DPD[i].ExtremumCount++;
                }
            }
    }	
    
    DataProc_DelInvalidExtremum();
}


/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/

uint8_t DataProc_ExpandBitmap(uint8_t ExpandFlag[XMTR_NUM][RECV_NUM],uint16_t Srcindex,uint16_t DstIndex)
{
    uint16_t i,j;
    uint16_t flag = 0;
    
    for(i = 0;i < XMTR_NUM;i++)
        for(j = 0;j < RECV_NUM;j++)
        {
            /***************************************************************************
                          * It should be the current BitMap first, from the point, expand the area;
                          * Make sure it is processed before, 
                          ****************************************************************************/
            if( bdt.DPD[DstIndex].BitMapDat[i] & (1<<j) )
            if( EXPAND_NEED == ExpandFlag[i][j] )
            {
                ExpandFlag[i][j] = EXPAND_DONE; /* Marked this location has been processed*/
                  if(i > 0)
                {
                    if(bdt.DPD[Srcindex].BitmapOrig[i-1] & (1<<j))          /* It has to be valid area originally*/
                    if(bdt.DeltaDat16A[i-1][j] <= bdt.DeltaDat16A[i][j])    /* From Left side, it has to be smaller*/
                    if(bdt.DeltaDat16A[i-1][j] > 0)                         /* It has to be bigger than zero*/
                    {
                        /************************************************
                                                  * It should be a peak point OR it is NOT a 
                                                  * neigborhood to a peak;
                                                  *************************************************/
                        if(DataProc_IsExtremum(i,j) || !DataProc_IsAroundExtremum(i-1,j))
                        {
                            bdt.DPD[DstIndex].BitMapDat[i-1] |= 1<<j;
                            flag = 1;
                        }
                    }
                }
                
                /************************************************
                                  * If this is not Right Side [ XMTR < (NUM-1) ]
                                  *************************************************/
                if(i < XMTR_NUM - 1)
                {
                    if(bdt.DPD[Srcindex].BitmapOrig[i+1] & (1<<j))          /* It has to be valid area originally*/
                    if(bdt.DeltaDat16A[i+1][j] <= bdt.DeltaDat16A[i][j])    /* From Right side, it has to be smaller*/
                    if(bdt.DeltaDat16A[i+1][j] > 0)                         /* It has to be bigger than zero*/
                    {
                        /*************************************************
                                                  * It should be a peak point OR it is NOT a 
                                                  * neigborhood to a peak;
                                                  **************************************************/
                        if(DataProc_IsExtremum(i,j) || !DataProc_IsAroundExtremum(i+1,j))
                        {
                            bdt.DPD[DstIndex].BitMapDat[i+1] |= 1<<j;
                            flag = 1;
                        }
                    }
                }
                
                /************************************************
                                  * If this is not Up Side (RECV > 0)
                                  *************************************************/
                if(j > 0)
                {
                    if(bdt.DPD[Srcindex].BitmapOrig[i] & (1<<(j-1)))
                    if(bdt.DeltaDat16A[i][j-1] <= bdt.DeltaDat16A[i][j])
                    if(bdt.DeltaDat16A[i][j-1] > 0)
                    {
                        if(DataProc_IsExtremum(i,j) || !DataProc_IsAroundExtremum(i,j-1))
                        {
                            bdt.DPD[DstIndex].BitMapDat[i] |= 1<<(j-1);
                            flag = 1;
                        }
                    }
                }
                
                /************************************************
                                  * If this is not Bottom Side [ RECV < (NUM-1) ]
                                  *************************************************/
                if(j < RECV_NUM - 1)
                {
                    if(bdt.DPD[Srcindex].BitmapOrig[i] & (1<<(j+1)))
                    if(bdt.DeltaDat16A[i][j+1] <= bdt.DeltaDat16A[i][j])
                    if(bdt.DeltaDat16A[i][j+1] > 0)
                    {
                        if(DataProc_IsExtremum(i,j) || !DataProc_IsAroundExtremum(i,j+1))
                        {
                            bdt.DPD[DstIndex].BitMapDat[i] |= 1<<(j+1);
                            flag = 1;
                        }
                    }
                }
            }
        }
    
    /****************************************************
          * Return 1, will make the funcation be called again
          *****************************************************/
    return flag;
}


/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/

void DataProc_ExtractOneFingerArea(uint16_t index, uint16_t ExIndex)
{
    uint16_t i, j, fnum;
    uint16_t DstIndex;
    uint8_t  MaxX, MaxY;
    uint8_t  ExpandFlag[XMTR_NUM][RECV_NUM];
    
        /*************************************************
                  * Initialize MaxX, MaxY
                  **************************************************/
    MaxX = bdt.DPD[index].Extremum_Pos_X[ExIndex];
    MaxY = bdt.DPD[index].Extremum_Pos_Y[ExIndex];
    
        /*************************************************
                  * Compute DstIndex: ID of this Extremum
                  **************************************************/
    fnum = bdt.FingerDetectNum;
    for(i = 0; i < index; i++)
    {
        fnum += bdt.DPD[i].ExtremumCount - 1;
    }
    if(ExIndex != 0)
    {
        DstIndex = fnum + ExIndex - 1;
        bdt.DPD[DstIndex].Extremum_Pos_X[0] = MaxX;
        bdt.DPD[DstIndex].Extremum_Pos_Y[0] = MaxY;
    }
    else 
    {
        DstIndex = index;
    }
        /*************************************************
                  * Initialize Flag Arrays
                  **************************************************/
    for (i = 0;i < XMTR_NUM;i++)
        for (j = 0;j < RECV_NUM;j++)
        {
            ExpandFlag[i][j] = EXPAND_NEED;
        }
        /*************************************************
                  * Initialize  Bitmap
                  **************************************************/
    for (i = 0; i < XMTR_NUM;i++)
    {
        bdt.DPD[DstIndex].BitMapDat[i] = 0;
    }
    bdt.DPD[DstIndex].BitMapDat[MaxX] |= 1<<MaxY;
    
        /*************************************************
                  * Expand bitmap from index to DstIndex
                  **************************************************/
    while(DataProc_ExpandBitmap(ExpandFlag, index, DstIndex));
}


/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/

void DataProc_ExtractExtremumArea(void)
{
    uint16_t i,j;
    for (i = 0; i < bdt.FingerDetectNum; i++)
    {
        DataProc_ExpandBorder1Bit(bdt.DPD[i].BitMapDat);
        
        /**********************************************************************
                  * Based on "ExtremumCount", Split it into differnt finger area
                  **********************************************************************/
        if(bdt.DPD[i].ExtremumCount > 1)
        {
            for (j = 0; j < XMTR_NUM;j++)
            {
                bdt.DPD[i].BitmapOrig[j] = bdt.DPD[i].BitMapDat[j];
                bdt.DPD[i].BitMapDat[j]  = 0;
            }
            for (j = 0; j < bdt.DPD[i].ExtremumCount; j++)
            {
                DataProc_ExtractOneFingerArea(i, j);
            }
        }
    }
}


/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/

void DataProc_ComputeOffset(uint16_t *FO, uint16_t *BO, uint16_t Pos)
{
    if(MIDDLE_OF_FIVE == Pos)      
    { 
        *FO = 2; 
        *BO = 2; 
    }
    else if(FRONT_OF_FOUR == Pos) 
    { 
        *FO = 1;
        *BO = 2;
    }
    else                           
    {
        *FO = 2; 
        *BO = 1;
    }
}


/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/

void DataProc_BalanceValNearPeak(uint16_t index, int16_t buf[XMTR_NUM][RECV_NUM])
{
    uint16_t xShape, yShape, exception = 0;
    int16_t  maxXp, maxYp;
    int16_t  Value1, Value2;
    int16_t  Value3, Value4;
    uint16_t FOX,  BOX,  FOY,  BOY;
    int16_t  i, j, mxyv;
    int16_t  mxm1v=0, mxp1v=0;
    int16_t  mxm2v=0, mxp2v=0;
    int16_t  mym1v=0, myp1v=0;
    int16_t  mym2v=0, myp2v=0;
    
    
    maxXp  = bdt.DPD[index].Extremum_Pos_X[0];    /* Location of X*/
    maxYp  = bdt.DPD[index].Extremum_Pos_Y[0];    /* Location of Y*/
    
    for (i = 0;i < bdt.LFrame_NUM; i++)
    {
        Value1 = abs16((int16_t)bdt.LFrame_X_XMTR[i] - maxXp);
        Value2 = abs16((int16_t)bdt.LFrame_Y_RECV[i] - maxYp);
        if((Value1 <= 1) && (Value2 <= 1))
        {
            exception = 1;
            break;
        }
    }
    
    xShape = MIDDLE_OF_FIVE;                      /* Default setting is  MIDDLE_OF_FIVE*/
    yShape = MIDDLE_OF_FIVE;                      /* Default setting is  MIDDLE_OF_FIVE*/
    mxyv   = buf[maxXp][maxYp];
    Value3 = (mxyv>>3);                           /* MaxValue(*0.125)*/
    Value4 = 0 - Value3;                          /* Negative Value3*/
    Value2 = (mxyv>>1);                           /* MaxValue(*0.5)*/
    Value1 = Value2 + (mxyv>>2);                  /* MaxValue(*0.75)*/
    bdt.DPD[index].DataInvalidFlag = 0;           /* Default we think all data is valid*/
    
    if(maxXp > 0)
    {
        mxm1v = buf[maxXp-1][maxYp];
    }
    if(maxYp > 0)
    {
        mym1v = buf[maxXp][maxYp-1];
    }
    if(maxXp < (XMTR_NUM - 1))
    {
        mxp1v = buf[maxXp+1][maxYp];
    }
    if(maxYp < (RECV_NUM - 1))
    {
        myp1v = buf[maxXp][maxYp+1];
    }
    
    #if 1
        if(maxXp > 1) 
        {
            mxm2v = buf[maxXp-2][maxYp];
        }
        if(maxYp > 1)
        {
            mym2v = buf[maxXp][maxYp-2];
        }
        if(maxXp < (XMTR_NUM - 2))
        {
            mxp2v = buf[maxXp+2][maxYp];
        }
        if(maxYp < (RECV_NUM - 2))
        {   
            myp2v = buf[maxXp][maxYp+2];
        }
    #endif
    
   /**********************************************
         * Compute the positon of extremum
         ***********************************************/
    if(mxm1v > Value1) 
    {
        xShape += BACK_OF_FOUR;     /* 4 Sample is good choice; L, LH, HH, L*/
    }
    if(mxp1v > Value1) 
    {
        xShape += FRONT_OF_FOUR;       /* 4 Sample is good choice; L, HH, LH, L*/
    }
    if(mym1v > Value1)
    {
        yShape += BACK_OF_FOUR;   
    }
    if(myp1v > Value1) 
    {
        yShape += FRONT_OF_FOUR;      
    }
   /*************************************************************
        * For Shape = MIDDLE_OF_FIVE, p2v and m2V should be small
        **************************************************************/
    if(MIDDLE_OF_FIVE == xShape)
    {
        if(mxm1v < (mxm2v<<1)) 
        {
            buf[maxXp-2][maxYp] = 0;
            if(maxYp > 0)           
            { 
                buf[maxXp-2][maxYp-1] = 0;
            }
            if(maxYp < (RECV_NUM - 1)) 
            {
                buf[maxXp-2][maxYp+1] = 0;
            }
            if(maxYp > 1)           
            {
                buf[maxXp-2][maxYp-2] = 0;
            }
            if(maxYp < (RECV_NUM - 2)) 
            {
                buf[maxXp-2][maxYp+2] = 0;
            }
        }
        if(mxp1v < (mxp2v<<1)) 
        {
            buf[maxXp+2][maxYp] = 0;
            if(maxYp > 0)            
                buf[maxXp+2][maxYp-1] = 0;
            if(maxYp < (RECV_NUM - 1))
                buf[maxXp+2][maxYp+1] = 0;
            if(maxYp > 1)            
                buf[maxXp+2][maxYp-2] = 0;
            if(maxYp < (RECV_NUM - 2)) 
                buf[maxXp+2][maxYp+2] = 0;
        }
    }
    if(MIDDLE_OF_FIVE == yShape)
    {
        if(mym1v < (mym2v<<1)) 
        {
            buf[maxXp][maxYp-2] = 0;
            if(maxXp > 0)           
            {    
                buf[maxXp-1][maxYp-2] = 0;
            }
            if(maxXp < (XMTR_NUM - 1)) 
            {    
                buf[maxXp+1][maxYp-2] = 0;
            }
            if(maxXp > 1)           
            { 
                buf[maxXp-2][maxYp-2] = 0;
            }
            if(maxXp < (XMTR_NUM - 2))
            { 
                buf[maxXp+2][maxYp-2] = 0;
            }
        }
        if(myp1v < (myp2v<<1)) 
        {
            buf[maxXp][maxYp+2] = 0;
            if(maxXp > 0)            
            { 
                buf[maxXp-1][maxYp+2] = 0;
            }
            if(maxXp < (XMTR_NUM - 1))
            {
                buf[maxXp+1][maxYp+2] = 0;
            }
            if(maxXp > 1)
            {
                buf[maxXp-2][maxYp+2] = 0;
            }
            if(maxXp < (XMTR_NUM - 2))
            {
                buf[maxXp+2][maxYp+2] = 0;
            }
        }
    }
    
   /**********************************************
        * Check whether it is a pure finger point
        * All the following case should be invalid data
        ***********************************************/
    if(exception != 0) 
    {
        bdt.DPD[index].DataInvalidFlag = 0;
    }
    else
    {
        if(xShape > BACK_OF_FOUR)  
        {
            bdt.DPD[index].DataInvalidFlag = 1;                        /* 两个旁瓣同时大于75%*/
        }
        else if(yShape > BACK_OF_FOUR) 
        {    
            bdt.DPD[index].DataInvalidFlag = 1;                        /* 两个旁瓣同时大于75%*/
        }
        else if((mxp2v > Value2) && (FRONT_OF_FOUR == xShape))
        {
            bdt.DPD[index].DataInvalidFlag = 1; /* L HH LH L,两个旁瓣同时大于75%||50%*/
        }
        else if((mxm2v > Value2) && (BACK_OF_FOUR == xShape))
        {
            bdt.DPD[index].DataInvalidFlag = 1; /* L LH HH L, 两个旁瓣同时大于75%||50%*/
        }
        else if((myp2v > Value2) && (FRONT_OF_FOUR == yShape))
        {
            bdt.DPD[index].DataInvalidFlag = 1; /* L HH LH L, 两个旁瓣同时大于75%||50%*/
        }
        else if((mym2v > Value2) && (BACK_OF_FOUR == yShape)) 
        {
            bdt.DPD[index].DataInvalidFlag = 1; /* L LH HH L, 两个旁瓣同时大于75%||50%*/
        }
        else if((mxm1v < Value4)||(mxp1v < Value4)) 
        {
            bdt.DPD[index].DataInvalidFlag = 1; /* 两个旁瓣同时小于负的12.5%*/
        }
        else if((mym1v < Value4)||(myp1v < Value4)) 
        {
            bdt.DPD[index].DataInvalidFlag = 1; /* 两个旁瓣同时小于负的12.5%*/
        }
        
        #if 1
        if(mxyv < FRAME_MAX_SAMPLE)
        {
            bdt.DPD[index].DataInvalidFlag = 1;   
        }
        
        Value3  = (bdt.MaxValueInFrame>>2) + (bdt.MaxValueInFrame>>3);    /* 0.375*/
        
        if(mxyv < Value3)
        {
            bdt.DPD[index].DataInvalidFlag = 1;
        }
        #endif
    }
    
  /*************************************************************
      * Compute the range of the valid value around extremum
      **************************************************************/
    DataProc_ComputeOffset(&FOX, &BOX, xShape);
    DataProc_ComputeOffset(&FOY, &BOY, yShape);
    
    if(bdt.DPD[index].DataInvalidFlag != 0)
    {
        for (i = bdt.DPD[index].start_x; i < bdt.DPD[index].start_x + bdt.DPD[index].len_x; i++)
            for (j = bdt.DPD[index].start_y; j < bdt.DPD[index].start_y + bdt.DPD[index].len_y; j++)
            {
                buf[i][j] = 0;
            }
    }
    else
    {
        for (i = bdt.DPD[index].start_x; i < bdt.DPD[index].start_x + bdt.DPD[index].len_x; i++)
            for (j = bdt.DPD[index].start_y; j < bdt.DPD[index].start_y + bdt.DPD[index].len_y; j++)
            {
                /**********************************************
                                  * Remove delta data out of range
                                  ***********************************************/
                if(i < maxXp - FOX || i > maxXp + BOX || j < maxYp - FOY || j > maxYp + BOY)
                {
                    buf[i][j] = 0;
                }
                
                /**********************************************
                                  * Remove more noise data we judged as it is
                                  ***********************************************/
                if(MIDDLE_OF_FIVE == xShape)
                {
                    if(i == (maxXp - FOX))
                    if((maxXp + BOX) < XMTR_NUM)
                    if(buf[maxXp+BOX][maxYp] == 0)
                    {
                        buf[i][j] = 0;
                    }
                    if(i == (maxXp + BOX))
                    if((maxXp - FOX) >= 0)
                    if(0 == buf[maxXp-FOX][maxYp])
                    {
                        buf[i][j] = 0;
                    }
                }
                if(MIDDLE_OF_FIVE == yShape)
                {
                    if(j == (maxYp - FOY))
                    if((maxYp + BOY) < RECV_NUM)
                    if(0 == buf[maxXp][maxYp+BOY])
                    {
                        buf[i][j] = 0;
                    }
                    if(j == (maxYp + BOY))
                    if((maxYp - FOY) >= 0)
                    if(0 == buf[maxXp][maxYp-FOY])
                    {
                        buf[i][j] = 0;
                    }
                }
            }
    }
}



/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/

void DataProc_ComputePoint(void)
{
    uint16_t fnum;
    uint16_t index;
    uint32_t rowExtent,colExtent;
    uint16_t xEnd,yEnd;
    int16_t  i, j;
    int16_t  SingleDelta[XMTR_NUM][RECV_NUM] = {{0}};
    
    /*******************************************************************
          * Make sure how many fingers does the system has now
          ********************************************************************/
    fnum = bdt.FingerDetectNum;
    for (i = 0;i < bdt.FingerDetectNum;i++)
    {
        fnum += bdt.DPD[i].ExtremumCount - 1;
    }
    bdt.FingerDetectNum   = fnum;
        bdt.FingerInvalidPNum = 0;
    for (index = 0; index < fnum; index++)
    {  
        /***************************************************************
                  * The following function will set:
                  * Input:  index
                  * Output: colExtent and rowExtent at the same time actually
                  ****************************************************************/
        rowExtent = DataProc_GetExtents(&colExtent, index);
        
        bdt.DPD[index].colLSB = (~(colExtent<<1)) & colExtent; /* Here it is LSB Actually;*/
        
        bdt.DPD[index].start_y = lsbPos(colExtent); 
        yEnd = msbPos(colExtent);
        
        bdt.DPD[index].start_x = XMTR_NUM - msbPos(rowExtent) - 1;
        xEnd = XMTR_NUM - lsbPos(rowExtent) - 1;
        
        bdt.DPD[index].len_x = xEnd - bdt.DPD[index].start_x + 1;
        bdt.DPD[index].len_y = yEnd - bdt.DPD[index].start_y + 1;
        
        /***************************************************************
                  * Get Single Delta Data
                  ****************************************************************/
        for (i=bdt.DPD[index].start_x; i<=xEnd; i++)
            for (j=bdt.DPD[index].start_y; j<=yEnd; j++)
            {
                SingleDelta[i][j] = 0;				
                if(bdt.DPD[index].BitMapDat[i] & (1<<j))
                if(bdt.DeltaDat16A[i][j] > 0)
                SingleDelta[i][j] = bdt.DeltaDat16A[i][j];
            }
        
        /***************************************************************
                  * Make sure the Delta Data Shape is correct
                  ****************************************************************/
        DataProc_BalanceValNearPeak(index, SingleDelta);
        if(bdt.DPD[index].DataInvalidFlag)
        {
            /**********************************************
            * Invalid data case is appeared for this point
            ***********************************************/
            bdt.DPD[index].Finger_X_XMTR   = 0; /* NO-Finger*/
            bdt.DPD[index].Finger_Y_RECV   = 0; /* No-Finger*/
            bdt.DPD[index].DataInvalidFlag = 0; /* Recover for the next judgement*/
            bdt.FingerInvalidPNum++;
        }
        else
        {
            DataProc_ProjectBuffers(index, 0, SingleDelta);
            DataProc_GetPoint(index);
        }
        
        /**************************************************************
                  *Delta Data Showing Information
                  ****************************************************************/
        for(i=bdt.DPD[index].start_x; i<=xEnd; i++)
            for(j=bdt.DPD[index].start_y; j<=yEnd; j++)
            {
                #ifdef COMMUNICATION_WITH_PC
                bdt.SigDeltaDat[i][j] |= SingleDelta[i][j];
                #endif
                
                #ifdef CN1100_LINUX
                bdt.SigDeltaDat[i][j] |= SingleDelta[i][j];
                #endif
            }
    
    }
}

/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
void DataProc_CVNRXMark(uint16_t RxLine, uint16_t *RX_Flag)
{
    int16_t Rs, Re;
    uint16_t i;
    
    Rs = (int16_t)RxLine - 1;
    Re = (int16_t)RxLine + 1;
    if(Rs < 0) 
    {
        Rs = 0;
    }
    if(Re > (RECV_NUM-1)) 
    {
        Re = RECV_NUM-1;
    }
    
    for (i=Rs; i<=Re; i++) 
    {
        RX_Flag[i] = 1;
    }
}

/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/

void DataProc_PNLJCurrentValidNoise(uint32_t *d)
{
    uint16_t i, j;
    int16_t  temp;
    uint16_t RX_NoiseLine[RECV_NUM];
 
    /*****************************************************
    *Initial Processing
    ******************************************************/
    for (i=0; i<RECV_NUM; i++)
    {
        RX_NoiseLine[i] = 0;
    }
    /*****************************************************
    * Extract Receive Line for Noise Summary
    ******************************************************/
    for (i=0; i<bdt.FingerDetectNum; i++)
    {
        if(bdt.DPD[i].Finger_X_Reported || bdt.DPD[i].Finger_Y_Reported)
        {
            DataProc_CVNRXMark((bdt.DPD[i].Finger_Y_Reported)>>8, RX_NoiseLine);
        }
    }

    /*****************************************************
    * Calculation the NOISE Value
    ******************************************************/
    d[0] = 0;
    d[1] = 0;
    if(FINGER_SHOW == bdt.BFD.bbdc.FingerExist)
    {
        for(j = 0; j < RECV_NUM; j++)
        {
            if(RX_NoiseLine[j] != 0)
            for(i = 0; i < XMTR_NUM; i++)
            {
                if(bdt.SigDeltaDat[i][j]<=0)
                {
                    /**************************************
                    // Only Big Negative Sample is used
                    ***************************************/
                    if(bdt.DeltaDat16A[i][j] < 0)
                    {
                        temp  = abs16(bdt.DeltaDat16A[i][j]);
                        if(temp>9) 
                        { 
                            d[0] += temp; 
                            d[1]++; 
                        }
                    }
                }
            }
        }
    }

}


/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/

void DataProc_PowerNoiseLevelJudge(void)
{
    uint32_t dnoise[2]; 
    uint8_t tab_low[4]  = {36,48,72,108};
    uint8_t tab_high[4] = {48,72,144,192};
    
    /***************************************************************
    * Calculating Current Noise 
    ****************************************************************/
    DataProc_PNLJCurrentValidNoise(dnoise);
    if(0 == dnoise[1]) 
    {
        dnoise[0] = 0; // We think noise is small; all DeltaData are regulat value
        return;
    }
    else 
    {
        dnoise[0] = dnoise[0]/dnoise[1]; // Get the average value of abnormal DeltaData
    }

    if(16 == bdt.NDD.Noise_Count)
    {
        bdt.NDD.Noise_Sum = bdt.NDD.Noise_Sum>>4; // After 16 times Calculating, we get Average Noise value
        
        switch(bdt.NDD.Battery_Level)
        {
            case BATTERY_GOOD:
            {
                if(bdt.NDD.Noise_Sum > 24)
                {
                    bdt.NDD.Battery_Level = BATTERY_BAD;
                }
                break;
            }
            case BATTERY_BAD:
            {
                if(bdt.NDD.Noise_Sum > 32) 
                {
                    bdt.NDD.Battery_Level = BATTERY_WORSE;
                }
                else if(bdt.NDD.Noise_Sum < 16)
                {
                    bdt.NDD.Battery_Level = BATTERY_GOOD;
                }
                break;
            }
            case BATTERY_WORSE:
            {
                if(bdt.NDD.Noise_Sum > 40)
                {
                    bdt.NDD.Battery_Level = BATTERY_WORST;
                }
                else if(bdt.NDD.Noise_Sum < 24) 
                {
                    bdt.NDD.Battery_Level = BATTERY_BAD;
                }
                break;
            }
            case BATTERY_WORST:
            {
                if(bdt.NDD.Noise_Sum < 32)
                {
                    bdt.NDD.Battery_Level = BATTERY_WORSE;
                }
                break;
            }
        }
        #ifdef STM32VC_LCD
        TFT_ShowNum(20, 29, bdt.NDD.Noise_Sum, 0xf800, LCD_COLOR_GREEN);
        TFT_ShowNum(30, 29, bdt.NDD.Battery_Level, 0xf800, LCD_COLOR_GREEN);
        #endif
        bdt.NDD.Noise_Count = 0;
        bdt.NDD.Noise_Sum   = 0;

        //**************************************************
        // When Finger Num is 4/5, we think it is 
        //**************************************************
        if(bdt.FingerDetectNum >3)
        {
            bdt.NDD.Battery_Level = BATTERY_WORST;
        }
        bdt.ThrLow4DistanceFilter  = tab_low[bdt.NDD.Battery_Level]; /* THR072 = 36*/
        bdt.ThrHigh4DistanceFilter = tab_high[bdt.NDD.Battery_Level]; /* THR144 = 48*/
        
    }
    else 
    {
        bdt.NDD.Noise_Count++;
        bdt.NDD.Noise_Sum += dnoise[0];
    }
}


/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
void DataProc_FreqHopUpdateThrd(uint16_t thrd) 
{
#if 0
  #ifdef FREQHOP_BYSTRETCH
    #ifdef FREQY_HOP_ONLY
    if(bdt.CurNoiseThrd < (thrd)) 
    {
        /**************************************************
        * Current Noise is Bigger than the current THRD
        ***************************************************/
        bdt.CurNoiseThrd = thrd;
    }
    #else
    if(bdt.CurNoiseThrd < (thrd>>1)) 
    {
        /**************************************************
        *Current Noise is Bigger than the current THRD
        ***************************************************/
        bdt.CurNoiseThrd = thrd>>1;
    }
    else if(8 == bdt.SNoiseCount)
    {
        /**************************************************
        * Current Noise is Smaller than 1/8 current THRD
        ***************************************************/
        bdt.CurNoiseThrd = thrd>>1;
    }
    #endif
    bdt.CurNoiseThrd++;
  #endif
#endif
}

/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
void DataProc_FHBSDeltaNoiseEstimation(int16_t *buf)
{
    #ifdef FREQHOP_BYSTRETCH
    uint16_t i, j, temp;
    bdt.NoiseDataFrFrame = 0;
    for (i = 0;i < XMTR_NUM;i++)
        for (j = 0;j < RECV_NUM;j++)
        {
            temp = abs16(buf[i*RECV_NUM+j]);
            bdt.NoiseDataFrFrame += (uint32_t)temp;
        }
    #endif
}

/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
uint16_t DataProc_FHBSBTFSSideInfo(uint16_t index, uint16_t *buf)
{
    uint16_t result = 0;
#ifdef FREQHOP_BYSTRETCH
    if(0 == index) result = buf[1];
    else if((STRETCH_STNUM-1) == index) result = buf[STRETCH_STNUM-2];
    else result = (buf[index-1]+buf[index+1])>>1;
#endif
    return result;
}

/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
void DataProc_FHBSBestTXFreqSearch(uint16_t *buf)
{
#ifdef FREQHOP_BYSTRETCH

    uint16_t i, t1, t2;
    uint16_t leastIndex, secondIndex;
    uint16_t leastValue, secondValue;
    //uint16_t fcolor = LCD_COLOR_RED;

    if(buf[1] < buf[0])
    {
        leastIndex  = 1;
        leastValue  = buf[1];
        secondIndex = 0;
        secondValue = buf[0];
    }
    else
    {
        leastIndex  = 0;
        leastValue  = buf[0];
        secondIndex = 1;
        secondValue = buf[1];
    }

    for(i = 2; i< STRETCH_STNUM; i++)
    {
        if(buf[i] < secondValue)
        {
            if(buf[i] < leastValue)
            {
                secondIndex = leastIndex;
                secondValue = leastValue;
                leastIndex  = i;
                leastValue  = buf[i];
            }
            else
            {
                secondIndex = i;
                secondValue = buf[i];
            }
        }
    }

#ifdef STM32VC_LCD
    //TFT_ShowNum(10, 22, leastIndex, fcolor, LCD_COLOR_GREEN);
    //TFT_ShowNum(20, 22, buf[leastIndex], fcolor, LCD_COLOR_GREEN);
    //TFT_ShowNum(10, 23, secondIndex, fcolor, LCD_COLOR_GREEN);
    //TFT_ShowNum(20, 23, buf[secondIndex], fcolor, LCD_COLOR_GREEN);
#endif
    //***********************************************************************
    // The following code will be an algorithm to decide which one is best
    //***********************************************************************
    if(bdt.StretchInReg == leastIndex)
    {
        //*******************************************************
        // We prefer the history data first
        //*******************************************************
        bdt.StretchInReg = leastIndex;
    }
    else if(bdt.StretchInReg == secondIndex)
    {
        //*******************************************************
        // We prefer the history data first
        //*******************************************************
        bdt.StretchInReg = secondIndex;
    }
    else
    {
        t1 = leastValue>>5;
        t2 = (secondValue - leastValue);
        if(t2 > t1)
        {
            //**************************************************************
            // The least one has obvious advantage
            //**************************************************************
            bdt.StretchInReg = leastIndex;
        }
        else
        {
            //**************************************************************
            // If the least one has obvious advantage, we judge based on 
            // more information from side stretch
            //**************************************************************
            t1 = DataProc_FHBSBTFSSideInfo(leastIndex, buf);
            t2 = DataProc_FHBSBTFSSideInfo(secondIndex, buf);

            if((t1+leastValue) < (t2+secondValue))
            {
                //**************************************************************
                // The least one has final advantage
                //**************************************************************
                bdt.StretchInReg = leastIndex;
            }
            else
            {
                //**************************************************************
                // The least one has final advantage
                //**************************************************************
                bdt.StretchInReg = leastIndex;
            }
        }
    }

#ifdef STM32VC_LCD
    //TFT_ShowNum(10, 24, bdt.StretchInReg, fcolor, LCD_COLOR_GREEN);
    //TFT_ShowNum(20, 24, buf[bdt.StretchInReg], fcolor, LCD_COLOR_GREEN);
#endif


#endif
}


//uint16_t maxS = 0,  minS=0;
uint16_t  DataProc_FHBSOperatingSwitch(void)
{
    uint16_t result; //= TXSCAN_DISABLE;

#ifdef FREQHOP_BYSTRETCH
    #ifdef STM32VC_LCD
    uint16_t fcolor = LCD_COLOR_RED;
    #endif
    result = TXSCAN_DISABLE;
    if(FINGER_SHOW == bdt.BFD.bbdc.FingerExist)
    {
         //**************************************************************
         // If bdt.StretchValue is small, means TxScan is ongoing
         //**************************************************************
         if(bdt.StretchValue < (STRETCH_STNUM<<STRETCH_SHIFT))
         {
             bdt.PCBA.DurStretch = bdt.StretchInReg;            // Restore the value into the previous one
             SPI_write_singleData(DURS_REG, DURS_STRETCH_DUR(bdt.PCBA.DurStretch)|DURS_STRETCH_INC(STRETCH_INC_REG25));
             bdt.StretchValue = (STRETCH_STNUM<<STRETCH_SHIFT)<<1; // Make sure we start from the 0 next time
         }
         bdt.CurNoiseSum = (bdt.CurNoiseThrHi + bdt.CurNoiseThrLo)>>1;
         bdt.FingerDelay = TXSCAN_FINGERDELAY;  // Make sure we start TxScan with a DELAY next time
         result = TXSCAN_DISABLE;               // Disable TxScan for sure
    }
    else
    {
        //**************************************************************
        // NO FINGER, NO FINGER, NO FINGER, NO FINGER, NO FINGER, 
        //**************************************************************
        if(0 != bdt.FingerDelay)
        {
            //***********************************************************
            // Finger is just gone, we should wait for a while
            //***********************************************************
            result = TXSCAN_DISABLE;   // Disable TxScan for sure
            bdt.FingerDelay--;         // Update the time of Delay
        }
        else
        {
            //***********************************************************
            // Calculating the Noise in the frame @ NO-FINGER
            //***********************************************************
            DataProc_FHBSDeltaNoiseEstimation((int16_t *)(bdt.DeltaDat16A));
            if(bdt.StretchValue >= (STRETCH_STNUM<<STRETCH_SHIFT))
            {
                //*************************************
                // No TX-SCAN is NOT operating NOW
                //*************************************
                bdt.CurNoiseSum  = (bdt.CurNoiseSum>>1) + (bdt.CurNoiseSum>>2);
                bdt.CurNoiseSum += (bdt.NoiseDataFrFrame>>2);


                //if(maxS < bdt.CurNoiseSum) maxS = bdt.CurNoiseSum;  
                //if(minS > bdt.CurNoiseSum) minS = bdt.CurNoiseSum;


                #ifdef STM32VC_LCD
                    //TFT_ShowNum(10, 24, bdt.CurNoiseThrLo, fcolor, LCD_COLOR_GREEN);
                    TFT_ShowNum(20, 23, bdt.CurNoiseSum,   fcolor, LCD_COLOR_GREEN);
                    //TFT_ShowNum(10, 23, minS,   fcolor, LCD_COLOR_GREEN);
                    //TFT_ShowNum(30, 23, maxS,   fcolor, LCD_COLOR_GREEN);
                    //TFT_ShowNum(30, 24, bdt.CurNoiseThrHi, fcolor, LCD_COLOR_GREEN);
                #endif
                if(bdt.CurNoiseSum > bdt.CurNoiseThrHi)
                {
                    //***********************************************************
                    // Judgement-1: The Noise is becoming a bigger, Tx Scan again
                    //***********************************************************
                    result = TXSCAN_ENABLE;
                }
                else if(bdt.CurNoiseSum < bdt.CurNoiseThrLo)
                {
                    //*************************************************************
                    // Judgement-2: The Noise is becoming a smaller, Tx Scan again
                    //*************************************************************
                    result = TXSCAN_ENABLE;
                }
                
                if(bdt.StretchValue > (STRETCH_STNUM<<STRETCH_SHIFT))
                {
                    //***********************************************************
                    // Judgement-3: Last time, TXSCAN is not finished
                    //***********************************************************
                    result = TXSCAN_ENABLE;
                }
                else if(0 == bdt.TxScanValid)
                {
                    //***********************************************************
                    // Judgement-4: Tx Scan again after long time waiting
                    //***********************************************************
                    result = TXSCAN_ENABLE;
                }
                else
                {
                    //***********************************************************
                    // TxScan result is still Valid, we should wait for a while
                    //***********************************************************
                    bdt.TxScanValid--;
                }

                if(TXSCAN_ENABLE == result)
                {
                    //*******************************************************************
                    // The first step, init Stretch for the first measurement
                    // We are prepare to do the measurement
                    //*******************************************************************
                    bdt.StretchValue    = 0;
                    bdt.PCBA.DurStretch = bdt.StretchValue;
                    bdt.SumNoiseDataFrFrame = 0;
                    SPI_write_singleData(DURS_REG, DURS_STRETCH_DUR(bdt.PCBA.DurStretch)|DURS_STRETCH_INC(STRETCH_INC_REG25));
                }   // Prepare to TxScan again
            }
            else
            {
                //*************************************
                // TX-SCAN is operating NOW, GO ON...
                //*************************************
                result = TXSCAN_ENABLE;
            }
        } // No FingerDelay Case
    }     // No Finger Case
#endif
    return result;
}

void DataProc_FrequencyHopByStretch(uint16_t *buf)
{
#if 1
  #ifdef FREQHOP_BYSTRETCH
    uint16_t temp;
    #ifdef STM32VC_LCD
    uint16_t fcolor = LCD_COLOR_RED;
    #endif

    bdt.SumNoiseDataFrFrame += bdt.NoiseDataFrFrame;

    if(bdt.TxScanNoiseCount < NOISECAL_NUM)
    {
        //*************************************************************
        // Record and Saving the Noise Level for many(16) times
        //*************************************************************
        bdt.TxScanNoiseCount++;
    }
    else
    {
        //*********************************************************************
        // We have saved the Noise Level with the same Stretch for 16 times
        // Based on the saving Noise Level, the Aveerage Value is calculated
        //*********************************************************************
        temp  = (uint16_t)((bdt.SumNoiseDataFrFrame)>>NOISECAL_SHIFT);
        bdt.NoiseDataTable[(bdt.StretchValue)>>STRETCH_SHIFT]   = temp;

        #ifdef STM32VC_LCD
        //*********************************************************************
        // Show the result into LCD SCREEN
        //*********************************************************************
        TFT_ShowNum(10, 2+((bdt.StretchValue)>>STRETCH_SHIFT), (bdt.StretchValue)>>STRETCH_SHIFT, fcolor, LCD_COLOR_GREEN);
        TFT_ShowNum(20, 2+((bdt.StretchValue)>>STRETCH_SHIFT), temp, fcolor, LCD_COLOR_GREEN);
        #endif

        //*********************************************************************
        // Reset the value for the next measuement
        //*********************************************************************
        bdt.TxScanNoiseCount    = 0;  // Init for next Stretch Measurement
        bdt.SumNoiseDataFrFrame = 0;  // Init for next Stretch Measurement

        if(bdt.StretchInReg == STRETCH_STNUM)
        {
             //***************************************************************
             // This is the FIRST TIME TX SCAN actually
             //***************************************************************
             bdt.StretchValue += STRETCH_STEP; // 0,1,2,3,4 "8 will be 0"
             if(bdt.StretchValue >= (STRETCH_STNUM>>1)) 
             { 
                 //*********************************************************************
                 // All measuement are done
                 //*********************************************************************
                 //DataProc_FHBSBestTXFreqSearch(bdt.NoiseDataTable); // bdt.StretchInReg
                 {
                     uint16_t i, leastval, leastindex;
                     leastval   = bdt.NoiseDataTable[0];
                     leastindex = 0;
                     for(i = 1; i<(STRETCH_STNUM>>1); i++)
                     {
                         if(leastval > bdt.NoiseDataTable[i])
                         {
                             leastval   = bdt.NoiseDataTable[i];
                             leastindex = i;
                         }
                     }
                     bdt.StretchInReg  = leastindex;
                     bdt.CurNoiseSum   = bdt.NoiseDataTable[bdt.StretchInReg];
                     bdt.CurNoiseThrHi = bdt.CurNoiseSum + (bdt.CurNoiseSum>>4); // High Threshold should be lower based on testing
                     bdt.CurNoiseThrLo = bdt.CurNoiseSum - (bdt.CurNoiseSum>>2);
                 }
                 bdt.PCBA.DurStretch = bdt.StretchInReg;
                 #ifdef STM32VC_LCD
                     TFT_ShowNum(10, 24, bdt.StretchInReg, fcolor, LCD_COLOR_GREEN);
                     TFT_ShowNum(20, 24, bdt.NoiseDataTable[bdt.StretchInReg], fcolor, LCD_COLOR_GREEN);
                     //maxS = bdt.CurNoiseSum;  
                     //minS = bdt.CurNoiseSum;
                 #endif
                 bdt.TxScanValid  = 20; // 250ms only; TXSCAN_VALIDPERIOD;
                 bdt.StretchValue = (STRETCH_STNUM<<STRETCH_SHIFT); // Make the measure loop is done!
             }
             else
             {
                 //*********************************************************************
                 // Now, we change the stretch to the new value
                 //*********************************************************************
                 bdt.PCBA.DurStretch = bdt.StretchValue;
             }
        }
        else
        {
            //***************************************************************
            // This is the routine TX SCAN now on
            //***************************************************************
            bdt.StretchValue += STRETCH_STEP; // 0,1,2,3,4,..., 14, "15 will be 0"
            if(bdt.StretchValue >= (STRETCH_STNUM<<STRETCH_SHIFT)) 
            { 
                //*********************************************************************
                // All measuement are done
                //*********************************************************************
                DataProc_FHBSBestTXFreqSearch(bdt.NoiseDataTable); // bdt.StretchInReg
                bdt.PCBA.DurStretch = bdt.StretchInReg;
                bdt.CurNoiseSum     = bdt.NoiseDataTable[bdt.StretchInReg];
                bdt.CurNoiseThrHi   = bdt.CurNoiseSum + (bdt.CurNoiseSum>>4); // High Threshold should be lower based on testing
                bdt.CurNoiseThrLo   = bdt.CurNoiseSum - (bdt.CurNoiseSum>>2);
                bdt.TxScanValid     = TXSCAN_VALIDPERIOD; //
                #ifdef STM32VC_LCD
                    TFT_ShowNum(10, 24, bdt.CurNoiseThrLo, fcolor, LCD_COLOR_GREEN);
                    TFT_ShowNum(20, 24, bdt.CurNoiseSum,   fcolor, LCD_COLOR_GREEN);
                    TFT_ShowNum(30, 24, bdt.CurNoiseThrHi, fcolor, LCD_COLOR_GREEN);
                    TFT_ShowNum(40, 24, bdt.StretchInReg,  fcolor, LCD_COLOR_GREEN);
                    //maxS = bdt.CurNoiseSum;  
                    //minS = bdt.CurNoiseSum;
                #endif
            }
            else
            {
                //*********************************************************************
                // Now, we change the stretch to the new value
                //*********************************************************************
                bdt.PCBA.DurStretch = bdt.StretchValue;
            }
        }
        SPI_write_singleData(DURS_REG, DURS_STRETCH_DUR(bdt.PCBA.DurStretch)|DURS_STRETCH_INC(STRETCH_INC_REG25));



    }
  #endif //FREQHOP_BYSTRETCH
#else
    #ifdef FREQHOP_BYSTRETCH
    
    DataProc_PowerNoiseLevelJudge();
    
    if(NO_FINGER == bdt.BFD.bbdc.FingerExist)
    {
        return;   // 思路改一下, we need noise estimate during "Non-Finger"
    }
    
    #ifdef FREQY_HOP_ONLY
    if(bdt.BNoiseCount < 4)
    #else
    if((bdt.BNoiseCount < 4) && (bdt.SNoiseCount < 4))
    #endif
    {
        /***********************************************************
        * if new Noise is too big for 8 times, tune new Freq;
        ************************************************************/
        
        if(dnoise[0] > bdt.CurNoiseThrd)
        {
            bdt.BNoiseCount++;
        }
        else
        {
            bdt.BNoiseCount = 0;
        }
        
        #ifndef FREQY_HOP_ONLY
        /***********************************************************
        * if new Noise is too small for 8 times, tune new Freq;
        ************************************************************/
        if(dnoise[0] < (bdt.CurNoiseThrd>>2))
        {
            bdt.SNoiseCount++;
        }
        else 
        {
            bdt.SNoiseCount = 0;
        }
        #endif
        
        return;
        }
        else if(TX_SPEED_TUNED_DONE == bdt.FreqHopState)
        {
            {
            bdt.FreqHopState    = TX_FAST_SPEED;
            #ifdef DISBALE_HWFH_INSWFH
            SPI_write_singleData(RXCN_REG, (bdt.REG3E_Value)&0x1f); /* REG3E*/
            #endif
            bdt.PCBA.DurStretch = 1;   /* Fast Speed*/
            SPI_write_singleData(DURS_REG, DURS_STRETCH_DUR(bdt.PCBA.DurStretch)|DURS_STRETCH_INC(STRETCH_INC_REG25));  /* cfg_reg25, 12'h000*/
            
            bdt.CalibCount      = 0;            /* Prepare to Calibrate*/
            bdt.CurrentNoise[0] = 0;            /* Current Noise*/
            bdt.CurrentNoise[1] = MAX_UINT16;   /* Minimum Noise*/
            bdt.CurrentNoise[2] = 1;            /* ID for Minimum Noise*/
            }
        }
    
        if(bdt.CalibCount >= 2)
        if(bdt.CalibCount <= CALINUM_EVERYSTEP)
        {
    /*****************************************************
            *2,3,4,5
          ******************************************************/
            bdt.CurrentNoise[0] += dnoise[0];       /* Current Noisev*/
        }
    
    
        /*****************************************************
        * Switch the TX Speed from slow to fast
        ******************************************************/
        switch(bdt.FreqHopState)
        {
            case TX_FAST_SPEED:
            {
                if(bdt.CalibCount < CALINUM_EVERYSTEP)
                {
                
                    bdt.CalibCount++;
                }
                else
                {
                            /*****************************************
                            * Extract the Minimum, Record the ID
                            ******************************************/
                    if(bdt.CurrentNoise[0] < bdt.CurrentNoise[1])
                    {
                        bdt.CurrentNoise[1] = bdt.CurrentNoise[0]; /* Record the Minimum Noise*/
                        bdt.CurrentNoise[2] = bdt.PCBA.DurStretch; /* Record the ID of Minimum Noise*/
                    }
                    
                                /*****************************************
                                * Update the Stretch Value
                                ******************************************/
                    if(bdt.PCBA.DurStretch<17)
                    {
                        bdt.PCBA.DurStretch += 2;
                        SPI_write_singleData(DURS_REG, DURS_STRETCH_DUR(bdt.PCBA.DurStretch)|DURS_STRETCH_INC(STRETCH_INC_REG25));  /* cfg_reg25, 12'h000*/
                    }
                    else
                    {
                        bdt.FreqHopState    = TX_SPEED_TUNING;
                    }
                    
                                /*****************************************
                                * Reset the Vars
                                ******************************************/
                    bdt.CalibCount      = 1;
                    bdt.CurrentNoise[0] = 0;
                }
                break;
            }
            
            case TX_MIDDLE_SPEED:
            case TX_SLOW_SPEED:
            {
                break;
            }
            case TX_SPEED_TUNING:
            {
                bdt.PCBA.DurStretch = bdt.CurrentNoise[2];
                SPI_write_singleData(DURS_REG, DURS_STRETCH_DUR(bdt.PCBA.DurStretch)|DURS_STRETCH_INC(STRETCH_INC_REG25));  /* cfg_reg25, 12'h000*/
                DataProc_FreqHopUpdateThrd(bdt.CurrentNoise[1]);
                
                #ifdef STM32VC_LCD
                TFT_ShowNum(20, 28, bdt.CurrentNoise[2], 0xf800, LCD_COLOR_GREEN);
                TFT_ShowNum(30, 28, bdt.CurNoiseThrd, 0xf800, LCD_COLOR_GREEN);
                #endif
                
                #ifdef DISBALE_HWFH_INSWFH
                SPI_write_singleData(RXCN_REG, (bdt.REG3E_Value)); /* REG3E*/
                #endif
                bdt.FreqHopState    = TX_SPEED_TUNED_DONE;
                bdt.FreqHopRestPerd = WORK_MODE_NOFING_MAXPERD;
                bdt.SNoiseCount     = 0;        /* Re-tuning*/
                bdt.BNoiseCount     = 0;        
                bdt.CalibCount      = 0;
                break;
            }
            case TX_SPEED_TUNED_DONE:
            {
                break;
            }
        }
    #endif //FREQHOP_BYSTRETCH
#endif
}

uint16_t DataProc_FDGetValidSampNumber(uint32_t *buf)
{
    uint16_t num = 0;
#ifdef AUTO_FACEDETECTION
    uint16_t i, j;

    for (i=0; i<XMTR_NUM; i++)
        for (j=0; j<RECV_NUM; j++)
        {
            if(buf[i] & (1<<j)) num++;        
        }
#endif
    return num;
}

void DataProc_FaceDetectProcess(void)
{
    #ifdef AUTO_FACEDETECTION
    uint16_t i, temp;
    temp = ((bdt.ThresholdInFrame)>>1);
    DataProc_DeltaMatrix2Array(bdt.DeltaBitDat, temp); /* mark finger point */

    bdt.FDC.WFNum  = bdt.FingerDetectNum;  // Get the all Number of Valid samples
    bdt.FDC.Flag   = FACE_DETECT_FAR;      // Reset it as no FACE Detect
    bdt.FDC.BigNum = 0;

    if(bdt.FDC.WFNum > MIN_VALIDDATA4FACEDET)
    {
        //************************************************************
        // Face may be appeared, we can find "FingerDetectNum" area
        //************************************************************
        DataProc_PullBitmapApart(bdt.DeltaBitDat);
        for(i = 0;i < bdt.FingerDetectNum; i++)
        {
            temp = DataProc_FDGetValidSampNumber(bdt.DPD[i].BitMapDat);
            if(bdt.FDC.BigNum < temp) 
            {
                bdt.FDC.BigNum = temp;
            }
            if(temp > MIN_CONNECTNUM4FACEDET)
            {
                bdt.FDC.Flag          = FACE_DETECT_NEAR;
                bdt.FaceDetectDelay   = MIN_FACEDET_LOSSPERD;
                break; 
            }
        }
    }

    if(bdt.FaceDetectDelay)
    {
        bdt.FaceDetectDelay--;
        bdt.FDC.Flag = FACE_DETECT_NEAR;
    }
    #endif
}

void DataProc_AMDPComputePoint(void)
{
    uint16_t i;
    uint16_t LMax, iLLoc,RMax, iRLoc, LocDiff;
    uint16_t wSum, iSum;
    uint16_t rslt[2], iStart, iEnd;
    
    //************************************************************
    // Calculating the Max Value's Location
    //************************************************************
    LMax  = bdt.DeltaDat16A[0][TRINGLE_LEFT];
    iLLoc = 0;
    RMax  = bdt.DeltaDat16A[0][TRINGLE_RIGHT];
    iRLoc = 0;
    for(i=1; i<(XMTR_NUM); i++)
    {
        if( bdt.DeltaDat16A[i][TRINGLE_LEFT] > LMax) 
        {
            iLLoc = i;
            LMax  = bdt.DeltaDat16A[i][TRINGLE_LEFT];
        }
        if( bdt.DeltaDat16A[i][TRINGLE_RIGHT] > RMax) 
        {
            iRLoc = i;
            RMax  = bdt.DeltaDat16A[i][TRINGLE_RIGHT];
        }
    }

    if(iRLoc > iLLoc) LocDiff = iRLoc - iLLoc;
    else LocDiff = iLLoc - iRLoc;
    
    //************************************************************
    // Finger is Appeared, and MAX Location is closer
    //************************************************************
    if(((LMax > 100) || (RMax>100) || ((LMax+RMax)>100)) && (LocDiff<=1))
    {
        rslt[0] = 0;
        rslt[1] = 0;
        iStart  = 0;
        iEnd    = 2;
        if(LMax < 100)
        {
            iStart  = 0;
            iEnd    = 1;
        }
        else if(RMax < 100)
        {
            iStart  = 1;
            iEnd    = 2;
        }
        
        for(i = iStart; i < iEnd; i++)
        {
            wSum = bdt.DeltaDat16A[iLLoc][i]*(iLLoc);
            iSum = bdt.DeltaDat16A[iLLoc][i];
            if(0 == iLLoc)
            {
                wSum += bdt.DeltaDat16A[iLLoc+1][i]*(iLLoc+1);
                iSum += bdt.DeltaDat16A[iLLoc+1][i];
            }
            else if((XMTR_NUM-1) == iLLoc)
            {
                wSum += bdt.DeltaDat16A[iLLoc-1][i]*(iLLoc-1);
                iSum += bdt.DeltaDat16A[iLLoc-1][i];
            }
            else
            {
                wSum += bdt.DeltaDat16A[iLLoc-1][i]*(iLLoc-1);
                iSum += bdt.DeltaDat16A[iLLoc-1][i];
                wSum += bdt.DeltaDat16A[iLLoc+1][i]*(iLLoc+1);
                iSum += bdt.DeltaDat16A[iLLoc+1][i];
            }
            rslt[i] = (wSum<<8)/iSum;
        }

        if(rslt[0] == 0)
            bdt.DPD[0].Finger_X_XMTR = rslt[1];
        else if(rslt[1] == 0)
            bdt.DPD[0].Finger_X_XMTR = rslt[0];
        else
            bdt.DPD[0].Finger_X_XMTR = (rslt[0] + rslt[1])>>1;
    }
    else
    {
        rslt[0] = 0;
        rslt[1] = 0;
        bdt.DPD[0].Finger_X_XMTR = 0;
    }
}

/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
void DataProc_AbsModeDataProc(void)
{
    int16_t  sY_Xmtr[XMTR_NUM+1];
    uint16_t i, sY_Xmtr_MaxLoc = 0;
    uint16_t sY_MaxVal = 0;
    uint32_t sY_WeigSum;
    uint16_t sVal_Left, sVal_Right;

    bdt.FingerDetectNum = 0;
    for(i = 0; i < FINGER_NUM_MAX; i++)
    {
        bdt.DPD[i].Finger_X_XMTR   = 0; // NO-Finger
        bdt.DPD[i].Finger_Y_RECV   = 0; // No-Finger
    }

#if 0
    DataProc_AMDPComputePoint();
    if(bdt.DPD[0].Finger_X_XMTR == 0) bdt.FingerDetectNum = 0;
    else bdt.FingerDetectNum = 1;
#else
    //************************************************************
    // Calculating the Vertical Location (XMTR, Y Position)
    //************************************************************
    for(i=0; i<(XMTR_NUM); i++)
    {
        sY_Xmtr[i]= bdt.DeltaDat16A[i][TRINGLE_LEFT]+bdt.DeltaDat16A[i][TRINGLE_RIGHT];
        if(sY_Xmtr[i] >= sY_MaxVal) 
        {
            sY_Xmtr_MaxLoc = i;
            sY_MaxVal      = sY_Xmtr[i];
        }
    }

    if(sY_MaxVal > 100)
    {
        //************************************************************
        // There is a finger detected
        //************************************************************
        i = sY_Xmtr_MaxLoc;
        sVal_Left  = bdt.DeltaDat16A[i][TRINGLE_LEFT];
        sVal_Right = bdt.DeltaDat16A[i][TRINGLE_RIGHT];
        if((XMTR_NUM-1) == i)
        {
            sY_Xmtr[XMTR_NUM] = 0;
            sVal_Left  += bdt.DeltaDat16A[XMTR_NUM-2][TRINGLE_LEFT]  + bdt.DeltaDat16A[XMTR_NUM-2][TRINGLE_LEFT];
            sVal_Right += bdt.DeltaDat16A[XMTR_NUM-2][TRINGLE_RIGHT] + bdt.DeltaDat16A[XMTR_NUM-2][TRINGLE_RIGHT];
        }
        else if(0 == i) 
        {
            i++;           // Special Handle(QFU)
            sVal_Left  += bdt.DeltaDat16A[1][TRINGLE_LEFT]  + bdt.DeltaDat16A[1][TRINGLE_LEFT];
            sVal_Right += bdt.DeltaDat16A[1][TRINGLE_RIGHT] + bdt.DeltaDat16A[1][TRINGLE_RIGHT];
        }
        else
        {
            sVal_Left  += bdt.DeltaDat16A[i-1][TRINGLE_LEFT]  + bdt.DeltaDat16A[i+1][TRINGLE_LEFT];
            sVal_Right += bdt.DeltaDat16A[i-1][TRINGLE_RIGHT] + bdt.DeltaDat16A[i+1][TRINGLE_RIGHT];
        }

        //****************************************************************
        // Calculating the Vertical Position Finger_X_XMTR
        //****************************************************************
        sY_WeigSum  = sY_Xmtr[i - 1]*(i - 1);
        sY_WeigSum += sY_Xmtr[i + 1]*(i + 1);
        sY_WeigSum += sY_Xmtr[i]    *(i);
        
        bdt.DPD[0].Finger_X_XMTR = (sY_WeigSum<<8)/(sY_Xmtr[i]+sY_Xmtr[i-1]+sY_Xmtr[i+1]);
                
        //************************************************************
        // Calculating the Horizontal Location (Finger_Y_RECV)
        // Based on the Tringle Shape
        //************************************************************
        bdt.DPD[0].Finger_Y_RECV = ((sVal_Left*RECV_NUM)<<8)/(sVal_Left + sVal_Right);
        bdt.FingerDetectNum = 1;
    }
#endif
	
}


/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/

void WholeFrame_DataProc(uint16_t *buffer)
{
    uint16_t i, j;

    /******************************************************
    *  Extract Delta Data for the current Buffer;
    *  Get the Maximum Value and its Location;
    *******************************************************/
    if(1 == bdt.NDD.Frame_Count)
    {
        bdt.PowerOnWithFinger  = 0;
        bdt.MEM_MIN_XY_Count = 0;
        bdt.BaseChangeFlag++;                /* Counting 1 for baseline updating*/
        for(i = 0;i < XMTR_NUM;i++)
            for(j = 0;j < RECV_NUM;j++)
            {
                bdt.BFD.BaseDat[i][j]      = buffer[(i*RECV_NUM) + j];
                bdt.BFD.BaseDatSaved[i][j] = bdt.BFD.BaseDat[i][j];
            }
    }

    DataProc_CalculateDeltaData(buffer); /* Extract Delta Data*//* Delta data == Origin data - Baseline data */
    
    DataProc_FindMaxAndMinValue();       /* Find Max Value and its Location, also Min Value*/
    bdt.ThresholdInFrame  = DataProc_Ratio2Threshold(bdt.MaxValueInFrame, BE_PERCENT_RATIO);
    #ifndef ABSO_MODE_ACTIVE
    DataProc_DepressNoise();
    DataProc_FindMaxAndMinValue();       /* Same as the previous one*/
    bdt.ThresholdInFrame  = DataProc_Ratio2Threshold(bdt.MaxValueInFrame, BE_PERCENT_RATIO);
    #endif
    bdt.PCBA.AbnormalMaxDiff = bdt.MaxValueInFrame>>3;               /* (1/8) of Max Value in the Frame*/
    if(bdt.PCBA.AbnormalMaxDiff < 20)
    {
        bdt.PCBA.AbnormalMaxDiff = 20; 
    }

    /******************************************************
    * Make sure the Max Value in the Frame is big enough
    * Normally it should be 200, if MAX value is smaller
    * than the value, we will not handle the finger PROC
    *******************************************************/
    #ifdef AUTO_FACEDETECTION
    DataProc_FaceDetectProcess();
    if((bdt.MaxValueInFrame < bdt.PCBA.MaxValueNoFinger) || (FACE_DETECT_NEAR == bdt.FDC.Flag))
    #else    
    if(bdt.MaxValueInFrame < bdt.PCBA.MaxValueNoFinger)
    #endif
    {
        bdt.FingerDetectNum = 0;
        bdt.BFD.bbdc.FingerExist = NO_FINGER;
    }
    else 
    {    
        DataProc_DeltaMatrix2Array(bdt.DeltaBitDat, bdt.ThresholdInFrame); /* mark finger point */
    }


    //if(UTIL_ValidPointsEnoughJudge4DataC(bdt.FingerDetectNum) != 0)  
    if(bdt.FingerDetectNum > MIN_VALUE_POINT) 
    {
#ifdef ABSO_MODE_ACTIVE
        DataProc_AbsModeDataProc();
#else
        #ifdef PRESS_KEY_DETECT
        DataProc_PressKeyDetect();
   
        for(i=SXMTR_NUM;i<XMTR_NUM;i++)
            for(j=0; j<RECV_NUM; j++)
            {
                bdt.DeltaDat_kp[j]    = bdt.DeltaDat16A[i][j];
                bdt.DeltaDat16A[i][j]=0;
            }
        #endif

        /***********************************************************
        * We think there maybe one or more fingers detected now
        * 1. Extract a new "bdt.FingerDetectNum"
        * 2. Pull new BitMap array from old array
        ************************************************************/
        DataProc_PullBitmapApart(bdt.DeltaBitDat);
        DataProc_FindExtremum();
        DataProc_ExtractExtremumArea();
        DataProc_ComputePoint();
#endif
    }
    else
    {
        bdt.FingerDetectNum = 0;
        for(i = 0; i < FINGER_NUM_MAX; i++)
        {
            bdt.DPD[i].Finger_X_XMTR   = 0; /* NO-Finger*/
            bdt.DPD[i].Finger_Y_RECV   = 0; /* No-Finger*/
        }
    }
    
    DataProc_HandleFingerInfo();
}


/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/

void FrameBuffer_Post_Handled(uint16_t *buffer)
{
    /*******************************************
    * Baseline Processing for every frame
    ********************************************/
    Baseline_BaseBufferHandled(buffer);
#ifndef TC1540
    #ifdef FREQHOP_BYSTRETCH
        if(TXSCAN_ENABLE == DataProc_FHBSOperatingSwitch())
        {
            DataProc_FrequencyHopByStretch(buffer);
        }
        else
        {
           if(FINGER_SHOW == bdt.BFD.bbdc.FingerExist)
               DataProc_PowerNoiseLevelJudge();
        }
    #else
        DataProc_PowerNoiseLevelJudge();
    #endif
    
    bdt.MEM_MAX_X_XMTR = bdt.FRM_MAX_X_XMTR;
    bdt.MEM_MAX_Y_RECV = bdt.FRM_MAX_Y_RECV;
    bdt.MEM_MIN_X_XMTR = bdt.FRM_MIN_X_XMTR;
    bdt.MEM_MIN_Y_RECV = bdt.FRM_MIN_Y_RECV;
#endif

    /********************************************************
    * Output the Frame Data to UART (PC showing)
    *********************************************************/
    #ifdef CN1100_STM32
    #ifdef COMMUNICATION_WITH_PC
    
    #ifdef SHOW_EVERY_FRAME_DATA
    Print_FrameAndDelta_Data(buffer);  // UART working only
    #else
    if(dbg.DebugInfoLevel >= DEBUG_INFO_DELTA)
    {
        Send_Delta_Data_To_PC(buffer);
    }
    #endif
    
    #endif
    #else
    #ifdef CN1100_PRINT
    Print_Select_Data(BufID);
    #else
    spidev->mode |= CN1100_DATA_PREPARED;
    bd->bdt = &bdt;
    wake_up_interruptible(&spidev->waitq);
    Report_Coordinate();
    #endif
    #endif
    
    #ifdef STM32VC_LCD
    LCD_Show_STM32(buffer);
    #endif
}

/***********************************************
* Check Buffer A/B is ready for handling
************************************************/
/*******************************************************************************
  * Function Name  : 
  * Description    : 
  * Input          : 
  * Output         : 
  * Return         : 
  *******************************************************************************/

void FrameBuffer_Full_Handled(uint16_t *buffer)
{
    uint16_t i, j; 
    #ifdef SCREEN_ADAPTIVE
    uint16_t max = 0,min=4095;
    #endif

    #ifdef ABSO_MODE_ACTIVE
    uint16_t buf[FRAMEBUF_SIZE];
    
    /*******************************************************
    * ABS MODE need Adjust the Buffer Order
    ********************************************************/
    #ifdef TC6336_CHIP
    uint8_t dmap[FRAMEBUF_SIZE] = {15,14,16,13,17,12,18,11,19,10,20,9,21,8,22,7,23,6,24,5,25,4,26,3,27,2,28,1,29,0};
    #endif
    
    #ifdef TC2138_CHIP
    uint8_t dmap[FRAMEBUF_SIZE] = {15,14,16,13,17,12,18,11,19,10,20,9,21,8,22,7,23,6,24,5,25,4,26,3,27,2,28,1,29,0};
    #endif
    
    #ifdef TC6206_CHIP
        #ifdef PCB_TRINGLE_BOARD
        //uint8_t dmap[FRAMEBUF_SIZE] = {27,26,12,25,13,11,14,10,15,9,16,8,17,7,18,6,19,5,20,4,21,3,22,2,23,1,24,0};   // PCB (15X10 Interface)
        //uint8_t dmap[FRAMEBUF_SIZE] = {26,27, 25,12, 11,13, 10,14, 9,15, 8,16, 7,17, 6,18, 5,19, 4,20, 3,21, 2,22, 1,23, 0,24};   // PCB (15X10 Interface)
        //uint8_t dmap[FRAMEBUF_SIZE] = {14,15,16,17,18,19,20,21,22,23,24,25,26,27};   // PCB (14+14 Interface)
        //uint8_t dmap[FRAMEBUF_SIZE] = {14,15,16,17,18,19,20,21,22,23,24,25,26,27};   // PCB (14+14 Interface)
        uint8_t dmap[FRAMEBUF_SIZE] = {14,13,15,12,16,11,17,10,18,9,19,8,20,7,21,6,22,5,23,4,24,3,25,2,26,1,27,0}; // Touch Screen
        #else
        uint8_t dmap[FRAMEBUF_SIZE] = {14,13,15,12,16,11,17,10,18,9,19,8,20,7,21,6,22,5,23,4,24,3,25,2,26,1,27,0}; // Touch Screen
        #endif
    #endif
    
    for(i=0; i<FRAMEBUF_SIZE; i++) buf[i]    = buffer[dmap[i]];
    for(j=0; j<FRAMEBUF_SIZE; j++) buffer[j] = buf[j];
    
    #ifdef TC6206_CHIP
        #ifdef PCB_TRINGLE_BOARD
            // for(j=0; j<8; j++) buffer[j] = 0; // Only 10 PAIRs of data is valid
        #endif
    #endif
    #endif

    /*******************************************************
    * Discard the first frame when mode is changed
    * from other mode  to normal mode
    ********************************************************/
    if(BASE_FRAME_DISCARD == bdt.BFD.bbdc.BaseUpdateCase)
    {
        bdt.BFD.bbdc.BaseUpdateCase = BASE_FRAME_HOLD;
        return;
    }
    
    /********************************************************
    * If there is no finger in the Frame, update Base Frame
    *********************************************************/
    if(bdt.BFD.InitCount < FRAME_0007)
    {
        switch(bdt.BFD.InitCount)
        { 
            #ifdef SCREEN_ADAPTIVE
            case FRAME_0000: 
            {
                break;
            }

            case FRAME_0001:  
            {
                break;
            }
             /* scan TP and config it */
            case FRAME_0002:
            {   
                for (i = 0;i < XMTR_NUM;i++)
                    for (j = 0;j < RECV_NUM;j++)
                    {
                         if(buffer[i*RECV_NUM+j]>max)
                         { 
                             max=buffer[i*RECV_NUM+j];
                         }
                         if(buffer[i*RECV_NUM+j]<min)
                         { 
                             min=buffer[i*RECV_NUM+j];
                         }
                     }
                DataProc_Screen_Adaptive(max,min);
                
                break;
            }
            case FRAME_0003:
            {
                break;
            }
            case FRAME_0004:
            {
                break;
            }
            case FRAME_0005:
            {
                break;
            }
            case FRAME_0006:
            {
                /***********************************************
                *Initial Baseline
                ************************************************/
                bdt.BaseChangeFlag++;                /*  Counting 1 for baseline updating*/
                bdt.BFD.TooLongTime4BaseUpdate = 0;  /*  Reset the Timing Count*/
                bdt.BFD.AfterBaseUpdatedTime   = 0;  /*  Reset the Timing Count*/
                bdt.BFD.bbdc.BaseUpdateCase    = BASELINE_HOLDING_CASE;
                for (i = 0;i < XMTR_NUM;i++)
                    for (j = 0; j < RECV_NUM;j++)
                    {
                        bdt.BFD.BaseDat[i][j] = buffer[i*RECV_NUM+j];
                        bdt.BFD.BaseDatSaved[i][j] = buffer[i*RECV_NUM+j];
                    }
                break;
            }
            #else /*NO ADAPTIVE */
            case FRAME_0000:
            {
                break;
            }
            case FRAME_0001:
            {
                /***********************************************
                * Initial Baseline
                ************************************************/
                for (i = 0;i < XMTR_NUM;i++)
                    for (j = 0; j < RECV_NUM;j++)
                    {
                        bdt.BFD.BaseDat[i][j] = buffer[i*RECV_NUM+j];
                    }
                break;
            }
            case FRAME_0002:
            {
                /***********************************************
                * Get The first Baseline
                ************************************************/
                for (i = 0;i < XMTR_NUM;i++)
                    for (j = 0; j < RECV_NUM;j++)
                    {
                        bdt.BFD.BaseDat[i][j]      = (bdt.BFD.BaseDat[i][j] + buffer[i*RECV_NUM+j])>>1;
                        bdt.BFD.BaseDatSaved[i][j] = bdt.BFD.BaseDat[i][j];
                    }
                bdt.BFD.InitCount = BASEFRAME_INITCNT;
                break;
            }
            #endif
        }
        bdt.BFD.InitCount++;
        return;
    } 
    else
    {
        /*******************************************
        * Data Processing for Buffer
        ********************************************/
        bdt.BFD.bbdc.FingerExist = FINGER_SHOW;
        WholeFrame_DataProc(buffer);
    }
    
    /******************************************************
    * 1. Process the base (update) handling
    * 2. Process the frequency hop based on NOISE
    *******************************************************/
    FrameBuffer_Post_Handled(buffer);
    
    #ifdef CN1100_STM32
    /***********************************************
    * Show LED on GPIOA_1 (D1) when Finger existed
    ************************************************/
    if(bdt.FingerDetectNum != 0) 
    {
        STM32_LEDx_TURN_ONorOFF(LED1, LED_ON);
    }
    else
    {
        STM32_LEDx_TURN_ONorOFF(LED1, LED_OFF);
    }
    #endif
}


/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
void BufferAHandle(void)
{
    uint16_t *buffer;
    
    buffer = (uint16_t *)(bdt.FrameDatLoadA);
    /*********************************************
    * Read data from Buffer A
    **********************************************/
#ifdef TC1540
    // TC1540
    SPI_read_DATAs(FRMA_BUFF,    FRAMEBUF_SIZE, buffer); // Read data to Buffer A
#else
    #ifdef ONE_MORE_LINE_SCAN
    SPI_read_DATAs(0x400+RECV_NUM, XMTR_NUM*RECV_NUM, buffer); /*  Read data to Buffer A*/
    #else
    SPI_read_DATAs(0x400, XMTR_NUM*RECV_NUM, buffer); /*  Read data to Buffer A*/
    #endif
#endif
    FrameBuffer_Full_Handled(buffer);
    bdt.BSDSTS.iBuf_A_Fill = FRAME_UNFILLED;

}


/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
void BufferBHandle(void)
{
    uint16_t *buffer;
    
    buffer = (uint16_t *)(bdt.FrameDatLoadB);
    /*********************************************
    * Read data from Buffer B
    **********************************************/
#ifdef TC1540
     // TC1540
     SPI_read_DATAs(FRMA_BUFF+FRAMEBUF_SIZE,  FRAMEBUF_SIZE, buffer); // Read data to Buffer B
#else
    #ifdef ONE_MORE_LINE_SCAN
    SPI_read_DATAs(0x400+RECV_NUM+XMTR_NUM*RECV_NUM+RECV_NUM, XMTR_NUM*RECV_NUM, buffer); /*  Read data to Buffer A*/
    #else
    SPI_read_DATAs(0x400+XMTR_NUM*RECV_NUM, XMTR_NUM*RECV_NUM, buffer); /*  Read data to Buffer B*/
    #endif
#endif
    FrameBuffer_Full_Handled(buffer);
    bdt.BSDSTS.iBuf_B_Fill = FRAME_UNFILLED;
}


#endif

