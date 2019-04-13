#ifndef _OPEN_PROTOCOL_H
#define _OPEN_PROTOCOL_H

#include "type.h"
#include "aes.h"
#include "MMU.h"

#define SET_CMD_SIZE (2u)

#define BUFFER_SIZE  1024
#define ACK_SESSION_IDLE 0
#define ACK_SESSION_PROCESS 1
#define ACK_SESSION_USING 2
#define CMD_SESSION_0 0
#define CMD_SESSION_1 1
#define CMD_SESSION_AUTO 32

#define POLL_TICK 20 // unit is ms

#define SOF        0xAA
#define CRCHead    (sizeof(uint16_t))
#define CRCData    (sizeof(uint32_t))
#define CRCHeadLen (sizeof(OpenHeader) - CRCHead)
#define PackageMin (sizeof(OpenHeader) + CRCData)

//#define     ACK_SIZE    10
//#define     SOF         0xAA
//#define     CRCHead     sizeof((uint16_t))
//#define     CRCData     sizeof((uint32_t))
//#define     CRCHeadLen  (sizeof((OpenHeader)) - (CRCHead))
//#define     PackageMin  (sizeof((OpenHeader)) + (CRCData))

#define _SDK_U32_SET(_addr, _val) (*((uint32_t*)(_addr)) = (_val))
#define _SDK_U16_SET(_addr, _val) (*((uint16_t*)(_addr)) = (_val))

// point function 
typedef void(*initFunc)(void);
// DJI c++  OpenProtocol of send
typedef void(*sendFunc)(uint8_t,bool,const uint8_t[],void*,size_t,int,int,bool,int);
typedef int(*sendDataFunc)(uint8_t*);
typedef RecvContainer*(*getFunc)(void);
typedef bool(*receFunc)(const uint8_t);
typedef void(*setKeyFunc)(const char*);
typedef void(*TwoByteFunc)(const char*, uint8_t*);

//class
typedef struct OpenProtocol{ // �ࣺopenProtocol
	// function
	initFunc     openProtocolInit;      // ��ʼ��
	sendFunc     openProtocolSend;      // ���ͺ���
	sendDataFunc openProtocolSendData;  // �������ݺ��� 
	getFunc      getReceivedFrame;
	receFunc     byteHandler;           // ���ڽ�������
	setKeyFunc   setKey;
	TwoByteFunc  transformTwoByte;
	// variate
	uint8_t*     encodeSendData;      
  int          buf_read_pos;
  int          read_len;
  uint8_t*     buf;
  uint8_t      HEADER_LEN;
  int          MAX_RECV_LEN;
  RecvContainer* p_recvContainer; // TODO ������
  SDKFilter*   p_filter;  
	CMDSession   CMDSessionTab[SESSION_TABLE_NUM];
  ACKSession   ACKSessionTab[SESSION_TABLE_NUM - 1];
	//! Frame-related.
  uint32_t ackFrameStatus;
  bool     broadcastFrameStatus;
  uint8_t* rawFrame;
	//! Memory management
  MMU*     mmu;
  //! Frame-related.
  uint16_t seq_num;
	bool reuse_buffer;
}OpenProtocol;

//function
void externOpenProtocolInit(void);  // ���ڳ�ʼ������

static void interOpenProtocolInit(void);
// ���ڽ���
bool byteHandler(const uint8_t in_data);

static bool streamHandler(uint8_t in_data);

static void storeData(uint8_t in_data);

static bool checkStream(void);

static bool verifyHead(void);

static bool verifyData(void);

static int crcHeadCheck(uint8_t* pMsg, size_t nLen);

static bool callApp(void);

static bool appHandler(void* protocolHeader);

static bool recvReqData(OpenHeader* protocolHeader);

static void shiftDataStream(void);

static int crcTailCheck(uint8_t* pMsg, size_t nLen);

static void reuseDataStream(void);

static void prepareDataStream(void);

static TypeUnion allocateACK(OpenHeader* protocolHeader);

static void setACKFrameStatus(uint32_t usageFlag);

static uint8_t getCmdSet(OpenHeader* protocolHeader);

static uint8_t getCmdCode(OpenHeader* protocolHeader);
// ���ڷ���
static void openProtocolSend(uint8_t session_mode, bool is_enc, const uint8_t cmd[], void* pdata,
            size_t len, int timeout, int retry_time,
            bool hasCallback, int callbackID);
//�������ݷ���
static int  openProtocolSendData(uint8_t* buf);

//����
static int sendInterface(void* cmd_container);

//�����ݷ��͵�����
size_t sendUsart(const uint8_t* buf, size_t len);

//���㷢�ͳ���
static uint16_t calculateLength(uint16_t size, uint16_t encrypt_flag);

//��ʼ���ڴ�
static void setup(void);

//��ʼ������
static void setupSession(void);

//�������
static CMDSession* allocSession(uint16_t session_id, uint16_t size);

//�ͷŻ���
static void freeSession(CMDSession* session);

//�������ݱ���
/****************************** Encryption ******************************/
static uint16_t encrypt(uint8_t* pdest, const uint8_t* psrc, uint16_t w_len,
                   uint8_t is_ack, uint8_t is_enc, uint8_t session_id,
                   uint16_t seq_num);
static void encodeData(OpenHeader* p_head, ptr_aes256_codec codec_func);



//CRC
static void calculateCRC(void* p_data);

static uint16_t crc16Calc(const uint8_t* pMsg, size_t nLen);

static uint32_t crc32Calc(const uint8_t* pMsg, size_t nLen);

static uint16_t crc16Update(uint16_t crc, uint8_t ch);

static uint32_t crc32Update(uint32_t crc, uint8_t ch);


static void setHeaderLength(uint8_t length); // ���� "ͷ" ��
 
static void setMaxRecvLength(int length);    // ���������ճ���

static void setRawFrame(uint8_t* p_header);

static RecvContainer* getReceivedFrame(void);

static void transformTwoByte(const char* pstr, uint8_t* pdata);

static void setKey(const char* key);

#endif
