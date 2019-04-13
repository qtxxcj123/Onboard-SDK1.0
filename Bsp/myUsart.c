#include "myUsart.h" 
#include "led.h"
#include "iap.h"

#ifdef IAP
u8 USART_RX_BUF[IAP_BUFFER] __attribute__ ((at(0X20001000)));
uint32_t iapCount = 0;
#endif

void USART_Clock(void)
{
	/*----------------------USART/UART Pin explain-------------------*/ 
	/*< USART1 pin is ( PA9  - PA10 | PB6  - PB7 ) >                 */
	/*< USART2 pin is ( PA2  - PA3  | PD5  - PD6 ) >                 */
	/*< USART3 pin is ( PB10 - PB11 | PD8  - PD9 | PC10 - PC11 ) >   */
	/*< UART4  pin is ( PA0  - PA1  | PC10 - PC11 )>                 */
	/*< UART5  pin is ( PC12 - PD2 ) >                               */
	/*< USART6 pin is ( PC6  - PC7  | PG14 - PG9 ) >                 */
	/*---------------------------------------------------------------*/
	
	/*---------------------explain USART and UART distinction------------------------------*/
	/*UART : ͨ���첽�շ������� (  Universal Asynchronous Receiver/Transmitter )           */
	/*USART: ͨ��ͬ��/�첽������( Universal Synchronous/Asynchronous Receiver/Transmitter) */
	/*USART pin , USART_CK USART_RX USART_TX  ----  UART pin , UART_RX UART_TX             */
	/*USART_RX: receive data pin  ----  USART_TX: transmit data pin                        */
	/*USART_CK: synchronous clock pin                                                      */
	/*-------------------------------------------------------------------------------------*/
	
	/*---------------------USART_HardwareFlowControl explain-------------------------------*/
	/*Hardware Flow Contro equal to USART_RTS USART_CTS can be set                         */
	/*USART_CTS: Request To Send  ---  USART_RTS: Clear To Send                            */
  /*-------------------------------------------------------------------------------------*/
	
	/*---------------------------Init USART/UART Clock------------------------------*/
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);  /*< GPIOA Clock Enable!  >*/
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);  /*< GPIOB Clock Enable!  >*/
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);  /*< GPIOC Clock Enable!  >*/
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE);  /*< GPIOD Clock Enable!  >*/
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG,ENABLE);  /*< GPIOG Clock Enable!  >*/
#if _USART1_IF
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE); /*< USART1 Clock Enable! >*/
#endif																																					
#if _USART2_IF
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE); /*< USART2 Clock Enable! >*/
#endif
#if _USART3_IF
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE); /*< USART3 Clock Enable! >*/
#endif
#if _UART4_IF
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4,ENABLE);  /*< UART1 Clock Enable!  >*/
#endif
#if _UART5_IF	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5,ENABLE);  /*< UART1 Clock Enable!  >*/
#endif
#if _USART6_IF
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6,ENABLE); /*< USART6 Clock Enable! >*/
#endif
	/*------------------------------------------------------------------------------*/
	
	NVIC_Config();
}
#if _USART1_IF
void USART1_Config(uint32_t baudrate) 
{
	USART1_GPIO_Config();
	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate   = baudrate;            /*< ����������   >*/
	USART_InitStructure.USART_WordLength = USART_WordLength_8b; /*< �ֳ�����     >*/
	USART_InitStructure.USART_StopBits   = USART_StopBits_1;    /*< ֹͣλ����   >*/
	USART_InitStructure.USART_Parity     = USART_Parity_No;     /*< ��żЧ������ >*/
	USART_InitStructure.USART_HardwareFlowControl               /*< Ӳ��������   >*/
	                                     = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode                              /*< �շ�ģʽ���� >*/
	                                     = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1, &USART_InitStructure);                   /*< ��ʼ������   >*/
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);              /*< ��������ж� >*/
  USART_Cmd(USART1, ENABLE);                                  /*< ʹ�ܴ���     >*/
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) != SET); /*< �ȴ����ڸ�λ >*/
}

static void USART1_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
#ifdef USART1_PIN_A  // Standard case use PA9 | PA10 reuse USART1
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1);   /*< Pin reuse ! >*/
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1);  /*< Pin reuse ! >*/
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_9 | GPIO_Pin_10; /*< �������� >*/
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;             /*< ����ģʽ >*/
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;         /*< �����ٶ� >*/
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;            /*< ���ģʽ >*/
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;             /*< �ڲ����� >*/
	GPIO_Init(GPIOA,&GPIO_InitStructure);                     /*< ��ʼ��   >*/
#else	// else use PB6 | PB7 reuse USART1
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource6,GPIO_AF_USART1);   /*< Pin reuse ! >*/
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource7,GPIO_AF_USART1);   /*< Pin reuse ! >*/
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_6 | GPIO_Pin_7;  /*< �������� >*/
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;             /*< ����ģʽ >*/
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;         /*< �����ٶ� >*/
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;            /*< ���ģʽ >*/
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;              /*< �ڲ����� >*/
	GPIO_Init(GPIOB,&GPIO_InitStructure);                     /*< ��ʼ��   >*/
#endif
}
#endif

#if _USART2_IF
void USART2_Config(uint32_t baudrate)
{
	USART2_GPIO_Config();
	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate   = baudrate;            /*< ����������   >*/
	USART_InitStructure.USART_WordLength = USART_WordLength_8b; /*< �ֳ�����     >*/
	USART_InitStructure.USART_StopBits   = USART_StopBits_1;    /*< ֹͣλ����   >*/
	USART_InitStructure.USART_Parity     = USART_Parity_No;     /*< ��żЧ������ >*/
	USART_InitStructure.USART_HardwareFlowControl               /*< Ӳ��������   >*/
	                                     = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode                              /*< �շ�ģʽ���� >*/
	                                     = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART2, &USART_InitStructure);                   /*< ��ʼ������   >*/
  USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);              /*< ��������ж� >*/
  USART_Cmd(USART2, ENABLE);                                  /*< ʹ�ܴ���     >*/
	while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) != SET); /*< �ȴ����ڸ�λ >*/
}

static void USART2_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
#ifdef USART2_PIN_A  // Standard case use PA2 | PA3 reuse USART2
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource2,GPIO_AF_USART2);   /*< Pin reuse ! >*/
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource3,GPIO_AF_USART2);   /*< Pin reuse ! >*/
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_2 | GPIO_Pin_3;  /*< �������� >*/
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;             /*< ����ģʽ >*/
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;         /*< �����ٶ� >*/
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;            /*< ���ģʽ >*/
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;             /*< �ڲ����� >*/
	GPIO_Init(GPIOA,&GPIO_InitStructure);                     /*< ��ʼ��   >*/
#else	// else use PD5 | PD6 reuse USART2
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource5,GPIO_AF_USART2);   /*< Pin reuse ! >*/
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource6,GPIO_AF_USART2);   /*< Pin reuse ! >*/
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_5 | GPIO_Pin_6;  /*< �������� >*/
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;             /*< ����ģʽ >*/
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;         /*< �����ٶ� >*/
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;            /*< ���ģʽ >*/
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;              /*< �ڲ����� >*/
	GPIO_Init(GPIOD,&GPIO_InitStructure);                     /*< ��ʼ��   >*/
#endif
}
#endif

#if _USART3_IF
void USART3_Config(uint32_t baudrate)
{
	USART3_GPIO_Config();
	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate   = baudrate;            /*< ����������   >*/
	USART_InitStructure.USART_WordLength = USART_WordLength_8b; /*< �ֳ�����     >*/
	USART_InitStructure.USART_StopBits   = USART_StopBits_1;    /*< ֹͣλ����   >*/
	USART_InitStructure.USART_Parity     = USART_Parity_No;     /*< ��żЧ������ >*/
	USART_InitStructure.USART_HardwareFlowControl               /*< Ӳ��������   >*/
	                                     = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode                              /*< �շ�ģʽ���� >*/
	                                     = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART3, &USART_InitStructure);                   /*< ��ʼ������   >*/
  USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);              /*< ��������ж� >*/
  USART_Cmd(USART3, ENABLE);                                  /*< ʹ�ܴ���     >*/
	while (USART_GetFlagStatus(USART3, USART_FLAG_TXE) != SET); /*< �ȴ����ڸ�λ >*/
}

static void USART3_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
#ifdef USART3_PIN_B  // Standard case use PB10 | PB11 reuse USART3
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource10,GPIO_AF_USART3);  /*< Pin reuse ! >*/
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource11,GPIO_AF_USART3);  /*< Pin reuse ! >*/
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_10 | GPIO_Pin_11;/*< �������� >*/
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;             /*< ����ģʽ >*/
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;         /*< �����ٶ� >*/
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;            /*< ���ģʽ >*/
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;             /*< �ڲ����� >*/
	GPIO_Init(GPIOB,&GPIO_InitStructure);                     /*< ��ʼ��   >*/
#else	// else use PD8 | PD9 reuse USART3
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource8,GPIO_AF_USART3);   /*< Pin reuse ! >*/
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource9,GPIO_AF_USART3);   /*< Pin reuse ! >*/
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_8 | GPIO_Pin_9;  /*< �������� >*/
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;             /*< ����ģʽ >*/
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;         /*< �����ٶ� >*/
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;            /*< ���ģʽ >*/
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;              /*< �ڲ����� >*/
	GPIO_Init(GPIOD,&GPIO_InitStructure);                     /*< ��ʼ��   >*/
#endif
}
#endif

#if _UART4_IF
void UART4_Config(uint32_t baudrate)
{
	UART4_GPIO_Config();
	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate   = baudrate;            /*< ����������   >*/
	USART_InitStructure.USART_WordLength = USART_WordLength_8b; /*< �ֳ�����     >*/
	USART_InitStructure.USART_StopBits   = USART_StopBits_1;    /*< ֹͣλ����   >*/
	USART_InitStructure.USART_Parity     = USART_Parity_No;     /*< ��żЧ������ >*/
	USART_InitStructure.USART_HardwareFlowControl               /*< Ӳ��������   >*/
	                                     = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode                              /*< �շ�ģʽ���� >*/
	                                     = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(UART4, &USART_InitStructure);                   /*< ��ʼ������   >*/
  USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);              /*< ��������ж� >*/
  USART_Cmd(UART4, ENABLE);                                  /*< ʹ�ܴ���     >*/
	while (USART_GetFlagStatus(UART4, USART_FLAG_TXE) != SET); /*< �ȴ����ڸ�λ >*/
}

static void UART4_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
#ifdef UART4_PIN_A  // Standard case use PA0 | PA1 reuse UART4
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource0,GPIO_AF_UART4);    /*< Pin reuse ! >*/
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource1,GPIO_AF_UART4);    /*< Pin reuse ! >*/
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_0 | GPIO_Pin_1;  /*< �������� >*/
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;             /*< ����ģʽ >*/
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;         /*< �����ٶ� >*/
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;            /*< ���ģʽ >*/
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;             /*< �ڲ����� >*/
	GPIO_Init(GPIOA,&GPIO_InitStructure);                     /*< ��ʼ��   >*/
#else	// else use PC10 | PC11 reuse UART4
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource10,GPIO_AF_UART4);   /*< Pin reuse ! >*/
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource11,GPIO_AF_UART4);   /*< Pin reuse ! >*/
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_10 | GPIO_Pin_11;/*< �������� >*/
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;             /*< ����ģʽ >*/
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;         /*< �����ٶ� >*/
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;            /*< ���ģʽ >*/
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;              /*< �ڲ����� >*/
	GPIO_Init(GPIOC,&GPIO_InitStructure);                     /*< ��ʼ��   >*/
#endif
}
#endif

#if _UART5_IF
void UART5_Config(uint32_t baudrate)
{
	UART5_GPIO_Config();
	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate   = baudrate;            /*< ����������   >*/
	USART_InitStructure.USART_WordLength = USART_WordLength_8b; /*< �ֳ�����     >*/
	USART_InitStructure.USART_StopBits   = USART_StopBits_1;    /*< ֹͣλ����   >*/
	USART_InitStructure.USART_Parity     = USART_Parity_No;     /*< ��żЧ������ >*/
	USART_InitStructure.USART_HardwareFlowControl               /*< Ӳ��������   >*/
	                                     = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode                              /*< �շ�ģʽ���� >*/
	                                     = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(UART5, &USART_InitStructure);                    /*< ��ʼ������   >*/
  USART_ITConfig(UART5, USART_IT_RXNE, ENABLE);               /*< ��������ж� >*/
  USART_Cmd(UART5, ENABLE);                                   /*< ʹ�ܴ���     >*/
	while (USART_GetFlagStatus(UART5, USART_FLAG_TXE) != SET);  /*< �ȴ����ڸ�λ >*/
}

static void UART5_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
  // Standard case use PC12 | PD2 reuse UART5
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource12,GPIO_AF_UART5);   /*< Pin reuse ! >*/
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource2,GPIO_AF_UART5);    /*< Pin reuse ! >*/
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_12;              /*< �������� >*/
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;             /*< ����ģʽ >*/
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;         /*< �����ٶ� >*/
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;            /*< ���ģʽ >*/
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;             /*< �ڲ����� >*/
	GPIO_Init(GPIOC,&GPIO_InitStructure);                     /*< ��ʼ��   >*/
	
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_2;               /*< �������� >*/
	GPIO_Init(GPIOD,&GPIO_InitStructure);                     /*< ��ʼ��   >*/
}
#endif

#if _USART6_IF
void USART6_Config(uint32_t baudrate)
{
	USART6_GPIO_Config();
	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate   = baudrate;            /*< ����������   >*/
	USART_InitStructure.USART_WordLength = USART_WordLength_8b; /*< �ֳ�����     >*/
	USART_InitStructure.USART_StopBits   = USART_StopBits_1;    /*< ֹͣλ����   >*/
	USART_InitStructure.USART_Parity     = USART_Parity_No;     /*< ��żЧ������ >*/
	USART_InitStructure.USART_HardwareFlowControl               /*< Ӳ��������   >*/
	                                     = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode                              /*< �շ�ģʽ���� >*/
	                                     = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART6, &USART_InitStructure);                   /*< ��ʼ������   >*/
  USART_ITConfig(USART6, USART_IT_RXNE, ENABLE);              /*< ��������ж� >*/
  USART_Cmd(USART6, ENABLE);                                  /*< ʹ�ܴ���     >*/
	while (USART_GetFlagStatus(USART6, USART_FLAG_TXE) != SET); /*< �ȴ����ڸ�λ >*/
}

static void USART6_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
#ifdef USART6_PIN_C  // Standard case use PC6 | PC7 reuse USART6
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource6,GPIO_AF_USART6);   /*< Pin reuse ! >*/
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource7,GPIO_AF_USART6);   /*< Pin reuse ! >*/
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_6 | GPIO_Pin_7;  /*< �������� >*/
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;             /*< ����ģʽ >*/
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;         /*< �����ٶ� >*/
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;            /*< ���ģʽ >*/
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;             /*< �ڲ����� >*/
	GPIO_Init(GPIOC,&GPIO_InitStructure);                     /*< ��ʼ��   >*/
#else	// else use PG14 | PG9 reuse USART6
	GPIO_PinAFConfig(GPIOG,GPIO_PinSource14,GPIO_AF_USART6);  /*< Pin reuse ! >*/
	GPIO_PinAFConfig(GPIOG,GPIO_PinSource9,GPIO_AF_USART6);   /*< Pin reuse ! >*/
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_14 | GPIO_Pin_9; /*< �������� >*/
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;             /*< ����ģʽ >*/
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;         /*< �����ٶ� >*/
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;            /*< ���ģʽ >*/
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;              /*< �ڲ����� >*/
	GPIO_Init(GPIOG,&GPIO_InitStructure);                     /*< ��ʼ��   >*/
#endif
}
#endif

void NVIC_Config(void)
{
	/*-----------------------------------priority explain-------------------------------------*/
	/* priority ranking : USART3 > USART2 > UART4 > UART5 > USART6 > USART1                   */
	/*----------------------------------------------------------------------------------------*/
	NVIC_InitTypeDef NVIC_InitStructure_USART;
#if _USART1_IF
	/*<------------------------------USART1-------------------------------------------------->*/
	NVIC_InitStructure_USART.NVIC_IRQChannelPreemptionPriority = 0x03;        /*< ��ռ���ȼ� >*/
  NVIC_InitStructure_USART.NVIC_IRQChannelSubPriority        = 0x03;        /*< ��Ӧ���ȼ� >*/
  NVIC_InitStructure_USART.NVIC_IRQChannel                   = USART1_IRQn; /*< �ж�ͨ��   >*/
  NVIC_InitStructure_USART.NVIC_IRQChannelCmd                = ENABLE;      /*< ͨ��ʹ��   >*/
  NVIC_Init(&NVIC_InitStructure_USART);                                     /*< ��ʼ��     >*/
	/*<-------------------------------------------------------------------------------------->*/
#endif
#if _USART2_IF	
	/*<------------------------------USART2-------------------------------------------------->*/
	NVIC_InitStructure_USART.NVIC_IRQChannelPreemptionPriority = 0x01;        /*< ��ռ���ȼ� >*/
  NVIC_InitStructure_USART.NVIC_IRQChannelSubPriority        = 0x00;        /*< ��Ӧ���ȼ� >*/
  NVIC_InitStructure_USART.NVIC_IRQChannel                   = USART2_IRQn; /*< �ж�ͨ��   >*/
  NVIC_InitStructure_USART.NVIC_IRQChannelCmd                = ENABLE;      /*< ͨ��ʹ��   >*/
  NVIC_Init(&NVIC_InitStructure_USART);                                     /*< ��ʼ��     >*/
	/*<-------------------------------------------------------------------------------------->*/
#endif
#if _USART3_IF
	/*<------------------------------USART3-------------------------------------------------->*/
	NVIC_InitStructure_USART.NVIC_IRQChannelPreemptionPriority = 0x00;        /*< ��ռ���ȼ� >*/
  NVIC_InitStructure_USART.NVIC_IRQChannelSubPriority        = 0x00;        /*< ��Ӧ���ȼ� >*/
  NVIC_InitStructure_USART.NVIC_IRQChannel                   = USART3_IRQn; /*< �ж�ͨ��   >*/
  NVIC_InitStructure_USART.NVIC_IRQChannelCmd                = ENABLE;      /*< ͨ��ʹ��   >*/
  NVIC_Init(&NVIC_InitStructure_USART);                                     /*< ��ʼ��     >*/
	/*<-------------------------------------------------------------------------------------->*/	
#endif
#if _UART4_IF	
	/*<-------------------------------UART4-------------------------------------------------->*/
	NVIC_InitStructure_USART.NVIC_IRQChannelPreemptionPriority = 0x01;        /*< ��ռ���ȼ� >*/
  NVIC_InitStructure_USART.NVIC_IRQChannelSubPriority        = 0x01;        /*< ��Ӧ���ȼ� >*/
  NVIC_InitStructure_USART.NVIC_IRQChannel                   = UART4_IRQn;  /*< �ж�ͨ��   >*/
  NVIC_InitStructure_USART.NVIC_IRQChannelCmd                = ENABLE;      /*< ͨ��ʹ��   >*/
  NVIC_Init(&NVIC_InitStructure_USART);                                     /*< ��ʼ��     >*/
	/*<-------------------------------------------------------------------------------------->*/
#endif
#if _UART5_IF		
	/*<-------------------------------UART5-------------------------------------------------->*/
	NVIC_InitStructure_USART.NVIC_IRQChannelPreemptionPriority = 0x02;        /*< ��ռ���ȼ� >*/
  NVIC_InitStructure_USART.NVIC_IRQChannelSubPriority        = 0x00;        /*< ��Ӧ���ȼ� >*/
  NVIC_InitStructure_USART.NVIC_IRQChannel                   = UART5_IRQn;  /*< �ж�ͨ��   >*/
  NVIC_InitStructure_USART.NVIC_IRQChannelCmd                = ENABLE;      /*< ͨ��ʹ��   >*/
  NVIC_Init(&NVIC_InitStructure_USART);                                     /*< ��ʼ��     >*/
	/*<-------------------------------------------------------------------------------------->*/
#endif
#if _USART6_IF	
	/*<-------------------------------USART6------------------------------------------------->*/
	NVIC_InitStructure_USART.NVIC_IRQChannelPreemptionPriority = 0x02;        /*< ��ռ���ȼ� >*/
  NVIC_InitStructure_USART.NVIC_IRQChannelSubPriority        = 0x01;        /*< ��Ӧ���ȼ� >*/
  NVIC_InitStructure_USART.NVIC_IRQChannel                   = USART6_IRQn; /*< �ж�ͨ��   >*/
  NVIC_InitStructure_USART.NVIC_IRQChannelCmd                = ENABLE;      /*< ͨ��ʹ��   >*/
  NVIC_Init(&NVIC_InitStructure_USART);                                     /*< ��ʼ��     >*/
	/*<-------------------------------------------------------------------------------------->*/	
#endif
	
}
#ifdef __cplusplus
extern "C" {
#endif
uint16_t addrsCount = 0;
void USART1_IRQHandler()
{
  u8 Res;
	LED_RED = ON;
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)	{ 
		Res = USART_ReceiveData(USART1);
		if(iapCount<IAP_BUFFER) {
			USART_RX_BUF[iapCount]=Res;
			iapCount++;			 									     
		}
		if(iapCount == 0x4000) { // �յ�16kb������
			writeAppBin(FLASH_APP1_ADDRS + (addrsCount * 0x4000),USART_RX_BUF,iapCount);
			iapCount = 0;
			addrsCount++;
		}
  } 
}

void USART2_IRQHandler()
{
	while(1);
}

void USART3_IRQHandler()
{
	while(1);
}

void UART4_IRQHandler()
{
	while(1);
}

void UART5_IRQHandler()
{
	while(1);
}

void USART6_IRQHandler()
{
	while(1);
}

#if _PRINTF_IF
#include "stdio.h"
#pragma import(__use_no_semihosting) 
struct __FILE 
{ 
	int handle; 
}; 
FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
void _sys_exit(int x) 
{ 
	x = x; 
} 
int fputc(int ch, FILE *f)
{ 	
	while((USART1->SR&0X40)==0);//ѭ������,ֱ���������   
	USART1->DR = (u8) ch;      
	return ch;
}
#endif


#ifdef __cplusplus
}
#endif // __cplusplus
