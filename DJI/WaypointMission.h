#ifndef WAYPOINT_MISSION_H
#define WAYPOINT_MISSION_H
#include "type.h"

struct WaypointMission;
typedef ErrorCode(*waypointInitFunc)(WayPointInitSettings*, int);
typedef bool(*uploadDataFunc)(WayPointSettings*,VehicleCallBack, UserData); 
typedef void(*startFunc)(VehicleCallBack callback, UserData userData);

typedef struct WaypointMission{
	waypointInitFunc init;
	uploadDataFunc uploadIndexData;
	startFunc start;
	//stop
	//pause
	//resume
}WaypointMission;

void externWaypointInit(WaypointMission* waypoint);

#endif
