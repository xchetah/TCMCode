#ifndef TC1126_LCD_H
#define  TC1126_LCD_H

#ifdef STM32VC_LCD
/* 宏定义 */

#define   LCD_WR(val)   *(__IO  uint16_t *)(( uint32_t )0x60000000)=val //写指令
#define   LCD_WD(val)   *(__IO  uint16_t *)(( uint32_t )0x60020000)=val	//写数据
#define   LCD_RD()	    *(__IO  uint16_t *)(( uint32_t )0x60020000)//读数据

extern void FSMC_LCD_Init(void);							    //FSMC接口初始化
extern void CN1100_LCD_init(void);
extern void CN1100_LCD_test(void);
extern void LCD_Show_STM32(uint16_t *buffer);
extern void LCD_PointFill(uint16_t x1, uint16_t y1, uint16_t color);
extern void TC1126_Key1EventModeSelect_ISR(void);
extern void TC1126_Key2EventModeSelect_ISR(void);
extern void LCD_Show_Inc_info(void);
extern void LCD_DisplayLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t Color);
extern void TFT_ShowNum(unsigned int x,unsigned int y,unsigned long num,uint16_t fColor, uint16_t bColor);

#endif

#endif
