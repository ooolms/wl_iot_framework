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
	explicit ARpcVirtualDevice(const QUuid &id,const QByteArray &name,const QList<ARpcSensorDef> &sensors,
		const ARpcControlsGroup &controls,QObject *parent=nullptr);
	void reconnect();
	virtual bool writeMsg(const ARpcMessage &m) override;
	virtual bool isConnected() override;
	inline const QList<ARpcSensorDef>& sensors()
	{
		return mSensors;
	}

	inline const ARpcControlsGroup& controls()
	{
		return mControls;
	}

	void setSensors(const QList<ARpcSensorDef> &s);
	void setControls(const ARpcControlsGroup &c);
	void setSensors(const QByteArray &s);
	void setControls(const QByteArray &c);

public://messages from device
	void writeMsgFromDevice(const ARpcMessage &m);
	void writeInfo(const QByteArrayList &args);
	void writeMeasurement(const QByteArray &name,const QByteArrayList &values);
	void writeSync();

signals:
	void processDeviceCommand(const QByteArray &cmd,const QByteArrayList &args,bool &ok,QByteArrayList &retVal);
	void syncMsgNotify();

private:
	void writeOk(const QByteArrayList &args=QByteArrayList());
	void writeErr(const QByteArrayList &args);
	Q_INVOKABLE void writeMsgQueued(ARpcMessage m);

private:
	QUuid mId;
	QByteArray mName;
	QByteArray sensorsXml;
	QByteArray controlsXml;
	QList<ARpcSensorDef> mSensors;
	ARpcControlsGroup mControls;
	QByteArray callIdStr;
};

#endif // ARPCVIRTUALDEVICE_H
