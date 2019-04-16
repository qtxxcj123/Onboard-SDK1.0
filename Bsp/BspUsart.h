#ifndef BSP_USART_H
#define BSP_USART_H
#include "stm32f4xx_usart.h"
#include "stdint.h"
#include <stdbool.h>
#include "Vehicle.h"
#define _USART3_IF 1

void USART3_Config(uint32_t baudrate);
static void USART3_GPIO_Config(void);
void SystickConfig(void);
uint64_t getTimeStamp(void);

#endif

