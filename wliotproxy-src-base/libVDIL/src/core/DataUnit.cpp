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

#include "VDIL/core/DataUnit.h"
#include "wliot/WLIOTProtocolDefs.h"

using namespace WLIOT;
using namespace WLIOTVDIL;

DataUnit::DataUnit()
{
	constructByType(TypeAndDim(INVALID,1));
}

DataUnit::DataUnit(const DataUnit &t)
{
	mType=t.mType;
	mNumType=t.mNumType;
	mValueRefCount=t.mValueRefCount;
	mValue=t.mValue;
	++(*mValueRefCount);
}

DataUnit::DataUnit(TypeAndDim t)
{
	if(t.dim==0)
		constructByType(TypeAndDim(INVALID,1));
	else constructByType(t,F64);
}

DataUnit::DataUnit(TypeAndDim t,const QByteArrayList &msgArgs)
{
	if(t.dim==0)
	{
		constructByType(TypeAndDim(INVALID,1));
		return;
	}
	if(t.type==BOOL)
	{
		if(msgArgs.count()!=1||(msgArgs[0]!="1"&&msgArgs[0]!="0"))
		{
			constructByType(TypeAndDim(INVALID,1));
			return;
		}
		constructByType(TypeAndDim(BOOL,1));
		((SensorValueU8*)mValue)->setT(0,msgArgs[0]=="0"?0:1);
		return;
	}
	else if(t.type==DATETIME)
	{
		if(msgArgs.count()!=1)
		{
			constructByType(TypeAndDim(INVALID,1));
			return;
		}
		bool ok=false;
		qint64 t=msgArgs[0].toLongLong(&ok);
		if(!ok)
		{
			constructByType(TypeAndDim(INVALID,1));
			return;
		}
		constructByType(TypeAndDim(DATETIME,1));
		((SensorValueS64*)mValue)->setT(0,t);
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
	valType.dim=t.dim;
	valType.numType=isFloatVal?SensorDef::F64:SensorDef::S64;
	if(t.type==ARRAY)
		valType.packType=SensorDef::PACKET;
	SensorValue *val=SensorValue::createSensorValue(valType);
	if(!val->parseMsgArgs(msgArgs))
	{
		constructByType(TypeAndDim(INVALID,1));
		delete val;
	}
	else
	{
		constructByType(t,isFloatVal?F64:S64);
		delete mValue;
		mValue=val;
	}
}

DataUnit::DataUnit(const SensorValue *v,bool numOnly)
{
	if(!canCreateFromValue(v->type()))
	{
		constructByType(TypeAndDim(INVALID,1));
		return;
	}
	SensorDef::Type t=v->type();
	if(t.numType==SensorDef::F32||t.numType==SensorDef::F64)
	{
		if(v->type().packType==SensorDef::PACKET)
			constructByType(TypeAndDim(ARRAY,t.dim),F64);
		else constructByType(TypeAndDim(SINGLE,t.dim),F64);
		QVector<double> vv;
		vv.resize((int)v->totalCount());
		for(quint32 i=0;i<v->totalCount();++i)
			vv[i]=v->valueToDouble(i);
		((SensorValueF64*)mValue)->setData(vv);
	}
	else if(t.numType==SensorDef::U8&&t.dim==1&&!numOnly&&
		(((const SensorValueU8*)v)->getT(0)==0||((const SensorValueU8*)v)->getT(0)==1))
	{
		constructByType(TypeAndDim(BOOL,1));
		((SensorValueU8*)mValue)->setT(0,((const SensorValueU8*)v)->getT(0));
	}
	else
	{
		if(v->type().packType==SensorDef::PACKET)
			constructByType(TypeAndDim(ARRAY,t.dim),S64);
		else constructByType(TypeAndDim(SINGLE,t.dim),S64);
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
		constructByType(TypeAndDim(INVALID,1));
		return;
	}
	SensorValue *v=vList[0];
	SensorDef::Type t=v->type();
	t.packType=SensorDef::PACKET;
	if(t.numType==SensorDef::F32||t.numType==SensorDef::F64)
	{
		constructByType(TypeAndDim(ARRAY,t.dim),F64);
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
		constructByType(TypeAndDim(ARRAY,t.dim),S64);
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
	constructByType(TypeAndDim(SINGLE,1),F64);
	((SensorValueF64*)mValue)->setT(0,v);
}

DataUnit::DataUnit(qint64 v)
{
	constructByType(TypeAndDim(SINGLE,1),S64);
	((SensorValueS64*)mValue)->setT(0,v);
}

DataUnit::DataUnit(bool v)
{
	constructByType(TypeAndDim(BOOL,1));
	((SensorValueU8*)mValue)->setT(0,(v?1:0));
}

DataUnit::DataUnit(const QDateTime &d)
{
	constructByType(TypeAndDim(DATETIME,1));
	((SensorValueS64*)mValue)->setT(0,d.toMSecsSinceEpoch());
}

DataUnit::DataUnit(const QVector<double> &vals)
{
	if(vals.size()==0)
		constructByType(TypeAndDim(INVALID,1));
	else
	{
		constructByType(TypeAndDim(SINGLE,vals.size()),F64);
		((SensorValueF64*)mValue)->setData(vals);
	}
}

DataUnit::DataUnit(const QVector<qint64> &vals)
{
	if(vals.size()==0)
		constructByType(TypeAndDim(INVALID,1));
	else
	{
		constructByType(TypeAndDim(SINGLE,vals.size()),S64);
		((SensorValueS64*)mValue)->setData(vals);
	}
}

DataUnit::DataUnit(const QVector<double> &vals,quint32 dim)
{
	if(vals.size()==0||dim==0||vals.size()%dim!=0)
		constructByType(TypeAndDim(INVALID,1));
	else
	{
		constructByType(TypeAndDim(ARRAY,dim),F64);
		((SensorValueF64*)mValue)->setData(vals);
	}
}

DataUnit::DataUnit(const QVector<qint64> &vals,quint32 dim)
{
	if(vals.size()==0||dim==0||vals.size()%dim!=0)
		constructByType(TypeAndDim(INVALID,1));
	else
	{
		constructByType(TypeAndDim(ARRAY,dim),S64);
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

bool DataUnit::operator==(const DataUnit &t)const
{
	if(mValue==t.mValue)return true;//same data unit
	if(mType!=t.mType)return false;
	if(mValue->type()!=t.mValue->type())return false;
	return mValue->isDataEqual(*t.mValue);
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

TypeAndDim DataUnit::typeAndDim()const
{
	return TypeAndDim(mType,mValue->type().dim);
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

void DataUnit::swap(DataUnit &t)
{
	std::swap(mValueRefCount,t.mValueRefCount);
	std::swap(mType,t.mType);
	std::swap(mNumType,t.mNumType);
	std::swap(mValue,t.mValue);
}

bool DataUnit::boolValue()const
{
	if(mType!=BOOL)
		return false;
	return ((SensorValueU8*)mValue)->getT(0)>0;
}

QDateTime DataUnit::dateTimeValue()const
{
	if(mType!=DATETIME)
		return QDateTime();
	return QDateTime::fromMSecsSinceEpoch(((SensorValueS64*)mValue)->getT(0));
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

void DataUnit::constructByType(TypeAndDim t,NumericType numType)
{
	mType=t.type;
	mNumType=numType;
	SensorDef::Type st;
	if(mNumType==S64)
		st.numType=SensorDef::S64;
	else st.numType=SensorDef::F64;
	st.dim=t.dim;
	if(mType==ARRAY)
		st.packType=SensorDef::PACKET;
	else if(mType==BOOL)
	{
		st.numType=SensorDef::U8;
		st.dim=1;
	}
	else if(mType==DATETIME)
	{
		st.numType=SensorDef::S64;
		st.dim=1;
	}
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

TypeConstraints::TypeConstraints(const TypeAndDim &t)
{
	types=t.type;
	dim=t.dim;
}

bool TypeConstraints::match(DataUnit::Type t,quint32 d)const
{
	if(t==DataUnit::INVALID||d==0)return false;
	if(dim!=0&&dim!=d)
		return false;
	return types.testFlag(t);
}

bool TypeConstraints::match(const TypeAndDim &t)const
{
	if(!t.isValid())return false;
	if(dim!=0&&dim!=t.dim)
		return false;
	return types.testFlag(t.type);
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
	else if(t==DataUnit::DATETIME)
		return "date";
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
	else if(str=="date")
		return DataUnit::DATETIME;
	return DataUnit::INVALID;
}

TypeAndDim::TypeAndDim()
{
	type=DataUnit::INVALID;
	dim=0;
}

TypeAndDim::TypeAndDim(DataUnit::Type t,quint32 d)
{
	type=t;
	dim=d;
}

bool TypeAndDim::operator==(const TypeAndDim &t)
{
	return type==t.type&&dim==t.dim;
}

bool TypeAndDim::isValid()const
{
	return type!=DataUnit::INVALID&&dim>0;
}
