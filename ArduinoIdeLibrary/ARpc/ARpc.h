#ifndef APRC_H
#define APRC_H

#include "Arduino.h"

typedef void (*ARpcCallback)(const char *cmd,const char *args[],int argsCount,Stream *s);

class ARpcParser
{
public:
	ARpcParser(int bSize,ARpcCallback cb,Stream *s);
	~ARpcParser();
	void readStream();
	
private:
	void processCommand(char *cmd,char *args[],int argsCount);
	int findDelim(int startFrom);
	void parseCommand();
	
private:
	Stream *stream;
	char *buffer;//буфер
	int bufSize;//размер буфера
	int bufIndex;
	ARpcCallback callback;
};

#endif
