#include <AltSoftSerial.h>
#include <ARpcStarNetDevice.h>

const ARpcUuid deviceId("31a608bc1b844ca0a5579c4736315083");
const char *deviceName="test1";

AltSoftSerial Serial2;

const char *sensorsStr="<sensors><sensor type=\"single\" name=\"temp1\"/></sensors>";

void writeCb1(void *data,const char *str,unsigned long size);
void writeCb2(void *data,const char *str,unsigned long size);
ARpcStarNetDevice net(300,&writeCb1,0,&writeCb2,0,deviceId,deviceName);

void serialMsgCallback(void *data,const char *cmd,const char *args[],unsigned char argsCount);
ARpcStreamParser serialParser(300,&serialMsgCallback,0);

void writeCb1(void *data,const char *str,unsigned long size)
{
    Serial.write(str,size);
    Serial1.write(str,size);
}

void writeCb2(void *data,const char *str,unsigned long size)
{
    Serial2.write(str,size);
}

void serialMsgCallback(void *data,const char *msg,const char *args[],unsigned char argsCount)
{
    if(argsCount==0)return;
    if(strcmp(msg,"#broadcast")==0)
        net.setBroadcast();
    else
    {
        ARpcUuid id(msg);
        if(!id.isValid())return;
        net.setDestDeviceId(id);
    }
    net.writeMsg(args[0],&(args[1]),argsCount-1);
}

void netCommandCb(void *data,const char *cmd,const char *args[],unsigned char argsCount)
{
    net.writeInfo("Command: ",cmd);
    net.writeOk();
}

void setup()
{
    Serial.begin(9600);
    Serial1.begin(9600);
    Serial2.begin(9600);
    net.installCommandHandler(&netCommandCb,0);
    net.setSensorsDescription(sensorsStr);
}

void loop()
{
    if(Serial1.available())
    {
        while(Serial1.available())
        {
            char c=Serial1.read();
            net.putByte1(c);
            Serial.write(&c,1);
        }
    }
    if(Serial2.available())
    {
        while(Serial2.available())
        {
            char c=Serial2.read();
            net.putByte2(c);
            Serial.write(&c,1);
        }
    }
    if(Serial.available())
    {
        while(Serial.available())
        {
            char c=Serial.read();
            serialParser.putByte(c);
            Serial.write(&c,1);
        }
    }
}
