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

#ifndef ARRAYCOMBINEBLOCKEDITORWIDGET_H
#define ARRAYCOMBINEBLOCKEDITORWIDGET_H

#include <QWidget>

class QSpinBox;

namespace WLIOTVDIL
{
	class ArrayCombineBlockEditorWidget
		:public QWidget
	{
		Q_OBJECT
	public:
		explicit ArrayCombineBlockEditorWidget(QWidget *parent=nullptr);
		void setParams(quint32 inCount,quint32 dim);
		quint32 inputsCount()const;
		quint32 dim()const;

	private:
		QSpinBox *inCountEdit,*dimEdit;
	};
}

#endif // ARRAYCOMBINEBLOCKEDITORWIDGET_H
