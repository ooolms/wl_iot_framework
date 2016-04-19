#include <ARpc.h>

//обработка команд
void processCommand(const char *cmd,const char *args[],int argsCount,Stream *s)
{
    if(strcmp(cmd,"testOk")==0)
    {
        s->print("ok\n");
    }
    else if(strcmp(cmd,"testErr")==0)
    {
        s->print("err|epic fail с русским текстом\n");
    }
    else if(strcmp(cmd,"testLongCmd")==0)
    {
        delay(1000);
        s->print("sync\n");
        delay(1000);
        s->print("sync\n");
        delay(1000);
        s->print("ok\n");
    }
    else if(strcmp(cmd,"testLongCmdNoSync")==0)
    {
        delay(1000);
        //no sync
        delay(1000);
        //no sync
        delay(1000);
        s->print("ok\n");
    }
    s->print("info|Cmd: ");
    s->print(cmd);
    s->print("\n");
    for(int i=0;i<argsCount;++i)
    {
        s->print("info|Arg: ");
        s->print(args[i]);
        s->print("\n");
    }
}

ARpcParser parser(300,&processCommand,&Serial);

void setup()
{
    Serial.begin(9600);
}

void loop()
{
    parser.readStream();
}
 
