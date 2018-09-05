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

#ifndef ARPCLASTNVALUESSTORAGE_H
#define ARPCLASTNVALUESSTORAGE_H

#include "ARpcBaseFSSensorStorage.h"
#include "ARpcStorages/ARpcDBDriverHelpers.h"
#include "ARpcStorages/ARpcDBDriverFixedBlocks.h"

/**
 * @brief The ARpcLastNValuesStorage class
 * Хранит последние N значений в N файлах по принципу циклического FIFO буфера.
 * В файле db.index - индекс, с которого начинается буфер (текстом).
 * Элементы выдаются в порядке записи. Т.е. по индексу 0 база выдает самый старый элемент.
 */
class ARpcLastNValuesStorage
	:public ARpcBaseFSSensorStorage
{
	Q_OBJECT

public:
	explicit ARpcLastNValuesStorage(const QUuid &devId,const QByteArray &devName,
		const ARpcSensorDef &sensor,TimestampRule tsRule,QObject *parent=0);
	virtual ~ARpcLastNValuesStorage();
	bool create(quint32 storedValuesCount);
	virtual bool isOpened() const override;

public:
	virtual bool open() override;//use dbDir when opening
	virtual bool writeSensorValue(const ARpcSensorValue *val) override;
	virtual quint64 valuesCount() override;
	virtual ARpcSensorValue* valueAt(quint64 index) override;

protected:
	virtual void closeFS()override;

private:
	ARpcSensorValue* readValue(quint32 index);
	QByteArray readValueData(quint32 index);

private:
	quint32 storedCount;
	quint32 startIndex;//индекс первого элемента в циклическом буфере
	bool opened;
	enum
	{
		FILES,
		FIXED_BLOCKS
	}dbType;
	QByteArrayList values;
	ARpcDBDriverFixedBlocks dbFixesBlocks;
};

#endif // ARPCLASTNVALUESSTORAGE_H
