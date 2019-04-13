#include "Ack.h"
#include "malloc.h"
extern const uint8_t setControl[];
extern const uint8_t setArm[];
extern const uint8_t control[];
extern const uint8_t task[];
extern const uint8_t killSwitch[];
extern const uint8_t pauseResume[];
extern const uint8_t get[];
extern const FirmWare M100_31;

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
