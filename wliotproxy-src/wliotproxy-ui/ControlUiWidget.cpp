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

#include "ControlUiWidget.h"
#include <QTimer>
#include <QEventLoop>
#include <QLayout>

using namespace WLIOT;
using namespace WLIOTUi;

ControlUiWidget::ControlUiWidget(RealDevice *d,QWidget *parent)
	:QWidget(parent)
{
	dev=d;
	new QVBoxLayout(this);
	connect(dev,&RealDevice::disconnected,this,&ControlUiWidget::close);
}

bool ControlUiWidget::prepare()
{
	ControlsGroup grp;
	if(!dev->getControlsDescription(grp))
	{
		//TODO msg
		return false;
	}
	ui=new ControlUi(dev,grp,this);
	layout()->addWidget(ui);
	return true;
}
