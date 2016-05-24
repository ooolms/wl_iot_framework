#include <ARpc.h>

//обработка команд
void processCommand(const char *cmd,const char *args[],int argsCount,ARpc *parser)
{
    if(strcmp(cmd,"testOk")==0)
    {
        parser->writeMsg("ok");
    }
    else if(strcmp(cmd,"testErr")==0)
    {
        parser->writeMsg("err","epic fail с русским текстом");
    }
    else if(strcmp(cmd,"testLongCmd")==0)
    {
        delay(1000);
        parser->writeMsg("sync");
        delay(1000);
        parser->writeMsg("sync");
        delay(1000);
        parser->writeMsg("ok");
    }
    else if(strcmp(cmd,"testLongCmdNoSync")==0)
    {
        delay(1000);
        //no sync
        delay(1000);
        //no sync
        delay(1000);
        parser->writeMsg("ok");
    }
    else if(strcmp(cmd,"testInfoMsg")==0)
    {
        parser->writeInfo("info_msg");
        parser->writeMsg("ok");
    }
    else if(strcmp(cmd,"testMeasMsg")==0)
    {
        parser->writeMeasurement("sens1","val1");
        parser->writeMsg("ok");
    }
}

void arpcWriteCallback(const char *str)
{
    Serial.print(str);
}

ARpc parser(300,&processCommand,&arpcWriteCallback);

void setup()
{
    Serial.begin(9600);
}

void loop()
{
    while(Serial.available())
        parser.putChar(Serial.read());
}
 
