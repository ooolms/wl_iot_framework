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

#ifndef STATICSOURCEBLOCKEDITORWIDGET_H
#define STATICSOURCEBLOCKEDITORWIDGET_H

#include <QWidget>
#include "VDIL/core/DataUnit.h"
#include "VDIL/editor/DataUnitEdit.h"

class QCheckBox;

namespace WLIOTVDIL
{
	class StaticSourceBlockEditorWidget
		:public QWidget
	{
		Q_OBJECT
	public:
		explicit StaticSourceBlockEditorWidget(QWidget *parent=nullptr);
		void setParams(const DataUnit &u,bool configurable);
		DataUnit value()const;
		bool configurable()const;

	private:
		DataUnitEdit *edit;
		QCheckBox *configurableCheck;
	};
}

#endif // STATICSOURCEBLOCKEDITORWIDGET_H
