#ifndef TYPE_H
#define TYPE_H

#include <cstdio>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#define SESSION_TABLE_NUM  32
/**
 * @note size is in Bytes
 */
#define MAX_INCOMING_DATA_SIZE  300
#define MAX_ACK_SIZE            107

#define MY_DEBUG(format,...) printf("FILE: "__FILE__", LINE: %d: "format"\r\n", __LINE__, ##__VA_ARGS__)

typedef uint64_t time_ms;

typedef unsigned int size_t;

typedef float  float32_t;
typedef double float64_t;

#pragma pack(1)
typedef struct ActivateData{ //激活数据
    uint32_t ID;
    uint32_t reserved;
    uint32_t version;
    uint8_t  iosID[32]; //! @note useless
    char*    encKey;
} ActivateData; // pack(1)
#pragma pack()

typedef struct MMU_Tab {
  uint32_t tabIndex : 8;
  uint32_t usageFlag : 8;
  uint32_t memSize : 16;
  uint8_t* pmem;
} MMU_Tab;

typedef struct OpenHeader {
  uint32_t sof : 8;
  uint32_t length : 10;
  uint32_t version : 6;
  uint32_t sessionID : 5;
  uint32_t isAck : 1;
  uint32_t reserved0 : 2; // always 0
  uint32_t padding : 5;
  uint32_t enc : 3;
  uint32_t reserved1 : 24;
  uint32_t sequenceNumber : 16;
  uint32_t crc : 16;
} OpenHeader;

typedef struct CMDSession {
  uint8_t  cmd_set;
  uint8_t  cmd_id;
  uint8_t* buf;
  uint32_t sessionID : 5;
  uint32_t usageFlag : 1;
  uint32_t sent : 5;
  uint32_t retry : 5;
  uint32_t timeout : 16;
  MMU_Tab* mmu;  // 内存管理，考虑使用自己的嘛？
  bool     isCallback;
  int      callbackID;
  uint32_t preSeqNum;
  time_ms  preTimestamp;
} CMDSession;

typedef struct ACKSession {
  uint32_t sessionID : 5;
  uint32_t sessionStatus : 2;
  uint32_t res : 25;
  MMU_Tab* mmu;
} ACKSession;

typedef struct SDKFilter {
  uint32_t reuseIndex;
  uint32_t reuseCount;
  uint32_t recvIndex;
  uint8_t* recvBuf;
  uint8_t  sdkKey[32]; // for encryption
  uint8_t  encode;
} SDKFilter;

// 航点相关结构体
#pragma pack(1)
typedef struct HotPointSettings {
  uint8_t   version;   /*!< Reserved, kept as 0 */
  float64_t latitude;  /*!< Latitude (radian) */
  float64_t longitude; /*!< Longitude (radian) */
  float64_t height;    /*!< Altitude (relative altitude from takeoff point */
  float64_t radius;    /*!< Radius (5m~500m) */
  float32_t yawRate;   /*!< Angle rate (0~30°/s) */
  uint8_t   clockwise; /*!< 0->fly in counter-clockwise direction, 1->clockwise direction */
  uint8_t startPoint;  /*!< Start point position <br>*/
  /*!< 0: north to the hot point <br>*/
  /*!< 1: south to the hot point <br>*/
  /*!< 2: west to the hot point <br>*/
  /*!< 3: east to the hot point <br>*/
  /*!< 4: from current position to nearest point on the hot point */
  uint8_t yawMode; /*!< Yaw mode <br>*/
  /*!< 0: point to velocity direction <br>*/
  /*!< 1: face inside <br>*/
  /*!< 2: face ouside <br>*/
  /*!< 3: controlled by RC <br>*/
  /*!< 4: same as the starting yaw<br> */
  uint8_t reserved[11]; /*!< Reserved */
} HotPointSettings;     // pack(1)

/**
 * @brief Waypoint Mission Initialization settings
 * @details This is one of the few structs in the OSDK codebase that
 * is used in both a sending and a receiving API.
 */
typedef struct WayPointInitSettings {
  uint8_t   indexNumber; /*!< Total number of waypoints <br>*/
  float32_t maxVelocity; /*!< Maximum speed joystick input(2~15m) <br>*/
  float32_t idleVelocity; /*!< Cruising Speed */
  /*!< (without joystick input, no more than vel_cmd_range) */
  uint8_t finishAction; /*!< Action on finish <br>*/
  /*!< 0: no action <br>*/
  /*!< 1: return to home <br>*/
  /*!< 2: auto landing <br>*/
  /*!< 3: return to point 0 <br>*/
  /*!< 4: infinite mode， no exit <br>*/
  uint8_t executiveTimes; /*!< Function execution times <br>*/
  /*!< 1: once <br>*/
  /*!< 2: twice <br>*/
  uint8_t yawMode; /*!< Yaw mode <br>*/
  /*!< 0: auto mode(point to next waypoint) <br>*/
  /*!< 1: lock as an initial value <br>*/
  /*!< 2: controlled by RC <br>*/
  /*!< 3: use waypoint's yaw(tgt_yaw) */
  uint8_t traceMode; /*!< Trace mode <br>*/
  /*!< 0: point to point, after reaching the target waypoint hover, 
   * complete waypoints action (if any), 
   * then fly to the next waypoint <br>
   * 1: Coordinated turn mode, smooth transition between waypoints,
   * no waypoints task <br>
   */
  uint8_t RCLostAction; /*!< Action on rc lost <br>*/
  /*!< 0: exit waypoint and failsafe <br>*/
  /*!< 1: continue the waypoint <br>*/
  uint8_t gimbalPitch; /*!< Gimbal pitch mode <br>*/
  /*!< 0: free mode, no control on gimbal <br>*/
  /*!< 1: auto mode, Smooth transition between waypoints <br>*/
  float64_t latitude;     /*!< Focus latitude (radian) */
  float64_t longitude;    /*!< Focus longitude (radian) */
  float32_t altitude;     /*!< Focus altitude (relative takeoff point height) */
  uint8_t   reserved[16]; /*!< Reserved, must be set to 0 */

} WayPointInitSettings; // pack(1)

typedef struct WayPointSettings {
  uint8_t   index;     /*!< Index to be uploaded */
  float64_t latitude;  /*!< Latitude (radian) */
  float64_t longitude; /*!< Longitude (radian) */
  float32_t altitude;  /*!< Altitude (relative altitude from takeoff point) */
  float32_t damping; /*!< Bend length (effective coordinated turn mode only) */
  int16_t   yaw;     /*!< Yaw (degree) */
  int16_t   gimbalPitch; /*!< Gimbal pitch */
  uint8_t   turnMode;    /*!< Turn mode <br> */
  /*!< 0: clockwise <br>*/
  /*!< 1: counter-clockwise <br>*/
  uint8_t reserved[8]; /*!< Reserved */
  uint8_t hasAction;   /*!< Action flag <br>*/
  /*!< 0: no action <br>*/
  /*!< 1: has action <br>*/
  uint16_t actionTimeLimit;      /*!< Action time limit */
  uint8_t  actionNumber : 4;     /*!< Total number of actions */
  uint8_t  actionRepeat : 4;     /*!< Total running times */
  uint8_t  commandList[16];      /*!< Command list */
  uint16_t commandParameter[16]; /*!< Command parameters */
} WayPointSettings;              // pack(1)
#pragma pack()

//----- ack 相关
typedef uint32_t FirmWare;
#pragma pack(1)
typedef struct Entry {
  uint8_t           cmd_set;
  uint8_t           cmd_id;
  uint16_t          len;
  uint8_t*          buf;
  uint8_t           seqNumber;
  FirmWare          version;
} Entry; // pack(1)
typedef struct HotPointStartInternal{
  uint8_t   ack;
  float32_t maxRadius;
} HotPointStartInternal; // pack(1)

typedef struct HotPointReadInternal {
  uint8_t          ack;
  HotPointSettings data;

    // TODO fix/remove once verified with FC team
  uint8_t extraByte;
} HotPointReadInternal; // pack(1)

typedef struct WayPointAddPointInternal {
  uint8_t ack;
  uint8_t index;
} WayPointAddPointInternal; // pack(1)

typedef struct WayPointIndexInternal {
  uint8_t          ack;
  WayPointSettings data;
} WayPointIndexInternal; // pack(1)

typedef struct WayPointVelocityInternal {
  uint8_t   ack;
  float32_t idleVelocity;
} WayPointVelocityInternal; // pack(1)

typedef struct WayPointInitInternal {
  uint8_t              ack;
  WayPointInitSettings data;
} WayPointInitInternal; // pack(1)

typedef struct MFIOGetInternal{
  uint8_t  result;
  uint32_t value;
} MFIOGetInternal; // pack(1)

  /*
   * ACK structures exposed to user
   */

  /*! @brief This struct is returned from all blocking calls, except certain
   * mission calls that have explicit types defined later in this file
   * @note NOT TO BE CONFUSED with class \ref DJI::OSDK::ErrorCode "ErrorCode"
   * that contains parsing for acknowledgements
   *
   */
typedef struct ErrorCode {
  Entry    info;
  uint32_t data;
} ErrorCode; // pack(1)

typedef struct MFIOGet {
  ErrorCode ack;
  uint32_t  value;
} MFIOGet; // pack(1)

  /*! @brief This struct is returned from the DJI::OSDK::HotpointMission::start
   * blocking API
   *
   */
typedef struct HotPointStart {
  ErrorCode ack;
  float32_t maxRadius;
} HotPointStart; // pack(1)

  /*! @brief This struct is returned from the DJI::OSDK::HotpointMission::read
   * blocking API
   *
   */
typedef struct HotPointRead {
  ErrorCode        ack;
  HotPointSettings data;

    // TODO fix/remove once verified with FC team
  uint8_t extraByte;
} HotPointRead; // pack(1)

  /*! @brief This struct is returned from the
   * DJI::OSDK::WaypointMission::waypointIndexDownload blocking API
   *
   */
typedef struct WayPointIndex {
  ErrorCode        ack;
  WayPointSettings data;
} WayPointIndex; // pack(1)

  /*! @brief This struct is returned from the
   * DJI::OSDK::WaypointMission::uploadIndexData blocking API
   *
   */
typedef struct WayPointAddPoint {
  ErrorCode ack;
  uint8_t   index;
} WayPointAddPoint; // pack(1)

  /*! @brief This struct is returned from the
   * DJI::OSDK::WaypointMission::updateIdleVelocity blocking API
   *
   */
typedef struct WayPointVelocity {
  ErrorCode ack;
  float32_t idleVelocity;
} WayPointVelocity; // pack(1)

  /*! @brief This struct is used in the readInitData non-blocking API callback
   *
   */
typedef struct WayPointInit {
  ErrorCode            ack;
  WayPointInitSettings data;
} WayPointInit; // pack(1)

  /*! @brief This struct is returned from the
   * DJI::OSDK::Vehicle::getDroneVersion blocking API
   *
   */
typedef struct VersionData {
  uint16_t version_ack;
  uint32_t version_crc;
  char     hw_serial_num[16];
  char     hwVersion[12]; //! Current longest product code: pm820v3pro
  FirmWare fwVersion;

    //! Legacy member
  char version_name[32];
} VersionData;

typedef struct DroneVersion {
  ErrorCode       ack;
  VersionData data;
} DroneVersion; // pack(1)

  /*!
   * @brief This struct captures PushData while ground-station is enabled on
   * Assistant's SDK Page, CMD: 0x02, 0x04
   */
typedef struct WayPointReachedData {
  uint8_t incident_type;  /*! see WayPointIncidentType */
  uint8_t waypoint_index; /*! the index of current waypt mission */
  uint8_t current_status; /*! 4: pre-action, 6: post-action */
  uint8_t reserved_1;
  uint8_t reserved_2;
} WayPointReachedData; // pack(1)

  /*!
   * @brief This struct captures PushData while ground-station is enabled on
   * Assistant's SDK Page, CMD: 0x02, 0x03
   */
typedef struct WayPointStatusPushData {
  uint8_t mission_type;      /*! see WayPointPushDataIncidentType */
  uint8_t waypoint_index;    /*! the index of current waypt mission */
  uint8_t current_status;    /*! 0: pre-mission, 1: in-action, 5: first waypt , 6: reached */
  uint8_t error_notification;
  uint16_t reserved_1;
} WayPointStatusPushData;
  /*!
   * @brief This constant variable defines number of pixels for QVGA images
   */
#define IMG_240P_SIZE  240 * 320
typedef uint8_t  Image[IMG_240P_SIZE];
  /*!
   * @brief sub-struct for stereo image with raw data and camera name
   */
typedef struct ImageMeta {
  Image image;
  char  name[12];
} ImageMeta; // pack(1)
  /*!
   * @brief This struct captures PushData when subscribe to QVGA images
   */
typedef struct StereoImgData {
  uint32_t frame_index;
  uint32_t time_stamp;
  uint8_t  num_imgs;
    /*
     * There could be 50 different kinds of images coming from the drone,
     * 5 camera pairs and 10 images types.
     * Here we use an uint64_t to describe which image is coming
     * from the USB line, each bit represents if there's data or not
     * Please use AdvancedSensing::ReceivedImgDesc to match them
     * For M210, we support up to 4 images at the same time
     */
  uint64_t img_desc;
    // @note for M210, at most 4 imgs come at the same time.
  ImageMeta img_vec[4];
} StereoImgData; // pack(1)
  /*!
   * @brief This constant variable defines number of pixels for VGA images
   */
#define IMG_VGA_SIZE 640 * 480
typedef uint8_t  VGAImage[IMG_VGA_SIZE];
  /*!
   * @brief This struct captures PushData when subscribe to VGA images
   */
typedef struct StereoVGAImgData {
  uint32_t frame_index;
  uint32_t time_stamp;
  uint8_t  num_imgs;
  uint8_t  direction;
    // @note VGA imgs always come in pair
  VGAImage img_vec[2];
} StereoVGAImgData; // pack(1)

typedef union TypeUnion {
  uint8_t  raw_ack_array[MAX_INCOMING_DATA_SIZE];
  uint8_t  versionACK[MAX_ACK_SIZE];
  uint16_t ack;
  uint8_t  commandACK;
  uint8_t  missionACK;
  uint8_t  subscribeACK;
  uint8_t  mfioACK;

    /*
     * ACK(s) containing ACK data plus extra payload
     */
  HotPointStartInternal    hpStartACK;
  HotPointReadInternal     hpReadACK;
  WayPointInitInternal     wpInitACK;
  WayPointAddPointInternal wpAddPointACK;
  WayPointIndexInternal    wpIndexACK;
  WayPointVelocityInternal wpVelocityACK;
  MFIOGetInternal          mfioGetACK;

    /*
     * Push Data in ground-station mode
     */
  WayPointReachedData wayPointReachedData;
  WayPointStatusPushData wayPointStatusPushData;

    /*
     * Push Data from AdvancedSensing protocol
     */
  StereoImgData           *stereoImgData;
  StereoVGAImgData        *stereoVGAImgData;

} TypeUnion; // pack(1)

#pragma pack()

typedef struct DispatchInfo {
  bool    isAck;
  bool    isCallback;
  uint8_t callbackID;
} DispatchInfo;

typedef struct RecvContainer {
  Entry          recvInfo;
  TypeUnion      recvData;
  DispatchInfo   dispatchInfo;
} RecvContainer;

typedef struct Command
{
  uint16_t sessionMode : 2;
  uint16_t encrypt : 1;
  uint16_t retry : 13;
  uint16_t timeout; // unit is ms
  size_t   length;
  uint8_t* buf;
  uint8_t  cmd_set;
  uint8_t  cmd_id;
  bool     isCallback;
  int      callbackID;
} Command;

//crc
extern const uint16_t crc_tab16[];
extern const uint32_t crc_tab32[];
extern const uint8_t CRC8_TAB[256];
extern const uint16_t wCRC_Table[256];
#define  CRC8_INIT   0x77
#define  CRC16_INIT  0x3692
#define  CRC_INIT    0x3AA3

struct Vehicle;
typedef void* UserData;
typedef void (*VehicleCallBack)(struct Vehicle* vehicle, RecvContainer recvFrame,UserData userData);
typedef struct VehicleCallBackHandler{  //车辆回调处理程序
	 //车辆回调
  VehicleCallBack callback;
  UserData        userData;
} VehicleCallBackHandler;

#define ACTIVATION    0x00
#define CONTROL       0x01
#define BROADCAST     0x02
#define MISSION       0x03
#define HARDWARE_SYNC 0x04
#define VIRTUA_IRC    0x05
#define MFIO          0x09
#define SUBSCRIBE     0x0B
//ACK
#pragma pack(1)
typedef struct WayPointAddPoints {
    ErrorCode ack;
    uint8_t   index;
} WayPointAddPoints; // pack(1)

typedef struct WayPointInits {
    ErrorCode            ack;
    WayPointInitSettings data;
} WayPointInits; // pack(1)

typedef struct WayPointIndexs {
    ErrorCode        ack;
    WayPointSettings data;
} WayPointIndexs; // pack(1)

typedef struct HotPointStarts {
    ErrorCode ack;
    float32_t maxRadius;
} HotPointStarts; // pack(1)

typedef struct HotPointReads {
    ErrorCode        ack;
    HotPointSettings data;

    // TODO fix/remove once verified with FC team
    uint8_t extraByte;
} HotPointReads; // pack(1)

typedef struct DroneVersions {
    ErrorCode       ack;
    VersionData     data;
} DroneVersions; // pack(1)

typedef struct MFIOGets {
    ErrorCode ack;
    uint32_t  value;
} MFIOGets; // pack(1)
#pragma pack()


#endif
