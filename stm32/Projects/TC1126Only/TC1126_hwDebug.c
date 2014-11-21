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
 *
 * 修改记录2: Add one feature that acquire 10 fingers then parse 5 fingers 
 * 修改日期: 2014-11-12
 * 版 本 号:
 * 修 改 人: Wangpc(R01)
 * 修改内容: 
 *
 * 修改记录3: Rebuild structure of register
 * 修改日期: 2014-11-19
 * 版 本 号:
 * 修 改 人: Wangpc(R02)
 * 修改内容: 
 *****************************************************************************/

#ifndef CN1100_HWDEBUG_C
#define  CN1100_HWDEBUG_C

#include "CN1100_common.h"
#ifdef CN1100_STM32
#include "CN1100_STM32.h"
#endif
#ifdef CN1100_LINUX
#include "CN1100_linux.h"
#endif
#include "CN1100_Function.h"

#ifdef CN1100_STM32

/*******************************************************************************
* Function Name  : 
* Description    : Useless Now 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
uint16_t SPI_read_chipID(void)
{
    uint8_t  spi_data[2];
    uint16_t temp;

    #ifdef CN1100_I2C_COMM
    //**********************************************
    // As I2C port, Read_ChipID is not implemented
    //**********************************************
    spi_data[0]= 0xff;
    spi_data[1]= 0xff;
    #else
    ARC_CN1100_CS_LOW();
    ARC_SPI_SendByte(SPI1, READ_CHIP_ID);
    spi_data[0] = ARC_SPI_SendByte(SPI1, FLASH_DUMMY_BYTE);
    spi_data[1] = ARC_SPI_SendByte(SPI1, FLASH_DUMMY_BYTE);
    ARC_CN1100_CS_HIGH(); 
    #endif

    temp  = spi_data[0];
    temp  = temp<<8;
    temp += spi_data[1]; 
    return temp;
}


/*******************************************************************************
* Function Name  : SPI_write_singleData(addr, data)
* Description    : write data into the location if IC (addr), we use it when
*                  a value is put into REG
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
void SPI_write_singleData(uint32_t addr, uint16_t data)
{
#ifdef CN1100_I2C_COMM
    uint8_t bytes[2];
    I2C_param_struct __IO *pI2C_param;

    //ARC_I2C_Init();

    bytes[0]   = (data>>8)&0xff;
    bytes[1]   = data&0xff;

    pI2C_param = ARC_get_I2C_param();

    pI2C_param->I2C_DIRECTION = ARC_I2C_DIRECTION_TX;
    pI2C_param->DeviceAddr    = I2C_DEVICE_ADDR;

    pI2C_param->SubAddr[0] = (addr>>8)&0xff;
    pI2C_param->SubAddr[1] = (addr)&0xff;
    pI2C_param->TxData = bytes; // (uint8_t *)(&data);
    pI2C_param->TxNumOfBytes = 2;
    pI2C_param->TX_I2C_Index = 0;
    pI2C_param->TX_Generate_stop = 1;
    pI2C_param->TX_addr          = I2C_IDLE_STATUS;

    ARC_I2C_Write(I2C_Sel, pI2C_param);

    //ARC_SysTick_Delay(20);
    while(pI2C_param->TX_addr != I2C_STOP_STATUS);
    pI2C_param->TX_addr = I2C_IDLE_STATUS;

    //ARC_I2C_DeInit();
#else
    ARC_CN1100_CS_LOW();
    ARC_SPI_SendByte(SPI1, FAST_WRITE_DATA);    // FAST Read
    ARC_SPI_SendByte(SPI1, (addr >> 8) & 0xff); // ADDRESS 0x000000 // 16bits
    ARC_SPI_SendByte(SPI1, addr & 0xff);        // ADDRESS 0x000000 // 16bits
    ARC_SPI_SendByte(SPI1, (data >> 8) & 0xff);
    ARC_SPI_SendByte(SPI1, data & 0xff);
    ARC_CN1100_CS_HIGH(); 
#endif
}

/*******************************************************************************
* Function Name  : SPI_read_singleData(addr, data)
* Description    : read data from the location if IC (addr), we use it when
*                  a value is read from a REG
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
uint16_t SPI_read_singleData(uint32_t addr)
{
#ifdef CN1100_I2C_COMM
    uint8_t bytes[2], recv[2];
    I2C_param_struct __IO *pI2C_param;

    //ARC_I2C_Init();

    pI2C_param = ARC_get_I2C_param();

    pI2C_param->I2C_DIRECTION    = ARC_I2C_DIRECTION_TX;
    pI2C_param->DeviceAddr       = I2C_DEVICE_ADDR;

    pI2C_param->SubAddr[0]       = (addr>>8)&0xff;
    pI2C_param->SubAddr[1]       = (addr)&0xff;
    pI2C_param->TxData           = bytes;
    pI2C_param->TxNumOfBytes     = 0;
    pI2C_param->TX_I2C_Index     = 0;
    pI2C_param->TX_Generate_stop = 0;
    pI2C_param->TX_addr          = I2C_IDLE_STATUS;

    pI2C_param->RxData           = recv;
    pI2C_param->RxNumOfBytes     = 2;
    pI2C_param->RX_I2C_Index     = 0;

    ARC_I2C_Write(I2C_Sel, pI2C_param);

    //ARC_SysTick_Delay(20);
    while(pI2C_param->TX_addr != I2C_STOP_STATUS);
    pI2C_param->TX_addr = I2C_IDLE_STATUS;

    //ARC_I2C_DeInit();
    return ((recv[0]<<8) + recv[1]);
#else
    uint8_t bytes[2];
    ARC_CN1100_CS_LOW();
    ARC_SPI_SendByte(SPI1, FAST_READ_DATA);     // FAST Read
    ARC_SPI_SendByte(SPI1, (addr>>8)&0xff);     // ADDRESS 0x000000 // 16bits
    ARC_SPI_SendByte(SPI1, (addr)&0xff);        // ADDRESS 0x000000 // 16bits
    bytes[0] = ARC_SPI_SendByte(SPI1, FLASH_DUMMY_BYTE);
    bytes[1] = ARC_SPI_SendByte(SPI1, FLASH_DUMMY_BYTE);
    ARC_CN1100_CS_HIGH(); 
    return ((bytes[0]<<8) + bytes[1]);
#endif
}

/*******************************************************************************
* Function Name  : SPI_write_DATAs(addr, data)
* Description    : Useless now
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
void SPI_write_DATAs(uint32_t addr, uint16_t num, uint16_t *data)
{
#ifdef CN1100_I2C_COMM
    uint8_t *bytes, btmp;
    uint16_t i;
    I2C_param_struct __IO *pI2C_param;

    //ARC_I2C_Init();
    pI2C_param = ARC_get_I2C_param();

    bytes = (uint8_t *)(data); // (uint8_t *)(&data);

    //***************************************
    // Big Endian and Little Endian issue
    //***************************************
    for(i=0; i<num; i++)
    {
        btmp            = bytes[i<<1];
        bytes[i<<1]     = bytes[1+(i<<1)];
        bytes[1+(i<<1)] = btmp;
    }
    //printf("WDATAs_temp[0]: %x \n", data[0]);
    //printf("DATA_p: %x \n", bytes);
    //printf("BYTE_p: %x \n", data);
    //printf("BYTEs[0]: %x  ---- BYTEs[1]: %x\n", bytes[0], bytes[1]);

    pI2C_param->I2C_DIRECTION = ARC_I2C_DIRECTION_TX;
    pI2C_param->DeviceAddr    = I2C_DEVICE_ADDR;

    pI2C_param->SubAddr[0] = (addr>>8)&0xff;
    pI2C_param->SubAddr[1] = (addr)&0xff;
    pI2C_param->TxData = bytes; //(uint8_t *)(data); // (uint8_t *)(&data);
    pI2C_param->TxNumOfBytes = num<<1;
    pI2C_param->TX_I2C_Index = 0;
    pI2C_param->TX_Generate_stop = 1;
    pI2C_param->TX_addr          = I2C_IDLE_STATUS;

    ARC_I2C_Write(I2C_Sel, pI2C_param);

    //ARC_SysTick_Delay(20);
    while(pI2C_param->TX_addr != I2C_STOP_STATUS);
    pI2C_param->TX_addr = I2C_IDLE_STATUS;

    //ARC_I2C_DeInit();
#else
    uint16_t i;
    uint8_t bytes[2];

    ARC_CN1100_CS_LOW();
    ARC_SPI_SendByte(SPI1, FAST_WRITE_DATA);     // FAST Read
    ARC_SPI_SendByte(SPI1, (addr>>8)&0xff);     // ADDRESS 0x000000 // 16bits
    ARC_SPI_SendByte(SPI1, (addr)&0xff);        // ADDRESS 0x000000 // 16bits
    for(i=0; i<num; i++)
    {
        bytes[0] = (data[i]>>8)&0xff;
        bytes[1] = (data[i])&0xff;
        ARC_SPI_SendByte(SPI1, bytes[0]);
        ARC_SPI_SendByte(SPI1, bytes[1]);
    }
    ARC_CN1100_CS_HIGH(); 
#endif
}

/*******************************************************************************
* Function Name  : SPI_read_DATAs(addr, data)
* Description    : use to read a frame data
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
void SPI_read_DATAs(uint32_t addr, uint16_t num, uint16_t *data)
{
#ifdef CN1100_I2C_COMM
    uint8_t *bytes, btmp;
    uint16_t i;
    I2C_param_struct __IO *pI2C_param;

    //ARC_I2C_Init();
    pI2C_param = ARC_get_I2C_param();

    pI2C_param->I2C_DIRECTION    = ARC_I2C_DIRECTION_TX;
    pI2C_param->DeviceAddr       = I2C_DEVICE_ADDR;

    pI2C_param->SubAddr[0]       = (addr>>8)&0xff;
    pI2C_param->SubAddr[1]       = (addr)&0xff;
    pI2C_param->TxNumOfBytes     = 0;
    pI2C_param->TX_I2C_Index     = 0;
    pI2C_param->TX_Generate_stop = 0;
    pI2C_param->TX_addr          = I2C_IDLE_STATUS;

    pI2C_param->RxData           = (uint8_t *)data;
    pI2C_param->RxNumOfBytes     = num<<1;
    pI2C_param->RX_I2C_Index     = 0;

    ARC_I2C_Write(I2C_Sel, pI2C_param);

    //ARC_SysTick_Delay(20);
    while(pI2C_param->TX_addr != I2C_STOP_STATUS);
    pI2C_param->TX_addr = I2C_IDLE_STATUS;

    //ARC_I2C_DeInit();

    bytes = (uint8_t *)(data); // (uint8_t *)(&data);

    //***************************************
    // Big Endian and Little Endian issue
    //***************************************
    if(1)
        for(i=0; i<num; i++)
        {
            btmp            = bytes[i<<1];
            bytes[i<<1]     = bytes[1+(i<<1)];
            bytes[1+(i<<1)] = btmp;
        }

#else
    uint16_t i;

    ARC_CN1100_CS_LOW();
    ARC_SPI_SendByte(SPI1, FAST_READ_DATA);     // FAST Read
    ARC_SPI_SendByte(SPI1, (addr>>8)&0xff);     // ADDRESS 0x000000 // 16bits
    ARC_SPI_SendByte(SPI1, (addr)&0xff);        // ADDRESS 0x000000 // 16bits
    for(i=0; i<num; i++)
    {
        data[i]  = ((uint16_t)(ARC_SPI_SendByte(SPI1, FLASH_DUMMY_BYTE)))<<8;
        data[i] += (uint16_t)(ARC_SPI_SendByte(SPI1, FLASH_DUMMY_BYTE));
    }
    ARC_CN1100_CS_HIGH(); 
#endif
}

/*******************************************************************************
* Function Name  : 
* Description    : Useless Now 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
void SPI_write_STATUS(uint16_t data)
{
#ifdef CN1100_I2C_COMM
#else
    ARC_CN1100_CS_LOW();
    ARC_SPI_SendByte(SPI1, WRITE_STATUS);     // Write Status Regsiter
    ARC_SPI_SendByte(SPI1, (data>>8)&0xff);
    ARC_SPI_SendByte(SPI1, (data)&0xff);
    ARC_CN1100_CS_HIGH(); 
#endif
}

/*******************************************************************************
* Function Name  : 
* Description    : Useless Now 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
uint16_t SPI_read_STATUS(void)
{
#ifdef CN1100_I2C_COMM
    return 0;
#else
    uint8_t  bytes[2];
    uint16_t temp;

    ARC_CN1100_CS_LOW();
    ARC_SPI_SendByte(SPI1, READ_STATUS);     // Write Status Regsiter
    bytes[0] = ARC_SPI_SendByte(SPI1, FLASH_DUMMY_BYTE);
    bytes[1] = ARC_SPI_SendByte(SPI1, FLASH_DUMMY_BYTE);
    ARC_CN1100_CS_HIGH(); 
    temp  = bytes[0];
    temp  = temp<<8;
    temp += bytes[1]; 
    return temp;
#endif
}


#ifdef COMMUNICATION_WITH_PC


/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
#ifdef USB_COMMUNICATION
void USB_SendString(u8 *str)
{
    u8 i=0; 
    
    while(EP_TX_VALID == GetEPTxStatus(ENDP2)) ;
    
    for (i = 0; i < USB_REPORT_LEN;i++)
    {
        dbg.Transi_Buffer[i] = str[i];
    }
    UserToPMABufferCopy(dbg.Transi_Buffer, ENDP2_TXADDR, USB_REPORT_LEN);
    /* SetEPTxCount(ENDP2,64); */
    SetEPTxValid(ENDP2);
}



/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
void Usb_IRQHandler(void)
{
    switch(dbg.ReceiveBuf[0])
    {
        case STOP_DEBUG_INFO:
        {
            dbg.DebugInfoLevel   = DEBUG_INFO_NONE;
            USB_SendString(dbg.Transi_Buffer);
            break;
        }
        case START_DEBUG_INFO:
        case SHOW_DIFF_VALUE:
        {
            dbg.DebugInfoLevel   = DEBUG_INFO_DELTA;
            break;
        }
        case SHOW_ORIG_VALUE:
        {
            dbg.DebugInfoLevel   = DEBUG_INFO_BASE;
            break;
        }
        case SHOW_BASE_VALUE:
        {
            dbg.DebugInfoLevel   = DEBUG_INFO_ORG;
            break;
        }
        case SHOW_DEBUG_VALUE:
        {
            dbg.DebugInfoLevel   = DEBUG_INFO_DBG;
            break;
        }
        case SHOW_FIG_LOC_ONLY:
        {
            dbg.DebugInfoLevel   = DEBUG_INFO_FIGLOC;
            break;
        }
        case WRITE_REG_VALUE:
        case READ_REG_VALUE:
        case SET_STM32_PARA:
        case SHOW_STM32_PARA:
        {
            dbg.validCommand = USART_EXECUTED_CMD;
            break;
        }
        default:
            break;
    }
    dbg.validCommand = USART_EXECUTED_CMD;
    dbg.TimeCount = 0;
}

#else

/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
void USART1_IRQHandler_ISR(void)
{
    uint8_t tchar;
    
  /***********************************************************
    * 若接收数据寄存器满，Put the received data into tchar
    ********************************************************** */
    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) 
    {
        tchar = USART_ReceiveData(USART1);
    }
    
    /**********************************************************
    *validCommand: show the Command Status
    *               0: Receive Command; 
    *               1: Received Para; 
    *               2: Execute Command
    ********************************************************** */
    if(USART_RECEIVED_PARA == dbg.validCommand)
    {
        dbg.ReceiveBuf[dbg.UartParaNum] = tchar;
        dbg.UartParaNum += 1;
        
        if(dbg.ReceiveBuf[0] == READ_REG_VALUE)
        {
        #ifdef REG_MEM_16BITS_ADDR
        if(dbg.UartParaNum >= 3)
        #else
        if(dbg.UartParaNum >= 2)
        #endif
        {
            /************************************************************
            * READ REG Value: READ_CMD, REG_ADDR;
            ************************************************************ */
            dbg.validCommand = USART_EXECUTED_CMD;
        }
        }
        else if(WRITE_REG_VALUE == dbg.ReceiveBuf[0])
        {
            #ifdef REG_MEM_16BITS_ADDR
            if(dbg.UartParaNum >= 5)
            #else
            if(dbg.UartParaNum >= 4)
            #endif
            {
                /************************************************************
                * WRITE REG Value: WRITE_CMD, REG_ADDR, VAL_HIGH, VAL_LOW;
                ************************************************************ */
                dbg.validCommand = USART_EXECUTED_CMD;
            }
        }
        else if(SET_STM32_PARA == dbg.ReceiveBuf[0])
        {
            if(dbg.UartParaNum >= 93)
            {
                dbg.validCommand = USART_EXECUTED_CMD;
            }
        }
    } 
    else if(USART_RECEIVED_CMD == dbg.validCommand)
    {
        switch(tchar)
        {
            case STOP_DEBUG_INFO:
            {
                dbg.DebugInfoLevel   = DEBUG_INFO_NONE;
                dbg.validCommand     = USART_EXECUTED_CMD;
                dbg.ReceiveBuf[0]         = tchar;
                break;
            }
            case START_DEBUG_INFO:
            case SHOW_DIFF_VALUE:
            {
                dbg.DebugInfoLevel   = DEBUG_INFO_DELTA;
                dbg.validCommand     = USART_EXECUTED_CMD;
                dbg.ReceiveBuf[0]         = tchar;
                break;
            }
            case SHOW_ORIG_VALUE:
            {
                dbg.DebugInfoLevel   = DEBUG_INFO_BASE;
                dbg.validCommand     = USART_EXECUTED_CMD;
                dbg.ReceiveBuf[0]         = tchar;
                break;
            }
            case SHOW_BASE_VALUE:
            {
                dbg.DebugInfoLevel   = DEBUG_INFO_ORG;
                dbg.validCommand     = USART_EXECUTED_CMD;
                dbg.ReceiveBuf[0]         = tchar;
                break;
            }
            case SHOW_DEBUG_VALUE:
            {
                dbg.DebugInfoLevel   = DEBUG_INFO_DBG;
                dbg.validCommand     = USART_EXECUTED_CMD;
                dbg.ReceiveBuf[0]         = tchar;
                break;
            }
            case SHOW_FIG_LOC_ONLY:
            {
                dbg.DebugInfoLevel   = DEBUG_INFO_FIGLOC;
                dbg.validCommand     = USART_EXECUTED_CMD;
                dbg.ReceiveBuf[0]         = tchar;
                break;
            }
            case WRITE_REG_VALUE:
            case READ_REG_VALUE:
            case SET_STM32_PARA:
            {
                dbg.ReceiveBuf[0]         = tchar;
                dbg.UartParaNum      = 1;     /*  Paramemter Index */
                dbg.validCommand     = USART_RECEIVED_PARA;
                break;
            }
            case SHOW_STM32_PARA:
            {
                dbg.ReceiveBuf[0]         = tchar;
                dbg.validCommand     = USART_EXECUTED_CMD;	
                break;
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
#ifdef SHOW_EVERY_FRAME_DATA
void Print_FrameAndDelta_Data(uint16_t *buffer)
{
    uint16_t   i, j;
    
    /*************************************************************
     * SHOW all data of "[XMTR]" X "[RECV]"
    **************************************************************/
    /* CN1100_print("TX--------0-----1-----2-----3-----4-----5-----6-----7-----8-----9\r\n"); */
    CN1100_print("\r\n\n");
    for (i=0; i<XMTR_NUM; i++)
    {
        /* CN1100_print("O-%2d:  ", i); */
        for (j=0; j<RECV_NUM; j++)
        {
            CN1100_print("%d  ", buffer[i*RECV_NUM+j]);
        }
        
        #ifdef BASE_DATA_SHOW
        CN1100_print(" --|-- ");
        for (j=0; j<RECV_NUM; j++)
        {
            CN1100_print("%d  ", bdt.BFD.BaseDat[i][j]);
        }
        #endif
        CN1100_print("\r\n");
    }
    #if 0
    CN1100_print("TX-------0----1----2----3----4----5----6----7----8----9\r\n");
    for(i=0; i<XMTR_NUM; i++)
    {
    CN1100_print("D-%2d:  ", i);
    for(j=0; j<RECV_NUM; j++)
    {
    CN1100_print("%d  ", bdt.DeltaDat16A[i][j]);
    }
    CN1100_print("\r\n");
    }
    #endif
}
#endif




#endif


/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
void SendOutOneByte(uint8_t chardata) 
{
    #ifdef USB_COMMUNICATION
    u8 i;
    
    dbg.Transi_Buffer[dbg.TbufCount + dbg.RowCount*USB_REPORT_LEN] = chardata;
    if(USB_DATA_END == dbg.USB_dataflag)
    {
        dbg.TbufCount = 0;
        for (i = 0;i <= dbg.RowCount;i++)
        {
            USB_SendString(dbg.Transi_Buffer + i * USB_REPORT_LEN);
        }
        dbg.RowCount = 0;
    }
    else
    {
        dbg.TbufCount++;
        if(USB_REPORT_LEN == dbg.TbufCount)
        {
            dbg.TbufCount = 0;
            dbg.RowCount++;
            if(USB_ROW_MAX == dbg.RowCount)
            {
                for (i = 0;i <= dbg.RowCount;i++)
                {
                    USB_SendString(dbg.Transi_Buffer + i * USB_REPORT_LEN);
                }
                dbg.RowCount = 0;
            }
        }
    }   
    #else
    while (RESET == USART_GetFlagStatus(USART1, USART_FLAG_TC))
    {
    }
    USART_SendData(USART1, chardata);
    #endif
}



/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/

void SendOutApartBitmaps(void)
{
    uint8_t fnum = 0;
    uint16_t i,j;
    
    if(DEBUG_INFO_FIGLOC == dbg.DebugInfoLevel)
    {
        return;
    }
    
    for(i = 0;i < bdt.FingerDetectNum;i++)
    {
        if(bdt.DPD[i].Finger_X_XMTR || bdt.DPD[i].Finger_Y_RECV)
        {
            fnum++;
        }
    }

    SendOutOneByte(fnum);
    for(i = 0;i < fnum;i++)
    {
        for(j = 0;j < XMTR_NUM;j++)
        {
            SendOutOneByte((bdt.DPD[i].BitMapDat[j]>>24)&0xFF);
            SendOutOneByte((bdt.DPD[i].BitMapDat[j]>>16)&0xFF);
            SendOutOneByte((bdt.DPD[i].BitMapDat[j]>>8)&0xFF);
            SendOutOneByte(bdt.DPD[i].BitMapDat[j]&0xFF);
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

void Send_Buffer_DeltaData_To_Host(uint8_t *CRCT, uint16_t *buffer)
{
    uint16_t i, j, tempi;

    #ifdef PRESS_KEY_DETECT

    #if (KXMTR_NUM == 1)
    for (i=0; i<SXMTR_NUM; i++)
    {
        tempi = i*RECV_NUM;
        for (j=0; j<RECV_NUM; j++)
        {
            CRCT[1] = (uint8_t)(buffer[tempi+j]>>8);
            CRCT[0] ^= CRCT[1];
            SendOutOneByte(CRCT[1]);
            CRCT[1] = (uint8_t)(buffer[tempi+j]&0xff);
            CRCT[0] ^= CRCT[1];
            SendOutOneByte(CRCT[1]);
        }
    }
    
    for (j=0; j<RECV_NUM; j++)
    {
        CRCT[1] = (uint8_t)(bdt.DeltaDat_kp[j]>>8);
        CRCT[0] ^= CRCT[1];
        SendOutOneByte(CRCT[1]);
        CRCT[1] = (uint8_t)(bdt.DeltaDat_kp[j]&0xff);
        CRCT[0] ^= CRCT[1];
        SendOutOneByte(CRCT[1]);
    }
    #endif

    #if (KRECV_NUM == 1)
    for (i=0; i<SXMTR_NUM; i++)
    {
        tempi = i*RECV_NUM;
        for (j=0; j<(RECV_NUM-1); j++)
        {
            CRCT[1] = (uint8_t)(buffer[tempi+j]>>8);
            CRCT[0] ^= CRCT[1];
            SendOutOneByte(CRCT[1]);
            CRCT[1] = (uint8_t)(buffer[tempi+j]&0xff);
            CRCT[0] ^= CRCT[1];
            SendOutOneByte(CRCT[1]);
        }
#if 1 
        buffer[tempi+j] = (uint16_t)(bdt.DeltaDat_kp[i]);
            CRCT[1] = (uint8_t)(buffer[tempi+j]>>8);
            CRCT[0] ^= CRCT[1];
            SendOutOneByte(CRCT[1]);
            CRCT[1] = (uint8_t)(buffer[tempi+j]&0xff);
            CRCT[0] ^= CRCT[1];
            SendOutOneByte(CRCT[1]);
#else
        CRCT[1] = (uint8_t)(bdt.DeltaDat_kp[i]>>8);
        CRCT[0] ^= CRCT[1];
        SendOutOneByte(CRCT[1]);
        CRCT[1] = (uint8_t)(bdt.DeltaDat_kp[j]&0xff);
        CRCT[0] ^= CRCT[1];
        SendOutOneByte(CRCT[1]);
#endif
    }
    #endif

    #else

    for (i=0; i<XMTR_NUM; i++)
    {
        tempi = i*RECV_NUM;
        for (j=0; j<RECV_NUM; j++)
        { 
            CRCT[1] = (uint8_t)(buffer[tempi+j]>>8);
            CRCT[0] ^= CRCT[1];
            SendOutOneByte(CRCT[1]);
            CRCT[1] = (uint8_t)(buffer[tempi+j]&0xff);
            CRCT[0] ^= CRCT[1];
            SendOutOneByte(CRCT[1]);
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

void Send_Buffer_Data_To_Host(uint8_t *CRCT, uint16_t *buffer)
{
    uint16_t i, j, tempi;

    for (i=0; i<XMTR_NUM; i++)
    {
        tempi = i*RECV_NUM;
        for (j=0; j<RECV_NUM; j++)
        { 
            CRCT[1] = (uint8_t)(buffer[tempi+j]>>8);
            CRCT[0] ^= CRCT[1];
            SendOutOneByte(CRCT[1]);
            CRCT[1] = (uint8_t)(buffer[tempi+j]&0xff);
            CRCT[0] ^= CRCT[1];
            SendOutOneByte(CRCT[1]);
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
void Send_Delta_Data_To_PC(uint16_t *buf)
{
    uint8_t    CRCT[2];
    uint16_t   i, X, Y, *buffer;
    uint8_t    fnum, XH, XL, YH, YL;
    #ifdef USB_COMMUNICATION
    dbg.USB_dataflag = USB_DATA_BEGIN;
    #endif
    if(USART_EXECUTED_CMD == dbg.validCommand)
    {
        return;
    }
    /**************************************************************
    *  SHOW all data of "[XMTR]" X "[RECV]"
    **************************************************************/
    SendOutOneByte(0xFA);               /*  SYNC word */
    SendOutOneByte(0xF5);               /*  SYNC word */
    SendOutOneByte(0x00);               /*  SYNC word */
    SendOutOneByte(0x52);               /*  SYNC word */
    SendOutOneByte(dbg.DebugInfoLevel); /*  Tell PC which kind of data is send out */
    CRCT[0] = 0;
    if(dbg.DebugInfoLevel != DEBUG_INFO_FIGLOC)
    {
        SendOutOneByte(XMTR_NUM);           /*  TRANSMIT width */
        SendOutOneByte(RECV_NUM);           /*  Receiver width */
        
        if(DEBUG_INFO_DELTA == dbg.DebugInfoLevel)  
        {   
            buffer = (uint16_t *)(bdt.DeltaDat16A);
            Send_Buffer_DeltaData_To_Host(CRCT, buffer);
        }
        else if(DEBUG_INFO_DBG == dbg.DebugInfoLevel) 
        { 
            buffer = (uint16_t *)(bdt.SigDeltaDat);
            Send_Buffer_Data_To_Host(CRCT, buffer);
        }
        else if(DEBUG_INFO_BASE == dbg.DebugInfoLevel) 
        {
            buffer = (uint16_t *)(bdt.BFD.BaseDat);
            Send_Buffer_Data_To_Host(CRCT, buffer);
        }
        else if(DEBUG_INFO_ORG == dbg.DebugInfoLevel)
        {
            buffer = buf;
            Send_Buffer_Data_To_Host(CRCT, buffer);
        }
        SendOutOneByte(1);
        SendOutOneByte(((uint8_t)bdt.FRM_MAX_X_XMTR));
        SendOutOneByte(((uint8_t)bdt.FRM_MAX_Y_RECV));
        CRCT[0] ^= 1;
        CRCT[0] ^= ((uint8_t)bdt.FRM_MAX_X_XMTR);
        CRCT[0] ^= ((uint8_t)bdt.FRM_MAX_Y_RECV);
    }
    
   /****************************************************************
    * After sortfinger, maybe detected finger information is in 
    * bdt.DPD[1] when bdt.DPD[0] have nothing,
    * So, we send all the finger's information 
    **************************************************************** */
    fnum = FINGER_NUM_MAX;
    SendOutOneByte(fnum);
    CRCT[0] ^= fnum;
    
    for (i=0; i<fnum; i++)
    {
        X  = bdt.DPD[i].Finger_X_Reported; /*  X -> XTMR (800) */
        Y  = bdt.DPD[i].Finger_Y_Reported; /*  Y -> RECV (480) */
        
        X  = (uint16_t)(( ((uint32_t)X) * XMTR_SCALE )>>16);
        Y  = (uint16_t)(( ((uint32_t)Y) * RECV_SCALE )>>16);		
        
        #ifdef INVERSE_RX
        if(X||Y)
        {
        Y = SCREEN_WIDTH - Y;
        }
        #endif
        
        XH = (uint8_t)(X>>8);
        SendOutOneByte(XH);
        CRCT[0] ^= (XH);
        
        XL = (uint8_t)(X&0xff);
        SendOutOneByte(XL);
        CRCT[0] ^= (XL);
        
        YH = (uint8_t)(Y>>8);
        SendOutOneByte(YH);
        CRCT[0] ^= (YH);
        
        YL = (uint8_t)(Y&0xff);
        SendOutOneByte(YL);
        CRCT[0] ^= (YL);
    }
    
    SendOutOneByte(CRCT[0]);
    SendOutOneByte(bdt.PressKeyFlag1);
    
    /****************************************************************
    *  Send debug data	
   **************************************************************** */
    dbg.debug_value[0] = bdt.FingerDetectNum;
    dbg.debug_value[1] = bdt.ThresholdInFrame;
    dbg.debug_value[2] = bdt.BaseChangeFlag;
    dbg.debug_value[3] = (uint16_t)(( ((uint32_t)bdt.DPD[0].Finger_X_Reported) * XMTR_SCALE )>>16);
    dbg.debug_value[4] = (uint16_t)(( ((uint32_t)bdt.DPD[0].Finger_Y_Reported) * RECV_SCALE )>>16);

#ifdef SHOW_H_DATA
    dbg.debug_value[6] = bdt.Left_h;
    dbg.debug_value[7] = bdt.Right_h;
    dbg.debug_value[8] = bdt.Top_h;
    dbg.debug_value[9] = bdt.Bottom_h;
#else
    #ifdef AUTO_FACEDETECTION
    dbg.debug_value[5] = bdt.FDC.WFNum;
    dbg.debug_value[6] = bdt.FDC.BigNum;
    dbg.debug_value[7] = bdt.FDC.Flag;
    #else
    dbg.debug_value[5] = bdt.BFD.bbdc.BaseUpdateCase;
    #endif
#endif

    SendOutOneByte(dbg.debug_value_num);      /* send debug_value_num */
    for (i = 0;i < dbg.debug_value_num;i++)
    {
        SendOutOneByte((uint8_t)(dbg.debug_value[i]>>8));  /* send debug_value */
        SendOutOneByte(dbg.debug_value[i]&0xff);
    }
    
    SendOutOneByte(SIMULATE_DELTA_DISABLE);
    
    #ifdef SEND_FINGER_AREA_TO_PC
    SendOutApartBitmaps();
    #endif
    
    #ifdef USB_COMMUNICATION
    dbg.USB_dataflag = USB_DATA_END;
    #endif
    
    /****************************************************************
    *  Send END BYTE Finally
    *****************************************************************/
    SendOutOneByte(0xFF);
}



/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
void SendOut32bitdata(uint32_t data)
{
    SendOutOneByte(data>>24);
    SendOutOneByte((data>>16)&0xff);
    SendOutOneByte((data>>8)&0xff);
    SendOutOneByte(data&0xff);
}



/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
uint32_t Get32bitData(uint8_t *pdata)
{
    uint32_t result;
    result  = (uint32_t)(pdata[0]<<24);
    result |= (uint32_t)(pdata[1]<<16);
    result |= (uint32_t)(pdata[2]<<8);
    result |= (uint32_t)(pdata[3]);
    return result;
}



/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
void USART_COMM_Handle()
{
    uint16_t tshort,i;
    uint32_t addr;
    
    #ifdef REG_MEM_16BITS_ADDR
    dbg.RegisterAddr  = (uint16_t)(dbg.ReceiveBuf[1]);
    dbg.RegisterAddr |= (uint16_t)(dbg.ReceiveBuf[2]<<8);
    #else
    dbg.RegisterAddr  = (uint16_t)(dbg.ReceiveBuf[1]);
    #endif
    addr              = dbg.RegisterAddr;
    
    if(WRITE_REG_VALUE == dbg.ReceiveBuf[0])
    {
        /*******************************************
        * Write a Value into a Register
        ****************************************** */
        #ifdef REG_MEM_16BITS_ADDR
        dbg.RegisterVal  = ((uint16_t)(dbg.ReceiveBuf[3]))<<8;
        dbg.RegisterVal |= ((uint16_t)(dbg.ReceiveBuf[4]));
        #else
        dbg.RegisterVal  = ((uint16_t)(dbg.ReceiveBuf[2]))<<8;
        dbg.RegisterVal |= ((uint16_t)(dbg.ReceiveBuf[3]));
        #endif
        SPI_write_singleData(addr, dbg.RegisterVal);
        
        /* reinitialize baseline */
        bdt.BFD.InitCount = 0;
    }
    else if(READ_REG_VALUE == dbg.ReceiveBuf[0])
    {
        /*******************************************
        * Read a Value from a Register
        * Format: SYNC(0xFA-F5-00-53) DATAH DATAL
        ******************************************* */
        tshort = SPI_read_singleData(addr);

        #ifdef USB_COMMUNICATION
        dbg.USB_dataflag = USB_DATA_BEGIN;
        #endif

        for (i=0; i<10; i++)
        {
            SendOutOneByte(0xfa);
            SendOutOneByte(0xf5);
            SendOutOneByte(0x00);
            SendOutOneByte(0x53);
            SendOutOneByte((uint8_t)(tshort>>8));
            SendOutOneByte((uint8_t)(tshort&0xff));
        }

        #ifdef USB_COMMUNICATION
        dbg.USB_dataflag = USB_DATA_END;
        SendOutOneByte(0xff);
        #endif
    }
    else if(SHOW_STM32_PARA == dbg.ReceiveBuf[0])
    {
        #ifdef USB_COMMUNICATION
        dbg.USB_dataflag = USB_DATA_BEGIN;
        #endif

        #if 0
        /*send reply sync*/
        SendOutOneByte(0xfa);
        SendOutOneByte(0xf5);
        SendOutOneByte(0x00);
        SendOutOneByte(0x53);
        /*send Para*/
        SendOut32bitdata(bdt.PCBA.ProtectTime);
        SendOut32bitdata(bdt.PCBA.FrameMaxSample);
        SendOut32bitdata(bdt.PCBA.AbnormalMaxDiff);
        SendOut32bitdata(bdt.PCBA.AbnormalNumDiff);
        SendOut32bitdata(bdt.PCBA.FixPointMaxTime);
        SendOut32bitdata(bdt.PCBA.MaxUpdateTime);
        SendOut32bitdata(bdt.PCBA.HighRefSet);
        SendOut32bitdata(bdt.PCBA.HighRefPlSet);
        SendOut32bitdata(bdt.PCBA.LowRefSet);
        SendOut32bitdata(bdt.PCBA.LowRefPlSet);
        SendOut32bitdata(bdt.PCBA.HighRefGainSet);
        SendOut32bitdata(bdt.PCBA.LowRefGainSet);
        SendOut32bitdata(bdt.PCBA.TxPolarity);
        SendOut32bitdata(bdt.PCBA.FinAdjDisMin);
        SendOut32bitdata(bdt.PCBA.FinAdjDisMax);
        SendOut32bitdata(bdt.PCBA.MaxValueNoFinger);
        SendOut32bitdata(bdt.PCBA.FinThrMin);
        SendOut32bitdata(bdt.PCBA.FinAdjDisX);
        SendOut32bitdata(bdt.PCBA.RcvmRcvrFcapSet);
        SendOut32bitdata(bdt.PCBA.DurReset);
        SendOut32bitdata(bdt.PCBA.DurInteg);
        SendOut32bitdata(bdt.PCBA.DurStretch);
        SendOut32bitdata(bdt.PCBA.BurstCnt);
        #endif

        #ifdef USB_COMMUNICATION
        dbg.USB_dataflag = USB_DATA_END;
        SendOutOneByte(0xff);
        #endif
    }
    else if(SET_STM32_PARA == dbg.ReceiveBuf[0])
    {
        #ifdef USB_COMMUNICATION
        if( 0 == dbg.ReceiveBuf[1])
        {
            #if 0
            bdt.PCBA.ProtectTime           = Get32bitData(&dbg.ReceiveBuf[2]);
            bdt.PCBA.FrameMaxSample        = Get32bitData(&dbg.ReceiveBuf[6]);
            bdt.PCBA.AbnormalMaxDiff       = Get32bitData(&dbg.ReceiveBuf[10]);
            bdt.PCBA.AbnormalNumDiff       = Get32bitData(&dbg.ReceiveBuf[14]);
            bdt.PCBA.FixPointMaxTime       = Get32bitData(&dbg.ReceiveBuf[18]);
            bdt.PCBA.MaxUpdateTime         = Get32bitData(&dbg.ReceiveBuf[22]);
            bdt.PCBA.HighRefSet            = Get32bitData(&dbg.ReceiveBuf[26]);
            bdt.PCBA.HighRefPlSet          = Get32bitData(&dbg.ReceiveBuf[30]);
            bdt.PCBA.LowRefSet             = Get32bitData(&dbg.ReceiveBuf[34]);
            bdt.PCBA.LowRefPlSet           = Get32bitData(&dbg.ReceiveBuf[38]);
            bdt.PCBA.HighRefGainSet        = Get32bitData(&dbg.ReceiveBuf[42]);
            bdt.PCBA.LowRefGainSet         = Get32bitData(&dbg.ReceiveBuf[46]);
            bdt.PCBA.TxPolarity            = Get32bitData(&dbg.ReceiveBuf[50]);
            bdt.PCBA.FinAdjDisMin          = Get32bitData(&dbg.ReceiveBuf[54]);
            bdt.PCBA.FinAdjDisMax          = Get32bitData(&dbg.ReceiveBuf[58]);
            if(1 == bdt.PCBA.TxPolarity)
            {
                SPI_write_singleData(TPL1_REG, 0xffff); /*  cfg_reg30, 12'h000 */
            }
            else if(0 == bdt.PCBA.TxPolarity)
            {
                SPI_write_singleData(TPL1_REG, 0x0000); /*  cfg_reg30, 12'h000 */
            }
            //R02 -a
            RegTab_t.Reg28BitDef_t.RefhRegConf = 0;
            RegTab_t.Reg28BitDef_t.RcvmReg_t.REFH_REFHI_EN = 1;
            RegTab_t.Reg28BitDef_t.RcvmReg_t.REFH_REFHI_INP= 1;
            high  = RegTab_t.Reg28BitDef_t.RefhRegConf;

            RegTab_t.Reg29BitDef_t.ReflRegConf = 0;
            RegTab_t.Reg29BitDef_t.RcvmReg_t.REFL_REFLO_EN = 1;
            RegTab_t.Reg29BitDef_t.RcvmReg_t.REFL_REFLO_INP = 1;
            low   =  RegTab_t.Reg29BitDef_t.ReflRegConf;

            RegTab_t.Reg28BitDef_t.RcvmReg_t.REFH_REFHI_EN = 0;
            RegTab_t.Reg28BitDef_t.RcvmReg_t.REFH_REFHI_INP= 0;
            RegTab_t.Reg28BitDef_t.RcvmReg_t.REFH_REFHI_TCAP = bdt.PCBA.HighRefSet;
            RegTab_t.Reg28BitDef_t.RcvmReg_t.REFH_REFHI_FCAP =	bdt.PCBA.HighRefGainSet;
            high |= RegTab_t.Reg28BitDef_t.RefhRegConf; /*  Pos1PF; */

            RegTab_t.Reg29BitDef_t.RcvmReg_t.REFL_REFLO_EN = 0;
            RegTab_t.Reg29BitDef_t.RcvmReg_t.REFL_REFLO_INP = 0;
            RegTab_t.Reg29BitDef_t.RcvmReg_t.REFL_REFLO_TCAP = bdt.PCBA.LowRefSet;
            RegTab_t.Reg29BitDef_t.RcvmReg_t.REFL_REFLO_FCAP = bdt.PCBA.LowRefGainSet;
            low  |= RegTab_t.Reg29BitDef_t.ReflRegConf;  /*  Neg2PF; */
            //R02 -e
            high |= (bdt.PCBA.HighRefPlSet<<5);
            low  |= (bdt.PCBA.LowRefPlSet<<5);
            SPI_write_singleData(REFH_REG, high);
            SPI_write_singleData(REFL_REG, low); 
            /* reinitialize baseline */
            bdt.BFD.InitCount = 0;
            USB_SendString(dbg.Transi_Buffer);
            #endif
        }
        else if(1 == dbg.ReceiveBuf[1])
        {
            #if 0
            bdt.PCBA.MaxValueNoFinger      = Get32bitData(&dbg.ReceiveBuf[2]);
            bdt.PCBA.FinThrMin             = Get32bitData(&dbg.ReceiveBuf[6]);
            bdt.PCBA.FinAdjDisX            = Get32bitData(&dbg.ReceiveBuf[10]);
            bdt.PCBA.RcvmRcvrFcapSet       = Get32bitData(&dbg.ReceiveBuf[14]);
            bdt.PCBA.DurReset              = Get32bitData(&dbg.ReceiveBuf[18]);
            bdt.PCBA.DurInteg              = Get32bitData(&dbg.ReceiveBuf[22]);
            bdt.PCBA.DurStretch            = Get32bitData(&dbg.ReceiveBuf[26]);
            bdt.PCBA.BurstCnt              = Get32bitData(&dbg.ReceiveBuf[30]);
            //R02 -a
            RegTab_t.Reg27BitDef_t.RcvmRegConf = 0;
            RegTab_t.Reg27BitDef_t.RcvmReg_t.RCVM_RJCT_EN = 1;
            RegTab_t.Reg27BitDef_t.RcvmReg_t.RCVM_RCVR_TURBO_EN1 =1;
            RegTab_t.Reg27BitDef_t.RcvmReg_t.RCVM_CHAN_RST_EN = 1;
            RegTab_t.Reg27BitDef_t.RcvmReg_t.RCVM_RCVR_FCAP = bdt.PCBA.RcvmRcvrFcapSet;
            RegTab_t.Reg27BitDef_t.RcvmReg_t.RCVM_FILT_BW = 3;
            SPI_write_singleData(RCVM_REG,RegTab_t.Reg27BitDef_t.RcvmRegConf);


            RegTab_t.Reg24BitDef_t.DurvRegConf = 0;
            RegTab_t.Reg24BitDef_t.DurvReg_t.DURV_RESET_DUR = bdt.PCBA.DurReset;
            RegTab_t.Reg24BitDef_t.DurvReg_t.DURV_INTEG_DUR = bdt.PCBA.DurInteg;
            SPI_write_singleData(DURV_REG,RegTab_t.Reg24BitDef_t.DurvRegConf);/*  cfg_reg24, 12'h144 */

            RegTab_t.Reg25BitDef_t.DursRegConf = 0;
            RegTab_t.Reg25BitDef_t.DursReg_t.DURS_STRETCH_DUR = bdt.PCBA.DurStretch;
            RegTab_t.Reg25BitDef_t.DursReg_t.DURS_STRETCH_INC = STRETCH_INC_REG25;	
            SPI_write_singleData(DURS_REG,RegTab_t.Reg25BitDef_t.DursRegConf);  /*  cfg_reg25, 12'h000 */

            RegTab_t.Reg26BitDef_t.BcntRegConf = 0;
            RegTab_t.Reg26BitDef_t.BcntReg_t.BCNT_BURST_CNT = bdt.PCBA.BurstCnt;
            SPI_write_singleData(BCNT_REG,RegTab_t.Reg26BitDef_t.BcntRegConf); /*  cfg_reg26, 12'h02f */
            //R02 -e
            /* reinitialize baseline */
            bdt.BFD.InitCount = 0;
            #endif
        }
        #else

        #if 0
        bdt.PCBA.ProtectTime           = Get32bitData(&dbg.ReceiveBuf[1]);
        bdt.PCBA.FrameMaxSample        = Get32bitData(&dbg.ReceiveBuf[5]);
        bdt.PCBA.AbnormalMaxDiff       = Get32bitData(&dbg.ReceiveBuf[9]);
        bdt.PCBA.AbnormalNumDiff       = Get32bitData(&dbg.ReceiveBuf[13]);
        bdt.PCBA.FixPointMaxTime       = Get32bitData(&dbg.ReceiveBuf[17]);
        bdt.PCBA.MaxUpdateTime         = Get32bitData(&dbg.ReceiveBuf[21]);
        bdt.PCBA.HighRefSet            = Get32bitData(&dbg.ReceiveBuf[25]);
        bdt.PCBA.HighRefPlSet          = Get32bitData(&dbg.ReceiveBuf[29]);
        bdt.PCBA.LowRefSet             = Get32bitData(&dbg.ReceiveBuf[33]);
        bdt.PCBA.LowRefPlSet           = Get32bitData(&dbg.ReceiveBuf[37]);
        bdt.PCBA.HighRefGainSet        = Get32bitData(&dbg.ReceiveBuf[41]);
        bdt.PCBA.LowRefGainSet         = Get32bitData(&dbg.ReceiveBuf[45]);
        bdt.PCBA.TxPolarity            = Get32bitData(&dbg.ReceiveBuf[49]);
        bdt.PCBA.FinAdjDisMin          = Get32bitData(&dbg.ReceiveBuf[53]);
        bdt.PCBA.FinAdjDisMax          = Get32bitData(&dbg.ReceiveBuf[57]);
        bdt.PCBA.MaxValueNoFinger      = Get32bitData(&dbg.ReceiveBuf[61]);
        bdt.PCBA.FinThrMin             = Get32bitData(&dbg.ReceiveBuf[65]);
        bdt.PCBA.FinAdjDisX            = Get32bitData(&dbg.ReceiveBuf[69]);
        bdt.PCBA.RcvmRcvrFcapSet       = Get32bitData(&dbg.ReceiveBuf[73]);
        bdt.PCBA.DurReset              = Get32bitData(&dbg.ReceiveBuf[77]);
        bdt.PCBA.DurInteg              = Get32bitData(&dbg.ReceiveBuf[81]);
        bdt.PCBA.DurStretch            = Get32bitData(&dbg.ReceiveBuf[85]);
        bdt.PCBA.BurstCnt              = Get32bitData(&dbg.ReceiveBuf[89]);
        if(1 == bdt.PCBA.TxPolarity)
        {
            SPI_write_singleData(TPL1_REG, 0xffff); /*  cfg_reg30, 12'h000 */
        }else if(0 == bdt.PCBA.TxPolarity)
        {
            SPI_write_singleData(TPL1_REG, 0x0000); /*  cfg_reg30, 12'h000 */
        }
        //R02 -a
        RegTab_t.Reg28BitDef_t.RefhRegConf = 0;
        RegTab_t.Reg28BitDef_t.RcvmReg_t.REFH_REFHI_EN = 1;
        RegTab_t.Reg28BitDef_t.RcvmReg_t.REFH_REFHI_INP= 1;
        high  = RegTab_t.Reg28BitDef_t.RefhRegConf;

        RegTab_t.Reg29BitDef_t.ReflRegConf = 0;
        RegTab_t.Reg29BitDef_t.RcvmReg_t.REFL_REFLO_EN = 1;
        RegTab_t.Reg29BitDef_t.RcvmReg_t.REFL_REFLO_INP = 1;
        low   = RegTab_t.Reg29BitDef_t.ReflRegConf;


        RegTab_t.Reg28BitDef_t.RcvmReg_t.REFH_REFHI_EN = 0;
        RegTab_t.Reg28BitDef_t.RcvmReg_t.REFH_REFHI_INP= 0;
        RegTab_t.Reg28BitDef_t.RcvmReg_t.REFH_REFHI_TCAP = bdt.PCBA.HighRefSet;
        RegTab_t.Reg28BitDef_t.RcvmReg_t.REFH_REFHI_FCAP= bdt.PCBA.HighRefGainSet;
        high |= RegTab_t.Reg28BitDef_t.RefhRegConf; /*  Pos1PF; */


        RegTab_t.Reg29BitDef_t.RcvmReg_t.REFL_REFLO_EN = 0;
        RegTab_t.Reg29BitDef_t.RcvmReg_t.REFL_REFLO_INP = 0;
        RegTab_t.Reg29BitDef_t.RcvmReg_t.REFL_REFLO_TCAP = bdt.PCBA.LowRefSet;
        RegTab_t.Reg29BitDef_t.RcvmReg_t.REFL_REFLO_FCAP = bdt.PCBA.LowRefGainSet;
        low  |= RegTab_t.Reg29BitDef_t.ReflRegConf;  /*  Neg2PF; */
        //R02 -e
        high |= (bdt.PCBA.HighRefPlSet<<5);
        low  |= (bdt.PCBA.LowRefPlSet<<5);
        SPI_write_singleData(REFH_REG, high);
        SPI_write_singleData(REFL_REG, low); 
        //R02 -a
        RegTab_t.Reg27BitDef_t.RcvmRegConf = 0;
        RegTab_t.Reg27BitDef_t.RcvmReg_t.RCVM_RJCT_EN = 1;
        RegTab_t.Reg27BitDef_t.RcvmReg_t.RCVM_RCVR_TURBO_EN1 =1;
        RegTab_t.Reg27BitDef_t.RcvmReg_t.RCVM_CHAN_RST_EN = 1;
        RegTab_t.Reg27BitDef_t.RcvmReg_t.RCVM_RCVR_FCAP = bdt.PCBA.RcvmRcvrFcapSet;
        RegTab_t.Reg27BitDef_t.RcvmReg_t.RCVM_FILT_BW = 3;
        SPI_write_singleData(RCVM_REG, RegTab_t.Reg27BitDef_t.RcvmRegConf);


        RegTab_t.Reg24BitDef_t.DurvRegConf = 0;
        RegTab_t.Reg24BitDef_t.DurvReg_t.DURV_RESET_DUR = bdt.PCBA.DurReset;
        RegTab_t.Reg24BitDef_t.DurvReg_t.DURV_INTEG_DUR = bdt.PCBA.DurInteg;					
        SPI_write_singleData(DURV_REG,RegTab_t.Reg24BitDef_t.DurvRegConf);  /*  cfg_reg24, 12'h144 */

        RegTab_t.Reg25BitDef_t.DursRegConf = 0;
        RegTab_t.Reg25BitDef_t.DursReg_t.DURS_STRETCH_DUR = bdt.PCBA.DurStretch;
        RegTab_t.Reg25BitDef_t.DursReg_t.DURS_STRETCH_INC = STRETCH_INC_REG25;	
        SPI_write_singleData(DURS_REG, RegTab_t.Reg25BitDef_t.DursRegConf);  /*  cfg_reg25, 12'h000 */

        RegTab_t.Reg26BitDef_t.BcntRegConf = 0;
        RegTab_t.Reg26BitDef_t.BcntReg_t.BCNT_BURST_CNT = bdt.PCBA.BurstCnt;
        SPI_write_singleData(BCNT_REG,RegTab_t.Reg26BitDef_t.BcntRegConf); /*  cfg_reg26, 12'h02f */
        //R02 -e
        /* reinitialize baseline */
        bdt.BFD.InitCount = 0;
        #endif
        #endif
    }
    else if(STOP_DEBUG_INFO == dbg.ReceiveBuf[0])
    {
        SPI_write_singleData(PERD_REG, ISCANMODE_PERD_REG_VALUE); 
    }
    else if(START_DEBUG_INFO == dbg.ReceiveBuf[0] 
            || (dbg.ReceiveBuf[0] >= SHOW_DIFF_VALUE && dbg.ReceiveBuf[0] <= SHOW_DEBUG_VALUE))
    {
        SPI_write_singleData(PERD_REG, ISCANMODE_PERD_REG_VALUE_NORMAL);
    }
    else if(SHOW_FIG_LOC_ONLY == dbg.ReceiveBuf[0])
    {
        SPI_write_singleData(PERD_REG, ISCANMODE_PERD_REG_VALUE_FINGER_ONLY);
    }
}





#endif

#else  // CN1100_STM32

#if(CN1100_LINUX_SPI)
void cn1100_complete(void *arg)
{
    complete(arg);
}

ssize_t cn1100_sync(struct spi_message *message)
{
    DECLARE_COMPLETION_ONSTACK(done);
    int status;

    message->complete = cn1100_complete;
    message->context = &done;
    if (spidev->spi == NULL)
        status = -ESHUTDOWN;
    else
        status = spi_async(spidev->spi, message);
    if (status == 0) {
        wait_for_completion(&done);
        status = message->status;
        if (status == 0)
            status = message->actual_length;
    }
    return status;
}

ssize_t cn1100_transfer(const uint8_t *tx,size_t count,uint8_t *rx)
{
    struct spi_transfer     t = { 
        .tx_buf     = tx,
        .rx_buf     = rx,
        .len        = count,
        .speed_hz   = SPI_HIGH_SPEED,
    };
    struct spi_message      m;
    spi_message_init(&m);
    spi_message_add_tail(&t, &m);
    return cn1100_sync(&m);
}
#endif
uint16_t SPI_read_chipID(void)
{
    uint16_t chipid = 0;
    uint8_t rx[8] = {0};
#if(CN1100_LINUX_SPI)
    uint8_t tmp[] = {
        READ_CHIP_ID,SPI_DUMMY_BYTE,SPI_DUMMY_BYTE,
    };
#elif(CN1100_LINUX_I2C)
    struct i2c_msg *msgs;
    int status = 0;
    uint8_t tmp[] = {
        0x0,
    };
#endif

#if(CN1100_LINUX_SPI)
    cn1100_transfer(tmp,ARRAY_SIZE(tmp),rx);

    chipid = rx[1] << 8;
    chipid += rx[2];
#elif(CN1100_LINUX_I2C)
    msgs = kmalloc(sizeof(struct i2c_msg)*I2C_MSGS,GFP_KERNEL);
    msgs[0].addr = CN1100_DEVICE_ADDR;
    msgs[0].flags = 0;
    msgs[0].buf = tmp;
    msgs[0].len = 1;
#ifdef CN1100_RK3026
    msgs[0].scl_rate=400*1000;
#endif

    msgs[1].addr = CN1100_DEVICE_ADDR;
    msgs[1].flags = I2C_M_RD;
    msgs[1].buf = rx;
    msgs[1].len = 2;
#ifdef CN1100_RK3026
    msgs[1].scl_rate=400*1000;
#endif

    status = i2c_transfer(spidev->client->adapter,msgs,2);
    if(status < 0){
        CN1100_print("failed to send i2c message\n");
        return -1;
    }
    chipid = rx[1] << 8;
    chipid += rx[2];
    kfree(msgs);
#endif
    return chipid;
}

void SPI_write_singleData(uint32_t addr, uint16_t data)
{
#if(CN1100_LINUX_SPI)
    uint8_t rx[8] = {0};
    uint8_t tmp[] = {
        FAST_WRITE_DATA,((addr>>8)&0xff),addr&0xff,((data>>8)&0xff),data&0xff,
    };
#elif(CN1100_LINUX_I2C)
#ifdef TC2126_TEST
    uint8_t tmp[] = {
        ((addr>>24)&0xff),((addr>>16)&0xff),((addr>>8)&0xff),((addr)&0xff),((data>>8)&0xff),data&0xff,
    };
#else
    uint8_t tmp[] = {
        ((addr>>8)&0xff),addr&0xff,((data>>8)&0xff),data&0xff,
    };
#endif

    struct i2c_msg msg;
    int status = 0;
#endif
#if(CN1100_LINUX_SPI)
    cn1100_transfer(tmp,ARRAY_SIZE(tmp),rx);
#elif(CN1100_LINUX_I2C)
    msg.addr = CN1100_DEVICE_ADDR;
    msg.flags = 0;
    msg.buf = tmp;
    msg.len = ARRAY_SIZE(tmp);
#ifdef CN1100_RK3026
    msg.scl_rate=400*1000;
#endif

    if(!spidev->client->adapter){
        CN1100_print("cannot get i2c adapter\n");
        return;
    }
    status = i2c_transfer(spidev->client->adapter,&msg,1);
    if(status < 0){
        CN1100_print("failed to send i2c message\n");
        return;
    }
#endif
}

uint16_t SPI_read_singleData(uint32_t addr)
{
    /*Test to see if sth wrong,change FAST_READ_DATA to READ_CHIP_ID*/
    uint16_t ret = 0;
#if(CN1100_LINUX_SPI)
    uint8_t tmp[] = {
        FAST_READ_DATA,((addr>>8)&0xff),addr&0xff,SPI_DUMMY_BYTE,SPI_DUMMY_BYTE,
    };
#elif(CN1100_LINUX_I2C)
    uint8_t tmp[] = {
        ((addr>>8)&0xff),addr&0xff,
    };
    struct i2c_msg *msg;
    int status = 0;
#endif
    uint8_t rx[8] = {0};
#if(CN1100_LINUX_SPI)
    ret = cn1100_transfer(tmp,ARRAY_SIZE(tmp),rx);
    if(ret < 0){
        CN1100_print("SPI transfer error!!\n");
        goto fail1;
    }
    ret = rx[3] << 8;
    ret += rx[4];
#elif(CN1100_LINUX_I2C)
    msg = kmalloc(sizeof(struct i2c_msg)*I2C_MSGS,GFP_KERNEL);
    msg[0].addr = CN1100_DEVICE_ADDR;
    msg[0].flags = 0;
    msg[0].buf = tmp;
    msg[0].len = ARRAY_SIZE(tmp);
#ifdef CN1100_RK3026
    msg[0].scl_rate=400*1000;
#endif

    msg[1].addr = CN1100_DEVICE_ADDR;
    msg[1].flags =  I2C_M_RD;
    msg[1].buf = rx;
    msg[1].len = 2; 
#ifdef CN1100_RK3026
    msg[1].scl_rate=400*1000;
#endif

    if(!spidev->client){
        CN1100_print("cannot get i2c adapter\n");
        goto fail1;
    }
    status = i2c_transfer(spidev->client->adapter,msg,2);
    if(status < 0){
        CN1100_print("failed to send i2c message\n");
        goto fail1;
    }
    ret = rx[0] << 8;
    ret +=rx[1];
    kfree(msg);
#endif
    return ret;
fail1:
    return 0;
}

void SPI_write_DATAs(uint32_t addr, uint16_t num, uint16_t *data)
{
    uint32_t i =0 ,count = 0,j = 0;
#if(CN1100_LINUX_SPI)
    uint8_t tmp[] = {
        FAST_WRITE_DATA,((addr>>8)&0xff),(addr&0xff),
    };
    uint8_t rx[512] = {0};
#elif(CN1100_LINUX_I2C)
    int status = 0;
    uint8_t tmp[] = {
        ((addr>>8)&0xff),(addr&0xff),
    };
    struct i2c_msg msg;
#endif
    uint8_t tx[512] = {0};
    for(i=0;i<ARRAY_SIZE(tmp);i++){
        tx[i] = tmp[i];
    }

    for(j=0;j<num;j++){
        tx[i++] = ((data[j]>>8)&0xff);
        tx[i++] = (data[j])&0xff;
    }
    count = i;
#if(CN1100_LINUX_SPI)
    cn1100_transfer(tx,count,rx);
#elif(CN1100_LINUX_I2C)
    msg.addr = CN1100_DEVICE_ADDR;
    msg.flags = 0;
    msg.buf = tx;
    msg.len = count;
#ifdef CN1100_RK3026
    msg.scl_rate=400*1000;
#endif

    status = i2c_transfer(spidev->client->adapter,&msg,1);	
    if(status < 0){
        CN1100_print("failed to send i2c message\n");
        return;
    }
#endif
    return;
}

void SPI_read_DATAs(uint32_t addr, uint16_t num, uint16_t *data)
{
    uint32_t i = 0,j = 0;

    uint8_t rx[512] = {0};
#if(CN1100_LINUX_SPI)
    uint8_t tx[512] = {0};
    uint32_t count = 0;
    uint8_t tmp[] = {
        FAST_READ_DATA,(addr>>8)&0xff,(addr)&0xff,
    };
#elif(CN1100_LINUX_I2C)
    uint8_t tmp[] = {
        (addr>>8)&0xff,(addr)&0xff,
    };
    struct i2c_msg *msgs;
    int status = 0;
#endif

#if(CN1100_LINUX_SPI)
    for(i=0;i<ARRAY_SIZE(tmp);i++){
        tx[i] = tmp[i];
    }
    for(j=0;j<2*num;j++){
        tx[i++] = SPI_DUMMY_BYTE;
    }

    count = i;
#endif
#if(CN1100_LINUX_SPI)
    cn1100_transfer(tx,count,rx);
    j=3;
    for(i=0;i<num;i++){
        data[i] = (uint16_t)rx[j++] << 8;
        data[i] += (uint16_t)rx[j++];
    }
#elif(CN1100_LINUX_I2C)
    msgs = kmalloc(sizeof(struct i2c_msg)*I2C_MSGS,GFP_KERNEL);

    msgs[0].addr = CN1100_DEVICE_ADDR;
    msgs[0].flags = 0;
    msgs[0].buf = tmp;
    msgs[0].len = ARRAY_SIZE(tmp);
#ifdef CN1100_RK3026
    msgs[0].scl_rate=400*1000;
#endif

    msgs[1].addr = CN1100_DEVICE_ADDR;
    msgs[1].flags =  I2C_M_RD;
    msgs[1].buf = rx;
    msgs[1].len = 2*num;
#ifdef CN1100_RK3026
    msgs[1].scl_rate=400*1000;
#endif

    if(!spidev->client->adapter){
        CN1100_print("cannot get i2c adapter\n");
        return;
    }
    status = i2c_transfer(spidev->client->adapter,msgs,2);
    if(status < 0){
        CN1100_print("failed to send i2c message\n");
        return;
    }
    j = 0;
    for(i=0;i<num;i++){
        data[i] = (uint16_t)rx[j++]<<8;
        data[i] += (uint16_t)rx[j++];
    }
    kfree(msgs);
#endif
}

void SPI_write_STATUS(uint16_t data)
{
    uint8_t tmp[] = {
        WRITE_STATUS,(data>>8)&0xff,data&0xff,
    };
#if(CN1100_LINUX_I2C)
    struct i2c_msg msg;
    int status = 0;
#endif
#if(CN1100_LINUX_SPI)
    uint8_t rx[8] = {0};
    cn1100_transfer(tmp, ARRAY_SIZE(tmp), rx);
#elif(CN1100_LINUX_I2C)
    msg.addr = CN1100_DEVICE_ADDR;
    msg.flags = 0;
    msg.buf = tmp;
    msg.len = ARRAY_SIZE(tmp);
#ifdef CN1100_RK3026
    msg.scl_rate=400*1000;
#endif

    status = i2c_transfer(spidev->client->adapter,&msg,1);
    if(status < 0){
        CN1100_print("failed to send i2c message\n");
        return;
    }
#endif
}

uint16_t SPI_read_STATUS(void)
{
    uint8_t rx[8] = {0};
#if(CN1100_LINUX_SPI)
    uint8_t tmp[] = {
    READ_STATUS,SPI_DUMMY_BYTE,SPI_DUMMY_BYTE,
    };
#elif(CN1100_LINUX_I2C)
    struct i2c_msg *msg;
    int status = 0;
    uint8_t tmp[] = {
    READ_STATUS,
    };
#endif
    uint16_t temp = 0;
#if(CN1100_LINUX_SPI)
    cn1100_transfer(tmp,ARRAY_SIZE(tmp),rx);
    temp  = rx[1] << 8;
    temp += rx[2];
#elif(CN1100_LINUX_I2C)
    msg = kmalloc(sizeof(struct i2c_msg)*I2C_MSGS,GFP_KERNEL);

    msg[0].addr = CN1100_DEVICE_ADDR;
    msg[0].flags = 0;
    msg[0].buf = tmp;
    msg[0].len = 1;
#ifdef CN1100_RK3026
    msg[0].scl_rate=400*1000;
#endif

    msg[1].addr = CN1100_DEVICE_ADDR;
    msg[1].flags = I2C_M_RD;
    msg[1].buf = rx;
    msg[1].len = 2;
#ifdef CN1100_RK3026
    msg[1].scl_rate=400*1000;
#endif
    status = i2c_transfer(spidev->client->adapter,msg,2);
    if(status < 0){
        CN1100_print("failed to send i2c message\n");
        return -1;
    }

    temp = rx[0] << 8;
    temp += rx[1];
    kfree(msg);
#endif
    return temp;
}
#endif //CN1100_LINUX




#endif //
