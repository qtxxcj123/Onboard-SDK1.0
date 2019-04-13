#ifndef SUBSCRIPTION_H
#define SUBSCRIPTION_H
#include "type.h"

typedef void(*dataSubInitFunc)(void);
typedef void(*dataSubVerifyFunc)(void);


typedef struct DataSubscription{
	dataSubInitFunc init;
	dataSubVerifyFunc verify;

}DataSubscription;

void externDataSubcripInit(void);

static void interiDataSubcripInit(void);

static void verify(void);

#endif
