#include <SoftwareSerial.h>
#include <ARpcUuid.h>
#include <ARpcDevice.h>
#include <ARpcStarNetEndPoint.h>

/*Leonardo:
 * Serial - to PC
 * Serial1 - to ADM488 RS-485 device (fullduplex)
 * serial2 - to ADM488 on softserial;
 */

const ARpcUuid deviceId("8a2241136181402c9fc94cc06a0f22db");
const char *deviceName="RS-485 hub";
SoftwareSerial serial2(4,3);

class WriteCbNet1
    :public ARpcIWriteCallback
{
public:
    virtual void writeData(const char *data,unsigned long sz)
    {
        Serial.write(data,sz);
    }
    virtual void writeStr(const char *str)
    {
        Serial.print(str);
    }
}netCb1;

class WriteCbNet2
    :public ARpcIWriteCallback
{
public:
    virtual void writeData(const char *data,unsigned long sz)
    {
        Serial1.write(data,sz);
    }
    virtual void writeStr(const char *str)
    {
        Serial1.print(str);
    }
}netCb2;

class WriteCbSerial
    :public ARpcIWriteCallback
{
public:
    virtual void writeData(const char *data,unsigned long sz)
    {
        serial2.write(data,sz);
    }
    virtual void writeStr(const char *str)
    {
        serial2.print(str);
    }
}serialCb;

#define B_SIZE 200
char b1[B_SIZE];
ARpcDevice dev(b1,B_SIZE,&serialCb,&deviceId,deviceName,true);

class NetMsgCallback
    :public ARpcINetMsgCallback
{
public:
    virtual void processMsg(const ARpcUuid &srcId,const char *msg,const char **args,unsigned char argsCount);
}netMsgCallback;

char b2[B_SIZE];
char b3[B_SIZE];
ARpcStarNetEndPoint net(b2,B_SIZE,&netCb1,&netMsgCallback,&deviceId);
ARpcStarNetEndPoint net2(b3,B_SIZE,&netCb2,&netMsgCallback,&deviceId);

class SerialCmdCallback
    :public ARpcIMessageCallback
{
public:
    virtual void processMessage(const char *msg,const char *args[],unsigned char argsCount)
    {
        dev.writeErr("unknown command");
    }
}serialCommandCallback;

class HubMsgHandler
    :public ARpcIMessageCallback
{
public:
    void processMessage(const char *msg,const char *args[],unsigned char argsCount)
    {
        if(argsCount<1)return;
        if(strcmp(msg,"#broadcast")==0)
        {
            net.writeBCastMsg(args[0],args+1,argsCount-1);
            net2.writeBCastMsg(args[0],args+1,argsCount-1);
        }
        else
        {
            dstId.parse(msg);
/*            if(!dstId.isValid())
            {
                Serial.println("bad id");
                return;
            }*/
            net.writeMsg(dstId,args[0],args+1,argsCount-1);
            net2.writeMsg(dstId,args[0],args+1,argsCount-1);
        }
    }

private:
    ARpcUuid dstId;
}hubMsgHandler;

void NetMsgCallback::processMsg(const ARpcUuid &srcId,const char *msg,const char **args,unsigned char argsCount)
{
    dev.writeMsgFromHub(srcId,msg,args,argsCount);
}

void setup()
{
    Serial.begin(9600);
    Serial1.begin(9600);
    serial2.begin(9600);
    dev.installCommandHandler(&serialCommandCallback);
    dev.installHubMsgHandler(&hubMsgHandler);
}

void loop()
{
    while(serial2.available())
        dev.putByte(serial2.read());
    while(Serial.available())
    {
        char c=Serial.read();
        net.putByte(c);
    }
    while(Serial1.available())
    {
        char c=Serial1.read();
        net2.putByte(c);
    }
    delay(1);
}
