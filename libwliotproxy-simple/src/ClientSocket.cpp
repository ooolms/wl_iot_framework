#include "wliot/simple/ClientSocket.h"

#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>
#include <errno.h>

ClientSocket::ClientSocket(IClientSocketCallbacks *cb)
{
	mSock=-1;
	callback=cb;
}

bool ClientSocket::isConnected()
{
	return mSock!=-1;
}

bool ClientSocket::connectToHost(const char *address)
{
	addrinfo *info;
	if(getaddrinfo(address,"4081",0,&info))
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

void ClientSocket::checkData()
{
	char c[64];
	if(mSock==-1)return;
	ssize_t num=recv(mSock,&c,64,MSG_DONTWAIT);
	if(num<0)
	{
		if(errno==EAGAIN||errno==EWOULDBLOCK)
			return;
		closeSock();
		callback->onConnectionLost();
	}
	else if(num>0)
		callback->onNewData(c,(size_t)num);
}

void ClientSocket::writeData(const char *s,size_t sz)
{
	if(mSock==-1)return;
	send(mSock,s,sz,0);
}

void ClientSocket::writeStr(const char *s)
{
	if(mSock==-1)return;
	send(mSock,s,strlen(s),0);
}

void ClientSocket::closeSock()
{
	close(mSock);
	mSock=-1;
}
