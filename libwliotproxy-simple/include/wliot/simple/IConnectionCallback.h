#ifndef ICONNECTIONCALLBACK_H
#define ICONNECTIONCALLBACK_H

#include <sys/types.h>

class IConnectionCallback
{
public:
	virtual ~IConnectionCallback(){}
	virtual void onConnected()=0;
	virtual void onDisconnected()=0;
	virtual void onSyncTimeout()=0;
};

#endif // ICONNECTIONCALLBACK_H
