#include "MissionManager.h"
#include "malloc.h"
#include "Vehicle.h"
//static(interior) function statement
static ErrorCode init(DJI_MISSION_TYPE type, int timeout, UserData missionData);
static ErrorCode initWayptMission(int timeout, UserData wayptData);
static void missionCallback(Vehicle* vehiclePtr, RecvContainer recvFrame,UserData userData);

//global variable
MissionManager* missionManager;

//extern variable
extern Vehicle* vehicle;

void externManagerInit(void)
{
	missionManager = (MissionManager*)mymalloc(sizeof(MissionManager));
	missionManager->init = init;
	missionManager->wayptCounter = 0;
	missionManager->missionCallback = missionCallback;
}

static ErrorCode 
init(DJI_MISSION_TYPE type, int timeout, UserData missionData)
{
  if (type == WAYPOINT)
  {
    initWayptMission(timeout, missionData);
  }
  else
  {
    MY_DEBUG("Cannot recognize the mission type provided\n");
  }
}

static ErrorCode
initWayptMission(int timeout, UserData wayptData)
{
  missionManager->wpMissionArray[missionManager->wayptCounter] = (WaypointMission*)mymalloc(sizeof(WaypointMission));
  missionManager->wpMission                    = missionManager->wpMissionArray[missionManager->wayptCounter];
  missionManager->wayptCounter++;
	externWaypointInit(missionManager->wpMission);

  missionManager->wpMission->init((WayPointInitSettings*)wayptData,timeout);
}

static void 
missionCallback(Vehicle* vehiclePtr, RecvContainer recvFrame,UserData userData)
{
	char           func[50];
  ErrorCode ack;

  if (recvFrame.recvInfo.len - PackageMin <=
      sizeof(ErrorCode))
  {
    ack.info = recvFrame.recvInfo;
    ack.data = recvFrame.recvData.missionACK;

    if (vehicle->ack->getError(ack))
    {
      vehicle->ack->getErrorCodeMessage(ack, func);
    }
  }
  else
  {
    MY_DEBUG("ACK is exception,sequence %d\n", recvFrame.recvInfo.seqNumber);
  }
}
