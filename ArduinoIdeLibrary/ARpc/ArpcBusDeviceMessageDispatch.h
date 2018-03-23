#ifndef ARPCBUSDEVICEMESSAGEDISPATCH_H
#define ARPCBUSDEVICEMESSAGEDISPATCH_H

#include "ARpcRealDeviceMessageDispatch.h"

class ArpcBusDeviceMessageDispatch
	:public ARpcRealDeviceMessageDispatch
{
public:
	explicit ArpcBusDeviceMessageDispatch(const char *deviceId,const char *deviceName,ARpcStreamWriter *p);
	void setDestDeviceId(const ARpcUuid &id);
	void setBroadcast();

protected:
	virtual void beginWriteMessage();

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
