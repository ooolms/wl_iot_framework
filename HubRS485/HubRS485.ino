//#include <SoftwareSerial.h>
#include <ARpcUuid.h>
#include <ARpcDevice.h>
#include <ARpcDeviceState.h>
#include <ARpcStreamWriter.h>
#include <ARpcStarNet.h>
#include <ArpcBusDeviceMessageDispatch.h>
#include <ARpcStreamParser.h>
#include <ARpcSrvReady.h>
#include <ARpcRealDeviceMessageDispatch.h>

/*Leonardo:
 * Serial - to PC
 * Serial1 - to device
 * 
 */

const char *deviceId="8a2241136181402c9fc94cc06a0f22db";
const char *deviceName="Test hub";
//SoftwareSerial serial2(8,7);

ARpcDevice dev(300,&serialWriteCallback,0,deviceId,deviceName,true);

void serialCommandCallback(void *data,const char *msg,const char *args[],unsigned char argsCount)
{
    dev.writeErr("unknown command");
}

void hubMsgHandler(void *data,const char *msg,const char *args[],unsigned char argsCount)
{
}

void setup()
{
    Serial.begin(9600);
    Serial1.begin(11520);
    dev.installCommandHandler(&serialCommandCallback,0);
    dev.installHubMsgHandler(&hubMsgHandler,0);
}

void loop()
{
}
