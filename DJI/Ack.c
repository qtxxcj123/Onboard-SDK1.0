#include "Ack.h"
#include "malloc.h"
#define dataSize(x) sizeof(x) / sizeof(x[0])
extern const uint8_t setControl[];
extern const uint8_t setArm[];
extern const uint8_t control[];
extern const uint8_t task[];
extern const uint8_t killSwitch[];
extern const uint8_t pauseResume[];
extern const uint8_t get[];
extern const FirmWare M100_31;

//ack key and value set
AckMap missionData[] = {
	{ MissionACK_Common_AT_NO_FLY_ZONE,
	               (const char*)"MISSION_AT_NO_FLY_ZONE\n" },
	{ MissionACK_Common_BAD_GPS,
	               (const char*)"MISSION_BAD_GPS\n" },
	{ MissionACK_Common_RTK_NOT_READY,
                 (const char*)"MISSION_RTK_NOT_READY\n" },
  { MissionACK_Common_BEGGINER_MODE_NOT_SUPPORTED,
	               (const char*)"MISSION_BEGGINER_MODE_NOT_SUPPORTED\n" }, 
  { MissionACK_Common_CLOSE_IOC_REQUIRED,
                 (const char*)"MISSION_CLOSE_IOC_REQUIRED\n" },
  { MissionACK_Common_CONDITIONS_NOT_SATISFIED,
                 (const char*)"MISSION_CONDITIONS_NOT_SATISFIED\n" },
  { MissionACK_Common_CROSSING_NO_FLY_ZONE,
                 (const char*)"MISSION_CROSSING_NO_FLY_ZONE\n"},
  { MissionACK_Common_INVALID_COMMAND,
                 (const char*)"MISSION_INVALID_COMMAND\n"},
  { MissionACK_Common_INVALID_PARAMETER,
                 (const char*)"MISSION_INVALID_PARAMETER\n" },
  { MissionACK_Common_IN_PROGRESS,
                 (const char*)"MISSION_IN_PROGRESS\n" },
  { MissionACK_Common_LANDING_IN_PROGRESS,
                 (const char*)"MISSION_LANDING_IN_PROGRESS\n" },
  { MissionACK_Common_LOW_BATTERY,
                 (const char*)"MISSION_LOW_BATTERY\n"},
  { MissionACK_Common_NOT_INITIALIZED,
                 (const char*)"MISSION_NOT_INITIALIZED\n" },
  { MissionACK_Common_NOT_RUNNING,
                 (const char*)"MISSION_NOT_RUNNING\n"},
  { MissionACK_Common_NOT_SUPPORTED,
                 (const char*)"MISSION_NOT_SUPPORTED\n"},
  { MissionACK_Common_OBTAIN_CONTROL_REQUIRED,
                 (const char*)"MISSION_OBTAIN_CONTROL_REQUIRED\n" },
  { MissionACK_Common_OTHER_MISSION_RUNNING,
                 (const char*)"OTHER_MISSION_RUNNING\n" },
  { MissionACK_Common_RC_NOT_IN_MODE_F,
                 (const char*)"MISSION_RC_NOT_IN_MODE_F\n" },
  { MissionACK_Common_RRETURN_HOME_IN_PROGRESS,
                 (const char*)"MISSION_RRETURN_HOME_IN_PROGRESS\n" },
  { MissionACK_Common_START_MOTORS_IN_PROGRESS,
                 (const char*)"MISSION_START_MOTORS_IN_PROGRESS\n" },
  { MissionACK_Common_SUCCESS,
                 (const char*)"MISSION_SUCCESS\n" },
  { MissionACK_Common_TAKE_OFF_IN_PROGRESS,
                 (const char*)"MISSION_TAKE_OFF_IN_PROGRESS\n" },
  { MissionACK_Common_TASK_TIMEOUT,
                 (const char*)"MISSION_TASK_TIMEOUT\n" },
  { MissionACK_Common_TOO_FAR_FROM_CURRENT_POSITION,
                 (const char*)"MISSION_TOO_FAR_FROM_CURRENT_POSITION\n" },
  { MissionACK_Common_TOO_FAR_FROM_HOME,
                 (const char*)"MISSION_TOO_FAR_FROM_HOME\n" },
  { MissionACK_Common_TOO_HIGH,
                 (const char*)"MISSION_TOO_HIGH\n" },
  { MissionACK_Common_TOO_LOW,
                 (const char*)"MISSION_TOO_LOW\n" },
  { MissionACK_Common_UNKNOWN_ERROR,
                 (const char*)"MISSION_UNKNOWN_ERROR\n" },
  { MissionACK_Common_UNRECORDED_HOME,
                 (const char*)"MISSION_UNRECORDED_HOME\n" },
  { MissionACK_Common_VEHICLE_DID_NOT_TAKE_OFF,
                 (const char*)"MISSION_VEHICLE_DID_NOT_TAKE_OFF\n" },
  { MissionACK_Common_WRONG_WAYPOINT_INDEX,
                 (const char*)"MISSION_WRONG_WAYPOINT_INDEX\n" },
  { MissionACK_Follow_CUTOFF_TIME_OVERFLOW,
                 (const char*)"FOLLOW_MISSION_CUTOFF_TIME_OVERFLOW\n" },
  { MissionACK_Follow_GIMBAL_PITCH_ANGLE_OVERFLOW,
                 (const char*)"FOLLOW_MISSION_GIMBAL_PITCH_ANGLE_OVERFLOW\n" },
  { MissionACK_Follow_TOO_FAR_FROM_YOUR_POSITION_LACK_OF_RADIO_CONNECTION,
                 (const char*)"FOLLOW_MISSION_TOO_FAR_FROM_YOUR_POSITION_LACK_"
                              "OF_RADIO_CONNECTION\n" },
  { MissionACK_HotPoint_FAILED_TO_PAUSE,
                 (const char*)"HOTPOINT_MISSION_FAILED_TO_PAUSE\n" },
  { MissionACK_HotPoint_INVALID_DIRECTION,
                 (const char*)"HOTPOINT_MISSION_INVALID_DIRECTION\n" },
  { MissionACK_HotPoint_INVALID_LATITUDE_OR_LONGITUTE,
                 (const char*)"HOTPOINT_MISSION_INVALID_LATITUDE_OR_LONGITUTE\n" },
  { MissionACK_HotPoint_INVALID_PAREMETER,
                 (const char*)"HOTPOINT_MISSION_INVALID_PAREMETER\n" },
  { MissionACK_HotPoint_INVALID_RADIUS,
                 (const char*)"HOTPOINT_MISSION_INVALID_RADIUS\n" },
  { MissionACK_HotPoint_INVALID_START_POINT,
                 (const char*)"HOTPOINT_MISSION_INVALID_VISION\n" },
  { MissionACK_HotPoint_INVALID_YAW_MODE,
                 (const char*)"HOTPOINT_MISSION_INVALID_YAW_MODE\n" },
  { MissionACK_HotPoint_IN_PAUSED_MODE,
                 (const char*)"HOTPOINT_MISSION_IN_PAUSED_MODE\n" },
  { MissionACK_HotPoint_TOO_FAR_FROM_HOTPOINT,
                 (const char*)"HOTPOINT_MISSION_TOO_FAR_FROM_HOTPOINT\n" },
  { MissionACK_HotPoint_YAW_RATE_OVERFLOW,
                 (const char*)"HOTPOINT_MISSION_YAW_RATE_OVERFLOW\n" },
  { MissionACK_WayPoint_CHECK_FAILED,
                 (const char*)"WAYPOINT_MISSION_CHECK_FAILED\n" },
  { MissionACK_WayPoint_DATA_NOT_ENOUGH,
                 (const char*)"WAYPOINT_MISSION_DATA_NOT_ENOUGH\n" },
  { MissionACK_WayPoint_TRACE_TOO_LONG,
                 (const char*)"WAYPOINT_MISSION_DISTANCE_OVERFLOW\n" },
  { MissionACK_WayPoint_INVALID_ACTION,
                 (const char*)"WAYPOINT_MISSION_INVALID_ACTION\n" },
  { MissionACK_WayPoint_INVALID_DATA,
                 (const char*)"WAYPOINT_MISSION_INVALID_DATA\n" },
  { MissionACK_WayPoint_INVALID_POINT_DATA,
                 (const char*)"WAYPOINT_MISSION_INVALID_POINT_DATA\n" },
  { MissionACK_WayPoint_INVALID_VELOCITY,
                 (const char*)"WAYPOINT_MISSION_INVALID_VELOCITY\n" },
  { MissionACK_WayPoint_IN_PROGRESS,
                 (const char*)"MISSION_IN_PROGRESS\n" },
  { MissionACK_WayPoint_NOT_IN_PROGRESS,
                 (const char*)"WAYPOINT_MISSION_NOT_IN_PROGRESS\n" },
  { MissionACK_WayPoint_POINTS_NOT_ENOUGH,
                 (const char*)"WAYPOINT_MISSION_POINTS_NOT_ENOUGH\n" },
  { MissionACK_WayPoint_POINTS_TOO_CLOSE,
                 (const char*)"WAYPOINT_MISSION_POINTS_TOO_CLOSE\n" },
  { MissionACK_WayPoint_POINTS_TOO_FAR,
                 (const char*)"WAYPOINT_MISSION_POINTS_TOO_FAR\n" },
  { MissionACK_WayPoint_POINT_DATA_NOT_ENOUGH,
                 (const char*)"WAYPOINT_MISSION_POINT_DATA_NOT_ENOUGH\n" },
  { MissionACK_WayPoint_POINT_OVERFLOW,
                 (const char*)"WAYPOINT_MISSION_POINT_OVERFLOW\n" },
  { MissionACK_WayPoint_TOTAL_DISTANCE_TOO_LONG,
                 (const char*)"WAYPOINT_MISSION_TIMEOUT\n" },
  { MissionACK_IOC_TOO_CLOSE_TO_HOME,
                 (const char*)"IOC_MISSION_TOO_CLOSE_TO_HOME\n" },
  { MissionACK_IOC_UNKNOWN_TYPE,
                 (const char*)"IOC_MISSION_UNKNOWN_TYPE\n" }
};

const AckMap activateData[] = {
  { ActivationACK_SUCCESS,
                 (const char*)"ACTIVATE_SUCCESS\n" },
  { ActivationACK_ACCESS_LEVEL_ERROR,
                 (const char*)"ACCESS_LEVEL_ERROR\n" },
  { ActivationACK_DJIGO_APP_NOT_CONNECTED,
                 (const char*)"DJIGO_APP_NOT_CONNECTED_ERROR\n" },
  { ActivationACK_ENCODE_ERROR,
                 (const char*)"ENCODE_ERROR\n" },
  { ActivationACK_NETWORK_ERROR,
                 (const char*)"NETWORK_ERROR\n" },
  { ActivationACK_NEW_DEVICE_ERROR,
                 (const char*)"NEW_DEVICE_ERROR\n" },
  { ActivationACK_OSDK_VERSION_ERROR,
                 (const char*)"OSDK_VERSION_ERROR\n" },
  { ActivationACK_PARAMETER_ERROR,
                 (const char*)"PARAMETER_ERROR\n" },
  { ActivationACK_SERVER_ACCESS_REFUSED,
                 (const char*)"SERVER_ACCESS_REFUSED\n" }
};


Ack* ack;
void 
externAckInit(void)
{
	ack = (Ack*)mymalloc(sizeof(Ack));
	ack->init = interiAckInit;
	ack->getError = getError;
	ack->getErrorCodeMessage = getErrorCodeMessage;
}

static void 
interiAckInit(void)
{

}

static bool
getError(ErrorCode ack)
{
  const uint8_t cmd[] = { ack.info.cmd_set, ack.info.cmd_id };

  if (ack.info.cmd_set == ACTIVATION)
  {
    return (ack.data == ActivationACK_SUCCESS)
             ? ACK_SUCCESS
             : ACK_FAIL;
  }
  else if (ack.info.cmd_set == BROADCAST)
  {
    // Push Data, no ACK
  }
  else if (memcmp(cmd, setControl,
                  sizeof(cmd)) == 0)
  {
    if (ack.info.buf[2] == ACK_OBTAIN_CONTROL)
    { //! Data is set at buf + SET_CMD_SIZE which is buf + 2;
      // Release control was called
      return (ack.data == ControlACK_SetControl_RELEASE_CONTROL_SUCCESS)
               ? ACK_SUCCESS
               : ACK_FAIL;
    }
    else if (ack.info.buf[2] == ACK_OBTAIN_CONTROL)
    {
      // Obtain control was called
      return (ack.data == ControlACK_SetControl_OBTAIN_CONTROL_SUCCESS)
               ? ACK_SUCCESS
               : ACK_FAIL;
    }
  }
  else if (memcmp(cmd, setArm, sizeof(cmd)) ==
           0)
  {
    /*
     * SetArm command supported in Matrice 100/ M600 old firmware
     */
    return (ack.data == ControlACK_SetArm_SUCCESS)
             ? ACK_SUCCESS
             : ACK_FAIL;
  }
  else if (memcmp(cmd, control,
                  sizeof(cmd)) == 0)
  {
    // Does not return an ACK
  }
  else if (memcmp(cmd, task, sizeof(cmd)) ==
           0)
  {
    if (ack.info.version == FW(3,2,15,62))
    {
      //! ACKs supported in Matrice 600 old firmware
      return (ack.data ==
          ControlACK_LegacyTask_SUCCESS)
             ? ACK_SUCCESS
             : ACK_FAIL;
    }
    else if (ack.info.version != M100_31)
    {
      return (ack.data == ControlACK_Task_SUCCESS)
               ? ACK_SUCCESS
               : ACK_FAIL;
    }
    else
    {
      //! ACKs supported in Matrice 100
      return (ack.data ==
              ControlACK_LegacyTask_SUCCESS)
               ? ACK_SUCCESS
               : ACK_FAIL;
    }
  }
  else if (memcmp(cmd, killSwitch, sizeof(cmd)) ==
           0)
  {
    return (ack.data ==
      ControlACK_KillSwitch_SUCCESS)
           ? ACK_SUCCESS : ACK_FAIL;
  }
  else if (ack.info.cmd_set == MISSION)
  {
    return (ack.data == MissionACK_Common_SUCCESS)
             ? ACK_SUCCESS
             : ACK_FAIL;
  }
  else if (ack.info.cmd_set == HARDWARE_SYNC)
  {
    // Verify ACK
  }
  else if (ack.info.cmd_set == VIRTUA_IRC)
  {
    // Deprecated in 3.2.20

    // TODO implement for backward compatibility
  }
  else if (ack.info.cmd_set == SUBSCRIBE)
  {
    if (memcmp(cmd, pauseResume,
               sizeof(cmd)) == 0)
    {
      return (ack.data == SubscribeACK_PAUSED ||
              ack.data == SubscribeACK_RESUMED)
               ? ACK_SUCCESS
               : ACK_FAIL;
    }
    else
    {
      return (ack.data == SubscribeACK_SUCCESS)
               ? ACK_SUCCESS
               : ACK_FAIL;
    }
  }
  else if (ack.info.cmd_set == MFIO)
  {
    if (memcmp(cmd, get, sizeof(cmd)) == 0)
    {
      return (ack.data == MFIOACK_get_SUCCESS)
               ? ACK_SUCCESS
               : ACK_FAIL;
    }
    else
    {
      return (ack.data == MFIOACK_init_SUCCESS)
               ? ACK_SUCCESS
               : ACK_FAIL;
    }
  }

  return ACK_FAIL;
}

static void
getErrorCodeMessage(ErrorCode ack, const char* func)
{
  MY_DEBUG("%s", func);
  switch (ack.info.cmd_set)
  {
    case ACTIVATION:
      // CMD_ID agnostic
      getCMDSetActivationMSG(ack);
      break;
    case CONTROL:
      // Get message by CMD_ID
      getCMDSetControlMSG(ack);
      break;
    case BROADCAST:
      getSetBroadcastMSG(ack);
      break;
    case MISSION:
      // CMD_ID agnostic
      getCMDSetMissionMSG(ack);
      break;
    case HARDWARE_SYNC:
      getCMDSetSyncMSG(ack);
      break;
    case VIRTUA_IRC:
      getCMDSetVirtualRCMSG(ack);
      break;
    case MFIO:
      getCMDSetMFIOMSG(ack);
      break;
    case SUBSCRIBE:
      // CMD_ID agnostic
      getCMDSetSubscribeMSG(ack);
      break;
    default:
      getCommonErrorCodeMessage(ack);
      break;
  }
}

static void
getCMDSetActivationMSG(ErrorCode ack)
{
	MY_DEBUG("getCMDSetActivationMSG");
	findActivateMeg(ack.data);
}

static void 
getCMDSetControlMSG(ErrorCode ack)
{
	MY_DEBUG("getCMDSetControlMSG");
}
static void
getSetBroadcastMSG(ErrorCode ack)
{
	MY_DEBUG("getSetBroadcastMSG");
}

static void
getCMDSetMissionMSG(ErrorCode ack)
{
	MY_DEBUG("getCMDSetMissionMSG");
}

static void
getCMDSetSyncMSG(ErrorCode ack)
{
	MY_DEBUG("getCMDSetSyncMSG");
}

static void
getCMDSetVirtualRCMSG(ErrorCode ack)
{
	MY_DEBUG("getCMDSetVirtualRCMSG");
}

static void
getCMDSetMFIOMSG(ErrorCode ack)
{
	MY_DEBUG("getCMDSetMFIOMSG");
}

static void
getCMDSetSubscribeMSG(ErrorCode ack)
{
	MY_DEBUG("getCMDSetSubscribeMSG");
}

static void
getCommonErrorCodeMessage(ErrorCode ack)
{
	MY_DEBUG("getCommonErrorCodeMessage");
}

static void
findActivateMeg(uint32_t ackData)
{
	uint8_t i = 0;
	for(i = 0; i < dataSize(activateData);i++)
	{
		if(ackData == activateData->key)break;
	}
	MY_DEBUG("%s",activateData[i].value);
}
