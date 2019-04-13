#include "Subscription.h"
#include "malloc.h"
#include "Vehicle.h"
const uint32_t DBVersion               = 0x00000100;
extern Vehicle* vehicle;
extern const uint8_t versionMatch[];
DataSubscription* subscribes;
void 
externDataSubcripInit(void)
{
	subscribes = (DataSubscription*)mymalloc(sizeof(DataSubscription));
	subscribes->init = interiDataSubcripInit;
	subscribes->verify = verify;
} //externDataSubcripInit

static void
interiDataSubcripInit(void)
{

} //interiDataSubcripInit

void
verifyCallback(Vehicle*      vehiclePtr,
                                 RecvContainer rcvContainer, UserData userData)
{
  ErrorCode ackErrorCode;
  ackErrorCode.info = rcvContainer.recvInfo;
  ackErrorCode.data = rcvContainer.recvData.subscribeACK;

  if (!vehicle->ack->getError(ackErrorCode))
  {
    MY_DEBUG("Verify subscription successful.");
//    subscribPtr->verifySuccessful = true;
  }
  else
  {
    vehicle->ack->getErrorCodeMessage(ackErrorCode, __func__);
  }
} //verifyCallback

static void 
verify(void)
{
  uint32_t data = DBVersion;

  int cbIndex                           = vehicle->callbackIdIndex();
  vehicle->nbCallbackFunctions[cbIndex] = (void*)verifyCallback;
  vehicle->nbUserData[cbIndex]          = NULL;

  vehicle->protocolLayer->openProtocolSend(2, vehicle->getEncryption(),
                 versionMatch, &data,
                 sizeof(data), 500, 2, true, cbIndex);
} //verify


