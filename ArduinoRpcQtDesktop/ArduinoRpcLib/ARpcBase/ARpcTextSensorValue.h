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

#ifndef ARPCTEXTSENSORVALUE_H
#define ARPCTEXTSENSORVALUE_H

#include "ARpcBase/ARpcISensorValue.h"

class ARpcTextSensorValue
	:public ARpcISensorValue
{
public:
	ARpcTextSensorValue();
	virtual ARpcSensor::Type type()const override;
	virtual bool parse(const QByteArrayList &args)override;
	virtual QByteArrayList dump()const override;
	virtual ARpcISensorValue* mkCopy()override;
	const QByteArray& value()const;
	void fromData(const char *str,int size);
	void fromData(const QByteArray &t);

private:
	QByteArray text;
};

#endif // ARPCTEXTSENSORVALUE_H
