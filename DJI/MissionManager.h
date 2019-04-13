#ifndef MISSIONM_MANAGER_H
#define MISSIONM_MANAGER_H
#include "type.h"
#include "WaypointMission.h"

struct Vehicle;
#define MAX_MISSION_SIZE  5
typedef enum DJI_MISSION_TYPE
{
  WAYPOINT = 0,
  HOTPOINT = 1,
}DJI_MISSION_TYPE;

typedef enum MISSION_ACTION
{
  START  = 0,
  STOP   = 1,
  PAUSE  = 2,
  RESUME = 3,
}MISSION_ACTION;

typedef ErrorCode(*managerInit)(DJI_MISSION_TYPE, int, UserData); // ≥ı º
typedef void(*managerCallback)(struct Vehicle*, RecvContainer,UserData);
typedef struct MissionManager {
// function pointer
	managerInit init;
	managerCallback missionCallback;
// variable
	int wayptCounter;
	WaypointMission* wpMission; 
	WaypointMission* wpMissionArray[MAX_MISSION_SIZE];
}MissionManager;

void externManagerInit(void);

#endif
