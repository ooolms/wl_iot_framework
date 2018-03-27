#include "ArpcBusDeviceMessageDispatch.h"

ArpcBusDeviceMessageDispatch::ArpcBusDeviceMessageDispatch(
	const ARpcUuid &deviceId,const char *deviceName,ARpcStreamWriter *p)
	:ARpcRealDeviceMessageDispatch(deviceId,deviceName,p)
{
	destMode=DEST_DEV;
	ARpcUuid nullId;
	nullId.toHex(destIdStr);
	devId.toHex(srcIdStr);
}

void ArpcBusDeviceMessageDispatch::setDestDeviceId(const ARpcUuid &id)
{
	destMode=DEST_DEV;
	id.toHex(destIdStr);
}

void ArpcBusDeviceMessageDispatch::setBroadcast()
{
	destMode=BROADCAST;
}

void ArpcBusDeviceMessageDispatch::beginWriteMessage()
{
	mWriter->writeDataNoEscape(srcIdStr,32);
	mWriter->writeDataNoEscape("|",1);
	if(destMode==BROADCAST)
		mWriter->writeDataNoEscape("#broadcast",10);
	else mWriter->writeDataNoEscape(destIdStr,32);
	mWriter->writeDataNoEscape("|",1);
}
