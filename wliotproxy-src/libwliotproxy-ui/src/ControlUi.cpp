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

#include "wliot/ControlUi.h"
#include "ControlUiGroup.h"
#include "wliot/devices/CommandCall.h"
#include <QLayout>

using namespace WLIOTUi;
using namespace WLIOT;

ControlUi::ControlUi(RealDevice *dev,const ControlsGroup &controlsDef,QWidget *parent)
	:QWidget(parent)
{
	device=dev;
	msgDsp=new SimpleMsgDispatch(this);
	connect(device,&RealDevice::destroyed,this,&ControlUi::onDeviceDestroyed);
	connect(device,&RealDevice::newMessageFromDevice,msgDsp,&SimpleMsgDispatch::onNewMessageFromDevice);
	mainGroup=new ControlUiGroup(controlsDef,this);

	if(dev&&dev->isConnected())
	{
		DeviceState st;
		if(dev->getState(st))
			updateState(st);
	}

	QVBoxLayout *l=new QVBoxLayout(this);
	l->setContentsMargins(0,0,0,0);
	l->addWidget(mainGroup->widget());

	connect(mainGroup,&ControlUiGroup::executeCommand,this,&ControlUi::onExecuteCommand);
	connect(msgDsp,&SimpleMsgDispatch::commandStateChanged,this,&ControlUi::onCommandStateChanged);
}

void ControlUi::updateState(const DeviceState &state)
{
	mainGroup->updateState(state);
}

void ControlUi::onDeviceDestroyed()
{
	device=nullptr;
}

void ControlUi::onExecuteCommand(const QByteArray &command,const QByteArrayList &args)
{
	if(device&&device->isReady())
		device->execCommand(command,args);
	else emit deviceIsNotReady();
}

void ControlUi::onCommandStateChanged(const QByteArray &command,quint32 index,const QByteArray &value)
{
	DeviceState st;
	st.commandParams[command][index]=value;
	updateState(st);
}
