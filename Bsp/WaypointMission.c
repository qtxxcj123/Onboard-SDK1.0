#include "WaypointMission.h"
#include "Vehicle.h"
#include "MissionManager.h"
// static(interior) function statement
static void init(WayPointInitSettings* Info, VehicleCallBack callback,UserData userData);
static void setInfo(WayPointInitSettings value);
static bool uploadIndexData(WayPointSettings* data,VehicleCallBack callback, UserData userData);
static void setIndex(WayPointSettings* value, size_t pos);
static void uploadIndexDataCallback(Vehicle* vehicle,RecvContainer recvFrame,UserData userData);
static void start(VehicleCallBack callback, UserData userData);
// static(interior) variable statement
static WayPointInitSettings info;
static WayPointSettings*    index;
//extern variable statement
extern Vehicle* vehicle;
extern MissionManager* missionManager;
extern const uint8_t waypointInit[];
extern const uint8_t waypointAddPoint[];
extern const uint8_t waypointSetStart[];

void externWaypointInit(WaypointMission* waypoint)
{
	waypoint->init = init;
	waypoint->uploadIndexData = uploadIndexData;
	waypoint->start = start;
}

static void
init(WayPointInitSettings* Info, VehicleCallBack callback,
                      UserData userData)
{
  if (Info)
    setInfo(*Info);

  int cbIndex = vehicle->callbackIdIndex();
  if (callback)
  {
    vehicle->nbCallbackFunctions[cbIndex] = (void*)callback;
    vehicle->nbUserData[cbIndex]          = userData;
  }
  else
  {
    vehicle->nbCallbackFunctions[cbIndex] = missionManager->missionCallback;
    vehicle->nbUserData[cbIndex] = NULL;
  }
  vehicle->protocolLayer->openProtocolSend(2, vehicle->getEncryption(),waypointInit,
                               &info, sizeof(info), 500, 2, true, cbIndex);
}

static void
setInfo(WayPointInitSettings value)
{
  //! @todo set information for way point
  info = value;
  for (int i         = 0; i < 16; ++i)
    info.reserved[i] = 0;
}

static bool
uploadIndexData(WayPointSettings* data,
                                 VehicleCallBack callback, UserData userData)
{
  int cbIndex = vehicle->callbackIdIndex();
  if (callback)
  {
    vehicle->nbCallbackFunctions[cbIndex] = (void*)callback;
    vehicle->nbUserData[cbIndex]          = userData;
  }
  else
  {
    vehicle->nbCallbackFunctions[cbIndex] = uploadIndexDataCallback;
    vehicle->nbUserData[cbIndex] = NULL;
  }
  
  vehicle->protocolLayer->openProtocolSend(2, vehicle->getEncryption(), waypointAddPoint, data,
                                      sizeof(*data), 1000, 4, true, cbIndex);
  return true;
}

static void
setIndex(WayPointSettings* value, size_t pos)
{
  if (index == 0)
  {
    //index = new WayPointSettings[info.indexNumber];
    if (index == NULL)
    {
      MY_DEBUG("Lack of memory\n");
      return;
    }
  }
  index[pos] = *value;
  for (int i               = 0; i < 8; ++i)
    index[pos].reserved[i] = 0;
}

static void
uploadIndexDataCallback(Vehicle*      vehicle,
                                         RecvContainer recvFrame,
                                         UserData      userData)
{
  WayPointAddPointInternal wpDataInfo;

  if (recvFrame.recvInfo.len - PackageMin <=
      sizeof(WayPointAddPointInternal))
  {
    wpDataInfo = recvFrame.recvData.wpAddPointACK;
  }
  else
  {
    MY_DEBUG("ACK is exception, sequence %d\n", recvFrame.recvInfo.seqNumber);
    return;
  }

  ErrorCode ack;
  ack.data = wpDataInfo.ack;
  ack.info = recvFrame.recvInfo;

  if (vehicle->ack->getError(ack))
    vehicle->ack->getErrorCodeMessage(ack, __func__);

  MY_DEBUG("Index number: %d\n", wpDataInfo.index);
}

static void
start(VehicleCallBack callback, UserData userData)
{
  uint8_t start = 0;

  int cbIndex = vehicle->callbackIdIndex();
  if (callback)
  {
    vehicle->nbCallbackFunctions[cbIndex] = (void*)callback;
    vehicle->nbUserData[cbIndex]          = userData;
  }
  else
  {
    vehicle->nbCallbackFunctions[cbIndex] = missionManager->missionCallback;
    vehicle->nbUserData[cbIndex] = NULL;
  }

  vehicle->protocolLayer->openProtocolSend(
    2, vehicle->getEncryption(), waypointSetStart, &start,
    sizeof(start), 500, 2, true, cbIndex);
}

