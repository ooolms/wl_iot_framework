#include "GDIL/core/DataUnit.h"

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
		constructByType(INVALID,1);
		return;
	}
	SensorDef::Type st=vList[0]->type();
	st.packType=SensorDef::PACKET;
	QByteArray binData;
	for(int i=0;i<vList.count();++i)
	{
		if(vList[i]->type()!=st)
		{
			constructByType(INVALID,1);
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
		constructByType(INVALID,1);
	}
	mType=ARRAY;
	calcNumType();
}

DataUnit::DataUnit(double v)
{
	mValueRefCount=new int;
	*mValueRefCount=1;
	mType=SINGLE;
	mNumType=F64;
	mValue=SensorValue::createSensorValue(SensorDef::Type(SensorDef::F64,SensorDef::SINGLE,SensorDef::NO_TIME,1));
	((SensorValueF64*)mValue)->setData(QVector<double>()<<v);
}

DataUnit::DataUnit(qint64 v)
{
	mValueRefCount=new int;
	*mValueRefCount=1;
	mType=SINGLE;
	mNumType=S64;
	mValue=SensorValue::createSensorValue(SensorDef::Type(SensorDef::S64,SensorDef::SINGLE,SensorDef::NO_TIME,1));
	((SensorValueS64*)mValue)->setData(QVector<qint64>()<<v);
}

DataUnit::DataUnit(bool v)
{
	mValueRefCount=new int;
	*mValueRefCount=1;
	mType=BOOL;
	mValue=SensorValue::createSensorValue(SensorDef::Type(SensorDef::U8,SensorDef::SINGLE,SensorDef::NO_TIME,1));
	((SensorValueU8*)mValue)->setData(QVector<quint8>()<<(v?1:0));
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

const SensorValue *DataUnit::value()const
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

quint32 DataUnit::dim()const
{
	return mValue->type().dim;
}

bool DataUnit::valueFromArgs(const QByteArrayList &args)
{
	return mValue->parseMsgArgs(args);
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

void DataUnit::constructByType(DataUnit::Type t,quint32 dim,NumericType numType)
{
	mType=t;
	mNumType=numType;
	SensorDef::Type st;
	if(mNumType==S64)
		st.numType=SensorDef::S64;
	else st.numType=SensorDef::F64;
	st.dim=dim;
	if(t==ARRAY)
		st.packType=SensorDef::PACKET;
	else if(t==BOOL)
		st.numType=SensorDef::U8;
	mValue=SensorValue::createSensorValue(st);
}

void DataUnit::calcNumType()
{
	auto t=mValue->type().numType;
	if(t==SensorDef::F32||t==SensorDef::F64)
		mNumType=F64;
	else mNumType=S64;
}
