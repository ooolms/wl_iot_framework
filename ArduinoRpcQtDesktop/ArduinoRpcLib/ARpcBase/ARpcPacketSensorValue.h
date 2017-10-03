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

#ifndef ARPCPACKETSENSORVALUE_H
#define ARPCPACKETSENSORVALUE_H

#include "ARpcBase/ARpcISensorValue.h"
#include <QVector>

class ARpcPacketSensorValue
	:public ARpcISensorValue
{
public:
	typedef float ValueType;

public:
	explicit ARpcPacketSensorValue(quint32 dims);//no timestamp
	explicit ARpcPacketSensorValue(quint32 dims,bool localTimeStamp);//true - local, false - global
	virtual ARpcSensor::Type type()const override;
	virtual bool parse(const QStringList &args)override;
	virtual QStringList dump()const override;
	virtual ARpcISensorValue* mkCopy()override;
	const QVector<ValueType>& values()const;
	float at(quint32 valIndex,quint32 dimension)const;
	quint32 dims()const;//dimensions
	quint32 valuesCount()const;
	void fromData(const QVector<ValueType> &vals,quint32 dims);
	void fromData(const ValueType *vals,quint32 dims,quint32 count);
	void makeZeroFilledPacket(quint32 count);

private:
	ARpcSensor::Type valueType;
	quint32 dimensions;
	quint32 valCount;
	QVector<ValueType> valuesList;//{valCount} строк последовательно по {dimensions} записей
};

#endif // ARPCPACKETSENSORVALUE_H
