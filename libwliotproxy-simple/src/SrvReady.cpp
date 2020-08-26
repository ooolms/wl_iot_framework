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
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

SrvReady::SrvReady(unsigned long bSize,ISrvReadyCallback *srcb)
	:parser(bSize,static_cast<IMessageCallback*>(this))
{
	srvReadyCb=srcb;
	mSock=socket(AF_INET,SOCK_DGRAM,0);
	if(mSock<0)return;
	sockaddr_in sa;
	sa.sin_family=AF_INET;
	sa.sin_port=htons(4081);
	sa.sin_addr.s_addr=htonl(INADDR_ANY);
	if(bind(mSock,(sockaddr*)&sa,sizeof(sa))<0)
	{
		close(mSock);
		return;
	}
}

void SrvReady::checkForMessages()
{
	struct sockaddr from;
	unsigned int len=sizeof(from);
	char buf[120];
	memset(buf,0,120);
	ssize_t sz=recvfrom(mSock,&buf,120,MSG_DONTWAIT,&from,&len);
	while(sz>0)
	{
		memcpy(&mSenderAddr,&from,sizeof(mSenderAddr));
		parser.putData(buf,120);
		sz=recvfrom(mSock,&buf,120,MSG_DONTWAIT,&from,&len);
	}
}

void SrvReady::processMsg(const char *msg,const char **args,unsigned char argsCount)
{
	if(strcmp(msg,"server_ready")==0&&argsCount==2&&srvReadyCb)
	{
		Uuid id(args[0]);
		if(!id.isValid())return;
		srvReadyCb->processSrvReadyMsg(id,args[1],mSenderAddr.sin_addr);
	}
}
