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

#ifndef ARPCDBDRIVERHELPERS_H
#define ARPCDBDRIVERHELPERS_H

#include "ARpcDBDriverFixedBlocks.h"
#include "ARpcDBDriverChainedBlocks.h"
#include "ARpcBase/ARpcSensorValue.h"
#include "ARpcISensorStorage.h"

class ARpcDBDriverHelpers
{
public:
	explicit ARpcDBDriverHelpers();
	explicit ARpcDBDriverHelpers(ARpcISensorStorage::TimestampRule rule);
	QByteArray packSensorValue(const ARpcSensorValue *val,int &hasTime,qint64 &timestamp);
	QByteArray packSensorValue(const ARpcSensorValue *val);
	ARpcSensorValue* unpackSensorValue(ARpcSensorDef::Type type,const QByteArray &data);
//	void detectIfHasTime(ARpcSensorDef::Type type,int &hasTime);
	static QVector<quint32> sizesForFixedBlocksDb(ARpcSensorDef::Type type);
	void getTimestampForVal(const ARpcSensorValue *val,int &hasTime,qint64 &timestamp);

private:
	ARpcISensorStorage::TimestampRule timeRule;
};

#endif // ARPCDBDRIVERHELPERS_H
