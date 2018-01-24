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
#include "ARpcBase/ARpcVirtualDevice.h"

class JSVirtualDevice
	:public JSDevice
{
	Q_OBJECT

public:
	explicit JSVirtualDevice(ARpcVirtualDevice *d,QScriptEngine *e,QObject *parent=nullptr);

public://messages from device
	Q_INVOKABLE void writeMsgFromDevice(QScriptValue titleStr,QScriptValue argsArray);//argsArray - array of strings
	Q_INVOKABLE void writeInfo(QScriptValue argsList);
	Q_INVOKABLE void writeMeasurement(QScriptValue name,QScriptValue values);
	Q_INVOKABLE void writeSync();
	Q_INVOKABLE void setCommandCallback(QScriptValue cbFunc);
	Q_INVOKABLE void setSensorsXml(QScriptValue xml);
	Q_INVOKABLE void setControlsXml(QScriptValue xml);

private slots:
	void onProcessDeviceCommand(const QString &cmd,const QStringList &args,bool &ok,QStringList &retVal);

private:
	ARpcVirtualDevice *vDev;
	QScriptValue cmdCallback;
};

#endif // JSVIRTUALDEVICE_H
