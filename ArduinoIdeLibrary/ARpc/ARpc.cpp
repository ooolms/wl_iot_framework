#include "ARpc.h"

const char delim='|';//код 124
const int maxArgCount=10;//максимальное число аргументов

ARpcParser::ARpcParser(int bSize,ARpcCallback cb,Stream *s)
{
	bufSize=bSize;
	buffer=(char*)malloc(bufSize+1);
	memset(buffer,0,bufSize+1);
	callback=cb;
	stream=s;
	bufIndex=0;
}

ARpcParser::~ARpcParser()
{
}

//обработка команд
void ARpcParser::processCommand(char *cmd,char *args[],int argsCount)
{
//	Serial.print("info|Cmd: ");
//	Serial.print(cmd);
//	Serial.print("\n");
//	for(int i=0;i<argsCount;++i)
//	{
//		Serial.print("info|Arg: ");
//		 Serial.print(args[i]);
//		Serial.print("\n");
//	}
	if(callback)callback((const char *)cmd,(const char **)args,argsCount,stream);
}

//поиск разделителя дальше по строке
int ARpcParser::findDelim(int startFrom)
{
    for(int i=startFrom;i<bufSize;++i)
    {
        if(buffer[i]==0)return -1;
        else if(buffer[i]==delim)return i;
    }
    return -1;
}

//разбор строки из Serial на команду и аргументы
void ARpcParser::parseCommand()
{
    if(buffer[0]==0||buffer[0]==delim)
        return;
    
    int bufIter=0;
    char *cmd=buffer;
    char *args[maxArgCount];//аргументы
    for(int i=0;i<maxArgCount;++i)
        args[i]=0;
    for(int i=0;i<maxArgCount;++i)
    {
        bufIter=findDelim(bufIter+1);//ищем разделитель
        if(bufIter==-1)//больше нет
        {
            processCommand(cmd,args,i);
            return;
        }
        buffer[bufIter]=0;//заменяем разделитель на символ с кодом 0
        if(bufIter==(bufSize-1))//разделитель в последнем символе в буфере, игнорируем
        {
            processCommand(cmd,args,i);
            return;
        }

        //следующий аргумент будет после позиции разделителя
        ++bufIter;
        args[i]=&buffer[bufIter];
    }
}

void ARpcParser::readStream()
{
	while(stream->available())
    {
        if(bufIndex==bufSize)//переполнение буфера, эпик фейл
        {
            //сбрасываем буфер
            memset(buffer,0,bufSize+1);
            bufIndex=0;
        }
		char c=(char)stream->read();
        if(c=='\n')//признак конца сообщения, обрабатываем
        {
            buffer[bufIndex]=0;
            parseCommand();
            memset(buffer,0,bufSize+1);
            bufIndex=0;
        }
        else//продолжаем накапливать буфер
        {
            buffer[bufIndex]=c;
            ++bufIndex;
        }
    }
}
 
