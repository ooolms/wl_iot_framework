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

#ifndef IPARAMELEMENT_H
#define IPARAMELEMENT_H

#include "wliot/devices/ControlsDefinition.h"
#include <QWidget>

namespace WLIOTUi
{
	class IParamElement
		:public QObject
	{
		Q_OBJECT
	public:
		explicit IParamElement(QObject *parent=0);
		virtual QByteArray paramValue()=0;
		virtual QWidget* widget()=0;
		virtual void setValue(const QByteArray &v)=0;

	public:
		static IParamElement* makeElement(const WLIOT::ControlParam &param);

	signals:
		void activated();
	};
}

#endif // IPARAMELEMENT_H
