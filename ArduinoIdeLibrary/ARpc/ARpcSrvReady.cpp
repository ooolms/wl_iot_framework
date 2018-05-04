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

#include "ARpcSrvReady.h"
#include <string.h>

const char *ARpcSrvReady::srvReadyMsg="server_ready";

ARpcSrvReady::ARpcSrvReady(unsigned long bSize,ARpcISrvReadyCallback *srcb)
	:parser(bSize,static_cast<ARpcIMessageCallback*>(this))
{
	srvReadyCb=srcb;
}

void ARpcSrvReady::putByte(char c)
{
	parser.putByte(c);
}

void ARpcSrvReady::putData(const char *byteData,unsigned long sz)
{
	parser.putData(byteData,sz);
}

void ARpcSrvReady::reset()
{
	parser.reset();
}

void ARpcSrvReady::processMessage(const char *msg,const char *args[],unsigned char argsCount)
{
	if(strcmp(msg,srvReadyMsg)==0&&srvReadyCb)
		srvReadyCb->processSrvReadyMsg(args,argsCount);
}
