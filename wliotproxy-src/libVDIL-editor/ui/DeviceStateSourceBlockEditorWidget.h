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

#ifndef DEVICESTATESOURCEBLOCKEDITORWIDGET_H
#define DEVICESTATESOURCEBLOCKEDITORWIDGET_H

#include <QWidget>
#include "EditorInternalApi.h"
#include "VDIL/editor/IEditorHelper.h"

namespace Ui
{
	class DeviceStateSourceBlockEditorWidget;
}

namespace WLIOTVDIL
{
	class EditorInternalApi;

	class DeviceStateSourceBlockEditorWidget
		:public QWidget
	{
		Q_OBJECT
	public:
		explicit DeviceStateSourceBlockEditorWidget(IEditorHelper *helper,QWidget *parent=nullptr);
		~DeviceStateSourceBlockEditorWidget();
		void setParams(const QUuid &devId,const QString &devName,const QByteArray &stateKey,
			bool cmdState,bool boolOut,quint32 cmdStateIndex);
		QUuid deviceId()const;
		QByteArray stateKey()const;
		bool commandState()const;
		bool boolOut()const;
		quint32 commandStateIndex()const;

	private slots:
		void onSelectDeviceClicked();
		void onNumBtnStateChanged();

	private:
		IEditorHelper *mHelper;
		Ui::DeviceStateSourceBlockEditorWidget *ui;
	};
}

#endif // DEVICESTATESOURCEBLOCKEDITORWIDGET_H
