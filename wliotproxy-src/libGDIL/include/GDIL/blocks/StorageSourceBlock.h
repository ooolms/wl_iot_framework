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

#include "GDIL/core/SourceBlock.h"
#include "wliot/storages/ISensorStorage.h"
#include "wliot/storages/StorageId.h"
#include <QPointer>

class StorageSourceBlock
	:public SourceBlock
{
public:
	explicit StorageSourceBlock(quint32 bId=0);
	virtual QString groupName()const override;
	virtual QString blockName()const override;
	void setParams(StorageId stId,const QString &devName,SensorDef::Type valType,quint32 cnt);
	quint32 count()const;
	const StorageId& storageId()const;
	const QString& devName()const;
	const SensorDef::Type& valuesType()const;

protected:
	virtual DataUnit extractDataInternal()override;

public:
	static const QString mBlockName;

private:
	StorageId mStorId;
	QString mDevName;
	quint32 mCount;
	SensorDef::Type mValType;
};

#endif // STORAGESOURCEBLOCK_H
