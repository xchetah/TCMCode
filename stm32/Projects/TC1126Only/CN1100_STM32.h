#ifndef CN1000_STM32_H
#define  CN1000_STM32_H

#ifdef CN1100_STM32

#include "stm32f10x.h"
#include "ARC_SPI_Flash.h"
#include "ARC_USART.h"
#include "ARC_LCD.h"
#include "ARC_SysTick.h"
#include "ARC_RCC.h"
#include "ARC_GPIO.h"
#include "ARC_NVIC_API.h"
#include "ARC_EXTI.h"
#include "ARC_I2C.h"
#include "stdio.h"

#ifdef USB_COMMUNICATION
#include "usb_lib.h"
#include "hw_config.h"
#include "stm32f10x_it.h"
#endif


#ifdef CN1100_I2C_COMM
#include "ARC_I2C.h"
#endif

#endif

#endif
