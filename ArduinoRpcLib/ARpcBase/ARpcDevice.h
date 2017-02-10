#ifndef ARPCDEVICE_H
#define ARPCDEVICE_H

#include "ARpcBase/ARpcMessage.h"
#include "ARpcBase/ARpcStreamParser.h"
#include "ARpcBase/ARpcMessageParser.h"
#include <QObject>
#include <QUuid>

class ARpcDevice
	:public QObject
{
	Q_OBJECT
public:
	explicit ARpcDevice(QObject *parent=0);
	virtual ~ARpcDevice(){}
	bool writeMsg(const QString &msg);
	bool writeMsg(const QString &msg,const QStringList &args);
	bool identify();
	bool isIdentified();
	QString name();//human-readable

public:
	virtual bool writeMsg(const ARpcMessage &m)=0;
	virtual bool isConnected()=0;

signals:
	void disconnected();
	void connected();
	void rawMessage(const ARpcMessage &m);

protected:
	ARpcMessageParser msgParser;
	ARpcStreamParser streamParser;
	QUuid uuid;
	QString deviceName;//human-readable
};

#endif // ARPCDEVICE_H
