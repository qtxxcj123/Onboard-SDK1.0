#ifndef _MY_USART_H
#define _MY_USART_H
#include "stm32f4xx.h"
#define IAP

#ifdef __cpulspuls
extern "C"{
#endif

#define _PRINTF_IF     1  /*< is support printf !>*/

#define _USART1_IF     1
#define _USART2_IF     0
#define _USART3_IF     0
#define _UART4_IF      0
#define _UART5_IF      0
#define _USART6_IF     0

#define _USART1_IRQ_IF _USART1_IF
#define _USART2_IRQ_IF _USART2_IF
#define _USART3_IRQ_IF _USART3_IF
#define _UART4_IRQ_IF  _UART4_IF
#define _UART5_IRQ_IF  _UART5_IF
#define _USART6_IRQ_IF _USART6_IF

#define _NVIC_IF       1

#define BAUD_RATE_9600   9600
#define BAUD_RATE_57600  57600
#define BAUD_RATE_115200 115200

#define USART1_PIN_A
#define USART2_PIN_A
#define USART3_PIN_B
#define UART4_PIN_A
#define USART6_PIN_C

void USART_Clock(void);

#if _USART1_IF
void USART1_Config(uint32_t baudrate);
static void USART1_GPIO_Config(void);
#endif
#if _USART2_IF
void USART2_Config(uint32_t baudrate);
static void USART2_GPIO_Config(void);
#endif
#if _USART3_IF
void USART3_Config(uint32_t baudrate);
static void USART3_GPIO_Config(void);
#endif
#if _UART4_IF
void UART4_Config(uint32_t baudrate);
static void UART4_GPIO_Config(void);
#endif
#if _UART5_IF
void UART5_Config(uint32_t baudrate);
static void UART5_GPIO_Config(void);
#endif
#if _USART6_IF
void USART6_Config(uint32_t baudrate);
static void USART6_GPIO_Config(void);
#endif
#if _NVIC_IF
void NVIC_Config(void);
#endif

// iap 串口接受相关
#ifdef IAP
#define IAP_BUFFER  			120*1024 //定义最大接收字节数 120K
#endif

#ifdef __cplusplus
}
#endif //__cplusplus

#endif
