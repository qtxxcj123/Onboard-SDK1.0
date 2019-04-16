#include "Vehicle.h"
#include "Activate.h"
#include "delay.h"
#include "BspUsart.h"
#include "usart.h"	
bool           isFrame = false;
RecvContainer *rFrame;
extern Vehicle* vehicle;
extern FirmWare extendedVersionBase;
extern const FirmWare M100_31;
void
delay_nms(uint16_t time)
{
    u32 i = 0;
    while (time--)
    {
        i = 30000;
        while (i--);
    }
}

int main(void)
{
	externVehicleInit();
	vehicle->init();
	SystickConfig();
	USART1_Confug(0);
	USART3_Config(0);
	NVIC_Config();
	vehicle->getDroneVersion(0,0);
	extendedVersionBase = FW(3, 2, 36, 6);
	if (vehicle->getFwVersion() > 0 &&
            vehicle->getFwVersion() < extendedVersionBase &&
            vehicle->getFwVersion() != M100_31)
  {
        MY_DEBUG("Upgrade firmware using Assistant software!\r\n");
        //delete (v);
        return false;
  }
	userActivate();
  delay_nms(500);
	if (vehicle->getFwVersion() != M100_31)
  {
      vehicle->subscribe->verify();
      delay_nms(500);
  }
	vehicle->obtainCtrlAuthority(0,0);
	while(1)
	{
		printf("init OK \r\n");
		delay_nms(5000);
	}
}
void setWaypointInitDefaults(WayPointInitSettings* fdata);
void setWaypointDefaults(WayPointSettings* wp);
void uploadingWaypoint(WayPointSettings* wp);

void waypointHandler()
{
	WayPointInitSettings fdata;
	setWaypointInitDefaults(&fdata);
	fdata.indexNumber = 3;
	vehicle->missionManager->init(WAYPOINT,1,&fdata);
	WayPointSettings wp;
	setWaypointDefaults(&wp);
	delay_nms(500);
	wp.index = 0;
	//经纬度 - 添加
	// TODO
  delay_nms(500);
	uploadingWaypoint(&wp);
	wp.index = 1;
	//经纬度 - 添加
	// TODO
	delay_nms(500);
	uploadingWaypoint(&wp);
	wp.index = 2;
	//经纬度 - 添加
	// TODO
	delay_nms(500);
	uploadingWaypoint(&wp);
	delay_nms(500);
	vehicle->missionManager->wpMission->start(0,0);
	
}

void setWaypointInitDefaults(WayPointInitSettings* fdata)
{
	fdata->maxVelocity    = 10;
  fdata->idleVelocity   = 5;
  fdata->finishAction   = 0;
  fdata->executiveTimes = 1;
  fdata->yawMode        = 0;
  fdata->traceMode      = 0;
  fdata->RCLostAction   = 1;
  fdata->gimbalPitch    = 0;
  fdata->latitude       = 0;
  fdata->longitude      = 0;
  fdata->altitude       = 0;
}

void setWaypointDefaults(WayPointSettings* wp)
{
  wp->damping         = 0;
  wp->yaw             = 0;
  wp->gimbalPitch     = 0;
  wp->turnMode        = 0;
  wp->hasAction       = 0;
  wp->actionTimeLimit = 100;
  wp->actionNumber    = 0;
  wp->actionRepeat    = 0;
  for (int i = 0; i < 16; ++i)
  {
    wp->commandList[i]      = 0;
    wp->commandParameter[i] = 0;
		if(i < 8)
			wp->reserved[i] = 0;
  }
}

void uploadingWaypoint(WayPointSettings* wp)
{
	static int Num = 0;
	printf("\n\r位置(%d) %f %f %f\n\r", Num, wp->latitude * 57.295780, wp->longitude * 57.295780, wp->altitude);
	vehicle->missionManager->wpMission->uploadIndexData(wp,0,0);
	Num++;
}


