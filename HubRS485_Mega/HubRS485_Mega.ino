#include <SoftwareSerial.h>
#include <ARpcUuid.h>
#include <ARpcDevice.h>
#include <ARpcStarNetEndPoint.h>

/*Mega:
 * Serial - to PC
 * Serial1 - to 1-st ADM488 RS-485 device (fullduplex)
 * Serial2 - to 2-nd ADM488 RS-485 device (fullduplex)
 */

const ARpcUuid deviceId("a90a1cbc13b649be95c2db864b920822");
const char *deviceName="RS-485_hub_Mega";

class WriteCbSerial
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
}serialCb;

class WriteCbNet1
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
}netCb1;

class WriteCbNet2
    :public ARpcIWriteCallback
{
public:
    virtual void writeData(const char *data,unsigned long sz)
    {
        Serial2.write(data,sz);
    }
    virtual void writeStr(const char *str)
    {
        Serial2.print(str);
    }
}netCb2;

ARpcDevice dev(300,&serialCb,&deviceId,deviceName,true);

class NetMsgCallback
    :public ARpcINetMsgCallback
{
public:
    virtual void processMsg(const ARpcUuid &srcId,const char *msg,const char **args,unsigned char argsCount);
}netMsgCallback;

ARpcStarNetEndPoint net(300,&netCb1,&netMsgCallback,&deviceId);
ARpcStarNetEndPoint net2(300,&netCb2,&netMsgCallback,&deviceId);

class SerialDevEventsCallback
    :public ARpcIDevEventsCallback
{
public:
    virtual void processCommand(const char *msg,const char *args[],unsigned char argsCount)
    {
        //TODO commands?
        dev.disp().writeErr("unknown command");
    }
}serialEcb;

class HubMsgHandler
    :public ARpcIMessageCallback
{
public:
    virtual void processMsg(const char *msg,const char *args[],unsigned char argsCount)
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
            if(dstId.isValid())
            {
                net.writeMsg(dstId,args[0],args+1,argsCount-1);
                net2.writeMsg(dstId,args[0],args+1,argsCount-1);
            }
        }
    }

private:
    ARpcUuid dstId;
}hubMsgHandler;

void NetMsgCallback::processMsg(const ARpcUuid &srcId,const char *msg,const char **args,unsigned char argsCount)
{
    dev.disp().writeMsgFromHub(srcId,msg,args,argsCount);
}

void setup()
{
    Serial.begin(9600);
    Serial1.begin(115200);
    Serial2.begin(115200);
    dev.disp().installDevEventsHandler(&serialEcb);
    dev.disp().installHubMsgHandler(&hubMsgHandler);
}

void loop()
{
    while(Serial.available())
        dev.putByte(Serial.read());
    while(Serial1.available())
    {
        char c=Serial1.read();
        net.putByte(c);
    }
    while(Serial2.available())
    {
        char c=Serial2.read();
        net2.putByte(c);
    }
    delay(1);
}
