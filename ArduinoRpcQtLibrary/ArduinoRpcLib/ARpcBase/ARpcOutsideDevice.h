#ifndef ARPCOUTSIDEDEVICE_H
#define ARPCOUTSIDEDEVICE_H

#include "ARpcBase/ARpcMessage.h"
#include "ARpcBase/ARpcStreamParser.h"
#include "ARpcBase/ARpcMessageParser.h"
#include <QObject>
#include <QIODevice>
#include <QUuid>

class ARpcOutsideDevice
	:public QObject
{
	Q_OBJECT
public:
	explicit ARpcOutsideDevice(QIODevice *d,QObject *parent=0);
	virtual ~ARpcOutsideDevice(){}
	bool writeMsg(const QString &msg);
	bool writeMsg(const QString &msg,const QStringList &args);
	bool writeMsg(const ARpcMessage &m);

signals:
	void disconnected();
	void connected();
	void rawMessage(const ARpcMessage &m);

private slots:
	void onDeviceDestroyed();
	void onDataReady();

protected://для потомков
	ARpcMessageParser msgParser;//использовать dump для реализации writeMsg
	ARpcStreamParser streamParser;//совать туда поток байт от устройства

private:
	QIODevice *dev;
};

#endif // ARPCOUTSIDEDEVICE_H
