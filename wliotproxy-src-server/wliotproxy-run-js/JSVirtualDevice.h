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

#ifndef JSVIRTUALDEVICE_H
#define JSVIRTUALDEVICE_H

#include "JSDevice.h"
#include "wliot/devices/VirtualDeviceBackend.h"

//TODO убрать автоматическое создание состояний элем. управления из описания
class JSVirtualDevice
	:public JSDevice
	,public WLIOT::IVirtualDeviceBackendCallback
{
	Q_OBJECT

public:
	/**
	 * @brief JSVirtualDevice
	 * на момент вызова device еще не содержит нужный backend
	 */
	explicit JSVirtualDevice(WLIOT::RealDevice *d,QScriptEngine *e,const QList<WLIOT::SensorDef> &sensors,
		const WLIOT::ControlsGroup &controls,IdType uid,QObject *parent=nullptr);
	virtual ~JSVirtualDevice();
	void setBackend(WLIOT::VirtualDeviceBackend *be);
	virtual void onMessageToVDev(WLIOT::VirtualDeviceBackend *vDev,const WLIOT::Message &m)override;
	Q_INVOKABLE void setupAdditionalStateAttributes(const QScriptValue &names);

public://messages from device
//	Q_INVOKABLE void writeMsgFromDevice(QScriptValue titleStr,QScriptValue argsArray);//argsArray - array of strings
	Q_INVOKABLE void writeInfo(QScriptValue argsList);
	Q_INVOKABLE void writeMeasurement(QScriptValue name,QScriptValue value);
	Q_INVOKABLE void setCommandCallback(QScriptValue cbFunc);
	Q_INVOKABLE void commandParamStateChanged(
		const QScriptValue &cmd,QScriptValue paramIndex,const QScriptValue &value);
	Q_INVOKABLE void additionalStateChanged(const QScriptValue &paramName,const QScriptValue &value);

signals:
	void syncMsgNotify();

private:
	void writeOk(const QByteArray &callId,const QByteArrayList &args);
	void writeErr(const QByteArray &callId,const QByteArrayList &args);
	bool processCommand(const QByteArray &cmd,const QByteArrayList &args,QByteArrayList &retVal);
	void prepareStateFromControls(const WLIOT::ControlsGroup &grp);

private:
	WLIOT::VirtualDeviceBackend *mBackend;
	QScriptValue cmdCallback;
	QList<WLIOT::SensorDef> mSensors;
	WLIOT::ControlsGroup mControls;
	WLIOT::DeviceState mState;
};

#endif // JSVIRTUALDEVICE_H
