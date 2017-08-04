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

#ifndef ARPCCONTROLUI_H
#define ARPCCONTROLUI_H

#include <QWidget>
#include "ARpcBase/ARpcDevice.h"
#include "ARpcBase/ARpcOutsideDevice.h"
#include "ARpcBase/ARpcControlsDefinition.h"
#include "ARpcBase/ARpcDeviceState.h"
#include "ARpcBase/ARpcSimpleMsgDispatch.h"

class ARpcControlUiGroup;

class ARpcControlUi
	:public QWidget
{
	Q_OBJECT
public:
	explicit ARpcControlUi(ARpcDevice *dev,const ARpcControlsGroup &controlsDef,QWidget *parent=0);
	void updateState(const ARpcDeviceState &state);

private slots:
	void onDeviceDestroyed();
	void onExecuteCommand(const QString &command,const QStringList &args,bool syncCall);
	void onCommandStateChanged(const QString &command,int index,const QString &value);


private:
	ARpcDevice *device;
	ARpcControlUiGroup *mainGroup;
	ARpcSimpleMsgDispatch *msgDsp;
};

#endif // ARPCCONTROLUI_H