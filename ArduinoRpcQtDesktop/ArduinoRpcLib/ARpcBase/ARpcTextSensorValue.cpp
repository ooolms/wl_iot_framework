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

/*ARpcTextSensorValue::ARpcTextSensorValue()
{
	mTime=0;
}

ARpcSensorDef::Type ARpcTextSensorValue::type() const
{
	return ARpcSensorDef::TEXT;
}

bool ARpcTextSensorValue::parseMsgArgs(const QByteArrayList &args)
{
	if(args.count()<1)
		return false;
	if(args.count()==1)
	{
		mTime=QDateTime::currentMSecsSinceEpoch();
		text=args[0];
	}
	else if(args.count()==2)
	{
		mTime=args[0].toULongLong();
		text=args[1];
	}
	return true;
}

bool ARpcTextSensorValue::parseF(const QByteArrayList &args)
{
	if(args.count()<1)
		return false;
	if(args.count()==1)
	{
		mTime=QDateTime::currentMSecsSinceEpoch();
		text=args[0];
	}
	else if(args.count()==2)
	{
		if(args[0].size()!=8)return false;
		mTime=*((qint64*)args[0].constData());
		text=args[1];
	}
	return true;
}

bool ARpcTextSensorValue::parseD(const QByteArrayList &args)
{
	return parseF(args);
}

QByteArrayList ARpcTextSensorValue::dump()const
{
	QByteArrayList retVal;
	retVal.append(QByteArray::number(mTime));
	retVal.append(text);
	return retVal;
}

ARpcSensorValue* ARpcTextSensorValue::mkCopy()const
{
	ARpcTextSensorValue *v=new ARpcTextSensorValue;
	v->text=text;
	return v;
}

const QByteArray& ARpcTextSensorValue::value() const
{
	return text;
}

void ARpcTextSensorValue::fromData(const char *str,int size)
{
	text=QByteArray(str,size);
}

void ARpcTextSensorValue::fromData(const QByteArray &t)
{
	text=t;
}
*/
