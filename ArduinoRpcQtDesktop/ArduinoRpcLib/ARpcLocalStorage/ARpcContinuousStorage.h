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

#ifndef ARPCCONTINUOUSSTORAGE_H
#define ARPCCONTINUOUSSTORAGE_H

#include "ARpcLocalStorage/ARpcISensorStorage.h"
#include "ARpcLocalStorage/ARpcDBDriverFixedBlocks.h"
#include "ARpcLocalStorage/ARpcDBDriverChainedBlocks.h"
#include "ARpcLocalStorage/ARpcDBDriverHelpers.h"
#include "ARpcLocalStorage/ARpcDBDriverGTimeIndex.h"

class ARpcContinuousStorage
	:public ARpcISensorStorage
{
public:
	explicit ARpcContinuousStorage(ARpcSensor::Type valType,QObject *parent=0);
	virtual ~ARpcContinuousStorage();
	bool createAsFixedBlocksDb(const ARpcISensorValue &templateValue,bool gtIndex=false);
	bool createAsChainedBlocksDb(bool gtIndex=false);
	bool isFixesBlocksDb()const;
	bool isChainedBlocksDb()const;
	quint64 valuesCount();
	ARpcISensorValue* valueAt(quint64 index);
	quint64 findInGTIndex(qint64 ts);
	virtual bool isOpened()const override;

public:
	virtual StoreMode getStoreMode()const override;
	virtual bool writeSensorValue(const ARpcISensorValue *val)override;
	virtual ARpcSensor::Type effectiveValuesType()const override;
	virtual bool open()override;
	virtual TimestampRule fixTimestampRule(TimestampRule rule)override;

protected:
	virtual void closeInternal()override;

private:
	QVector<quint32> sizesFromTemplateValue(const ARpcISensorValue &value);

private:
	ARpcDBDriverFixedBlocks *fbDb;
	ARpcDBDriverChainedBlocks *cbDb;
	ARpcDBDriverGTimeIndex *indDb;
	ARpcDBDriverHelpers hlp;
	enum{FIXED_BLOCKS,CHAINED_BLOCKS}dbType;
	bool opened;
	bool hasIndex;
	ARpcSensor::Type effectiveValType;
};

#endif // ARPCCONTINUOUSSTORAGE_H
