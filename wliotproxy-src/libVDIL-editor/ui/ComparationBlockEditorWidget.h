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

#ifndef COMPARATIONBLOCKEDITORWIDGET_H
#define COMPARATIONBLOCKEDITORWIDGET_H

#include <QObject>
#include <QWidget>
#include "VDIL/blocks/ComparationBlock.h"

namespace Ui
{
	class ComparationBlockEditorWidget;
}

namespace WLIOTVDIL
{
	class ComparationBlockEditorWidget
		:public QWidget
	{
		Q_OBJECT
	public:
		explicit ComparationBlockEditorWidget(QWidget *parent=nullptr);
		virtual ~ComparationBlockEditorWidget();
		void setParams(ComparationBlock::OutMode outMode,bool extV2Input,quint32 dimIndex,ComparationBlock::Operation op);
		void setDistValue(const DataUnit &v);
		void setV2Value(const DataUnit &v);
		ComparationBlock::OutMode outMode()const;
		DataUnit distValue()const;
		quint32 dimIndex()const;
		ComparationBlock::Operation operation()const;
		bool externalV2Input()const;
		DataUnit v2Value()const;

	private slots:
		void onDistCheckToggled();
		void onInternalV2CheckToggled();

	private:
		Ui::ComparationBlockEditorWidget *ui;
	};
}

#endif // COMPARATIONBLOCKEDITORWIDGET_H
