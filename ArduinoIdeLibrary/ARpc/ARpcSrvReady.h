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

#ifndef APRCSRVREADY_H
#define APRCSRVREADY_H

#include "ARpcStreamWriter.h"
#include "ARpcStreamParser.h"

class ARpcISrvReadyCallback
{
public:
	virtual void processSrvReadyMsg(const char *args[],unsigned char argsCount)=0;
};

class ARpcSrvReady
	:public ARpcIMessageCallback
{
public:
	explicit ARpcSrvReady(unsigned long bSize,ARpcISrvReadyCallback *srcb);
	virtual void process(const char *msg,const char *args[],unsigned char argsCount) override;

private:
	static const char *srvReadyMsg;

private:
	ARpcStreamParser parser;
	ARpcISrvReadyCallback *srvReadyCb;
};

#endif
