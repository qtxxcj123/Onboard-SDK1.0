#include "Vehicle.h"
#include "command.h"
#include "malloc.h"
extern const uint8_t waypointAddPoint[];
extern const uint8_t waypointDownload[];
extern const uint8_t waypointIndexDownload[];
extern const uint8_t hotpointStart[];
extern const uint8_t hotpointDownload[];
extern const uint8_t getVersion[];
extern const uint8_t init[];
extern const uint8_t get[];
extern const uint8_t activates[];
extern const uint8_t setControl[];
extern OpenProtocol* protocolLayer;
extern CircularBuffer* circularBuffer;
extern DataSubscription* subscribes;
extern Ack* ack;
extern MissionManager* missionManager;
extern const char* M100;

Vehicle* vehicle;
int callbackId;
void 
externVehicleInit(void)
{
	vehicle = (Vehicle*)mymalloc(sizeof(Vehicle));
	externOpenProtocolInit();  // 初始化协议
	externCircularBufferInit();// 初始化缓存
	externDataSubcripInit();   // 初始化sub
	externAckInit();  // 初始化ack
	externManagerInit(); // 初始化Manager
	vehicle->init = interiorVehicleInit;
	vehicle->processReceivedData = processReceivedData;
	vehicle->protocolLayer = protocolLayer;
	vehicle->subscribe     = subscribes;
	vehicle->ack						= ack;
	vehicle->missionManager = missionManager;
	vehicle->circularBuffer = circularBuffer;
	vehicle->getFwVersion   = getFwVersion;
	vehicle->activate       = activate;
	vehicle->callbackIdIndex = callbackIdIndex;
	vehicle->getEncryption  = getEncryption;
	vehicle->obtainCtrlAuthority = obtainCtrlAuthority;
	vehicle->releaseCtrlAuthority = releaseCtrlAuthority;
	vehicle->getDroneVersion      = getDroneVersion;
}

static void 
interiorVehicleInit(void)
{
	vehicle->nbCallbackRecvContainer = (RecvContainer*)mymalloc(sizeof(RecvContainer) * 200);
	vehicle->protocolLayer->openProtocolInit(); // 内部初始化
	vehicle->circularBuffer->init();            // 内部初始化
	vehicle->subscribe->init();
	vehicle->ack->init();
	vehicle->encrypt = false;
}

static void processReceivedData(RecvContainer *receivedFrame)
{
		receivedFrame->recvInfo.version = getFwVersion();
    if (receivedFrame->dispatchInfo.isAck)
    {
        // TODO Fill up ACKErrorCode Container 容器
        if (receivedFrame->dispatchInfo.isCallback)
        {
            vehicle->nbVehicleCallBackHandler.callback =(VehicleCallBack) vehicle->nbCallbackFunctions[receivedFrame->dispatchInfo.callbackID];
            vehicle->nbVehicleCallBackHandler.userData =
            vehicle->nbUserData[receivedFrame->dispatchInfo.callbackID];

            if (vehicle->nbVehicleCallBackHandler.callback)
            {
                if (vehicle->threadSupported)
                {
                    vehicle->nbCallbackRecvContainer[receivedFrame->dispatchInfo.callbackID] =*receivedFrame;
                    vehicle->circularBuffer->cbPush(
                    vehicle->circularBuffer, vehicle->nbVehicleCallBackHandler,
                    vehicle->nbCallbackRecvContainer[receivedFrame->dispatchInfo.callbackID]);
                }
                else
                {
                    vehicle->nbVehicleCallBackHandler.callback(vehicle, *receivedFrame, vehicle->nbVehicleCallBackHandler.userData);
                }
            }
        }
        else
        {            //调度员   发现       作为  阻塞  呼叫
            //DDEBUG("Dispatcher identified as blocking call\n");  //指定为阻塞调用的调度器
            // TODO remove
			      //最后收到的框架     收到框架
            vehicle->lastReceivedFrame = *receivedFrame;
             //ACK处理者                      收到框架
            ACKHandler((void*)(receivedFrame));
			  //协议层     得到线程句柄       通知
        }
    }
    else
    {          //指定为推送数据的调度器
       // MY_DEBUG("Dispatcher identified as push data\n");
		//推送数据处理程序                  收到框架
        //PushDataHandler(static_cast<void *>(receivedFrame));
    }
}

static void 
ACKHandler(void *eventData)
{
if (!eventData)
    {
        MY_DEBUG("Invalid ACK event data received!\n");
        return;
    }

    RecvContainer *ackData = (RecvContainer *)eventData;
    const uint8_t cmd[] = { ackData->recvInfo.cmd_set, ackData->recvInfo.cmd_id };

    if (ackData->recvInfo.cmd_set == MISSION)
    {
        if (memcmp(cmd, waypointAddPoint,
                   sizeof(cmd)) == 0)
        {
            vehicle->waypointAddPointACK.ack.info = ackData->recvInfo;
            vehicle->waypointAddPointACK.ack.data = ackData->recvData.wpAddPointACK.ack;
            vehicle->waypointAddPointACK.index    = ackData->recvData.wpAddPointACK.index;
        }
        else if (memcmp(cmd, waypointDownload,
                        sizeof(cmd)) == 0)
        {
            vehicle->waypointInitACK.ack.info = ackData->recvInfo;
            vehicle->waypointInitACK.ack.data = ackData->recvData.wpInitACK.ack;
            vehicle->waypointInitACK.data     = ackData->recvData.wpInitACK.data;
        }
        else if (memcmp(cmd, waypointIndexDownload,
                        sizeof(cmd)) == 0)
        {
            vehicle->waypointIndexACK.ack.info = ackData->recvInfo;
            vehicle->waypointIndexACK.ack.data = ackData->recvData.wpIndexACK.ack;
            vehicle->waypointIndexACK.data     = ackData->recvData.wpIndexACK.data;
        }
        else if (memcmp(cmd, hotpointStart,
                        sizeof(cmd)) == 0)
        {
            vehicle->hotpointStartACK.ack.info  = ackData->recvInfo;
            vehicle->hotpointStartACK.ack.data  = ackData->recvData.hpStartACK.ack;
            vehicle->hotpointStartACK.maxRadius = ackData->recvData.hpStartACK.maxRadius;
        }
        else if (memcmp(cmd, hotpointDownload,
                        sizeof(cmd)) == 0)
        {
            vehicle->hotpointReadACK.ack.info = ackData->recvInfo;
            vehicle->hotpointReadACK.ack.data = ackData->recvData.hpReadACK.ack;
            vehicle->hotpointReadACK.data     = ackData->recvData.hpReadACK.data;
        }
        else
        {
            vehicle->ackErrorCode.info = ackData->recvInfo;
            vehicle->ackErrorCode.data = ackData->recvData.missionACK;
        }
    }
    else if (memcmp(cmd, getVersion,
                    sizeof(cmd)) == 0)
    {
        size_t arrLength = sizeof(ackData->recvData.versionACK);
        for (int i = 0; i < arrLength; i++)
        {
            //! Interim stage: version data will be parsed before returned to user
            vehicle->rawVersionACK[i] = ackData->recvData.versionACK[i];
        }
        vehicle->droneVersionACK.ack.info = ackData->recvInfo;
    }
    else if (ackData->recvInfo.cmd_set == SUBSCRIBE)
    {
        vehicle->ackErrorCode.info = ackData->recvInfo;
        vehicle->ackErrorCode.data = ackData->recvData.subscribeACK;
    }
    else if (ackData->recvInfo.cmd_set == CONTROL)
    {
        vehicle->ackErrorCode.info = ackData->recvInfo;
        vehicle->ackErrorCode.data = ackData->recvData.commandACK;
    }
    else if (memcmp(cmd, init, sizeof(cmd)) == 0)
    {
        vehicle->ackErrorCode.info = ackData->recvInfo;
        vehicle->ackErrorCode.data = ackData->recvData.mfioACK;
    }
    else if (memcmp(cmd, get, sizeof(cmd)) == 0)
    {
        vehicle->mfioGetACK.ack.info = ackData->recvInfo;
        vehicle->mfioGetACK.ack.data = ackData->recvData.mfioGetACK.result;
        vehicle->mfioGetACK.value    = ackData->recvData.mfioGetACK.value;
    }
    else
    {
        vehicle->ackErrorCode.info = ackData->recvInfo;
        vehicle->ackErrorCode.data = ackData->recvData.ack;
    }
}

static void 
activate(ActivateData *data, VehicleCallBack callback,UserData userData)
{
		vehicle->getDroneVersion(0,0);
    data->version        = vehicle->versionData.fwVersion;
    vehicle->accountData          = *data;
    vehicle->accountData.reserved = 2;

    for (int i = 0; i < 32; ++i)
    {
        vehicle->accountData.iosID[i] = '0'; //! @note for ios verification   用于iOS验证
    }
    MY_DEBUG("version 0x%X\n", vehicle->versionData.fwVersion);
    MY_DEBUG("%.32s", vehicle->accountData.iosID);
    //! Using function prototype II of send
    int cbIndex = callbackIdIndex();
    if (callback)
    {
        vehicle->nbCallbackFunctions[cbIndex] = (void *)callback;
        vehicle->nbUserData[cbIndex]          = userData;
    }
    else
    {
        vehicle->nbCallbackFunctions[cbIndex] = (void *)activateCallback;
        vehicle->nbUserData[cbIndex]          = NULL;
    }
    vehicle->protocolLayer->openProtocolSend(
        2, 0, activates, (uint8_t *)&vehicle->accountData,
        sizeof(vehicle->accountData) - sizeof(char *), 1000, 3, true, cbIndex);
}

static void 
activateCallback(Vehicle *vehiclePtr, RecvContainer recvFrame,UserData userData)
{

    uint16_t ack_data;
    if (recvFrame.recvInfo.len - PackageMin <= 2)
    {
        ack_data = recvFrame.recvData.ack;

        vehiclePtr->ackErrorCode.data = ack_data;
        vehiclePtr->ackErrorCode.info = recvFrame.recvInfo;

        if (vehicle->ack->getError(vehiclePtr->ackErrorCode) &&
                ack_data ==
                ActivationACK_OSDK_VERSION_ERROR)
        {
            MY_DEBUG("SDK version did not match\n");
//            vehiclePtr->getDroneVersion();
        }

        //! Let user know about other errors if any
        vehicle->ack->getErrorCodeMessage(vehiclePtr->ackErrorCode, __func__);
    }
    else
    {
        MY_DEBUG("ACK is exception, sequence %d\n", recvFrame.recvInfo.seqNumber);
    }

    if (ack_data == ActivationACK_SUCCESS &&
            vehiclePtr->accountData.encKey)
    {
        vehiclePtr->protocolLayer->setKey(vehiclePtr->accountData.encKey);
    }
}

static void 
controlAuthorityCallback(Vehicle *vehiclePtr, RecvContainer recvFrame,
                                  UserData userData)
{
    ErrorCode ack;
    ack.data = CommonACK_NO_RESPONSE_ERROR;
    int     cbIndex = vehiclePtr->callbackIdIndex();

    if (recvFrame.recvInfo.len - PackageMin <= sizeof(uint16_t))
    {
        ack.data = recvFrame.recvData.ack;
        ack.info = recvFrame.recvInfo;
    }
    else
    {
        MY_DEBUG("ACK is exception, sequence %d\n", recvFrame.recvInfo.seqNumber);
        return;
    }

    if (ack.data == ControlACK_SetControl_OBTAIN_CONTROL_IN_PROGRESS)
    {
        vehicle->ack->getErrorCodeMessage(ack, __func__);
        vehiclePtr->obtainCtrlAuthority(controlAuthorityCallback,0);
    }
    else if (ack.data == ControlACK_SetControl_RELEASE_CONTROL_IN_PROGRESS)
    {
        vehicle->ack->getErrorCodeMessage(ack, __func__);
        vehiclePtr->releaseCtrlAuthority(controlAuthorityCallback,0);
    }
    else
    {
        vehicle->ack->getErrorCodeMessage(ack, __func__);
    }
}

static void
obtainCtrlAuthority(VehicleCallBack callback, UserData userData)
{
    uint8_t data    = 1;
    int     cbIndex = callbackIdIndex();
    if (callback)
    {
        vehicle->nbCallbackFunctions[cbIndex] = (void *)callback;
        vehicle->nbUserData[cbIndex]          = userData;
    }
    else
    {
        vehicle->nbCallbackFunctions[cbIndex] = (void *)controlAuthorityCallback;
        vehicle->nbUserData[cbIndex]          = NULL;
    }
    vehicle->protocolLayer->openProtocolSend(2, vehicle->encrypt,
                        setControl, &data, 1,
                        500, 2, true, cbIndex);
}

static void 
releaseCtrlAuthority(VehicleCallBack callback, UserData userData)
{
    uint8_t data    = 0;
    int     cbIndex = callbackIdIndex();
    if (callback)
    {
        vehicle->nbCallbackFunctions[cbIndex] = (void *)callback;
        vehicle->nbUserData[cbIndex]          = userData;
    }
    else
    {
        // nbCallbackFunctions[cbIndex] = (void*)ReleaseCtrlCallback;
        vehicle->nbUserData[cbIndex] = NULL;
    }
    vehicle->protocolLayer->openProtocolSend(2, vehicle->encrypt,
												setControl, &data, 1,500, 2, true, cbIndex);
}

static void 
getDroneVersion(VehicleCallBack callback, UserData userData)
{
    vehicle->versionData.version_ack = CommonACK_NO_RESPONSE_ERROR;
    vehicle->versionData.version_crc     = 0x0;
    vehicle->versionData.version_name[0] = 0;
    vehicle->versionData.fwVersion       = 0;

    uint32_t cmd_timeout = 100; // unit is ms
    uint32_t retry_time  = 3;
    uint8_t  cmd_data    = 0;
    int      cbIndex     = callbackIdIndex();
    if (callback)
    {
        vehicle->nbCallbackFunctions[cbIndex] = (void *)callback;
        vehicle->nbUserData[cbIndex]          = userData;
    }
    else
    {
        vehicle->nbCallbackFunctions[cbIndex] = (void *)getDroneVersionCallback;
        vehicle->nbUserData[cbIndex]          = NULL;
    }

    // When UserData is implemented, pass the Vehicle as userData.
    vehicle->protocolLayer->openProtocolSend(2, 0, getVersion,(uint8_t *)&cmd_data, 1, 
																	cmd_timeout, retry_time, true,cbIndex);
}

static void 
getDroneVersionCallback(Vehicle *vehiclePtr, RecvContainer recvFrame, UserData userData)
{

    if (!parseDroneVersionInfo(&vehiclePtr->versionData,recvFrame.recvData.versionACK))
    {
        MY_DEBUG("Drone version not obtained! Please do not proceed.\n"
               "Possible reasons:\n"
               "\tSerial port connection:\n"
               "\t\t* SDK is not enabled, please check DJI Assistant2 -> SDK -> [v] Enable API Control.\n"
               "\t\t* Baudrate is not correct, please double-check from DJI Assistant2 -> SDK -> baudrate.\n"
               "\t\t* TX and RX pins are inverted.\n"
               "\t\t* Serial port is occupied by another program.\n"
               "\t\t* Permission required. Please do 'sudo usermod -a -G dialout $USER' "
               "(you do not need to replace $USER with your username). Then logout and login again\n");

        //! Set fwVersion to 0 so we can catch the error.
        vehiclePtr->versionData.fwVersion = 0;
    }
    else
    {
        //! Finally, we print stuff out.
        if (vehiclePtr->versionData.fwVersion > FW(3, 1, 0, 0))
        {
            MY_DEBUG("Device Serial No. = %.16s\n",
                    vehiclePtr->versionData.hw_serial_num);
        }
        MY_DEBUG("Hardware = %.12s\n", vehiclePtr->versionData.hwVersion);
        MY_DEBUG("Firmware = %X\n", vehiclePtr->versionData.fwVersion);
        if (vehiclePtr->versionData.fwVersion < FW(3, 2, 0, 0))
        {
            MY_DEBUG("Version CRC = 0x%X\n", vehiclePtr->versionData.version_crc);
        }
    }
}

static bool 
parseDroneVersionInfo(VersionData* versionData,uint8_t *ackPtr)
{

    VersionData versionStruct;

    //! Note down our starting point as a sanity check
    uint8_t *startPtr = ackPtr;
    //! 2b ACK.
    versionStruct.version_ack = ackPtr[0] + (ackPtr[1] << 8);
    ackPtr += 2;

    //! Next, we might have CRC or ID; Put them into a variable that we will parse
    //! later. Find next \0
    uint8_t crc_id[16] = {};
    int     i          = 0;
    while (*ackPtr != '\0')
    {
        crc_id[i] = *ackPtr;
        i++;
        ackPtr++;
        if (ackPtr - startPtr > 18)
        {
            return false;
        }
    }
    //! Fill in the termination character
    crc_id[i] = *ackPtr;
    ackPtr++;

    //! Now we're at the name. First, let's fill up the name field.
    memcpy(versionStruct.version_name, ackPtr, 32);

    //! Now, we start parsing the name. Let's find the second space character.
    while (*ackPtr != ' ')
    {
        ackPtr++;
        if (ackPtr - startPtr > 64)
        {
            return false;
        }
    } //! Found first space ("SDK-v1.x")
    ackPtr++;

    while (*ackPtr != ' ')
    {
        ackPtr++;
        if (ackPtr - startPtr > 64)
        {
            return false;
        }
    } //! Found second space ("BETA")
    ackPtr++;

    //! Next is the HW version
    int j = 0;
    while (*ackPtr != '-')
    {
        versionStruct.hwVersion[j] = *ackPtr;
        ackPtr++;
        j++;
        if (ackPtr - startPtr > 64)
        {
            return false;
        }
    }
    //! Fill in the termination character
    versionStruct.hwVersion[j] = '\0';
    ackPtr++;

    //! Finally, we come to the FW version. We don't know if each clause is 2 or 3
    //! digits long.
    int ver1 = 0, ver2 = 0, ver3 = 0, ver4 = 0;

    while (*ackPtr != '.')
    {
        ver1 = (*ackPtr - 48) + 10 * ver1;
        ackPtr++;
        if (ackPtr - startPtr > 64)
        {
            return false;
        }
    }
    ackPtr++;
    while (*ackPtr != '.')
    {
        ver2 = (*ackPtr - 48) + 10 * ver2;
        ackPtr++;
        if (ackPtr - startPtr > 64)
        {
            return false;
        }
    }
    ackPtr++;
    while (*ackPtr != '.')
    {
        ver3 = (*ackPtr - 48) + 10 * ver3;
        ackPtr++;
        if (ackPtr - startPtr > 64)
        {
            return false;
        }
    }
    ackPtr++;
    while (*ackPtr != '\0')
    {
        ver4 = (*ackPtr - 48) + 10 * ver4;
        ackPtr++;
        if (ackPtr - startPtr > 64)
        {
            return false;
        }
    }

    versionStruct.fwVersion = FW(ver1, ver2, ver3, ver4);

    //! Special cases
    //! M100:
    if (strcmp(versionStruct.hwVersion, M100) == 0)
    {
        //! Bug in M100 does not report the right FW.
        ver3                    = 10 * ver3;
        versionStruct.fwVersion = FW(ver1, ver2, ver3, ver4);
    }
    //! M600/A3 FW 3.2.10
    if (versionStruct.fwVersion == FW(3, 2, 10, 0))
    {
        //! Bug in M600 does not report the right FW.
        ver3                    = 10 * ver3;
        versionStruct.fwVersion = FW(ver1, ver2, ver3, ver4);
    }

    //! Now, we can parse the CRC and ID based on FW version. If it's older than
    //! 3.2 then it'll have a CRC, else not.
    if (versionStruct.fwVersion < FW(3, 2, 0, 0))
    {
        versionStruct.version_crc =
            crc_id[0] + (crc_id[1] << 8) + (crc_id[2] << 16) + (crc_id[3] << 24);
        uint8_t *id_ptr = &crc_id[4];

        int i = 0;
        while (*id_ptr != '\0')
        {
            versionStruct.hw_serial_num[i] = *id_ptr;
            i++;
            id_ptr++;
            if (id_ptr - &crc_id[4] > 12)
            {
                return false; //! Not catastrophic error
            }
        }
        //! Fill in the termination character
        versionStruct.hw_serial_num[i] = *id_ptr;
    }
    else
    {
        versionStruct.version_crc = 0;
        uint8_t *id_ptr           = &crc_id[0];

        int i = 0;
        while (*id_ptr != '\0')
        {
            versionStruct.hw_serial_num[i] = *id_ptr;
            i++;
            id_ptr++;
            if (id_ptr - &crc_id[0] > 16)
            {
                return false; //! Not catastrophic error
            }
        }
        //! Fill in the termination character
        versionStruct.hw_serial_num[i] = *id_ptr;
    }

    //! Finally, we print stuff out.

    if (versionStruct.fwVersion > FW(3, 1, 0, 0))
    {
        MY_DEBUG("Device Serial No. = %.16s\n", versionStruct.hw_serial_num);
    }
    MY_DEBUG("Hardware = %.12s\n", versionStruct.hwVersion);
    MY_DEBUG("Firmware = %d.%d.%d.%d\n", ver1, ver2, ver3, ver4);
    if (versionStruct.fwVersion < FW(3, 2, 0, 0))
    {
        MY_DEBUG("Version CRC = 0x%X\n", versionStruct.version_crc);
    }

    *versionData = versionStruct;
    return true;
}

static int 
callbackIdIndex(void)
{
    if (callbackId == 199)
    {
        callbackId = 0;
        return 0;
    }
    else
    {
        callbackId++;
        return callbackId;
    }
}

static FirmWare 
getFwVersion()
{
	return vehicle->versionData.fwVersion;
}

static bool
getEncryption()
{
    return vehicle->encrypt;
}
