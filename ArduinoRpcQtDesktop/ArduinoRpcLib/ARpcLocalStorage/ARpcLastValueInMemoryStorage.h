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

#ifndef ARPCLASTVALUEINMEMORYSTORAGE_H
#define ARPCLASTVALUEINMEMORYSTORAGE_H

#include "ARpcISensorStorage.h"
#include "ARpcDBDriverHelpers.h"

//TODO store N values instead of one
class ARpcLastValueInMemoryStorage
	:public ARpcISensorStorage
{
	Q_OBJECT

public:
	explicit ARpcLastValueInMemoryStorage(const ARpcSensor &sensor,const QUuid &devId,const QByteArray &devName,
		QObject *parent=0);
	virtual ~ARpcLastValueInMemoryStorage();

public:
	virtual bool open() override;
	virtual bool isOpened() const override;
	virtual quint64 valuesCount() override;
	virtual ARpcISensorValue* valueAt(quint64 index) override;
	virtual StoreMode getStoreMode() const override;
	virtual bool writeSensorValue(const ARpcISensorValue *val) override;
	virtual ARpcSensor::Type effectiveValuesType() const override;
	virtual TimestampRule fixTimestampRule(TimestampRule rule) override;

protected:
	virtual void closeInternal() override;

private:
	ARpcDBDriverHelpers hlp;
	QByteArray value;
	bool opened;
	ARpcSensor::Type effectiveValType;
};

#endif // ARPCLASTVALUEINMEMORYSTORAGE_H
