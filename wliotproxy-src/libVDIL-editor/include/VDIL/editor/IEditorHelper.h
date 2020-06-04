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

#ifndef IEDITORHELPER_H
#define IEDITORHELPER_H

#include <QUuid>
#include <QString>
#include <wliot/devices/ControlsDefinition.h>
#include <wliot/devices/SensorDef.h>
#include <wliot/storages/StorageId.h>

namespace WLIOTVDIL
{
	class IEditorHelper
	{
	public:
		virtual ~IEditorHelper(){}
		virtual QString deviceName(const QUuid &devId)=0;
		virtual bool selectDevice(QUuid &deviceId,QString &deviceName,WLIOT::ControlsGroup &controls)=0;
		virtual bool selectStorage(WLIOT::StorageId &storId,QString &deviceName,WLIOT::SensorDef::Type &valuesType)=0;
	};
}

#endif // IEDITORHELPER_H
