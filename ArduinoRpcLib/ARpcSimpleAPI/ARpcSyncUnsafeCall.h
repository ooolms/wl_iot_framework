#ifndef ARPCSYNCUNSAFECALL_H
#define ARPCSYNCUNSAFECALL_H

#include "ARpcBase/ARpcConfig.h"
#include "ARpcBase/ARpcMessage.h"
#include <QObject>

class ARpcDevice;

class ARpcSyncUnsafeCall
	:public QObject
{
	Q_OBJECT
public:
	ARpcSyncUnsafeCall(const ARpcConfig &cfg,QObject *parent=0);
	bool call(ARpcDevice *dev,const ARpcMessage &callMsg,QStringList &retVal);
	void abort();

signals:
//	void unhandledMsg(const ARpcMessage &m);
	void abortInternal();

private:
	ARpcConfig config;
};

#endif // ARPCSYNCUNSAFECALL_H
