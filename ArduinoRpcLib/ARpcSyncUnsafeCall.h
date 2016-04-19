#ifndef ARPCSYNCUNSAFECALL_H
#define ARPCSYNCUNSAFECALL_H

#include "ARpcConfig.h"
#include "ARpcMessage.h"
#include <QObject>

class ARpcDevice;

class ARpcSyncUnsafeCall
	:public QObject
{
	Q_OBJECT
public:
	ARpcSyncUnsafeCall(ARpcConfig &cfg,QObject *parent=0);
	ARpcSyncUnsafeCall(const QString &okMsg,const QString &errMsg,const QString &syncMsg,QObject *parent=0);
	bool call(const ARpcMessage &callMsg,ARpcDevice *dev,QStringList &retVal);

signals:
//	void unhandledMsg(const ARpcMessage &m);

private:
	ARpcConfig &config;
};

#endif // ARPCSYNCUNSAFECALL_H
