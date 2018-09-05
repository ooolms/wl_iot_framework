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

#ifndef ARPCLASTNVALUESINMEMORYSTORAGE_H
#define ARPCLASTNVALUESINMEMORYSTORAGE_H

#include "ARpcBaseFSSensorStorage.h"
#include "ARpcDBDriverHelpers.h"

class ARpcLastNValuesInMemoryStorage
	:public ARpcBaseFSSensorStorage
{
	Q_OBJECT

public:
	explicit ARpcLastNValuesInMemoryStorage(const QUuid &devId,const QByteArray &devName,
		const ARpcSensorDef &sensor,TimestampRule tsRule,QObject *parent=0);
	virtual ~ARpcLastNValuesInMemoryStorage();
	bool create(quint32 storedValuesCount);

public:
	virtual bool open()override;
	virtual bool isOpened()const override;
	virtual quint64 valuesCount()override;
	virtual ARpcSensorValue* valueAt(quint64 index)override;
	virtual bool writeSensorValue(const ARpcSensorValue *val)override;

protected:
	virtual void closeFS()override;

private:
	void resizeValues(quint32 sz);

private:
	QList<QByteArray> values;
	bool opened;
};

#endif // ARPCLASTNVALUESINMEMORYSTORAGE_H
