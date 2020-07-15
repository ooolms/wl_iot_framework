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

#ifndef PARAMHIDDEN_H
#define PARAMHIDDEN_H

#include "IParamElement.h"

namespace WLIOTUi
{
	class ParamHidden
		:public IParamElement
	{
	public:
		explicit ParamHidden(const WLIOT::ControlsCommandParam &p,QObject *parent=0);
		virtual QByteArray paramValue()override;
		virtual QWidget *widget()override;
		virtual void setValue(const QByteArray &v)override;

	private:
		QByteArray value;
	};
}

#endif // PARAMHIDDEN_H
