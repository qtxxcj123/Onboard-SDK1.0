#ifndef ACK_H
#define ACK_H
#include "type.h"
#include "Error.h"
#include "version.h"
#define ACK_SUCCESS  0
#define ACK_FAIL     1

#define ACK_OBTAIN_CONTROL   1
#define ACK_RELEASE_CONTROL  0

typedef void(*ackInitFunc)(void);
typedef bool(*ackGetErrorFunc)(ErrorCode);
typedef void(*ackGetErrorCodeMessage)(ErrorCode, const char*);

typedef struct Ack{
	ackInitFunc init;
	ackGetErrorFunc getError;
	ackGetErrorCodeMessage getErrorCodeMessage;
}Ack;

void externAckInit(void);

static void interiAckInit(void);

static bool getError(ErrorCode ack);

static void getErrorCodeMessage(ErrorCode ack, const char* func);

static void getCMDSetActivationMSG(ErrorCode ack);

static void getCMDSetControlMSG(ErrorCode ack);

static void getSetBroadcastMSG(ErrorCode ack);

static void getCMDSetMissionMSG(ErrorCode ack);

static void getCMDSetSyncMSG(ErrorCode ack);

static void getCMDSetVirtualRCMSG(ErrorCode ack);

static void getCMDSetMFIOMSG(ErrorCode ack);

static void getCMDSetSubscribeMSG(ErrorCode ack);

static void getCommonErrorCodeMessage(ErrorCode ack);

#endif
