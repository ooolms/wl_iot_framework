#ifndef ARPCSYNCCALL_H
#define ARPCSYNCCALL_H

#include "ARpcBase/ARpcConfig.h"
#include "ARpcBase/ARpcMessage.h"
#include <QObject>

class ARpcDevice;

class ARpcSyncCall
	:public QObject
{
	Q_OBJECT
public:
	ARpcSyncCall(QObject *parent=0);
	bool call(ARpcDevice *dev,const QString &func,const QStringList &args,QStringList &retVal);
	bool call(ARpcDevice *dev,const QString &func,QStringList &retVal);
	void abort();

signals:
//	void unhandledMsg(const ARpcMessage &m);
	void abortInternal();//for internal use

public:
	static const int defaultTimeout;//msec
};

#endif // ARPCSYNCCALL_H
