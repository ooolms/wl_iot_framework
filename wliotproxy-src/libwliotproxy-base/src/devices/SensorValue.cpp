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

#include "wliot/devices/SensorValue.h"

SensorValue* SensorValue::mkCopy()const
{
	SensorValue *v=createSensorValue(mType);
	v->copyDataFrom(this);
	return v;
}

SensorValue* SensorValue::createSensorValue(SensorDef::Type t)
{
	if(t.numType==SensorDef::S8)
		return (SensorValue*)new SensorValueS8(t);
	else if(t.numType==SensorDef::U8)
		return (SensorValue*)new SensorValueU8(t);
	else if(t.numType==SensorDef::S16)
		return (SensorValue*)new SensorValueS16(t);
	else if(t.numType==SensorDef::U16)
		return (SensorValue*)new SensorValueU16(t);
	else if(t.numType==SensorDef::S32)
		return (SensorValue*)new SensorValueS32(t);
	else if(t.numType==SensorDef::U32)
		return (SensorValue*)new SensorValueU32(t);
	else if(t.numType==SensorDef::S64)
		return (SensorValue*)new SensorValueS64(t);
	else if(t.numType==SensorDef::U64)
		return (SensorValue*)new SensorValueU64(t);
	else if(t.numType==SensorDef::F32)
		return (SensorValue*)new SensorValueF32(t);
	else if(t.numType==SensorDef::F64)
		return (SensorValue*)new SensorValueF64(t);
	else if(t.numType==SensorDef::TEXT)
		return (SensorValue*)new SensorValueText(t);
	else return 0;
}

SensorValue::SensorValue(SensorDef::Type t)
{
	mType=t;
	mTime=0;
	mPacketsCount=1;
}

SensorValue::SensorValue(const SensorValue &t)
{
	mType=t.mType;
	mTime=t.mTime;
	mPacketsCount=t.mPacketsCount;
}

SensorValue::~SensorValue()
{
}

bool SensorValue::isEmpty()const
{
	return mType.dim==0||mPacketsCount==0;
}

SensorValue& SensorValue::operator=(const SensorValue &t)
{
	mType=t.mType;
	mTime=t.mTime;
	mPacketsCount=t.mPacketsCount;
	freeData();
	copyDataFrom(&t);
	return *this;
}

SensorValueText::SensorValueText(SensorDef::Type t)
	:SensorValue(t)
{
	createData();
}

SensorValueText::~SensorValueText()
{
	freeData();
}

SensorValueText::SensorValueText(const SensorValueText &t)
	:SensorValue(t)
{
	createData();
}

double SensorValueText::valueToDouble(quint32 totalIndex)const
{
	return mData[totalIndex].toDouble();
}

qint64 SensorValueText::valueToS64(quint32 totalIndex)const
{
	return mData[totalIndex].toLongLong();
}

bool SensorValue::parseMsgArgs(const QByteArrayList &args)
{
	quint32 argsCount=args.count();
	if(mType.numType==SensorDef::BAD_TYPE||mType.dim==0)return false;
	quint32 dataOffset=(mType.tsType==SensorDef::NO_TIME)?0:1;
	if(argsCount<dataOffset)return false;
	if(mType.packType==SensorDef::SINGLE)
	{
		if(argsCount!=(mType.dim+dataOffset))return false;
		mPacketsCount=1;
	}
	else
	{
		if((argsCount-dataOffset)%mType.dim!=0)return false;
		mPacketsCount=(argsCount-dataOffset)/mType.dim;
	}
	if(mType.tsType!=SensorDef::NO_TIME)
	{
		bool ok=false;
		mTime=args[0].toLongLong(&ok);
		if(!ok)return false;
	}
	return parseDataFromMsgArgs(args);
}

bool SensorValue::parseBinary(const QByteArray &data)
{
	quint32 dataSize=data.size();
	if(mType.numType==SensorDef::BAD_TYPE||mType.dim==0||mType.numType==SensorDef::TEXT)
		return false;
	quint32 dataOffset=(mType.tsType==SensorDef::NO_TIME)?0:sizeof(qint64);
	if(dataSize<dataOffset)return false;
	quint32 valSize=mType.valueSizeInBytes();
	if(mType.packType==SensorDef::SINGLE)
	{
		if(dataSize!=(mType.dim*valSize+dataOffset))return false;
		mPacketsCount=1;
	}
	else
	{
		if((dataSize-dataOffset)%(mType.dim*valSize)!=0)return false;
		mPacketsCount=(dataSize-dataOffset)/(mType.dim*valSize);
	}
	if(mType.tsType!=SensorDef::NO_TIME)
		mTime=*((qint64*)data.constData());
	return parseDataFromBinary(data);
}

bool SensorValue::parseBase64(const QByteArray &data)
{
	return parseBinary(QByteArray::fromBase64(data));
}

QByteArrayList SensorValue::dumpToMsgArgs()const
{
	QByteArrayList retVal;
	if(mType.tsType!=SensorDef::NO_TIME)
		retVal.append(QByteArray::number(mTime));
	retVal.append(dumpToMsgArgsNoTime());
	return retVal;
}

QByteArrayList SensorValue::dumpToMsgArgsNoTime()const
{
	QByteArrayList retVal;
	quint32 totalValuesCount=mType.dim*mPacketsCount;
	for(quint32 i=0;i<totalValuesCount;++i)
		retVal.append(valueToString(i));
	return retVal;
}

QByteArray SensorValue::dumpToBinary()const
{
	if(mType.numType==SensorDef::TEXT)return QByteArray();
	QByteArray retVal;
	if(mType.tsType!=SensorDef::NO_TIME)
		retVal.append((const char*)&mTime,sizeof(qint64));
	retVal.append(dumpToBinaryNoTime());
	return retVal;
}

QByteArray SensorValue::valueToString(quint32 dimIndex,quint32 packIndex)const
{
	return valueToString(packIndex*mType.dim+dimIndex);
}

double SensorValue::valueToDouble(quint32 dimIndex,quint32 packIndex)const
{
	return valueToDouble(packIndex*mType.dim+dimIndex);
}

qint64 SensorValue::valueToS64(quint32 dimIndex,quint32 packIndex)const
{
	return valueToS64(packIndex*mType.dim+dimIndex);
}

quint32 SensorValue::packetsCount()const
{
	return mPacketsCount;
}

const SensorDef::Type& SensorValue::type()const
{
	return mType;
}

quint32 SensorValue::totalCount()const
{
	return mType.dim*mPacketsCount;
}

qint64 SensorValue::time()const
{
	return mTime;
}

void SensorValue::setTime(qint64 t)
{
	mTime=t;
}

bool SensorValue::isDataEqual(const SensorValue &t)
{
	if(mType.numType!=t.mType.numType||mType.dim!=t.mType.dim||mPacketsCount!=t.mPacketsCount)return false;
	quint32 valCount=mType.dim*mPacketsCount;
	for(quint32 i=0;i<valCount;++i)
		if(!valueIsEqual(&t,i))return false;
	return true;
}

bool SensorValue::parseDataFromMsgArgs(const QByteArrayList &args)
{
	freeData();
	createData();
	quint32 dataOffset=(mType.tsType==SensorDef::NO_TIME)?0:1;
	quint32 totalValuesCount=mType.dim*mPacketsCount;
	for(quint32 i=0;i<totalValuesCount;++i)
		if(!valueFromString(i,args[i+dataOffset]))return false;
	return true;
}

bool SensorValue::parseDataFromBinary(const QByteArray &data)
{
	freeData();
	createData();
	quint32 dataOffset=(mType.tsType==SensorDef::NO_TIME)?0:sizeof(qint64);
	const char *dataWithOffset=data.constData()+dataOffset;
	copyFromBinaryData(dataWithOffset);
	return true;
}

bool SensorValue::operator==(const SensorValue &t)const
{
	if(!(mType==t.mType&&mTime==t.mTime&&mPacketsCount==t.mPacketsCount))return false;
	quint32 totalValuesCount=mType.dim*mPacketsCount;
	for(quint32 i=0;i<totalValuesCount;++i)
		if(!valueIsEqual(&t,i))
			return false;
	return true;
}

bool SensorValueF32::valueFromString(quint32 totalIndex,const QByteArray &data)
{
	bool ok=false;
	mData[totalIndex]=data.toFloat(&ok);
	return ok;
}

QByteArray SensorValueF32::valueToString(quint32 index) const
{
	return QByteArray::number(mData[index],'g',200);
}

bool SensorValueF64::valueFromString(quint32 totalIndex,const QByteArray &data)
{
	bool ok=false;
	mData[totalIndex]=data.toDouble(&ok);
	return ok;
}

QByteArray SensorValueF64::valueToString(quint32 index)const
{
	return QByteArray::number(mData[index],'g',200);
}

bool SensorValueS8::valueFromString(quint32 totalIndex,const QByteArray &data)
{
	bool ok=false;
	mData[totalIndex]=(qint8)data.toShort(&ok);
	return ok;
}

bool SensorValueU8::valueFromString(quint32 totalIndex,const QByteArray &data)
{
	bool ok=false;
	mData[totalIndex]=(qint8)data.toUShort(&ok);
	return ok;
}

bool SensorValueS16::valueFromString(quint32 totalIndex,const QByteArray &data)
{
	bool ok=false;
	mData[totalIndex]=data.toShort(&ok);
	return ok;
}

bool SensorValueU16::valueFromString(quint32 totalIndex,const QByteArray &data)
{
	bool ok=false;
	mData[totalIndex]=data.toUShort(&ok);
	return ok;
}

bool SensorValueS32::valueFromString(quint32 totalIndex,const QByteArray &data)
{
	bool ok=false;
	mData[totalIndex]=data.toLong(&ok);
	return ok;
}

bool SensorValueU32::valueFromString(quint32 totalIndex,const QByteArray &data)
{
	bool ok=false;
	mData[totalIndex]=data.toULong(&ok);
	return ok;
}

bool SensorValueS64::valueFromString(quint32 totalIndex,const QByteArray &data)
{
	bool ok=false;
	mData[totalIndex]=data.toLongLong(&ok);
	return ok;
}

bool SensorValueU64::valueFromString(quint32 totalIndex,const QByteArray &data)
{
	bool ok=false;
	mData[totalIndex]=data.toULongLong(&ok);
	return ok;
}

bool SensorValueText::valueFromString(quint32 totalIndex,const QByteArray &data)
{
	mData[totalIndex]=data;
	return true;
}

QByteArray SensorValueText::valueToString(quint32 totalIndex)const
{
	return mData.value(totalIndex);
}

QByteArray SensorValueText::get(quint32 dimIndex, quint32 packIndex)const
{
	return mData.value(dimIndex+mType.dim*packIndex);
}

QByteArray SensorValueText::getT(quint32 totalIndex) const
{
	return mData.value(totalIndex);
}

void SensorValueText::createData()
{
	mData.resize(mType.dim*mPacketsCount);
}

void SensorValueText::freeData()
{
	mData.clear();
}

void SensorValueText::copyDataFrom(const SensorValue *from)
{
	SensorValueText *tt=(SensorValueText*)from;
	mData=tt->mData;
}

bool SensorValueText::valueIsEqual(const SensorValue *t,quint32 index)const
{
	SensorValueText *tt=(SensorValueText*)t;
	return mData[index]==tt->mData[index];
}

QByteArray SensorValueText::dumpToBinaryNoTime()const
{
	return QByteArray();
}

bool SensorValueText::copyFromBinaryData(const char *data)
{
	Q_UNUSED(data)
	return false;
}
