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

#ifndef ARPCISENSORVALUE_H
#define ARPCISENSORVALUE_H

#include "ARpcBase/ARpcSensor.h"
#include "ARpcBase/ARpcMessage.h"

class ARpcISensorValue
{
public:
	ARpcISensorValue();
	virtual ~ARpcISensorValue(){}
	qint64 time()const;
	void setTime(qint64 t);

public:
	virtual ARpcSensor::Type type()const=0;
	virtual bool parse(const QByteArrayList &data)=0;//msg args contain sensor value
	virtual QByteArrayList dump()const=0;//to msg args
	virtual ARpcISensorValue* mkCopy()=0;

protected:
	qint64 timestamp;
};

#endif // ARPCISENSORVALUE_H
