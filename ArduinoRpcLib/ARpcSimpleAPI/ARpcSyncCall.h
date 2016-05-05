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
	ARpcSyncCall(ARpcConfig &cfg,QObject *parent=0);
	ARpcSyncCall(const QString &okMsg,const QString &errMsg,const QString &syncMsg,QObject *parent=0);
	bool call(const ARpcMessage &callMsg,ARpcDevice *dev,QStringList &retVal);

signals:
//	void unhandledMsg(const ARpcMessage &m);

public:
	static const int defaultTimeout;//msec

private:
	ARpcConfig &config;
};

#endif // ARPCSYNCCALL_H
