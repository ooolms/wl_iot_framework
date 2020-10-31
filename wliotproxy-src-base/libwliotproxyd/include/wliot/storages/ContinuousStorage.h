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

#ifndef CONTINUOUSSTORAGE_H
#define CONTINUOUSSTORAGE_H

#include "wliot/storages/BaseFSSensorStorage.h"

namespace WLIOT
{
	class DBDriverFixedBlocks;
	class DBDriverChainedBlocks;
	class DBDriverHelpers;
	class DBDriverGTimeIndex;

	class ContinuousStorage
		:public BaseFSSensorStorage
	{
	public:
		explicit ContinuousStorage(const QString &path,const QUuid &devId,const QByteArray &devName,
			const SensorDef &sensor,TimestampRule tsRule,QObject *parent=0);
		virtual ~ContinuousStorage();
		bool create(bool gtIndex=false);
		virtual void close()override;

	public:
		virtual bool writeSensorValue(const SensorValue *val)override;
		virtual bool open()override;
		virtual bool isOpened()const override;
		virtual quint64 valuesCount()override;
		virtual SensorValue* valueAt(quint64 index)override;
		virtual bool hasGTIndex()override;
		virtual quint64 findInGTIndex(qint64 ts)override;

	private:
		bool createAsFixedBlocksDb(bool gtIndex);
		bool createAsChainedBlocksDb(bool gtIndex);

	private:
		DBDriverFixedBlocks *fbDb;
		DBDriverChainedBlocks *cbDb;
		DBDriverGTimeIndex *indDb;
		enum
		{
			FIXED_BLOCKS,
			CHAINED_BLOCKS
		} dbType;

		bool opened;
		bool mHasGTIndex;
	};
}

#endif // CONTINUOUSSTORAGE_H
