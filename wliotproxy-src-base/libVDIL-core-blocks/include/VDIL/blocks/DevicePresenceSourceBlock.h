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

#ifndef DEVICEPRESENCESOURCEBLOCK_H
#define DEVICEPRESENCESOURCEBLOCK_H

#include "VDIL/core/SourceBlock.h"

namespace WLIOTVDIL
{
	class DevicePresenceSourceBlock
		:public SourceBlock
	{
	public:
		explicit DevicePresenceSourceBlock(quint32 blockId=0);
		virtual QString groupName()const override;
		virtual QString blockName()const override;
		QUuid deviceId()const;
		void setDeviceId(const QUuid &deviceId);

	protected:
		virtual DataUnit extractDataInternal()override;
		virtual void evalInternal(const DataUnit &data,bool triggerActivated)override;

	public:
		static const QString mBlockName;

	private:
		BlockOutput *connectedOut,*triggerOut;
		QUuid mDevId;
	};
}

#endif // DEVICEPRESENCESOURCEBLOCK_H
