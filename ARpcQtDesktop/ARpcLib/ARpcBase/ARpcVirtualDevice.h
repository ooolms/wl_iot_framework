/*******************************************
Copyright 2017 OOO "LMS"

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

	http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

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
	void setupAdditionalStateAttributes(const QByteArrayList &names);
	virtual bool writeMsg(const ARpcMessage &m)override;
	virtual bool isConnected() override;
	inline const QList<ARpcSensorDef>& sensors()
	{
		return mSensors;
	}

	inline const ARpcControlsGroup& controls()
	{
		return mControls;
	}
	virtual QByteArray deviceType(){return "virtual";}
	void commandParamStateChanged(const QByteArray &name,int paramNum /*from 1*/,const QByteArray &value);

public://messages from device
	void writeMsgFromDevice(const ARpcMessage &m);
	void writeInfo(const QByteArrayList &args);
	void writeMeasurement(const QByteArray &name,const QByteArrayList &values);

signals:
	void processDeviceCommand(const QByteArray &cmd,const QByteArrayList &args,bool &ok,QByteArrayList &retVal);
	void syncMsgNotify();

private:
	void writeOk(const QByteArrayList &args=QByteArrayList());
	void writeErr(const QByteArrayList &args);
	void prepareStateFromControls(const ARpcControlsGroup &grp);
	Q_INVOKABLE void writeMsgQueued(ARpcMessage m);

private:
	QUuid mId;
	QByteArray mName;
	QByteArray sensorsXml;
	QByteArray controlsXml;
	QList<ARpcSensorDef> mSensors;
	ARpcControlsGroup mControls;
	ARpcDeviceState selfState;
	QByteArray callIdStr;
};

#endif // ARPCVIRTUALDEVICE_H
