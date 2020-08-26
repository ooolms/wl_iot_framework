#ifndef CLIENTSOCKET_H
#define CLIENTSOCKET_H

#include "wliot/simple/IWriteCallback.h"
#include <sys/types.h>
#include <netinet/in.h>

class Device;

class ClientSocket
	:public IWriteCallback
{
public:
	explicit ClientSocket(Device *d);
	~ClientSocket();
	bool isConnected();
	bool connectToHost(const char *hostName);
	bool connectToAddr(const in_addr ipAddr);
	void disconnect();
	void checkForNewData();
	void waitForNewData();
	virtual void writeData(const char *data,unsigned long sz)override;
	virtual void writeStr(const char *str)override;

private:
	void closeSock();

private:
	Device *dev;
	int mSock;
};

#endif // CLIENTSOCKET_H
