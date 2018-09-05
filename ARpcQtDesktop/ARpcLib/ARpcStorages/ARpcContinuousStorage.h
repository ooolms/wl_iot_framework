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

#include "ARpcStorages/ARpcBaseFSSensorStorage.h"
#include "ARpcStorages/ARpcDBDriverFixedBlocks.h"
#include "ARpcStorages/ARpcDBDriverChainedBlocks.h"
#include "ARpcStorages/ARpcDBDriverHelpers.h"
#include "ARpcStorages/ARpcDBDriverGTimeIndex.h"

class ARpcContinuousStorage
	:public ARpcBaseFSSensorStorage
{
public:
	explicit ARpcContinuousStorage(const QUuid &devId,const QByteArray &devName,const ARpcSensorDef &sensor,
		TimestampRule tsRule,QObject *parent=0);
	virtual ~ARpcContinuousStorage();
	quint64 findInGTIndex(qint64 ts);
	bool create(bool gtIndex=false);

public:
	virtual bool writeSensorValue(const ARpcSensorValue *val)override;
	virtual bool open()override;
	virtual bool isOpened()const override;
	virtual quint64 valuesCount()override;
	virtual ARpcSensorValue* valueAt(quint64 index)override;

protected:
	virtual void closeFS()override;

private:
	bool createAsFixedBlocksDb(bool gtIndex);
	bool createAsChainedBlocksDb(bool gtIndex);

private:
	ARpcDBDriverFixedBlocks *fbDb;
	ARpcDBDriverChainedBlocks *cbDb;
	ARpcDBDriverGTimeIndex *indDb;
	enum
	{
		FIXED_BLOCKS,
		CHAINED_BLOCKS
	} dbType;

	bool opened;
	bool hasIndex;
};

#endif // ARPCCONTINUOUSSTORAGE_H
