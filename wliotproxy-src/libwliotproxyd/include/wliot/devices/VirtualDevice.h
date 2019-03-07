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

#ifndef VIRTUALDEVICE_H
#define VIRTUALDEVICE_H

#include "wliot/devices/RealDevice.h"

/**
 * @brief The ARpcVirtualDevice class
 * Виртуальное устройство. Идентификатор и имя задаются заранее,
 * сообщения к устройству обрабатываются вовне (сигнал messageToDevice),
 * сообщения от устройства так же генерируются извне (функция writeMsgFromDevice).
 * Устройство всегда подключено и идентифицировано.
 */
class VirtualDevice
	:public RealDevice
{
	Q_OBJECT

public:
	explicit VirtualDevice(const QUuid &id,const QByteArray &name,const QList<SensorDef> &sensors,
		const ControlsGroup &controls,QObject *parent=nullptr);
	void setupAdditionalStateAttributes(const QByteArrayList &names);
	virtual bool writeMsgToDevice(const Message &m)override;
	void setConnected(bool c);
	inline const QList<SensorDef>& sensors()
	{
		return mSensors;
	}
	inline const ControlsGroup& controls()
	{
		return mControls;
	}
	virtual QByteArray deviceType(){return "virtual";}

public://messages from device, called when some messages comes from client
	void writeInfo(const QByteArrayList &args);
	void writeMeasurement(const QByteArray &name,const QByteArrayList &values);
	void commandParamStateChanged(const QByteArray &name,int paramNum /*from 1*/,const QByteArray &value);
	void additionalStateChanged(const QByteArray &name,const QByteArray &value);
	void writeCommandCallSync(const QByteArray &callId);

signals://device actions, to process send some messages to client and wait for answer
	void processDeviceCommand(const QByteArray &cmd,const QByteArrayList &args,bool &ok,QByteArrayList &retVal);
	void syncMsgNotify();

private:
	void writeOk(const QByteArrayList &args=QByteArrayList());
	void writeErr(const QByteArrayList &args);
	void prepareStateFromControls(const ControlsGroup &grp);
	Q_INVOKABLE void writeMsgToDeviceQueued(Message m);

private:
	QUuid mId;
	QByteArray mName;
	QByteArray sensorsXml;
	QByteArray controlsXml;
	QList<SensorDef> mSensors;
	ControlsGroup mControls;
	DeviceState selfState;
	QByteArray callIdStr;
};

#endif // VIRTUALDEVICE_H
