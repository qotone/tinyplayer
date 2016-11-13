
#ifndef __HW_CONFIG_H
#define __HW_CONFIG_H

#include "stm32f10x.h"
#include "platform_config.h"

#define SYSTEM_US_TICKS  (SystemCoreClock / 1000000)
// OLED apis
#define OLED_CS_HIGH()  GPIO_SetBits(OLED_CS_GPIO_PORT, OLED_CS_GPIO_PIN)


#define OLED_CS_LOW()  GPIO_ResetBits(OLED_CS_GPIO_PORT, OLED_CS_GPIO_PIN)

#define OLED_DC_LOW()  GPIO_ResetBits(OLED_DC_GPIO_PORT,OLED_DC_GPIO_PIN)

#define OLED_DC_HIGH()  GPIO_SetBits(OLED_DC_GPIO_PORT,OLED_DC_GPIO_PIN)

#define OLED_RST_HIGH()  GPIO_SetBits(OLED_RST_GPIO_PORT,OLED_RST_GPIO_PIN)


#define OLED_RST_LOW()   GPIO_ResetBits(OLED_RST_GPIO_PORT, OLED_RST_GPIO_PIN)

uint8_t OLED_SPI_Write(uint8_t data);
void OLED_IO_Init(void);

// nvic configuration
void NVIC_Configuration(void);
void SysTick_Configuration(void);
void Set_System(void);

void BUTTON_ResetDebouncedState(Button_TypeDef);
uint16_t BUTTON_GetDebouncedTime(Button_TypeDef);
uint8_t BUTTON_GetSate(Button_TypeDef);
void BUTTON_EXTI_Config(Button_TypeDef,FunctionalState);
void BUTTON_Init(Button_TypeDef,ButtonMode_TypeDef);
#endif
