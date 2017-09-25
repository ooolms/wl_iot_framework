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
	explicit ARpcVirtualDevice(const QUuid &id,const QString &name,const QList<ARpcSensor> &sensors,
		const ARpcControlsGroup &controls,QObject *parent=nullptr);
	virtual bool writeMsg(const ARpcMessage &m) override;
	virtual bool isConnected() override;
	inline const QList<ARpcSensor>& sensors()
	{
		return mSensors;
	}

	inline const ARpcControlsGroup& controls()
	{
		return mControls;
	}

public://messages from device
	void writeMsgFromDevice(const ARpcMessage &m);
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
	QList<ARpcSensor> mSensors;
	ARpcControlsGroup mControls;
};

#endif // ARPCVIRTUALDEVICE_H
