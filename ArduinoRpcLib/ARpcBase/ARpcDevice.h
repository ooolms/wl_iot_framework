#ifndef ARPCDEVICE_H
#define ARPCDEVICE_H

#include "ARpcBase/ARpcMessage.h"
#include "ARpcBase/ARpcConfig.h"
#include "ARpcBase/ARpcStreamParser.h"
#include "ARpcBase/ARpcMessageParser.h"
#include <QObject>

class ARpcDevice
	:public QObject
{
	Q_OBJECT
public:
	explicit ARpcDevice(const ARpcConfig &cfg,QObject *parent=0);
	virtual ~ARpcDevice(){}
	bool writeMsg(const QString &msg);
	bool writeMsg(const QString &msg,const QStringList &args);
	bool callSync(const ARpcMessage &m,QStringList &retVal);
	bool callSyncUnsafe(const ARpcMessage &m,QStringList &retVal);//with no sync every second

public:
	virtual bool writeMsg(const ARpcMessage &m)=0;
	virtual bool isConnected()=0;

protected slots:
	void processMessage(const ARpcMessage &m);

signals:
	void disconnected();
	void connected();
	void infoMessage(const ARpcMessage &m);
	void rawMessage(const ARpcMessage &m);

protected:
	ARpcConfig config;
	ARpcMessageParser msgParser;
	ARpcStreamParser streamParser;
};

#endif // ARPCDEVICE_H
