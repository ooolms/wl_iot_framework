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
#include "ARpcArduStrHlp.h"

ARpcSrvReady::ARpcSrvReady(unsigned long bSize,ARpcISrvReadyCallback *srcb)
	:parser(bSize,static_cast<ARpcIMessageCallback*>(this))
{
	srvReadyCb=srcb;
}

ARpcSrvReady::ARpcSrvReady(char *buf,unsigned long bSize,ARpcISrvReadyCallback *srcb)
	:parser(buf,bSize,static_cast<ARpcIMessageCallback*>(this))
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

void ARpcSrvReady::processMsg(const char *msg,const char **args,unsigned char argsCount)
{
	if(use_strcmp(msg,PSTR("server_ready"))==0&&argsCount==2&&srvReadyCb)
	{
		ARpcUuid id(args[0]);
		if(!id.isValid())return;
		srvReadyCb->processSrvReadyMsg(id,args[1]);
	}
}
