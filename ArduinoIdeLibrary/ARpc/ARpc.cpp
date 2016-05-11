#include "ARpc.h"

const char delim='|';//код 124
const int maxArgCount=10;//максимальное число аргументов
const char *ARpc::okMsg="ok";
const char *ARpc::errMsg="err";
const char *ARpc::infoMsg="info";
const char *ARpc::measurementMsg="meas";

ARpc::ARpc(int bSize,ARpcCallback cb,Stream *s)
{
	bufSize=bSize;
	buffer=(char*)malloc(bufSize+1);
	memset(buffer,0,bufSize+1);
	callback=cb;
	stream=s;
	bufIndex=0;
}

ARpc::~ARpc()
{
}

//обработка команд
void ARpc::processCommand(char *cmd,char *args[],int argsCount)
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
	if(callback)callback((const char *)cmd,(const char **)args,argsCount,this);
}

//поиск разделителя дальше по строке
int ARpc::findDelim(int startFrom)
{
	for(int i=startFrom;i<bufSize;++i)
	{
		if(buffer[i]==0)return -1;
		else if(buffer[i]==delim)return i;
	}
	return -1;
}

//разбор строки из Serial на команду и аргументы
void ARpc::parseCommand()
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

void ARpc::readStream()
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

void ARpc::writeOk()
{
	stream->print(okMsg);
	stream->print("\n");
}

void ARpc::writeErr(const char *err)
{
	stream->print(errMsg);
	if(err)
	{
		stream->print("|");
		stream->print(err);
	}
	stream->print("\n");
}

void ARpc::writeInfo(const char *info)
{
	stream->print(infoMsg);
	if(info)
	{
		stream->print("|");
		stream->print(info);
	}
	stream->print("\n");
}

void ARpc::writeMsg(const char *msg,const char *args[],int argsCount)
{
	stream->print(msg);
	for(int i=0;i<argsCount;++i)
	{
		stream->print("|");
		stream->print(args[i]);
	}
	stream->print("\n");
}

void ARpc::writeMsg(const char *msg,const char *arg1,const char *arg2,const char *arg3,const char *arg4)
{
	stream->print(msg);
	if(arg1)
	{
		stream->print("|");
		stream->print(arg1);
	}
	if(arg2)
	{
		stream->print("|");
		stream->print(arg2);
	}
	if(arg3)
	{
		stream->print("|");
		stream->print(arg3);
	}
	if(arg4)
	{
		stream->print("|");
		stream->print(arg4);
	}
	stream->print("\n");
}

void ARpc::writeMeasurement(const char *sensor,const char *value)
{
	stream->print(measurementMsg);
	stream->print("|");
	stream->print(sensor);
	stream->print("|");
	stream->print(value);
	stream->print("\n");
}

