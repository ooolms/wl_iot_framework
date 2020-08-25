/*******************************************
Copyright 2017 OOO "LMS"

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

	http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#ifndef SRVREADY_H
#define SRVREADY_H

#include "wliot/simple/StreamWriter.h"
#include "wliot/simple/StreamParser.h"
#include "wliot/simple/Uuid.h"

class ISrvReadyCallback
{
public:
	virtual ~ISrvReadyCallback(){}
	virtual void processSrvReadyMsg(const Uuid &serverId,const char *serverName)=0;
};

class SrvReady
	:public IMessageCallback
{
public:
	explicit SrvReady(unsigned long bSize,ISrvReadyCallback *srcb);
	explicit SrvReady(char *buf,unsigned long bSize,ISrvReadyCallback *srcb);
	void putByte(char c);
	void putData(const char *byteData,unsigned long sz);
	void reset();
	virtual void processMsg(const char *msg,const char **args,unsigned char argsCount) override;

private:
	StreamParser parser;
	ISrvReadyCallback *srvReadyCb;
};

#endif
