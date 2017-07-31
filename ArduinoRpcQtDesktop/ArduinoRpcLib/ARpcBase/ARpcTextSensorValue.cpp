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

#include "ARpcTextSensorValue.h"
#include <QDateTime>

ARpcTextSensorValue::ARpcTextSensorValue()
{
	timestamp=0;
}

ARpcSensor::Type ARpcTextSensorValue::type()const
{
	return ARpcSensor::TEXT;
}

bool ARpcTextSensorValue::parse(ARpcMessage m)
{
	if(m.args.count()<2)return false;
	m.args.removeAt(0);
	timestamp=QDateTime::currentMSecsSinceEpoch();
	text=m.args.join(" ");
	return true;
}

const QString &ARpcTextSensorValue::value()const
{
	return text;
}

void ARpcTextSensorValue::fromData(const char *str,int size)
{
	text=QString::fromUtf8(str,size);
}
