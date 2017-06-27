#ifndef ARPCDEVICE_H
#define ARPCDEVICE_H

#include "ARpcBase/ARpcMessage.h"
#include "ARpcBase/ARpcStreamParser.h"
#include "ARpcBase/ARpcMessageParser.h"
#include "ARpcBase/ARpcSensor.h"
#include "ARpcBase/ARpcControlsDefinition.h"
#include "ARpcBase/ARpcDeviceState.h"
#include <QObject>
#include <QUuid>
#include <QTimer>

class ARpcDevice
	:public QObject
{
	Q_OBJECT
public:
	explicit ARpcDevice(QObject *parent=0);
	virtual ~ARpcDevice(){}
	bool writeMsg(const QString &msg);
	bool writeMsg(const QString &msg,const QStringList &args);
	bool isIdentified();
	QUuid id();
	QString name();//human-readable
	bool getSensorsDescription(QList<ARpcSensor> &sensors);
	bool getControlsDescription(ARpcControlsGroup &controls);
	bool getState(ARpcDeviceState &state);

public:
	virtual bool writeMsg(const ARpcMessage &m)=0;
	virtual bool isConnected()=0;

public slots:
	bool identify();

signals:
	void disconnected();
	void connected();
	void rawMessage(const ARpcMessage &m);

private slots:
	void onDisconnected();

private:
	bool internallFunctionCall(const ARpcMessage &msg,
		const QString &estimatedReturnMsg,int timeout,QStringList &retVal);//timeout: msecs

protected://для потомков
	ARpcMessageParser msgParser;//использовать dump для реализации writeMsg
	ARpcStreamParser streamParser;//совать туда поток байт от устройства

private:
	QUuid devId;
	QString deviceName;//human-readable
	QTimer identifyTimer;
};

#endif // ARPCDEVICE_H
