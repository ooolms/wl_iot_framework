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

#ifndef STORAGEID_H
#define STORAGEID_H

#include <QUuid>
#include <QMetaType>

namespace WLIOT
{
	class StorageId
	{
	public:
		bool operator==(const StorageId &id)const
		{
			return deviceId==id.deviceId&&sensorName==id.sensorName;
		}

		bool operator<(const StorageId &id)const
		{
			if(deviceId==id.deviceId)
				return sensorName<id.sensorName;
			return deviceId<id.deviceId;
		}

	public:
		QUuid deviceId;
		QByteArray sensorName;
	};

	uint qHash(const StorageId &id,uint seed=0)noexcept;
}

Q_DECLARE_METATYPE(WLIOT::StorageId)

#endif // STORAGEID_H
