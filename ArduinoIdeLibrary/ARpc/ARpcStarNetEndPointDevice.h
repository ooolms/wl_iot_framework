#ifndef ARPCSTARNETENDPOINTDEVICE_H
#define ARPCSTARNETENDPOINTDEVICE_H

#include "ARpcStarNetEndPoint.h"
#include "ARpcRealDeviceMessageDispatch.h"
#include "ARpcStarNetStreamWriter.h"

class ARpcStarNetEndPointDevice
	:public ARpcIMessageCallback
{
public:
	explicit ARpcStarNetEndPointDevice(unsigned long bSize,ARpcIWriteCallback *wcb,
		const ARpcUuid *deviceId,const char *devName,bool hub=false);
	explicit ARpcStarNetEndPointDevice(char *buf,unsigned long bSize,ARpcIWriteCallback *wcb,
		const ARpcUuid *deviceId,const char *devName,bool hub=false);
	void putByte(char c);
	void putData(const char *byteData,unsigned long sz);
	void reset();
	void setDestDeviceId(const ARpcUuid &id);
	void setBroadcast();
	void writeDeviceIdentified();
	inline const ARpcUuid* deviceId(){return msgDisp.deviceId();}
	inline ARpcRealDeviceMessageDispatch& disp(){return msgDisp;}
	virtual void processMsg(const char *msg,const char *args[],unsigned char argsCount)override;

private:
	ARpcStreamParser parser;
	ARpcStarNetStreamWriter writer;
	ARpcRealDeviceMessageDispatch msgDisp;
};

#endif // ARPCSTARNETENDPOINTDEVICE_H
