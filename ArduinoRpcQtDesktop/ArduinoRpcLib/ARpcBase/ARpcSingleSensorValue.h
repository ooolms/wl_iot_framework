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

#ifndef ARPCSINGLESENSORVALUE_H
#define ARPCSINGLESENSORVALUE_H

#include "ARpcBase/ARpcISensorValue.h"
#include <QVector>

class ARpcSingleSensorValue
	:public ARpcISensorValue
{
public:
	typedef double ValueType;

public:
	explicit ARpcSingleSensorValue(quint32 dims);//no timestamp
	explicit ARpcSingleSensorValue(quint32 dims,bool localTimeStamp);//true - local, false - global
	virtual ARpcSensor::Type type()const override;
	virtual bool parse(const QByteArrayList &args)override;
	virtual bool parseF(const QByteArrayList &args)override;
	virtual bool parseD(const QByteArrayList &args)override;
	virtual QByteArrayList dump()const override;
	virtual ARpcISensorValue* mkCopy()const override;
	const QVector<ValueType>& values()const;
	quint32 dims()const;//dimensions
	void fromData(const QVector<ValueType> &vals);
	void fromData(const ValueType *vals,quint32 dims);

private:
	ARpcSensor::Type valueType;
	quint32 dimensions;
	QVector<ValueType> valuesList;
};

#endif // ARPCSINGLESENSORVALUE_H
