//#include <SoftwareSerial.h>
#include <ARpcUuid.h>
#include <ARpcDevice.h>
#include <ARpcStarNetEndPoint.h>

/*Leonardo:
 * Serial - to PC
 * Serial1 - to device
 */

const char *deviceId="8a2241136181402c9fc94cc06a0f22db";
const char *deviceName="Test hub";
//SoftwareSerial serial2(8,7);

void serialWriteCallback(void *data,const char *str,unsigned long size);
ARpcDevice dev(300,&serialWriteCallback,0,deviceId,deviceName,true);

void netWriteCallback(void *data,const char *str,unsigned long size);
void netMsgCallback(void *data,const ARpcUuid &srcId,const char *msg,const char **args,unsigned char argsCount);
ARpcStarNetEndPoint net(300,&netWriteCallback,0,&netMsgCallback,0,deviceId);

void serialCommandCallback(void *data,const char *msg,const char *args[],unsigned char argsCount)
{
    dev.writeErr("unknown command");
}

void serialWriteCallback(void *data,const char *str,unsigned long size)
{
    Serial.write(str,size);
}

void hubMsgHandler(void *data,const char *msg,const char *args[],unsigned char argsCount)
{
    if(argsCount<1)return;
    if(strcmp(msg,"#broadcast")==0)
        net.writeBCastMsg(args[0],args+1,argsCount-1);
    else
    {
        ARpcUuid dstId(msg);
        if(!dstId.isValid())return;
        net.writeMsg(dstId,args[0],args+1,argsCount-1);
    }
}

void netWriteCallback(void *data,const char *str,unsigned long size)
{
    Serial1.write(str,size);
}

void netMsgCallback(void *data,const ARpcUuid &srcId,const char *msg,const char **args,unsigned char argsCount)
{
    dev.writeMsgFromHub(srcId,msg,args,argsCount);
}

void setup()
{
    Serial.begin(9600);
    Serial1.begin(19200);
    dev.installCommandHandler(&serialCommandCallback,0);
    dev.installHubMsgHandler(&hubMsgHandler,0);
}

void loop()
{
    while(Serial.available())
        dev.putByte(Serial.read());
    while(Serial1.available())
        net.putByte(Serial1.read());
}
