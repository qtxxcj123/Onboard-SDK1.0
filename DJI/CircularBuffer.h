#ifndef CIRCULARBUFFER_H
#define CIRCULARBUFFER_H
#include "type.h"
struct CircularBuffer;
typedef void(*cirBufferInitFunc)(void);
typedef int(*cbPushFunc)(struct CircularBuffer*, VehicleCallBackHandler,
              RecvContainer);
typedef int(*cbPopFunc)(struct CircularBuffer*, VehicleCallBackHandler*,
            RecvContainer*);
typedef struct CircularBuffer{
	// point function
	cirBufferInitFunc init;
	cbPushFunc        cbPush;
	cbPopFunc         cbPop;
	
	// 变量声明
	int head;
  int tail;
	VehicleCallBackHandler* buffer;
  RecvContainer*          buffer2;
  int               maxLen;
}CircularBuffer;

void externCircularBufferInit(void);// 外部初始化

static void interCircularBufferInit(void); // 内部初始化

static int cbPush(CircularBuffer* CBuffer, VehicleCallBackHandler data,
             RecvContainer data2);
static int cbPop(CircularBuffer* CBuffer, VehicleCallBackHandler* data,
            RecvContainer* data2);

#endif
