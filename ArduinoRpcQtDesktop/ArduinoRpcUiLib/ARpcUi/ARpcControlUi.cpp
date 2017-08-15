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
#include "ARpcBase/ARpcUnsafeCall.h"
#include <QLayout>

//TODO show values for slider and dial
//TODO slider and dial with separated button

ARpcControlUi::ARpcControlUi(ARpcDevice *dev,const ARpcControlsGroup &controlsDef,QWidget *parent)
	:QWidget(parent)
{
	device=dev;
	msgDsp=new ARpcSimpleMsgDispatch(dev,this);
	connect(device,&ARpcDevice::destroyed,this,&ARpcControlUi::onDeviceDestroyed);
	mainGroup=new ARpcControlUiGroup(controlsDef,this);

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

void ARpcControlUi::onExecuteCommand(const QString &command,const QStringList &args,bool syncCall)
{
	if(syncCall)
	{
		ARpcSyncCall call;
		QStringList retVal;
		call.call(device,command,args,retVal);
	}
	else
	{
		ARpcUnsafeCall call;
		QStringList retVal;
		call.call(device,command,args,retVal);
	}
}

void ARpcControlUi::onCommandStateChanged(const QString &command,int index,const QString &value)
{
	ARpcDeviceState st;
	st.commandParams[command][index]=value;
	updateState(st);
}
