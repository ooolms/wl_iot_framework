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

#include "ARpcBase.h"

class ARpcSrvReady;
typedef void (*ARpcSrvReadyCallback)(char *args[],unsigned char argsCount,ARpcSrvReady *obj);

class ARpcSrvReady
	:public ARpcBase
{
public:
	explicit ARpcSrvReady(unsigned long bSize,ARpcWriteCallback wcb,ARpcSrvReadyCallback srcb);

protected:
	virtual void processMessage(char *cmd,char *args[],unsigned char argsCount);

private:
	static const char *srvReadyMsg;

private:
	ARpcSrvReadyCallback srvReadyCB;
};

#endif
