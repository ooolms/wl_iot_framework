#ifndef ARPCVIRTUALDEVICE_H
#define ARPCVIRTUALDEVICE_H

#include "ARpcRealDevice.h"

/**
 * @brief The ARpcVirtualDevice class
 * Виртуальное устройство. Идентификатор и имя задаются заранее,
 * сообщения к устройству обрабатываются вовне (сигнал messageToDevice),
 * сообщения от устройства так же генерируются извне (функция writeMsgFromDevice).
 * Устройство всегда подключено и идентифицировано.
 */
class ARpcVirtualDevice
	:public ARpcRealDevice
{
	Q_OBJECT

public:
	explicit ARpcVirtualDevice(const QUuid &id,const QString &name,QObject *parent=nullptr);
	virtual bool writeMsg(const ARpcMessage &m)override;
	virtual bool isConnected()override;

public://messages from device
	void writeMsgFromDevice(const ARpcMessage &m);
	void setSensors(const QList<ARpcSensor> &sensors);
	void setControls(const ARpcControlsGroup &controls);
	void writeInfo(const QStringList &args);
	void writeMeasurement(const QString &name,const QStringList &values);
	void writeSync();

signals:
	void processDeviceCommand(const QString &cmd,const QStringList &args,bool &ok,QStringList &retVal);

private:
	void writeOk(const QStringList &args=QStringList());
	void writeErr(const QStringList &args);

private:
	QUuid mId;
	QString mName;
	QString sensorsXml;
	QString controlsXml;
};

#endif // ARPCVIRTUALDEVICE_H
