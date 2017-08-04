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
	virtual ARpcSensor::Type type()const=0;
	virtual ~ARpcISensorValue(){}
	virtual bool parse(ARpcMessage m)=0;
	qint64 time()const;
	void setTime(qint64 t);

protected:
	qint64 timestamp;
};

#endif // ARPCISENSORVALUE_H