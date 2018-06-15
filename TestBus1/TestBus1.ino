//#include <AltSoftSerial.h>
#include <SoftwareSerial.h>
#include <ARpcStarNetDevice.h>
#include <ArduSched.h>

ArduTimer timer(2);

const ARpcUuid deviceId("5565ca0306db448ba3e75544415f4143");
const char *deviceName="test1";

//const ARpcUuid deviceId("830286f330f14557b1582d54b36d9fd8");
//const char *deviceName="test2";

const char *sensorsStr="<sl>"
"<s t=\"single\" n=\"temp\"/>"
"<s t=\"single\" n=\"hum\"/>"
"<s t=\"single\" n=\"pres\"/>"
"</sl>";

SoftwareSerial serial2(8,9);

class WriteCb1
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
}cb1;

class WriteCb2
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
}cb2;

char buf1[200];
char buf2[200];
ARpcStarNetDevice net(buf1,buf2,200,&cb1,&cb2,&deviceId,deviceName);

void readBME()
{
    net.disp().writeMeasurement("temp","21");
    net.disp().writeMeasurement("hum","34");
    net.disp().writeMeasurement("pres","95691");
}

void publishDeviceInfo()
{
    net.writeDeviceIdentified();
}

void setup()
{
    delay(1000);
    Serial.begin(9600);
    serial2.begin(9600);
    timer.setEvent(0,&readBME);
    timer.execRepeated(0,3000);
    timer.setEvent(1,&publishDeviceInfo);
    timer.execRepeated(1,5000);
    net.disp().setSensors(sensorsStr);
}

void loop()
{
    timer.process();
    while(Serial.available())
    {
        char c=Serial.read();
        net.putByte1(c);
    }
    while(serial2.available())
    {
        char c=serial2.read();
        net.putByte2(c);
    }
    delay(10);
}
