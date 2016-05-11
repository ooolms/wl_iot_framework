#ifndef APRC_H
#define APRC_H

#include "Arduino.h"

class ARpc;
typedef void (*ARpcCallback)(const char *cmd,const char *args[],int argsCount,ARpc *parser);

class ARpc
{
public:
	ARpc(int bSize,ARpcCallback cb,Stream *s);
	~ARpc();
	void readStream();
	void writeOk();
	void writeErr(const char *err);
	void writeInfo(const char *info);
	void writeMsg(const char *msg,const char *args[],int argsCount);
	void writeMsg(const char *msg,const char *arg1=0,const char *arg2=0,const char *arg3=0,const char *arg4=0);
	void writeMeasurement(const char *sensor,const char *value);

private:
	void processCommand(char *cmd,char *args[],int argsCount);
	int findDelim(int startFrom);
	void parseCommand();

public:
	static const char *okMsg;
	static const char *errMsg;
	static const char *infoMsg;
	static const char *measurementMsg;

private:
	Stream *stream;
	char *buffer;//буфер
	int bufSize;//размер буфера
	int bufIndex;
	ARpcCallback callback;
};

#endif
