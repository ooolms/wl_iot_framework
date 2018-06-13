#include "ARpcStarNetEndPointDevice.h"
#include "ARpcConfig.h"
#include <string.h>

ARpcStarNetEndPointDevice::ARpcStarNetEndPointDevice(
	unsigned long bSize,ARpcIWriteCallback *wcb,const ARpcUuid *deviceId,const char *devName,bool hub)
	:parser(bSize,static_cast<ARpcIMessageCallback*>(this))
	,writer(deviceId,wcb)
	,msgDisp(deviceId,devName,&writer,hub)
{
}

ARpcStarNetEndPointDevice::ARpcStarNetEndPointDevice(
	char *buf,unsigned long bSize,ARpcIWriteCallback *wcb,const ARpcUuid *deviceId,const char *devName,bool hub)
	:parser(buf,bSize,static_cast<ARpcIMessageCallback*>(this))
	,writer(deviceId,wcb)
	,msgDisp(deviceId,devName,&writer,hub)
{
}

void ARpcStarNetEndPointDevice::putByte(char c)
{
	parser.putByte(c);
}

void ARpcStarNetEndPointDevice::putData(const char *byteData,unsigned long sz)
{
	parser.putData(byteData,sz);
}

void ARpcStarNetEndPointDevice::reset()
{
	parser.reset();
}

void ARpcStarNetEndPointDevice::processMessage(const char *msg,const char *args[],unsigned char argsCount)
{
	if(argsCount<2||msg[0]==0||args[0][0]==0||args[1][0]==0)return;
	ARpcUuid srcId(msg);
	if(!srcId.isValid())return;
	if(args[0][0]=='#')//reserved messages
	{
		if(strcmp(args[0],bCastMsg)!=0)
			return;
	}
	else
	{
		ARpcUuid dstId(args[0]);
		if(dstId!=*msgDisp.deviceId())return;
	}
	writer.setDestId(srcId);
	msgDisp.processMessage(args[1],args+2,argsCount-2);
}

void ARpcStarNetEndPointDevice::setDestDeviceId(const ARpcUuid &id)
{
	writer.setDestId(id);
}

void ARpcStarNetEndPointDevice::setBroadcast()
{
	writer.setBroadcast();
}

void ARpcStarNetEndPointDevice::writeDeviceIdentified()
{
	writer.setBroadcast();
	msgDisp.writeMsg("device_identified",msgDisp.deviceName());
}
