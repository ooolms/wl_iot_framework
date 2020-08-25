#ifndef CLIENTSOCKET_H
#define CLIENTSOCKET_H

#include <sys/types.h>

class IClientSocketCallbacks
{
public:
	void onConnectionLost();
	void onNewData(const char *data,size_t sz);
};

class ClientSocket
{
public:
	explicit ClientSocket(IClientSocketCallbacks *cb);
	bool isConnected();
	bool connectToHost(const char *address);
	void checkData();
	void writeData(const char *s,size_t sz);
	void writeStr(const char *s);

private:
	void closeSock();

private:
	int mSock;
	IClientSocketCallbacks *callback;
};

#endif // CLIENTSOCKET_H
