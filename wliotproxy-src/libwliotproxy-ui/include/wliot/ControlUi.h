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

#ifndef CONTROLUI_H
#define CONTROLUI_H

#include <QWidget>
#include "wliot/devices/RealDevice.h"
#include "wliot/devices/ControlsDefinition.h"
#include "wliot/devices/DeviceState.h"
#include "wliot/devices/SimpleMsgDispatch.h"

class ControlUiGroup;

class ControlUi
	:public QWidget
{
	Q_OBJECT
public:
	explicit ControlUi(RealDevice *dev,const ControlsGroup &controlsDef,QWidget *parent=nullptr);
	void updateState(const DeviceState &state);

signals:
	void deviceIsNotReady();

private slots:
	void onDeviceDestroyed();
	void onExecuteCommand(const QByteArray &command,const QByteArrayList &args);
	void onCommandStateChanged(const QByteArray &command,quint32 index,const QByteArray &value);

private:
	RealDevice *device;
	ControlUiGroup *mainGroup;
	SimpleMsgDispatch *msgDsp;
};

#endif // CONTROLUI_H
