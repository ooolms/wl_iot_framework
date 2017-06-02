#include <ARpc.h>

int ledPin=13;
bool needBlink=false;
unsigned int blinksCount=0;
const char *deviceName="led_blink_test";
const char *deviceId="{f84526c1-5e88-4315-81f8-f7da45daa09d}";

const char *controlIface="<controls><group title=\"Device controls\"><control sync=\"0\" title=\"Blink\" command=\"blink\"/>"
"<control sync=\"0\" title=\"Read blinks count\" command=\"get_blinks_count\"/><control layout=\"h\" sync=\"0\" "
"title=\"Write dbg msg\" command=\"write_dbg_msg\"><param type=\"text_edit\" title=\"message\"/></control></group></controls>";

const char *sensorsDef="<sensors><sensor name=\"blinks_count\" type=\"single\"/><sensor name=\"dbg_log\" type=\"text\"/></sensors>";

//обработка команд
void processCommand(const char *cmd,const char *args[],int argsCount,ARpc *parser)
{
    if(strcmp(cmd,"blink")==0)
    {
        needBlink=true;
        parser->writeOk();
    }
    else if(strcmp(cmd,"get_blinks_count")==0)
    {
        parser->writeMeasurement("blinks_count",String(blinksCount).c_str());
        parser->writeOk();
    }
    else if(strcmp(cmd,"write_dbg_msg")==0)
    {
        if(argsCount<1)
            parser->writeErr("Wrong parameter");
        else
        {
            parser->writeMeasurement("dbg_log",args[0]);
            parser->writeOk();
        }
    }
    else parser->writeErr("Unknown cmd");
}

void arpcWriteCallback(const char *str)
{
    Serial.print(str);
}

ARpc parser(300,&processCommand,&arpcWriteCallback,deviceId,deviceName);

void setup()
{
    Serial.begin(9600);
    pinMode(ledPin,OUTPUT);
    parser.setControlsInterface(controlIface);
    parser.setSensorsDescription(sensorsDef);
}

void blink(int dl)
{
    digitalWrite(ledPin,HIGH);
    delay(dl);
    digitalWrite(ledPin,LOW);
    delay(dl);
}

void loop()
{
    while(Serial.available())
        parser.putChar(Serial.read());
    if(needBlink)
    {
        blink(200);
        blink(400);
        blink(200);
        blink(100);
        blink(100);
        ++blinksCount;
        needBlink=false;
        parser.writeMeasurement("blinks_count",String(blinksCount).c_str());
    }
}

