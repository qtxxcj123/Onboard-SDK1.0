#ifndef _VEHICLE_H
#define _VEHICLE_H

#include "OpenProtocol.h"
#include "CircularBuffer.h"
#include "Subscription.h"
#include "MissionManager.h"
#include "version.h"
#include "Error.h"
#include "Ack.h"

typedef void(*vehicleInit)(void);
typedef void(*vehicleInitProcessReceivedData)(RecvContainer*);
typedef FirmWare(*vehicleGetFwVersion)(void);
typedef void(*vehicleActivate)(ActivateData*, VehicleCallBack,UserData);
typedef int(*vehicleCallbackIdIndex)(void);
typedef bool(*vehicleGetEncryption)(void);
//typedef void(*obtainAndrelease)(VehicleCallBack, UserData);
typedef void(*basicFunc)(VehicleCallBack, UserData);
typedef struct Vehicle{
	OpenProtocol*           protocolLayer; //添加一个open类
	struct CircularBuffer*  circularBuffer; //缓存
	DataSubscription*       subscribe;
	MissionManager*         missionManager;
	Ack*                    ack;

	//下面是函数指针
	vehicleInit                    init; // 相当于构造函数
	vehicleInitProcessReceivedData processReceivedData;//接收函数
	vehicleGetFwVersion            getFwVersion;
	vehicleActivate								 activate;
	vehicleCallbackIdIndex         callbackIdIndex;
	vehicleGetEncryption           getEncryption;
	basicFunc                      obtainCtrlAuthority;
	basicFunc                      releaseCtrlAuthority;
	basicFunc                      getDroneVersion;
	// 变量
	void*    nbCallbackFunctions[200]; //! @todo magic number
	UserData nbUserData[200];          //! @todo magic number
	uint8_t rawVersionACK[MAX_ACK_SIZE];
	bool        threadSupported;
	RecvContainer* nbCallbackRecvContainer;
	VehicleCallBackHandler nbVehicleCallBackHandler; //回调
	//! Added for connecting protocolLayer to Vehicle
  RecvContainer lastReceivedFrame;
	WayPointAddPoints waypointAddPointACK;
	WayPointInits waypointInitACK;
	WayPointIndexs waypointIndexACK;
	HotPointStarts hotpointStartACK;
	HotPointReads hotpointReadACK;
	ErrorCode ackErrorCode;
	DroneVersions droneVersionACK;
	MFIOGets mfioGetACK;
	
	
	VersionData versionData;
	ActivateData accountData;
	bool encrypt;
}Vehicle;



void externVehicleInit(void);

static void interiorVehicleInit(void);

static void activate(ActivateData *data, VehicleCallBack callback,UserData userData);

static void processReceivedData(RecvContainer *receivedFrame);

static void ACKHandler(void *eventData);

static FirmWare getFwVersion(void);

static void activateCallback(Vehicle *vehiclePtr, RecvContainer recvFrame,UserData userData);

static int callbackIdIndex(void);

static bool getEncryption(void);

static void obtainCtrlAuthority(VehicleCallBack callback, UserData userData);

static void releaseCtrlAuthority(VehicleCallBack callback, UserData userData);

static void controlAuthorityCallback(Vehicle *vehiclePtr, RecvContainer recvFrame,UserData userData);

static bool parseDroneVersionInfo(VersionData* versionData,uint8_t *ackPtr);

static void getDroneVersionCallback(Vehicle *vehiclePtr, RecvContainer recvFrame, UserData userData);

static void getDroneVersion(VehicleCallBack callback, UserData userData);

#endif
