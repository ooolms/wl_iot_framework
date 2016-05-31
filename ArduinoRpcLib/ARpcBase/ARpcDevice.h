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

public:
	virtual bool writeMsg(const ARpcMessage &m)=0;
	virtual bool isConnected()=0;

signals:
	void disconnected();
	void connected();
	void rawMessage(const ARpcMessage &m);

protected:
	ARpcConfig config;
	ARpcMessageParser msgParser;
	ARpcStreamParser streamParser;
};

#endif // ARPCDEVICE_H
