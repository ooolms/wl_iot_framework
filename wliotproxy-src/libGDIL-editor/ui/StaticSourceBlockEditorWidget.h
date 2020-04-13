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
#include "GDIL/core/DataUnit.h"

namespace Ui
{
	class StaticSourceBlockEditorWidget;
}

class StaticSourceBlockEditorWidget
	:public QWidget
{
	Q_OBJECT
public:
	explicit StaticSourceBlockEditorWidget(QWidget *parent=nullptr);
	virtual ~StaticSourceBlockEditorWidget();
	DataUnit value()const;
	void setValue(const DataUnit &u);

private slots:
	void onTypeBtnClicked();
	void onArrAddRowClicked();
	void onArrDelRowClicked();

private:
	QString valToStr(const SensorValue *v,quint32 packIndex);

private:
	Ui::StaticSourceBlockEditorWidget *ui;
};

#endif // STATICSOURCEBLOCKEDITORWIDGET_H
