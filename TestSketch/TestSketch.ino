#include <ARpcDevice.h>

const ARpcUuid devId("c63811aa425348c8a647d4fcfb185a43");
const char *devName="test";

class WriteCb
    :public ARpcIWriteCallback
{
public:
    void writeData(const char *d,unsigned long sz)
    {
        Serial.write(d,sz);
    }
    void writeStr(const char *str)
    {
        Serial.print(str);
    }
}writeCb;

ARpcDevice parser(300,&writeCb,&devId,devName);

class DevEventsCb
    :public ARpcIDevEventsCallback
{
    virtual void processCommand(const char *cmd,const char *args[],unsigned char argsCount)
    {
        if(strcmp(cmd,"testOk")==0)
        {
            parser.disp().writeOk();
        }
        else if(strcmp(cmd,"testErr")==0)
        {
            parser.disp().writeErr("epic fail с русским текстом");
        }
        else if(strcmp(cmd,"testCmd")==0)
        {
            delay(3000);
            parser.disp().writeCmdSync();
            delay(3000);
            parser.disp().writeCmdSync();
            delay(3000);
            parser.disp().writeOk();
        }
        else if(strcmp(cmd,"testInfoMsg")==0)
        {
            parser.disp().writeInfo("info_msg");
            parser.disp().writeOk();
        }
        else if(strcmp(cmd,"testMeasMsg")==0)
        {
            parser.disp().writeMeasurement("sens1","val1");
            parser.disp().writeOk();
        }
    }
}ecb;

void setup()
{
    Serial.begin(9600);
    parser.disp().installDevEventsHandler(&ecb);
}

void loop()
{
    while(Serial.available())
        parser.putByte(Serial.read());
    delay(1);
}
 
