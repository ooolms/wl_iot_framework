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

using namespace WLIOT;
using namespace WLIOTGDIL;

DataUnit::DataUnit()
{
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

DataUnit::DataUnit(DataUnit::Type t,quint32 dim)
{
	if(dim==0)
		constructByType(INVALID,1);
	else constructByType(t,dim,F64);
}

DataUnit::DataUnit(DataUnit::Type t,quint32 dim,const QByteArrayList &msgArgs)
{
	if(dim==0)
	{
		constructByType(INVALID,1);
		return;
	}
	if(t==BOOL)
	{
		if(msgArgs.count()!=1||(msgArgs[0]!="1"&&msgArgs[0]!="0"))
		{
			constructByType(INVALID,1);
			return;
		}
		constructByType(BOOL,1);
		((SensorValueU8*)mValue)->setT(0,msgArgs[0]=="0"?0:1);
		return;
	}
	bool isFloatVal=false;
	for(const QByteArray &a:msgArgs)
	{
		if(a.contains('.')||a.contains('e')||a.contains('E'))
		{
			isFloatVal=true;
			break;
		}
	}
	SensorDef::Type valType;
	valType.dim=dim;
	valType.numType=isFloatVal?SensorDef::F64:SensorDef::S64;
	if(t==ARRAY)
		valType.packType=SensorDef::PACKET;
	SensorValue *val=SensorValue::createSensorValue(valType);
	if(!val->parseMsgArgs(msgArgs))
	{
		constructByType(INVALID,1);
		delete val;
	}
	else
	{
		constructByType(t,dim,isFloatVal?F64:S64);
		delete mValue;
		mValue=val;
	}
}

DataUnit::DataUnit(const SensorValue *v)
{
	if(!canCreateFromValue(v->type()))
	{
		constructByType(INVALID,1);
		return;
	}
	SensorDef::Type t=v->type();
	if(t.numType==SensorDef::F32||t.numType==SensorDef::F64)
	{
		if(v->type().packType==SensorDef::PACKET)
			constructByType(ARRAY,t.dim,F64);
		else constructByType(SINGLE,t.dim,F64);
		QVector<double> vv;
		vv.resize((int)v->totalCount());
		for(quint32 i=0;i<v->totalCount();++i)
			vv[i]=v->valueToDouble(i);
		((SensorValueF64*)mValue)->setData(vv);
	}
	else if(t.numType==SensorDef::U8&&t.dim==1&&
		(((const SensorValueU8*)v)->getT(0)==0||((const SensorValueU8*)v)->getT(0)==1))
	{
		constructByType(BOOL,1);
		((SensorValueU8*)mValue)->setT(0,((const SensorValueU8*)v)->getT(0));
	}
	else
	{
		if(v->type().packType==SensorDef::PACKET)
			constructByType(ARRAY,t.dim,S64);
		else constructByType(SINGLE,t.dim,S64);
		QVector<qint64> vv;
		vv.resize((int)v->totalCount());
		for(quint32 i=0;i<v->totalCount();++i)
			vv[i]=v->valueToS64(i);
		((SensorValueS64*)mValue)->setData(vv);
	}
	mValue->setTime(v->time());
}

DataUnit::DataUnit(const QVector<SensorValue*> &vList)
{
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
		constructByType(ARRAY,t.dim,F64);
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
		constructByType(ARRAY,t.dim,S64);
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
}

DataUnit::DataUnit(double v)
{
	constructByType(SINGLE,1,F64);
	((SensorValueF64*)mValue)->setT(0,v);
}

DataUnit::DataUnit(qint64 v)
{
	constructByType(SINGLE,1,S64);
	((SensorValueS64*)mValue)->setT(0,v);
}

DataUnit::DataUnit(bool v)
{
	constructByType(BOOL,1);
	((SensorValueU8*)mValue)->setT(0,(v?1:0));
}

DataUnit::DataUnit(const QVector<double> &vals)
{
	if(vals.size()==0)
		constructByType(INVALID,1);
	else
	{
		constructByType(SINGLE,vals.size(),F64);
		((SensorValueF64*)mValue)->setData(vals);
	}
}

DataUnit::DataUnit(const QVector<qint64> &vals)
{
	if(vals.size()==0)
		constructByType(INVALID,1);
	else
	{
		constructByType(SINGLE,vals.size(),S64);
		((SensorValueS64*)mValue)->setData(vals);
	}
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
	mValueRefCount=new int;
	*mValueRefCount=1;
	mValue=SensorValue::createSensorValue(st);
}

TypeConstraints::TypeConstraints()
{
	types=DataUnit::INVALID;
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
