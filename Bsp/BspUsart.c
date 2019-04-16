#include "BspUsart.h"
extern bool           isFrame;
bool                  isACKProcessed    = false;
bool                  ackReceivedByUser = false;
extern RecvContainer *rFrame;
extern Vehicle* vehicle;
uint64_t tick = 0;

void
USART1_Confug(uint32_t baudrate)
{
	USART1_GPIO_Config();
	
	USART_InitTypeDef USART_InitStructure;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

  USART_InitStructure.USART_BaudRate   = 115200;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits   = USART_StopBits_1;
  USART_InitStructure.USART_Parity     = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl =
      USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

  USART_Init(USART1, &USART_InitStructure);
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

  USART_Cmd(USART1, ENABLE);
  while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) != SET);
}

void USART1_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE); 

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; 
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1); 
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1); 
}

void
USART3_Config(uint32_t baudrate)
{
    USART3_GPIO_Config();

    USART_InitTypeDef USART_InitStructure;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);

    USART_InitStructure.USART_BaudRate   = 230400;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits   = USART_StopBits_1;
    USART_InitStructure.USART_Parity     = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl =
        USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

    USART_Init(USART3, &USART_InitStructure);
    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);

    USART_Cmd(USART3, ENABLE);
    while (USART_GetFlagStatus(USART3, USART_FLAG_TXE) != SET);
		
		
		NVIC_InitTypeDef NVIC_InitStructure;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0x00;
    NVIC_InitStructure.NVIC_IRQChannel                   = USART3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
		NVIC_Init(&NVIC_InitStructure);
} //USART3_Config

static void 
USART3_GPIO_Config(void)
{
	 GPIO_InitTypeDef GPIO_InitStructure;

   RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

   GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_10 | GPIO_Pin_11;
   GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
   GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
   GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
   GPIO_Init(GPIOB, &GPIO_InitStructure);

   GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_USART3); // tx
   GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_USART3); // rx
} //USART3_GPIO_Config

void 
NVIC_Config(void)
{
	 NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	 NVIC_InitTypeDef NVIC_InitStructure;
	
	 NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
   NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0x00;
   NVIC_InitStructure.NVIC_IRQChannel                   = USART3_IRQn;
   NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
	 NVIC_Init(&NVIC_InitStructure);
	
	 NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
   NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0x01;
   NVIC_InitStructure.NVIC_IRQChannel                   = USART1_IRQn;
   NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
	 NVIC_Init(&NVIC_InitStructure);
}

uint64_t
getTimeStamp(void)
{
	return tick;
}

void
SystickConfig()
{
    if (SysTick_Config(SystemCoreClock / 1000)) // 1000 ticks per second.
    {
        while (1); // run here when error.
    }
}

void
SysTick_Handler(void)
{
    if (tick > 4233600000ll) // 49 days non-reset would cost a tick reset.
    {
        tick = 0;
    }
    tick++;
}

void 
USART3_IRQHandler(void)   // ��̨�ɿ�
{
    if (USART_GetFlagStatus(USART3, USART_FLAG_RXNE) == SET)
    {
			//�Ѵ���
        isACKProcessed = false;
			//���           Э���         �ֽڴ���
        isFrame = vehicle->protocolLayer->byteHandler(USART_ReceiveData(USART3));
        if (isFrame == true)
        {                               //��ÿ��
            rFrame = vehicle->protocolLayer->getReceivedFrame();

            //! Trigger default or user defined callback  ����Ĭ�ϻ��û�����Ļص�
					  //�����е�����
            vehicle->processReceivedData(rFrame);

            //! Reset   ����
            isFrame        = false;
            isACKProcessed = true;
        }
    }
}

void 
HardFault_Handler(void)
{
    uint32_t temp;
    temp = SCB->CFSR;					//fault״̬�Ĵ���(@0XE000ED28)����:MMSR,BFSR,UFSR
    printf("CFSR:%8X\r\n", temp);	//��ʾ����ֵ
    temp = SCB->HFSR;					//Ӳ��fault״̬�Ĵ���
    printf("HFSR:%8X\r\n", temp);	//��ʾ����ֵ
    temp = SCB->DFSR;					//����fault״̬�Ĵ���
    printf("DFSR:%8X\r\n", temp);	//��ʾ����ֵ
    temp = SCB->AFSR;					//����fault״̬�Ĵ���
    printf("AFSR:%8X\r\n", temp);	//��ʾ����ֵ
    while(1);
}


