#ifndef ARPCSYNCCALL_H
#define ARPCSYNCCALL_H

#include "ARpcMessage.h"
#include <QObject>

class ARpcDevice;

class ARpcSyncCall
	:public QObject
{
	Q_OBJECT
public:
	explicit ARpcSyncCall(QObject *parent=0);
	ARpcSyncCall(const QString &okMsg,const QString &errMsg,QObject *parent=0);
	ARpcSyncCall(const QString &okMsg,const QString &errMsg,const QString &syncMsg,QObject *parent=0);
	bool call(const ARpcMessage &callMsg,ARpcDevice *dev,QStringList &retVal);

signals:
//	void unhandledMsg(const ARpcMessage &m);

public:
	static const int defaultTimeout;//msec

private:
	QString okMessage;//default - ok
	QString errMessage;//default - err
	QString syncMessage;//default - sync
};

#endif // ARPCSYNCCALL_H
