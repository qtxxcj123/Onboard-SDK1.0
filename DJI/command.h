#ifndef _COMMAND_H
#define _COMMAND_H
#include "type.h"
#define MAX_CMD_ARRAY_SIZE  2
typedef struct Activation {
  const  uint8_t getVersion[MAX_CMD_ARRAY_SIZE];
  const  uint8_t activate[MAX_CMD_ARRAY_SIZE];
  const  uint8_t frequency[MAX_CMD_ARRAY_SIZE];
  const  uint8_t toMobile[MAX_CMD_ARRAY_SIZE];
} Activation;

typedef struct Broadcast {
  const  uint8_t broadcast[MAX_CMD_ARRAY_SIZE];
  const  uint8_t fromMobile[MAX_CMD_ARRAY_SIZE];
  const  uint8_t lostCTRL[MAX_CMD_ARRAY_SIZE];
  const  uint8_t mission[MAX_CMD_ARRAY_SIZE];
  const  uint8_t subscribe[MAX_CMD_ARRAY_SIZE];
  const  uint8_t test[MAX_CMD_ARRAY_SIZE];
  const  uint8_t waypoint[MAX_CMD_ARRAY_SIZE];
} Broadcast;

typedef struct Control {
  const  uint8_t setControl[MAX_CMD_ARRAY_SIZE];
  const  uint8_t task[MAX_CMD_ARRAY_SIZE];
  // CMD_ID_STATUS Not used at all
  const  uint8_t status[MAX_CMD_ARRAY_SIZE];
  const  uint8_t control[MAX_CMD_ARRAY_SIZE];
  // CMD_ID_SETARM Supported on Matrice 100, A3, N3
  // with firmware version < 3.3
  const  uint8_t setArm[MAX_CMD_ARRAY_SIZE];
  const  uint8_t killSwitch[MAX_CMD_ARRAY_SIZE];
  const  uint8_t gimbalSpeed[MAX_CMD_ARRAY_SIZE];
  const  uint8_t gimbalAngle[MAX_CMD_ARRAY_SIZE];
  const  uint8_t cameraShot[MAX_CMD_ARRAY_SIZE];
  const  uint8_t cameraVideoStart[MAX_CMD_ARRAY_SIZE];
  const  uint8_t cameraVideoStop[MAX_CMD_ARRAY_SIZE];
} Control;

typedef struct Mission {
  // Waypoint mission commands
  const  uint8_t waypointInit[MAX_CMD_ARRAY_SIZE];
  const  uint8_t waypointAddPoint[MAX_CMD_ARRAY_SIZE];
  const  uint8_t waypointSetStart[MAX_CMD_ARRAY_SIZE];
  const  uint8_t waypointSetPause[MAX_CMD_ARRAY_SIZE];
  const  uint8_t waypointDownload[MAX_CMD_ARRAY_SIZE];
  const  uint8_t waypointIndexDownload[MAX_CMD_ARRAY_SIZE];
  const  uint8_t waypointSetVelocity[MAX_CMD_ARRAY_SIZE];
  const  uint8_t waypointGetVelocity[MAX_CMD_ARRAY_SIZE];
  // Hotpint mission commands
  const  uint8_t hotpointStart[MAX_CMD_ARRAY_SIZE];
  const  uint8_t hotpointStop[MAX_CMD_ARRAY_SIZE];
  const  uint8_t hotpointSetPause[MAX_CMD_ARRAY_SIZE];
  const  uint8_t hotpointYawRate[MAX_CMD_ARRAY_SIZE];
  const  uint8_t hotpointRadius[MAX_CMD_ARRAY_SIZE];
  const  uint8_t hotpointSetYaw[MAX_CMD_ARRAY_SIZE];
  const  uint8_t hotpointDownload[MAX_CMD_ARRAY_SIZE];
  // Follow mission commands
  const  uint8_t followStart[MAX_CMD_ARRAY_SIZE];
  const  uint8_t followStop[MAX_CMD_ARRAY_SIZE];
  const  uint8_t followSetPause[MAX_CMD_ARRAY_SIZE];
  const  uint8_t followTarget[MAX_CMD_ARRAY_SIZE];
} Mission;

typedef struct HardwareSync {
  const  uint8_t broadcast[MAX_CMD_ARRAY_SIZE];
} HardwareSync;

typedef struct VirtualRC {
  const  uint8_t settings[MAX_CMD_ARRAY_SIZE];
  const  uint8_t data[MAX_CMD_ARRAY_SIZE];
} VirtualRC;

typedef struct MFIO_ {
  const  uint8_t init[MAX_CMD_ARRAY_SIZE];
  const  uint8_t get[MAX_CMD_ARRAY_SIZE];
  const  uint8_t set[MAX_CMD_ARRAY_SIZE];
} MFIO_;

typedef struct Subscribe {
  const  uint8_t versionMatch[MAX_CMD_ARRAY_SIZE];
  const  uint8_t addPackage[MAX_CMD_ARRAY_SIZE];
  const  uint8_t reset[MAX_CMD_ARRAY_SIZE];
  const  uint8_t removePackage[MAX_CMD_ARRAY_SIZE];
  // TODO implement API call
  const  uint8_t updatePackageFreq[MAX_CMD_ARRAY_SIZE];
  const  uint8_t pauseResume[MAX_CMD_ARRAY_SIZE];
  // TODO implement API call
  const  uint8_t getConfig[MAX_CMD_ARRAY_SIZE];
} Subscribe;

#endif
