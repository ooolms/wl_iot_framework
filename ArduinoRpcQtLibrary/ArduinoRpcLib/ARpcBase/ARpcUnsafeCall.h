#ifndef ARPCUNSAFECALL_H
#define ARPCUNSAFECALL_H

#include "ARpcBase/ARpcConfig.h"
#include "ARpcBase/ARpcMessage.h"
#include <QObject>

class ARpcDevice;

class ARpcUnsafeCall
	:public QObject
{
	Q_OBJECT
public:
	ARpcUnsafeCall(QObject *parent=0);
	bool call(ARpcDevice *dev,const QString &func,const QStringList &args,QStringList &retVal);
	bool call(ARpcDevice *dev,const QString &func,QStringList &retVal);
	void abort();

signals:
//	void unhandledMsg(const ARpcMessage &m);
	void abortInternal();
};

#endif // ARPCUNSAFECALL_H
