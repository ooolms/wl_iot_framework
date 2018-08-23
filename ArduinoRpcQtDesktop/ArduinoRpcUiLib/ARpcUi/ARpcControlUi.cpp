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

#include "ARpcUi/ARpcControlUi.h"
#include "ARpcUi_Private/ARpcControlUiGroup.h"
#include "ARpcBase/ARpcSyncCall.h"
#include <QLayout>

ARpcControlUi::ARpcControlUi(ARpcRealDevice *dev,const ARpcControlsGroup &controlsDef,QWidget *parent)
	:QWidget(parent)
{
	device=dev;
	msgDsp=new ARpcSimpleMsgDispatch(dev,this);
	connect(device,&ARpcRealDevice::destroyed,this,&ARpcControlUi::onDeviceDestroyed);
	mainGroup=new ARpcControlUiGroup(controlsDef,this);

	if(dev->isConnected())
	{
		ARpcDeviceState st;
		if(dev->getState(st))
			updateState(st);
	}

	QVBoxLayout *l=new QVBoxLayout(this);
	l->setContentsMargins(0,0,0,0);
	l->addWidget(mainGroup->widget());

	connect(mainGroup,&ARpcControlUiGroup::executeCommand,this,&ARpcControlUi::onExecuteCommand);
	connect(msgDsp,&ARpcSimpleMsgDispatch::commandStateChanged,this,&ARpcControlUi::onCommandStateChanged);
}

void ARpcControlUi::updateState(const ARpcDeviceState &state)
{
	mainGroup->updateState(state);
}

void ARpcControlUi::onDeviceDestroyed()
{
	device=0;
}

void ARpcControlUi::onExecuteCommand(const QByteArray &command,const QByteArrayList &args)
{
	if(!device)return;
	ARpcSyncCall call(device);
	call.call(command,args);
}

void ARpcControlUi::onCommandStateChanged(const QByteArray &command,int index,const QByteArray &value)
{
	ARpcDeviceState st;
	st.commandParams[command][index]=value;
	updateState(st);
}
