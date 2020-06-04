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

#ifndef STORAGESOURCEBLOCKEDITORWIDGET_H
#define STORAGESOURCEBLOCKEDITORWIDGET_H

#include <QWidget>
#include <wliot/storages/StorageId.h>
#include <wliot/devices/SensorDef.h>
#include "VDIL/editor/IEditorHelper.h"

namespace Ui
{
	class StorageSourceBlockEditorWidget;
}

namespace WLIOTVDIL
{
	class EditorInternalApi;

	class StorageSourceBlockEditorWidget
		:public QWidget
	{
		Q_OBJECT
	public:
		explicit StorageSourceBlockEditorWidget(IEditorHelper *helper,QWidget *parent=nullptr);
		virtual ~StorageSourceBlockEditorWidget();
		void setParams(WLIOT::StorageId stId,const QString &devName,WLIOT::SensorDef::Type valType,
			quint32 cnt,bool needDevice);
		quint32 count()const;
		WLIOT::StorageId storageId()const;
		WLIOT::SensorDef::Type valuesType()const;
		bool needDevice()const;

	private slots:
		void onSelectStorageClicked();

	private:
		Ui::StorageSourceBlockEditorWidget *ui;
		WLIOT::StorageId mStorId;
		WLIOT::SensorDef::Type mValuesType;
		IEditorHelper *mHelper;
	};
}

#endif // STORAGESOURCEBLOCKEDITORWIDGET_H
