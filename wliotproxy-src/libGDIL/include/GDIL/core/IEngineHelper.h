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

#ifndef IENGINEHELPER_H
#define IENGINEHELPER_H

#include "wliot/devices/RealDevice.h"
#include "wliot/storages/ISensorStorage.h"
#include "wliot/storages/StorageId.h"

namespace WLIOTGDIL
{
	class IEngineHelper
	{
	public:
		virtual ~IEngineHelper(){}
		virtual bool devStateById(const QUuid &id,WLIOT::DeviceState &state)=0;
		virtual bool devIsConnected(const QUuid &id)=0;
		virtual WLIOT::ISensorStorage* storageById(const WLIOT::StorageId &id)=0;
	};
}

#endif // IENGINEHELPER_H
