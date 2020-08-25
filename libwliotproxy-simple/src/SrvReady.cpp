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

#include "wliot/simple/SrvReady.h"
#include <string.h>

SrvReady::SrvReady(unsigned long bSize,ISrvReadyCallback *srcb)
	:parser(bSize,static_cast<IMessageCallback*>(this))
{
	srvReadyCb=srcb;
}

SrvReady::SrvReady(char *buf,unsigned long bSize,ISrvReadyCallback *srcb)
	:parser(buf,bSize,static_cast<IMessageCallback*>(this))
{
	srvReadyCb=srcb;
}

void SrvReady::putByte(char c)
{
	parser.putByte(c);
}

void SrvReady::putData(const char *byteData,unsigned long sz)
{
	parser.putData(byteData,sz);
}

void SrvReady::reset()
{
	parser.reset();
}

void SrvReady::processMsg(const char *msg,const char **args,unsigned char argsCount)
{
	if(strcmp(msg,"server_ready")==0&&argsCount==2&&srvReadyCb)
	{
		Uuid id(args[0]);
		if(!id.isValid())return;
		srvReadyCb->processSrvReadyMsg(id,args[1]);
	}
}
