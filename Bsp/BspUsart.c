#include "BspUsart.h"
extern bool           isFrame;
bool                  isACKProcessed    = false;
bool                  ackReceivedByUser = false;
extern RecvContainer *rFrame;
extern Vehicle* vehicle;
uint64_t tick = 0;
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
USART3_IRQHandler(void)   // 云台飞控
{
    if (USART_GetFlagStatus(USART3, USART_FLAG_RXNE) == SET)
    {
			//已处理
        isACKProcessed = false;
			//框架           协议层         字节处理
        isFrame = vehicle->protocolLayer->byteHandler(USART_ReceiveData(USART3));
        if (isFrame == true)
        {                               //获得框架
            rFrame = vehicle->protocolLayer->getReceivedFrame();

            //! Trigger default or user defined callback  触发默认或用户定义的回调
					  //处理中的数据
            vehicle->processReceivedData(rFrame);

            //! Reset   重置
            isFrame        = false;
            isACKProcessed = true;
        }
    }
}

void 
HardFault_Handler(void)
{
    uint32_t temp;
    temp = SCB->CFSR;					//fault状态寄存器(@0XE000ED28)包括:MMSR,BFSR,UFSR
    printf("CFSR:%8X\r\n", temp);	//显示错误值
    temp = SCB->HFSR;					//硬件fault状态寄存器
    printf("HFSR:%8X\r\n", temp);	//显示错误值
    temp = SCB->DFSR;					//调试fault状态寄存器
    printf("DFSR:%8X\r\n", temp);	//显示错误值
    temp = SCB->AFSR;					//辅助fault状态寄存器
    printf("AFSR:%8X\r\n", temp);	//显示错误值
    while(1);
}


