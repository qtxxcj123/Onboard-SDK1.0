#include "OpenProtocol.h"
#include "malloc.h"
#include "BspUsart.h"
extern MMU mmuPtr;
OpenProtocol* protocolLayer;
extern uint64_t tick;

void 
externOpenProtocolInit()
{
	protocolLayer = (OpenProtocol*)mymalloc(sizeof(OpenProtocol));
	protocolLayer->openProtocolInit     = interOpenProtocolInit;
	protocolLayer->openProtocolSend     = openProtocolSend;
	protocolLayer->openProtocolSendData = openProtocolSendData;
	protocolLayer->getReceivedFrame     = getReceivedFrame;
	protocolLayer->byteHandler          = byteHandler;
	protocolLayer->setKey               = setKey;
	protocolLayer->transformTwoByte     = transformTwoByte;
	setHeaderLength(sizeof(OpenHeader));
	setMaxRecvLength(BUFFER_SIZE);
} // externOpenProtocolInit

static void 
interOpenProtocolInit(void)
{
	protocolLayer->p_recvContainer      = (RecvContainer*)mymalloc(sizeof(RecvContainer));
	protocolLayer->seq_num              = 0;
  protocolLayer->ackFrameStatus       = 11;
  protocolLayer->broadcastFrameStatus = false;
	protocolLayer->p_filter             = (SDKFilter*)mymalloc(sizeof(SDKFilter));
	protocolLayer->p_filter->recvIndex  = 0;
  protocolLayer->p_filter->reuseCount = 0;
  protocolLayer->p_filter->reuseIndex = 0;
  protocolLayer->p_filter->encode     = 0;
  protocolLayer->p_filter->recvBuf    = (uint8_t*)mymalloc(protocolLayer->MAX_RECV_LEN);
	protocolLayer->buf                  = (uint8_t*)mymalloc(BUFFER_SIZE);
  protocolLayer->encodeSendData       = (uint8_t*)mymalloc(BUFFER_SIZE);
	
	protocolLayer->mmu          = &mmuPtr;
  protocolLayer->buf_read_pos = 0;
  protocolLayer->read_len     = 0;
	protocolLayer->reuse_buffer = false;
	
	
	setup();
} // interOpenProtocolInit

static void 
setup(void)
{
	protocolLayer->mmu->setupMMU(protocolLayer->mmu);
	setupSession();
} // setup

static void
setupSession(void)
{
	uint32_t i;
  for (i = 0; i < SESSION_TABLE_NUM; i++)
  {
    protocolLayer->CMDSessionTab[i].sessionID = i;
    protocolLayer->CMDSessionTab[i].usageFlag = 0;
    protocolLayer->CMDSessionTab[i].mmu       = (MMU_Tab*)NULL;
  }

  for (i = 0; i < (SESSION_TABLE_NUM - 1); i++)
  {
    protocolLayer->ACKSessionTab[i].sessionID     = i + 1;
    protocolLayer->ACKSessionTab[i].sessionStatus = ACK_SESSION_IDLE;
    protocolLayer->ACKSessionTab[i].mmu           = (MMU_Tab*)NULL;
  }
} // setupSession

static CMDSession* 
allocSession(uint16_t session_id, uint16_t size)
{
	uint32_t i; 
	       //分配大小
  //DDEBUG("Allocation size %d", size);
  MMU_Tab* memoryTab = NULL;

  if (session_id == 0 || session_id == 1)
  {
    if ( protocolLayer->CMDSessionTab[session_id].usageFlag == 0)
      i = session_id;
    else
    {
      /* session is busy   会话正在忙*/
      MY_DEBUG("session %d is busy\n", session_id);
      return NULL;
    }
  }
  else
  {
    for (i = 2; i < SESSION_TABLE_NUM; i++)
      if (protocolLayer->CMDSessionTab[i].usageFlag == 0)
        break;
  }
  if (i < 32 &&  protocolLayer->CMDSessionTab[i].usageFlag == 0)
  {
     protocolLayer->CMDSessionTab[i].usageFlag = 1;
    memoryTab                  =  protocolLayer->mmu->allocMemory(protocolLayer->mmu,size);
    if (memoryTab == NULL)
       protocolLayer->CMDSessionTab[i].usageFlag = 0;
    else
    {
      protocolLayer->CMDSessionTab[i].mmu = memoryTab;
      return &protocolLayer->CMDSessionTab[i];
    }
  }
  return NULL;
} //allocSession

static void 
freeSession(CMDSession* session)
{
  if (session->usageFlag == 1)
  {
    MY_DEBUG("session id %d\n", session->sessionID);
    protocolLayer->mmu->freeMemory(protocolLayer->mmu,session->mmu);
    session->usageFlag = 0;
  }
} //freeSession

static void 
openProtocolSend(uint8_t session_mode, bool is_enc, const uint8_t cmd[], void* pdata,
            size_t len, int timeout, int retry_time,
            bool hasCallback, int callbackID)
{
  Command  cmdContainer;
  uint8_t* ptemp = (uint8_t*)protocolLayer->encodeSendData;
  *ptemp++       = cmd[0];
  *ptemp++       = cmd[1];

  memcpy(protocolLayer->encodeSendData + SET_CMD_SIZE, pdata, len);

  cmdContainer.sessionMode = session_mode;  //会话模式
  cmdContainer.length      = len + SET_CMD_SIZE; //Len+set_cmd大小
  cmdContainer.buf         = protocolLayer->encodeSendData;  //编码数据
  cmdContainer.cmd_set     = cmd[0]; // cmd set
  cmdContainer.cmd_id      = cmd[1]; // cmd id
  cmdContainer.retry       = retry_time;

  cmdContainer.timeout = timeout;
  cmdContainer.encrypt = is_enc ? 1 : 0;

  //! Callback  回调
  cmdContainer.isCallback = hasCallback;
  cmdContainer.callbackID = callbackID;

  sendInterface((void*)&cmdContainer);
} //openProtocolSend

static int sendInterface(void* cmd_container)
{
  uint16_t    ret          = 0;
  CMDSession* cmdSession   = (CMDSession*)NULL;
  Command*    cmdContainer = (Command*)cmd_container;
  if (cmdContainer->length > PRO_PURE_DATA_MAX_SIZE)
  {
    MY_DEBUG("ERROR,length=%lu is over-sized\n", cmdContainer->length);
    return -1;
  }
  /*! Switch on session to decide whether the command is requesting an ACK and
   * whether it is requesting
   *  guarantees on transmission
   */

  switch (cmdContainer->sessionMode)
  {
    case 0:
      //! No ACK required and no retries
      cmdSession =
        allocSession(CMD_SESSION_0, calculateLength(cmdContainer->length,
                                                    cmdContainer->encrypt));

      if (cmdSession == (CMDSession*)NULL)
      {
        MY_DEBUG("ERROR,there is not enough memory\n");
        return -1;
      }
      //! Encrypt the data being sent
      ret =
        encrypt(cmdSession->mmu->pmem, cmdContainer->buf, cmdContainer->length,
                0, cmdContainer->encrypt, cmdSession->sessionID, protocolLayer->seq_num);
      if (ret == 0)
      {
        MY_DEBUG("encrypt ERROR\n");
        freeSession(cmdSession);
        return -1;
      }

      MY_DEBUG("send data in session mode 0\n");

      //! Actually send the data
      protocolLayer->openProtocolSendData(cmdSession->mmu->pmem);
      protocolLayer->seq_num++;
      freeSession(cmdSession);
      break;

    case 1:
      //! ACK required; Session 1; will retry until failure
      cmdSession =
        allocSession(CMD_SESSION_1, calculateLength(cmdContainer->length,
                                                    cmdContainer->encrypt));
      if (cmdSession == (CMDSession*)NULL)
      {
        MY_DEBUG("ERROR,there is not enough memory\n");
        return -1;
      }
      if (protocolLayer->seq_num == cmdSession->preSeqNum)
      {
        protocolLayer->seq_num++;
      }
      ret =
        encrypt(cmdSession->mmu->pmem, cmdContainer->buf, cmdContainer->length,
                0, cmdContainer->encrypt, cmdSession->sessionID, protocolLayer->seq_num);
      if (ret == 0)
      {
        //DERROR("encrypt ERROR\n"); //加密错误
        freeSession(cmdSession);
        return -1;
      }
      cmdSession->preSeqNum = protocolLayer->seq_num++;

      //@todo replace with a bool
      cmdSession->isCallback = cmdContainer->isCallback;
      cmdSession->callbackID = cmdContainer->callbackID;
      cmdSession->timeout =
        (cmdContainer->timeout > POLL_TICK) ? cmdContainer->timeout : POLL_TICK;
      cmdSession->preTimestamp = getTimeStamp();//deviceDriver->getTimeStamp();
      cmdSession->sent         = 1;
      cmdSession->retry        = 1;
			         //发送会话
      MY_DEBUG("sending session %d\n", cmdSession->sessionID);
      protocolLayer->openProtocolSendData(cmdSession->mmu->pmem);
      break;

    case 2:
      //! ACK required, Sessions 2 - END; no guarantees and no retries.  要求，会话2结束；没有保证，没有重试。
      cmdSession =  allocSession(CMD_SESSION_AUTO, 
											calculateLength(cmdContainer->length,cmdContainer->encrypt));
      if (cmdSession == (CMDSession*)NULL)
      {
				        //错误，内存不足
        MY_DEBUG("ERROR,there is not enough memory\n");
        return -1;
      }
      if (protocolLayer->seq_num == cmdSession->preSeqNum)
      {
        protocolLayer->seq_num++;
      }
      ret =
        encrypt(cmdSession->mmu->pmem, cmdContainer->buf, cmdContainer->length,
                0, cmdContainer->encrypt, cmdSession->sessionID, protocolLayer->seq_num);

      if (ret == 0)
      {
				      //加密错误
        MY_DEBUG("encrypt ERROR");
        freeSession(cmdSession);
        return -1;
      }

      // To use in ErrorCode manager  用于错误代码管理器
      cmdSession->cmd_set = cmdContainer->cmd_set;
      cmdSession->cmd_id  = cmdContainer->cmd_id;
      // Will carry information: obtain/release control  将携带信息：获取/释放控制
      cmdSession->buf = cmdContainer->buf;

      cmdSession->preSeqNum = protocolLayer->seq_num++;
      //@todo replace with a bool  用双关替换
      cmdSession->isCallback = cmdContainer->isCallback;
      cmdSession->callbackID = cmdContainer->callbackID;
      cmdSession->timeout =
        (cmdContainer->timeout > POLL_TICK) ? cmdContainer->timeout : POLL_TICK;
      cmdSession->preTimestamp = getTimeStamp();//deviceDriver->getTimeStamp();
      cmdSession->sent         = 1;
      cmdSession->retry        = cmdContainer->retry;
			     //发送会话
      MY_DEBUG("Sending session %d\n", cmdSession->sessionID);
      protocolLayer->openProtocolSendData(cmdSession->mmu->pmem);
      break;
    default:
			        //未知模式
      MY_DEBUG("Unknown mode:%d\n", cmdContainer->sessionMode);
      break;
  }
  return 0;
} //sendInterface

// !Step 2
bool 
byteHandler(const uint8_t in_data)
{
	protocolLayer->p_filter->reuseCount = 0;
  protocolLayer->p_filter->reuseIndex = protocolLayer->MAX_RECV_LEN;

  //! Bool to check if the protocol parser has finished a full frame  检查协议分析器是否完成了完整的框架
  bool isFrame = streamHandler(in_data);

  /*! @note Just think a command as below
    *
    * [123456HHD1234567===HHHH------------------] --- is buf un-used part
    *
    * if after recv full of above, but crc failed, we throw all data?
    * NO!
    * Just throw ONE BYTE, we move like below
    *
    * [123456HH------------------D1234567===HHHH]
    *
    * Use the buffer high part to re-loop, try to find a new command
    *
    * if new cmd also fail, and buf like below
    *
    * [56HHD1234567----------------------===HHHH]
    *
    * throw one byte, buf looks like
    *
    * [6HHD123-----------------------4567===HHHH]
    *
    * the command tail part move to buffer right
    * */
  if (protocolLayer->reuse_buffer)
  {
    if (protocolLayer->p_filter->reuseCount != 0)
    {
      while (protocolLayer->p_filter->reuseIndex < protocolLayer->MAX_RECV_LEN)
      {
        /*! @note because reuse_index maybe re-located, so reuse_index must
         * be
         *  always point to un-used index
         *  re-loop the buffered data
         *  */
        isFrame = streamHandler(protocolLayer->p_filter->recvBuf[protocolLayer->p_filter->reuseIndex++]);
      }
      protocolLayer->p_filter->reuseCount = 0;
    }
  }
  return isFrame;
} //byteHandler

//! Step 3
static bool
streamHandler(uint8_t in_data)
{
  storeData(in_data);
  //! Bool to check if the protocol parser has finished a full frame
  bool isFrame = checkStream();
  return isFrame;
} //streamHandler

//! Step 4
//! @note push data to filter buffer.  按数据过滤缓冲区
//! SDKFilter is just a buffer.   过滤器只是一个缓冲区。
static void
storeData(uint8_t in_data)
{
  if (protocolLayer->p_filter->recvIndex < protocolLayer->MAX_RECV_LEN)
  {
    protocolLayer->p_filter->recvBuf[protocolLayer->p_filter->recvIndex] = in_data;
    protocolLayer->p_filter->recvIndex++;
		//DERROR("p_filter->recvBuf[p_filter->recvIndex] = %c",p_filter->recvBuf[p_filter->recvIndex]);
  }
  else
  {
    //DERROR("buffer overflow");
    memset(protocolLayer->p_filter->recvBuf, 0, protocolLayer->p_filter->recvIndex);
    protocolLayer->p_filter->recvIndex = 0;
  }
} //storeData

//! Step 5
static bool
checkStream()
{
  OpenHeader* p_head = (OpenHeader*)(protocolLayer->p_filter->recvBuf);
  //! Bool to check if the protocol parser has finished a full frame
  bool isFrame = false;
  if (protocolLayer->p_filter->recvIndex < sizeof(OpenHeader))
  {
    // Continue receive data, nothing to do
    return false;
  }
  else if (protocolLayer->p_filter->recvIndex == sizeof(OpenHeader))
  {
    // recv a full-head
    isFrame = verifyHead();
  }
  else if (protocolLayer->p_filter->recvIndex == p_head->length)
  {
    isFrame = verifyData();
  }
  return isFrame;
} //checkStream

//! Step 6
static bool
verifyHead(void)
{
  OpenHeader* p_head = (OpenHeader*)(protocolLayer->p_filter->recvBuf);

  //! Bool to check if the protocol parser has finished a full frame
  bool isFrame = false;

  if ((p_head->sof == SOF) && (p_head->version == 0) &&
      (p_head->length < protocolLayer->MAX_RECV_LEN) &&
      (p_head->reserved0 == 0) && (p_head->reserved1 == 0) &&
      (crcHeadCheck((uint8_t*)p_head, sizeof(OpenHeader)) == 0))
  {
    // check if this head is a ack or simple package
    if (p_head->length == sizeof(OpenHeader))
    {
      isFrame = callApp();
    }
  }
  else
  {
    shiftDataStream();
  }
  return isFrame;
}

//! Step 7
static bool
verifyData(void)
{
  OpenHeader* p_head = (OpenHeader*)(protocolLayer->p_filter->recvBuf);

  //! Bool to check if the protocol parser has finished a full frame
  bool isFrame = false;

  if (crcTailCheck((uint8_t*)p_head, p_head->length) == 0)
  {
    isFrame = callApp();
  }
  else
  {
    //! @note data crc fail, re-use the data part
    reuseDataStream();
  }
  return isFrame;
}

//! Step 8
static bool
callApp(void)
{
  // pass current data to handler
  OpenHeader* p_head = (OpenHeader*)protocolLayer->p_filter->recvBuf;

  encodeData(p_head, aes256_decrypt_ecb);
  bool isFrame = appHandler((OpenHeader*)protocolLayer->p_filter->recvBuf);
  prepareDataStream();

  return isFrame;
}

static bool
appHandler(void* protocolHeader)
{
//! @todo Filter replacement
#ifdef API_TRACE_DATA
  printFrame(serialDevice, openHeader, false);
#endif

  OpenHeader* p2protocolHeader;
  OpenHeader* openHeader = (OpenHeader*)protocolHeader;
  //! Bool to check if the protocol parser has finished a full frame
  bool isFrame = false;

  if (openHeader->isAck == 1)
  {
    //! Case 0: This is an ACK frame that came in.
    if (openHeader->sessionID > 1 && openHeader->sessionID < 32)
    {
      //! Session is valid
      if (protocolLayer->CMDSessionTab[openHeader->sessionID].usageFlag == 1)
      {
        //! Session in use
        p2protocolHeader =
          (OpenHeader*)protocolLayer->CMDSessionTab[openHeader->sessionID].mmu->pmem;
        if (p2protocolHeader->sessionID == openHeader->sessionID &&
            p2protocolHeader->sequenceNumber == openHeader->sequenceNumber)
        {
          MY_DEBUG("Recv Session %d ACK\n", p2protocolHeader->sessionID);

          //! Create receive container for error code management
          protocolLayer->p_recvContainer->dispatchInfo.isAck = true;
          protocolLayer->p_recvContainer->recvInfo.cmd_set =
            protocolLayer->CMDSessionTab[openHeader->sessionID].cmd_set;
          protocolLayer->p_recvContainer->recvInfo.cmd_id =
            protocolLayer->CMDSessionTab[openHeader->sessionID].cmd_id;
          protocolLayer->p_recvContainer->recvData = allocateACK(openHeader);
          protocolLayer->p_recvContainer->dispatchInfo.isCallback =
            protocolLayer->CMDSessionTab[openHeader->sessionID].isCallback;
          protocolLayer->p_recvContainer->dispatchInfo.callbackID =
            protocolLayer->CMDSessionTab[openHeader->sessionID].callbackID;
          protocolLayer->p_recvContainer->recvInfo.buf =
            protocolLayer->CMDSessionTab[openHeader->sessionID].buf;
          protocolLayer->p_recvContainer->recvInfo.seqNumber = openHeader->sequenceNumber;
          protocolLayer->p_recvContainer->recvInfo.len       = openHeader->length;
          //! Set bool
          isFrame = true;

          //! Finish the session
          freeSession(&protocolLayer->CMDSessionTab[openHeader->sessionID]);
          /**
           * Set end of ACK frame
           * @todo Implement proper notification mechanism
           */
          setACKFrameStatus((&protocolLayer->CMDSessionTab[openHeader->sessionID])->usageFlag);
        }
        else
        {
				
        }
      }
    }
  }
  else
  {
    //! Not an ACK frame
    switch (openHeader->sessionID)
    {
      case 0:
        isFrame = recvReqData(openHeader);
        break;
      case 1:
      //! @todo unnecessary ack in case 1. Maybe add code later
      //! @todo check algorithm,
      //! @attention here real have a bug about self-looping issue.
      //! @bug not affect OSDK currerently. 2017-1-18
      default: //! @note session id is 2
        MY_DEBUG("ACK %d", openHeader->sessionID);

        if (protocolLayer->ACKSessionTab[openHeader->sessionID - 1].sessionStatus ==
            ACK_SESSION_PROCESS)
        {
					MY_DEBUG("This session is waiting for App ACK:"
								 "session id=%d,seq_num=%d\n",
								 openHeader->sessionID, openHeader->sequenceNumber);
        }
        else if (protocolLayer->ACKSessionTab[openHeader->sessionID - 1].sessionStatus ==
                 ACK_SESSION_IDLE)
        {
          if (openHeader->sessionID > 1)
            protocolLayer->ACKSessionTab[openHeader->sessionID - 1].sessionStatus =
              ACK_SESSION_PROCESS;
          isFrame = recvReqData(openHeader);
        }
        else if (protocolLayer->ACKSessionTab[openHeader->sessionID - 1].sessionStatus ==
                 ACK_SESSION_USING)
        {
          //threadHandle->lockRecvContainer();
          p2protocolHeader =
            (OpenHeader*)protocolLayer->ACKSessionTab[openHeader->sessionID - 1].mmu->pmem;
          if (p2protocolHeader->sequenceNumber == openHeader->sequenceNumber)
          {
            MY_DEBUG("Repeat ACK to remote,session "
                   "id=%d,seq_num=%d\n",
                   openHeader->sessionID, openHeader->sequenceNumber);
            protocolLayer->openProtocolSendData(protocolLayer->ACKSessionTab[openHeader->sessionID - 1].mmu->pmem);
            //threadHandle->freeRecvContainer();
          }
          else
          {
            MY_DEBUG("Same session,but new seq_num pkg,session id=%d,"
                   "pre seq_num=%d,cur seq_num=%d\n",
                   openHeader->sessionID, p2protocolHeader->sequenceNumber,
                   openHeader->sequenceNumber);
            protocolLayer->ACKSessionTab[openHeader->sessionID - 1].sessionStatus =
              ACK_SESSION_PROCESS;
            //threadHandle->freeRecvContainer();
            isFrame = recvReqData(openHeader);
          }
        }
        break;
    }
  }
  return isFrame;
}

//! Step 10: In case we received a CMD frame and not an ACK frame
static bool
recvReqData(OpenHeader* protocolHeader)
{
  //@todo: Please monitor lengths to see whether we need to change the max size
  // of RecvContainer.recvData
  protocolLayer->p_recvContainer->dispatchInfo.isAck = false;
  uint8_t* payload = (uint8_t*)protocolHeader + sizeof(OpenHeader) + 2;
  protocolLayer->p_recvContainer->recvInfo.cmd_set = getCmdSet(protocolHeader);
  protocolLayer->p_recvContainer->recvInfo.cmd_id  = getCmdCode(protocolHeader);
  protocolLayer->p_recvContainer->recvInfo.len     = protocolHeader->length;
  //@todo: Please monitor to make sure the length is correct
  memcpy(protocolLayer->p_recvContainer->recvData.raw_ack_array, payload,
         (protocolHeader->length - (PackageMin + 2)));

  protocolLayer->p_recvContainer->dispatchInfo.isCallback = false;
  protocolLayer->p_recvContainer->dispatchInfo.callbackID = 0;

  //! isFrame = true
  return true;
}

static void
shiftDataStream(void)
{
  if (protocolLayer->p_filter->recvIndex)
  {
    protocolLayer->p_filter->recvIndex--;
    if (protocolLayer->p_filter->recvIndex)
    {
      memmove(protocolLayer->p_filter->recvBuf, protocolLayer->p_filter->recvBuf + 1, 
							protocolLayer->p_filter->recvIndex);
    }
  }
}

static void
reuseDataStream(void)
{
  uint8_t* p_buf         = protocolLayer->p_filter->recvBuf;
  uint16_t bytes_to_move = protocolLayer->p_filter->recvIndex - protocolLayer->HEADER_LEN;
  uint8_t* p_src         = p_buf + protocolLayer->HEADER_LEN;

  uint16_t n_dest_index = protocolLayer->p_filter->reuseIndex - bytes_to_move;
  uint8_t* p_dest       = p_buf + n_dest_index;

  memmove(p_dest, p_src, bytes_to_move);

  protocolLayer->p_filter->recvIndex = protocolLayer->HEADER_LEN;
  shiftDataStream();

  protocolLayer->p_filter->reuseIndex = n_dest_index;
  protocolLayer->p_filter->reuseCount++;
}

static void 
prepareDataStream(void)
{
  uint32_t bytes_to_move = protocolLayer->HEADER_LEN - 1;
  uint32_t index_of_move = protocolLayer->p_filter->recvIndex - bytes_to_move;

  memmove(protocolLayer->p_filter->recvBuf, protocolLayer->p_filter->recvBuf + index_of_move, bytes_to_move);
  memset(protocolLayer->p_filter->recvBuf + bytes_to_move, 0, index_of_move);
  protocolLayer->p_filter->recvIndex = bytes_to_move;
}

static TypeUnion
allocateACK(OpenHeader* protocolHeader)
{

  TypeUnion recvData;

  if (protocolHeader->length <= MAX_ACK_SIZE)
  {
    memcpy(recvData.raw_ack_array,
           ((uint8_t*)protocolHeader) + sizeof(OpenHeader),
           (protocolHeader->length - PackageMin));
  }
  else
  {
    //! @note throw not supported in STM32
    // throw std::runtime_error("Unknown ACK");
  }

  return recvData;
}

static uint8_t
getCmdSet(OpenHeader* protocolHeader)
{
  uint8_t* ptemp = ((uint8_t*)protocolHeader) + sizeof(OpenHeader);
  return *ptemp;
}

static uint8_t
getCmdCode(OpenHeader* protocolHeader)
{
  uint8_t* ptemp = ((uint8_t*)protocolHeader) + sizeof(OpenHeader);
  ptemp++;
  return *ptemp;
}


static void
setACKFrameStatus(uint32_t usageFlag)
{
  protocolLayer->ackFrameStatus = usageFlag;
}

static int
crcTailCheck(uint8_t* pMsg, size_t nLen)
{
  return crc32Calc(pMsg, nLen);
}

static int
crcHeadCheck(uint8_t* pMsg, size_t nLen)
{
  return crc16Calc(pMsg, nLen);
}

static uint16_t 
calculateLength(uint16_t size, uint16_t encrypt_flag)
{
	uint16_t len;
  if (encrypt_flag)
    len = size + sizeof(OpenHeader) + 4 + (16 - size % 16);
  else
    len = size + sizeof(OpenHeader) + 4;
  return len;
} //calculateLength

static uint16_t encrypt(uint8_t* pdest, const uint8_t* psrc, uint16_t w_len,
                   uint8_t is_ack, uint8_t is_enc, uint8_t session_id,
                   uint16_t seq_num)
{
	uint16_t data_len;
  //打开标头   头
  OpenHeader* p_head = (OpenHeader*)pdest;

  if (w_len > 1024)
    return 0;

  if (protocolLayer->p_filter->encode == 0 && is_enc)
  { 
		
    MY_DEBUG("Can not send encode data, Please activate your device to get an "//无法发送编码数据，请激活您的设备以获得
           "available key.\n");  //可获得的 钥匙
    return 0;
  }
  if (w_len == 0 || psrc == 0)
    data_len = (sizeof(OpenHeader));
  else
    data_len = (sizeof(OpenHeader) + CRCData + w_len);

  if (is_enc)
    data_len = data_len + (16 - w_len % 16);

  MY_DEBUG("data len: %d\n", data_len);

  p_head->sof       = SOF;
  p_head->length    = data_len;
  p_head->version   = 0;
  p_head->sessionID = session_id;
  p_head->isAck     = is_ack ? 1 : 0;
  p_head->reserved0 = 0;

  p_head->padding   = is_enc ? (16 - w_len % 16) : 0;
  p_head->enc       = is_enc ? 1 : 0;
  p_head->reserved1 = 0;

  p_head->sequenceNumber = seq_num;
  p_head->crc            = 0;

  if (psrc && w_len)
    memcpy(pdest + sizeof(OpenHeader), psrc, w_len);
  encodeData(p_head, aes256_encrypt_ecb);

  calculateCRC(pdest);

  return data_len;								 
} //calculateLength

static void 
encodeData(OpenHeader* p_head, ptr_aes256_codec codec_func)
{
	aes256_context ctx;
  uint32_t       buf_i;
  uint32_t       loop_blk;
  uint32_t       data_len;
  uint32_t       data_idx;
  uint8_t*       data_ptr;

  if (p_head->enc == 0)
    return;
  if (p_head->length <= PackageMin)
    return;

  data_ptr = (uint8_t*)p_head + sizeof(OpenHeader);
  data_len = p_head->length - PackageMin;

  loop_blk = data_len / 16;
  data_idx = 0;

  aes256_init(&ctx, protocolLayer->p_filter->sdkKey);
  for (buf_i = 0; buf_i < loop_blk; buf_i++)
  {
    codec_func(&ctx, data_ptr + data_idx);
    data_idx += 16;
  }
  aes256_done(&ctx);

  if (codec_func == aes256_decrypt_ecb)
    p_head->length = p_head->length - p_head->padding; // minus padding length;

  if(data_len == 32)
  {
    setRawFrame(data_ptr);
  }
} //encodeData

static void 
calculateCRC(void* p_data)
{
 OpenHeader* p_head = (OpenHeader*)p_data;
  uint8_t*    p_byte = (uint8_t*)p_data;
  uint32_t    index_of_crc32;

  if (p_head->sof != SOF)
    return;
  if (p_head->version != 0)
    return;
  if (p_head->length > protocolLayer->MAX_RECV_LEN)
    return;
  if (p_head->length > sizeof(OpenHeader) &&
      p_head->length < PackageMin)
    return;

  p_head->crc = crc16Calc(p_byte, CRCHeadLen);

  if (p_head->length >= PackageMin)
  {
    index_of_crc32 = p_head->length - CRCData;
    _SDK_U32_SET(p_byte + index_of_crc32, crc32Calc(p_byte, index_of_crc32));
  }
} //calculateCRC

static uint16_t 
crc16Calc(const uint8_t* pMsg, size_t nLen)
{
  size_t   i;
  uint16_t wCRC = CRC_INIT;

  for (i = 0; i < nLen; i++)
  {
    wCRC = crc16Update(wCRC, pMsg[i]);
  }

  return wCRC;
} //crc16Calc

static uint32_t 
crc32Calc(const uint8_t* pMsg, size_t nLen)
{
  size_t   i;
  uint32_t wCRC = CRC_INIT;

  for (i = 0; i < nLen; i++)
  {
    wCRC = crc32Update(wCRC, pMsg[i]);
  }

  return wCRC;
} //crc32Calc

static uint16_t 
crc16Update(uint16_t crc, uint8_t ch)
{
  uint16_t tmp;
  uint16_t msg;

  msg = 0x00ff & (uint16_t)(ch);
  tmp = crc ^ msg;
  crc = (crc >> 8) ^ crc_tab16[tmp & 0xff];

  return crc;
} //crc16Update

static uint32_t 
crc32Update(uint32_t crc, uint8_t ch)
{
  uint32_t tmp;
  uint32_t msg;

  msg = 0x000000ffL & (uint32_t)(ch);
  tmp = crc ^ msg;
  crc = (crc >> 8) ^ crc_tab32[tmp & 0xff];
  return crc;
} //crc32Update

static int  
openProtocolSendData(uint8_t* buf)
{
  size_t      ans;
  OpenHeader* pHeader = (OpenHeader*)buf;
	ans = sendUsart(buf, pHeader->length);
	if (ans == 0)
    MY_DEBUG("Port did not send");
  if (ans == (size_t)-1)
    MY_DEBUG("Port closed.");

  if (ans != pHeader->length)
  {
    MY_DEBUG("Open Protocol cmd send failed, send_len: %d packet_len: %d\n", ans,
           pHeader->length);
  }
  else
  {
    MY_DEBUG("Open Protocol cmd send success\n");
  }
  return (int)ans;
} //openProtocolSendData

size_t 
sendUsart(const uint8_t* buf, size_t len)
{
	static int count = 0;
  char* p = (char*)buf;
  if (NULL == buf)
  {
    return 0;
  }

  int sent_byte_count = 0;
	printf("<@-----");
  while (len--)
  {
		while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
		USART_SendData(USART1, *p);
    while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);
    USART_SendData(USART3, *p++);

    ++sent_byte_count;
  }
	printf("-----@>%d",count);
	count++;
  return sent_byte_count;
} //sendUsart

void 
setHeaderLength(uint8_t length)
{
  protocolLayer->HEADER_LEN = length;
} //setHeaderLength

void 
setMaxRecvLength(int length)
{
  protocolLayer->MAX_RECV_LEN = length;
} //setMaxRecvLength

static void  
setRawFrame(uint8_t* p_header)
{
 protocolLayer->rawFrame = p_header;
} //setRawFrame

static RecvContainer*
getReceivedFrame()
{
  return protocolLayer->p_recvContainer;
}

static void
setKey(const char* key)
{
  transformTwoByte(key, protocolLayer->p_filter->sdkKey);
  protocolLayer->p_filter->encode = 1;
}

static void
transformTwoByte(const char* pstr, uint8_t* pdata)
{
  int      i;
  char     temp_area[3];
  uint32_t temp8;
  temp_area[0] = temp_area[1] = temp_area[2] = 0;

  for (i = 0; i < 32; i++)
  {
    temp_area[0] = pstr[0];
    temp_area[1] = pstr[1];
    sscanf(temp_area, "%x", &temp8);
    pdata[i] = temp8;
    pstr += 2;
  }
}
