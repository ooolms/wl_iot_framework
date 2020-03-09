#include "GIDL/core/DataUnit.h"

Q_DECLARE_OPERATORS_FOR_FLAGS(DataUnit::Types)

DataUnit::DataUnit()
{
}

DataUnit::DataUnit(const DataUnit &t)
{
	mType=t.mType;
	mNumType=t.mNumType;
	mValueRefCount=t.mValueRefCount;
	mValue=t.mValue;
	++(*mValueRefCount);
}

DataUnit::DataUnit(DataUnit::Type t)
{
	mValueRefCount=new int;
	*mValueRefCount=1;
	constructByType(t);
}

DataUnit::DataUnit(const SensorValue *v)
{
	mValueRefCount=new int;
	*mValueRefCount=1;
	if(!canCreateFromValue(v->type()))
	{
		constructByType(INVALID);
		return;
	}
	mValue=v->mkCopy();
	if(mValue->type().packType==SensorDef::PACKET)
		mType=ARRAY;
	else mType=SINGLE;
	calcNumType();
}

DataUnit::DataUnit(const QVector<SensorValue*> &vList)
{
	mValueRefCount=new int;
	*mValueRefCount=1;
	if(vList.isEmpty()||!canCreateFromArrayOfValues(vList[0]->type()))
	{
		constructByType(INVALID);
		return;
	}
	SensorDef::Type st=vList[0]->type();
	st.packType=SensorDef::PACKET;
	QByteArray binData;
	for(int i=0;i<vList.count();++i)
	{
		if(vList[i]->type()!=st)
		{
			constructByType(INVALID);
			return;
		}
		binData.append(vList[i]->dumpToBinaryNoTime());
	}
	mValue=SensorValue::createSensorValue(st);
	if(st.tsType!=SensorDef::NO_TIME)
	{
		qint64 ts=vList.last()->time();
		binData.prepend(QByteArray((const char*)&ts,sizeof(qint64)));
	}
	if(!mValue->parseBinary(binData))
	{
		delete mValue;
		constructByType(INVALID);
	}
	mType=ARRAY;
	calcNumType();
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

const SensorValue *DataUnit::value()
{
	return mValue;
}

DataUnit DataUnit::mkCopy()const
{
	DataUnit u;
	u.mType=mType;
	u.mNumType=mNumType;
	u.mValueRefCount=new int;
	*u.mValueRefCount=1;
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

void DataUnit::constructByType(DataUnit::Type t)
{
	mType=t;
	mNumType=F64;
	SensorDef::Type st;
	st.numType=SensorDef::F64;
	if(t==ARRAY)
		st.packType=SensorDef::PACKET;
	else if(t==BOOL)
		st.numType=SensorDef::U8;
	mValue=SensorValue::createSensorValue(st);
}

void DataUnit::calcNumType()
{
	auto t=mValue->type().numType;
	if(t==SensorDef::F32)
		mNumType=F32;
	else if(t==SensorDef::U8||t==SensorDef::U16||t==SensorDef::U32||t==SensorDef::U64)
		mNumType=U64;
	else if(t==SensorDef::S8||t==SensorDef::S16||t==SensorDef::S32||t==SensorDef::S64)
		mNumType=S64;
	else mNumType=F64;
}
