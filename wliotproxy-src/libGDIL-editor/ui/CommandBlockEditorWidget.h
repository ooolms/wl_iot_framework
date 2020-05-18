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

#ifndef COMMANDBLOCKEDITORWIDGET_H
#define COMMANDBLOCKEDITORWIDGET_H

#include <QWidget>
#include <wliot/devices/ControlsDefinition.h>
#include "GDIL/editor/IEditorHelper.h"

namespace Ui
{
	class CommandBlockEditorWidget;
}

class EditorInternalApi;

class CommandBlockEditorWidget
	:public QWidget
{
	Q_OBJECT
public:
	explicit CommandBlockEditorWidget(IEditorHelper *helper,QWidget *parent=nullptr);
	virtual ~CommandBlockEditorWidget();
	void setParams(const QUuid &devId,const QString &devName,const QByteArray &cmd,
		const QByteArrayList &args,quint32 inCount,bool enableConditionInput);
	QUuid devId()const;
	QString devName()const;
	QByteArray cmd()const;
	QByteArrayList args()const;
	quint32 inCount()const;
	bool enableConditionInput()const;

private slots:
	void onAddArgClicked();
	void onDelArgClicked();
	void onSelectDevClicked();

private:
	Ui::CommandBlockEditorWidget *ui;
	IEditorHelper *mHelper;
};

#endif // COMMANDBLOCKEDITORWIDGET_H
