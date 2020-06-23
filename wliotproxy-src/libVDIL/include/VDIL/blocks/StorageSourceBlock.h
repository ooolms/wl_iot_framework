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

#ifndef STORAGESOURCEBLOCK_H
#define STORAGESOURCEBLOCK_H

#include "VDIL/core/SourceBlock.h"
#include "wliot/storages/ISensorStorage.h"
#include "wliot/storages/StorageId.h"
#include <QPointer>

namespace WLIOTVDIL
{
	class StorageSourceBlock
		:public SourceBlock
	{
	public:
		explicit StorageSourceBlock(quint32 bId=0);
		virtual QString groupName()const override;
		virtual QString blockName()const override;
		void setParams(WLIOT::StorageId stId,WLIOT::SensorDef::Type valType,
			quint32 cnt,bool needDevice,bool useTrigger);
		quint32 count()const;
		const WLIOT::StorageId& storageId()const;
		const WLIOT::SensorDef::Type& valuesType()const;
		bool needDevice()const;
		bool useTrigger()const;
		virtual QSet<ITrigger*> mkTriggers()override;

	protected:
		virtual DataUnit extractDataInternal()override;
		virtual QList<QUuid> usedDevices()const override;

	public:
		static const QString mBlockName;

	private:
		WLIOT::StorageId mStorId;
		quint32 mCount;
		WLIOT::SensorDef::Type mValType;
		bool mNeedDevice;
		bool mUseTriggger;
	};
}

#endif // STORAGESOURCEBLOCK_H
