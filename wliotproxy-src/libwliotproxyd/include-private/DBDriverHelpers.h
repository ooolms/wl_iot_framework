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

#ifndef DBDRIVERHELPERS_H
#define DBDRIVERHELPERS_H

#include "wliot/devices/SensorValue.h"
#include "wliot/storages/ISensorStorage.h"

class DBDriverHelpers
{
public:
	explicit DBDriverHelpers();
	explicit DBDriverHelpers(ISensorStorage::TimestampRule rule);
	QByteArray packSensorValue(const SensorValue *val,int &hasTime,qint64 &timestamp);
	QByteArray packSensorValue(const SensorValue *val);
	SensorValue* unpackSensorValue(SensorDef::Type type,const QByteArray &data);
	static QVector<quint32> sizesForFixedBlocksDb(SensorDef::Type type);
	void getTimestampForVal(const SensorValue *val,int &hasTime,qint64 &timestamp);

private:
	ISensorStorage::TimestampRule timeRule;
};

#endif // DBDRIVERHELPERS_H
