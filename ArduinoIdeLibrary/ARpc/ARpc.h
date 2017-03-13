/*******************************************
This file is part of ArduinoRpc. ArduinoRpc is a simple text
communication protocol for Arduino boards.
Copyright (C) 2016 Denis Bespalov <master1312@yandex.ru>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with this program. If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef APRC_H
#define APRC_H

#include "Arduino.h"

class ARpc;
typedef void (*ARpcCommandCallback)(const char *cmd,const char *args[],int argsCount,ARpc *parser);
//write data to other side
typedef void (*ARpcWriteCallback)(const char *str);

class ARpc
{
public:
	ARpc(int bSize,ARpcCommandCallback ccb,ARpcWriteCallback wcb,const char *deviceId,const char *deviceName);
	// !!! deviceId and deviceName are NOT copied (mem economy)
	~ARpc();
	void putChar(char c);
	void writeOk(const char *arg1=0,const char *arg2=0,const char *arg3=0,const char *arg4=0);
	void writeErr(const char *arg1=0,const char *arg2=0,const char *arg3=0,const char *arg4=0);
	void writeInfo(const char *info,const char *arg1=0,const char *arg2=0,const char *arg3=0,const char *arg4=0);
	void writeMeasurement(const char *sensor,const char *value);
	void writeSync();
	void setControlsInterface(const char *iface);// !!! NOT copied
	void setSensorsDescription(const char *descr);// !!! NOT copied
	//TODO func calls

private:
	void processMessage(char *cmd,char *args[],int argsCount);
	int findDelim(int startFrom);
	void parseCommand();
	void writeMsg(const char *msg,const char *args[],int argsCount);
	void writeMsg(const char *msg,const char *arg1=0,const char *arg2=0,const char *arg3=0,const char *arg4=0);

public:
	static const char *okMsg;
	static const char *errMsg;
	static const char *infoMsg;
	static const char *measurementMsg;
	static const char *syncMsg;

private:
	char *buffer;//буфер
	int bufSize;//размер буфера
	int bufIndex;
	bool cmdReplied;
	const char *devId,*devName;
	const char *controlInterface;
	const char *sensorsDescription;
	ARpcCommandCallback cmdCallback;
	ARpcWriteCallback writeCallback;
};

#endif