#ifndef IOTSERVERIDEVICESSOURCE_H
#define IOTSERVERIDEVICESSOURCE_H

#include <QObject>
#include "ARpcBase/ARpcRealDevice.h"
#include "IotServerTypes.h"

class IotServerIDevicesSource
	:public QObject
{
	Q_OBJECT
public:
	explicit IotServerIDevicesSource(QObject *parent = nullptr);
	virtual QList<IotServerTtyPortDescr> ttyPortsList()=0;
	virtual bool identifyTty(const QByteArray &portName)=0;
	virtual bool identifyTcp(const QByteArray &host)=0;
	virtual QList<QUuid> identifiedDevices()=0;
	virtual ARpcRealDevice *devById(const QUuid &id)=0;

signals:
	void deviceIdentified(const QUuid &id,const QByteArray &name);
	void deviceLost(const QUuid &id);
};

#endif // IOTSERVERIDEVICESSOURCE_H
