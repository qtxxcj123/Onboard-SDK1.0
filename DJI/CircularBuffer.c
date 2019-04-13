#include "CircularBuffer.h"
#include "malloc.h"
CircularBuffer* circularBuffer;
void externCircularBufferInit()
{
	circularBuffer = (CircularBuffer*)mymalloc(sizeof(CircularBuffer));
	circularBuffer->init = interCircularBufferInit;
	circularBuffer->cbPop  = cbPop;
	circularBuffer->cbPush = cbPush;
} //externCircularBufferInit

static void 
interCircularBufferInit(void)
{
	circularBuffer->maxLen = 5000;
	circularBuffer->buffer =
    (VehicleCallBackHandler*)mymalloc(5000 * sizeof(VehicleCallBackHandler));
  if(circularBuffer->buffer == NULL){
    //DERROR("buffer memory alloc failed\n");
  }
  circularBuffer->buffer2 = (RecvContainer*)mymalloc(5000 * sizeof(RecvContainer));
  if(circularBuffer->buffer2 == NULL){
    //DERROR("buffer2 memory alloc failed\n");
  }
	
  circularBuffer->head    = 0;
  circularBuffer->tail    = 0;
} //interCircularBufferInit

static int
cbPush(CircularBuffer*        CBuffer,
       VehicleCallBackHandler cbData,
       RecvContainer          recvData)
{
  int next = circularBuffer->head + 1;
  if (next >= circularBuffer->maxLen)
  {
    next = 0;
  }
  //! Circular buffer is full, pop the old value and discard.
  if (next == circularBuffer->tail)
  {
    CBuffer->cbPop(CBuffer, &cbData, &recvData);
    //DSTATUS("Warning: Circular Buffer Full. Discarded Callback from Tail \n");
  }
  circularBuffer->buffer2[circularBuffer->head] = recvData;
  circularBuffer->buffer[circularBuffer->head]  = cbData;
  circularBuffer->head          = next;
  return 0;
} //cbPush

static int
cbPop(CircularBuffer*         CBuffer,
      VehicleCallBackHandler* cbData,
      RecvContainer*          recvData)
{
  if (circularBuffer->head == circularBuffer->tail)
  {
    //DSTATUS("Circular Buffer empty \n");
    return -1;
  }
  *cbData   = circularBuffer->buffer[circularBuffer->tail];
  *recvData = circularBuffer->buffer2[circularBuffer->tail];

  //! Clear data
  memset(&circularBuffer->buffer[circularBuffer->tail], 0, sizeof(VehicleCallBackHandler));
  memset(&circularBuffer->buffer2[circularBuffer->tail], 0, sizeof(RecvContainer));

  int next = circularBuffer->tail + 1;
  if (next >= circularBuffer->maxLen)
    next = 0;
  circularBuffer->tail   = next;
  return 0;
} //cbPop
