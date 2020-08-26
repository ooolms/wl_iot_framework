#include "wliot/simple/ClientSocket.h"
#include "wliot/simple/Device.h"
#include <unistd.h>
#include <netdb.h>
#include <string.h>
#include <errno.h>

ClientSocket::ClientSocket(Device *d)
{
	dev=d;
	mSock=-1;
}

ClientSocket::~ClientSocket()
{
	closeSock();
}

bool ClientSocket::isConnected()
{
	return mSock!=-1;
}

bool ClientSocket::connectToHost(const char *hostName)
{
	if(mSock!=-1)return false;
	addrinfo *info;
	if(getaddrinfo(hostName,"4081",0,&info))
		return false;
	mSock=socket(info->ai_family,SOCK_STREAM,0);
	freeaddrinfo(info);
	if(mSock==-1)
		return false;
	if(connect(mSock,info->ai_addr,info->ai_addrlen)==-1)
	{
		closeSock();
		return false;
	}
	return true;
}

bool ClientSocket::connectToAddr(const in_addr ipAddr)
{
	if(mSock!=-1)return false;
	mSock=socket(AF_INET,SOCK_STREAM,0);
	if(mSock==-1)
		return false;
	sockaddr_in sa;
	sa.sin_addr=ipAddr;
	sa.sin_port=htons(4081);
	sa.sin_family=AF_INET;
	if(connect(mSock,(sockaddr*)&sa,sizeof(sa))==-1)
	{
		closeSock();
		return false;
	}
	dev->onConnected();
	return true;
}

void ClientSocket::disconnect()
{
	if(mSock!=-1)
	{
		closeSock();
		dev->onDisconnected();
	}
}

void ClientSocket::checkForNewData()
{
	if(mSock==-1)return;
	char c[64];
	ssize_t num=recv(mSock,&c,64,MSG_DONTWAIT);
	while(num!=0)
	{
		if(num<0)
		{
			if(errno!=EAGAIN&&errno!=EWOULDBLOCK)
			{
				closeSock();
				dev->onDisconnected();
			}
			return;
		}
		else dev->onNewData(c,(size_t)num);
		num=recv(mSock,&c,64,MSG_DONTWAIT);
	}
}

void ClientSocket::waitForNewData()
{
	if(mSock==-1)return;
	char c[64];
	ssize_t num=recv(mSock,&c,64,0);
	if(num<0)
	{
		closeSock();
		dev->onDisconnected();
	}
	else if(num>0)
		dev->onNewData(c,(size_t)num);
}

void ClientSocket::writeData(const char *data,unsigned long sz)
{
	if(mSock==-1)return;
	send(mSock,data,sz,0);
}

void ClientSocket::writeStr(const char *str)
{
	if(mSock==-1)return;
	if(send(mSock,str,strlen(str),MSG_NOSIGNAL)==-1)
	{
		closeSock();
		dev->onDisconnected();
	}
}

void ClientSocket::closeSock()
{
	close(mSock);
	mSock=-1;
	dev->onDisconnected();
}
