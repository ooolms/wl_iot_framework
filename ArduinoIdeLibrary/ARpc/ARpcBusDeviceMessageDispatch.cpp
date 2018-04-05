#include "ARpcBusDeviceMessageDispatch.h"
#include "ARpcConfig.h"

ARpcBusDeviceMessageDispatch::ARpcBusDeviceMessageDispatch(
	const ARpcUuid &deviceId,const char *deviceName,ARpcStreamWriter *p)
	:ARpcRealDeviceMessageDispatch(deviceId,deviceName,p)
{
	destMode=DEST_DEV;
	ARpcUuid nullId;
	nullId.toHex(destIdStr);
	devId.toHex(srcIdStr);
}

void ARpcBusDeviceMessageDispatch::setDestDeviceId(const ARpcUuid &id)
{
	destMode=DEST_DEV;
	id.toHex(destIdStr);
}

void ARpcBusDeviceMessageDispatch::setBroadcast()
{
	destMode=BROADCAST;
}

bool ARpcBusDeviceMessageDispatch::beginWriteMessage()
{
	mWriter->writeDataNoEscape(srcIdStr,32);
	mWriter->writeDataNoEscape(argDelim,1);
	if(destMode==BROADCAST)
		mWriter->writeDataNoEscape("#broadcast",10);
	else mWriter->writeDataNoEscape(destIdStr,32);
	mWriter->writeDataNoEscape(argDelim,1);
	return true;
}
