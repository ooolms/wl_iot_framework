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

#ifndef CONTROLUIGROUP_H
#define CONTROLUIGROUP_H

#include "ControlUiElement.h"
#include "wliot/devices/ControlsDefinition.h"
#include "wliot/devices/DeviceState.h"

class QGroupBox;

namespace WLIOTUi
{
	class ControlUiGroup
		:public ControlUiElement
	{
		Q_OBJECT
	public:
		explicit ControlUiGroup(const WLIOT::ControlsGroup &grp,QObject *parent=0);
		virtual QWidget *widget()override;
		void updateState(const WLIOT::DeviceState &state);
		virtual bool isGroup()const override{return true;}

	private:
		QGroupBox *w;
		QList<ControlUiElement*> elements;
	};
}

#endif // CONTROLUIGROUP_H
