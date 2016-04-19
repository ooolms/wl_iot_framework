#ifndef ARPCDEVICE_H
#define ARPCDEVICE_H

#include "ARpcMessage.h"
#include "ARpcSyncCall.h"
#include "ARpcConfig.h"
#include <QObject>

class ARpcDevice
	:public QObject
{
	Q_OBJECT
public:
	explicit ARpcDevice(const ARpcConfig &cfg,QObject *parent=0);
	virtual ~ARpcDevice(){}
	virtual bool writeMsg(const ARpcMessage &m)=0;
	virtual bool writeMsg(const QString &msg)=0;
	virtual bool writeMsg(const QString &msg,const QStringList &args)=0;
	virtual bool callSync(const ARpcMessage &m,QStringList &retVal);
	virtual bool callSyncUnsafe(const ARpcMessage &m,QStringList &retVal);//with no sync every second
	virtual bool isConnected()=0;

protected slots:
	void processMessage(const ARpcMessage &m);

signals:
	void ttyDisconnected();
	void ttyConnected();
	void infoMessage(const ARpcMessage &m);
	void rawMessage(const ARpcMessage &m);

protected:
	ARpcConfig config;
};

#endif // ARPCDEVICE_H
