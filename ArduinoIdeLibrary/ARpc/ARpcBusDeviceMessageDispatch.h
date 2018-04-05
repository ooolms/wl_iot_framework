#ifndef ARPCBUSDEVICEMESSAGEDISPATCH_H
#define ARPCBUSDEVICEMESSAGEDISPATCH_H

#include "ARpcRealDeviceMessageDispatch.h"

//TODO bus stream writer
class ARpcBusDeviceMessageDispatch
	:public ARpcRealDeviceMessageDispatch
{
public:
	explicit ARpcBusDeviceMessageDispatch(const ARpcUuid &deviceId,const char *deviceName,ARpcStreamWriter *p);
	void setDestDeviceId(const ARpcUuid &id);
	void setBroadcast();

private:
	enum
	{
		DEST_DEV,
		BROADCAST
	}destMode;
	char srcIdStr[33];
	char destIdStr[33];
};

#endif // ARPCBUSDEVICEMESSAGEDISPATCH_H
