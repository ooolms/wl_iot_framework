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
	ARpcSyncUnsafeCall(QObject *parent=0);
	bool call(ARpcDevice *dev,const QString &func,const QStringList &args,QStringList &retVal);
	bool call(ARpcDevice *dev,const QString &func,QStringList &retVal);
	void abort();

signals:
//	void unhandledMsg(const ARpcMessage &m);
	void abortInternal();
};

#endif // ARPCSYNCUNSAFECALL_H
