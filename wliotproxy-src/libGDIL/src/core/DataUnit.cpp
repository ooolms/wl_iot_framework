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

#include "GDIL/core/DataUnit.h"
#include "wliot/WLIOTProtocolDefs.h"

DataUnit::DataUnit()
{
	mValueRefCount=new int;
	*mValueRefCount=1;
	constructByType(INVALID,1);
}

DataUnit::DataUnit(const DataUnit &t)
{
	mType=t.mType;
	mNumType=t.mNumType;
	mValueRefCount=t.mValueRefCount;
	mValue=t.mValue;
	++(*mValueRefCount);
}

DataUnit::DataUnit(DataUnit::Type t,quint32 dim,NumericType numType)
{
	mValueRefCount=new int;
	*mValueRefCount=1;
	if(dim==0)
		dim=1;
	constructByType(t,dim,numType);
}

DataUnit::DataUnit(const SensorValue *v)
{
	mValueRefCount=new int;
	*mValueRefCount=1;
	if(!canCreateFromValue(v->type()))
	{
		constructByType(INVALID,1);
		return;
	}
	SensorDef::Type t=v->type();
	if(t.numType==SensorDef::F32||t.numType==SensorDef::F64)
	{
		t.numType=SensorDef::F64;
		mNumType=F64;
		mValue=SensorValue::createSensorValue(t);
		QVector<double> vv;
		vv.resize((int)v->totalCount());
		for(quint32 i=0;i<v->totalCount();++i)
			vv[i]=v->valueToDouble(i);
		((SensorValueF64*)mValue)->setData(vv);
	}
	else if(t.numType==SensorDef::U8&&t.dim==1)
	{
		mValue=v->mkCopy();
		mType=BOOL;
		SensorValueU8 *uVal=(SensorValueU8*)mValue;
		if(uVal->getT(0)!=0)
			uVal->setT(0,1);
	}
	else
	{
		t.numType=SensorDef::S64;
		mNumType=S64;
		mValue=SensorValue::createSensorValue(t);
		QVector<qint64> vv;
		vv.resize((int)v->totalCount());
		for(quint32 i=0;i<v->totalCount();++i)
			vv[i]=v->valueToS64(i);
		((SensorValueS64*)mValue)->setData(vv);
	}
	mValue->setTime(v->time());
	if(mValue->type().packType==SensorDef::PACKET)
		mType=ARRAY;
	else mType=SINGLE;
}

DataUnit::DataUnit(const QVector<SensorValue*> &vList)
{
	mValueRefCount=new int;
	*mValueRefCount=1;
	if(vList.isEmpty()||!canCreateFromArrayOfValues(vList[0]->type()))
	{
		constructByType(INVALID,1);
		return;
	}

	SensorValue *v=vList[0];
	SensorDef::Type t=v->type();
	t.packType=SensorDef::PACKET;
	if(t.numType==SensorDef::F32||t.numType==SensorDef::F64)
	{
		t.numType=SensorDef::F64;
		mNumType=F64;
		mValue=SensorValue::createSensorValue(t);
		QVector<double> vv;
		vv.resize(t.dim);
		for(int i=0;i<vList.count();++i)
		{
			for(quint32 i=0;i<t.dim;++i)
				vv[i]=v->valueToDouble(i);
			((SensorValueF64*)mValue)->addSample(vv);
		}
	}
	else
	{
		t.numType=SensorDef::S64;
		mNumType=S64;
		mValue=SensorValue::createSensorValue(t);
		QVector<qint64> vv;
		vv.resize(t.dim);
		for(int i=0;i<vList.count();++i)
		{
			for(quint32 i=0;i<t.dim;++i)
				vv[i]=v->valueToS64(i);
			((SensorValueS64*)mValue)->addSample(vv);
		}
	}
	mValue->setTime(v->time());
	mType=ARRAY;
}

DataUnit::DataUnit(double v)
{
	mValueRefCount=new int(1);
	mType=SINGLE;
	mNumType=F64;
	mValue=SensorValue::createSensorValue(SensorDef::Type(SensorDef::F64,SensorDef::SINGLE,SensorDef::NO_TIME,1));
	((SensorValueF64*)mValue)->setT(0,v);
}

DataUnit::DataUnit(qint64 v)
{
	mValueRefCount=new int(1);
	mType=SINGLE;
	mNumType=S64;
	mValue=SensorValue::createSensorValue(SensorDef::Type(SensorDef::S64,SensorDef::SINGLE,SensorDef::NO_TIME,1));
	((SensorValueS64*)mValue)->setT(0,v);
}

DataUnit::DataUnit(bool v)
{
	mValueRefCount=new int;
	*mValueRefCount=1;
	mType=BOOL;
	mValue=SensorValue::createSensorValue(SensorDef::Type(SensorDef::U8,SensorDef::SINGLE,SensorDef::NO_TIME,1));
	((SensorValueU8*)mValue)->setT(0,(v?1:0));
}

DataUnit& DataUnit::operator=(const DataUnit &t)
{
	--(*mValueRefCount);
	if(*mValueRefCount==0)
	{
		delete mValueRefCount;
		delete mValue;
	}
	mType=t.mType;
	mNumType=t.mNumType;
	mValueRefCount=t.mValueRefCount;
	mValue=t.mValue;
	++(*mValueRefCount);
	return *this;
}

DataUnit::~DataUnit()
{
	--(*mValueRefCount);
	if(*mValueRefCount==0)
	{
		delete mValueRefCount;
		delete mValue;
	}
}

bool DataUnit::isValid()const
{
	return mType!=INVALID;
}

DataUnit::Type DataUnit::type()const
{
	return mType;
}

DataUnit::NumericType DataUnit::numType()const
{
	return mNumType;
}

const SensorValue* DataUnit::value()const
{
	return mValue;
}

quint32 DataUnit::dim()const
{
	return mValue->type().dim;
}

bool DataUnit::parseMsgArgs(const QByteArrayList &args)
{
	if(*mValueRefCount>1)
	{
		--(*mValueRefCount);
		mValueRefCount=new int(1);
		mValue=mValue->mkCopy();
	}
	return mValue->parseMsgArgs(args);
}

DataUnit DataUnit::mkCopy()
{
	DataUnit u(*this);
	--(*mValueRefCount);
	u.mValueRefCount=new int(1);
	u.mValue=mValue->mkCopy();
	return u;
}

bool DataUnit::canCreateFromValue(SensorDef::Type t)
{
	return t.isNumeric();
}

bool DataUnit::canCreateFromArrayOfValues(SensorDef::Type t)
{
	return t.isNumeric()&&t.packType==SensorDef::SINGLE;
}

DataUnit::Type DataUnit::typeForSensorValue(SensorDef::Type t,bool singleValue)
{
	if(!singleValue&&t.packType==SensorDef::PACKET)
		return INVALID;
	if(!t.isNumeric())
		return INVALID;
	if(singleValue)
		return SINGLE;
	else return ARRAY;
}

DataUnit DataUnit::single1DimValueFromString(const QString &s)
{
	bool ok=false;
	DataUnit u;
	qint64 s64Val=s.toLongLong(&ok);
	if(!ok)
	{
		double f64Val=s.toUtf8().toDouble(&ok);
		if(ok)
			u=DataUnit(f64Val);
		else u=DataUnit(1.0);
	}
	else u=DataUnit(s64Val);
	return u;
}

DataUnit DataUnit::valueFromMsgArgs(DataUnit::Type t,quint32 dim,const QByteArrayList &args)
{
	if(dim==0)
		return DataUnit();
	if(t==BOOL)
	{
		if(args.count()!=1||(args[0]!="1"&&args[0]!="0"))
			return DataUnit();
		return DataUnit(args[0]=="1");
	}
	bool isFloatVal=false;
	for(const QByteArray &a:args)
	{
		if(a.contains('.')||a.contains('e'))
		{
			isFloatVal=true;
			break;
		}
	}
	SensorDef::Type valType;
	valType.dim=dim;
	if(isFloatVal)
		valType.numType=SensorDef::F64;
	else valType.numType=SensorDef::S64;
	if(t==ARRAY)
		valType.packType=SensorDef::PACKET;
	QScopedPointer<SensorValue> val(SensorValue::createSensorValue(valType));
	if(!val->parseMsgArgs(args))
		return DataUnit();
	return DataUnit(val.data());
}

void DataUnit::constructByType(DataUnit::Type t,quint32 dim,NumericType numType)
{
	mType=t;
	mNumType=numType;
	SensorDef::Type st;
	if(mNumType==S64)
		st.numType=SensorDef::S64;
	else st.numType=SensorDef::F64;
	st.dim=dim;
	if(mType==ARRAY)
		st.packType=SensorDef::PACKET;
	else if(mType==BOOL)
		st.numType=SensorDef::U8;
	mValue=SensorValue::createSensorValue(st);
}

TypeConstraints::TypeConstraints()
{
	types=DataUnit::ANY;
	dim=0;
}

TypeConstraints::TypeConstraints(DataUnit::Types t,quint32 d)
{
	types=t;
	dim=d;
}

bool TypeConstraints::match(DataUnit::Type t,quint32 d)const
{
	if(dim!=0&&dim!=d)
		return false;
	return types.testFlag(t);
}

bool TypeConstraints::match(const DataUnit &u)const
{
	return match(u.type(),u.dim());
}

QByteArray DataUnit::typeToStr(DataUnit::Type t)
{
	if(t==DataUnit::SINGLE)
		return "single";
	else if(t==DataUnit::ARRAY)
		return "array";
	else if(t==DataUnit::BOOL)
		return "bool";
	return "";
}

DataUnit::Type DataUnit::typeFromStr(const QByteArray &str)
{
	if(str=="single")
		return DataUnit::SINGLE;
	else if(str=="array")
		return DataUnit::ARRAY;
	else if(str=="bool")
		return DataUnit::BOOL;
	return DataUnit::INVALID;
}

QByteArray DataUnit::numTypeToStr(DataUnit::NumericType t)
{
	if(t==DataUnit::F64)
		return "f64";
	else if(t==DataUnit::S64)
		return "s64";
	return "";
}

DataUnit::NumericType DataUnit::numTypeFromStr(const QByteArray &str)
{
	if(str=="s64")
		return DataUnit::S64;
	return DataUnit::F64;
}
