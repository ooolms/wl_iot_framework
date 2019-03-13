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
	if(mType.numType==SensorDef::S8)
		*((SensorValueS8*)v)=*((SensorValueS8*)this);
	else if(mType.numType==SensorDef::U8)
		*((SensorValueU8*)v)=*((SensorValueU8*)this);
	else if(mType.numType==SensorDef::S16)
		*((SensorValueS16*)v)=*((SensorValueS16*)this);
	else if(mType.numType==SensorDef::U16)
		*((SensorValueU16*)v)=*((SensorValueU16*)this);
	else if(mType.numType==SensorDef::S32)
		*((SensorValueS32*)v)=*((SensorValueS32*)this);
	else if(mType.numType==SensorDef::U32)
		*((SensorValueU32*)v)=*((SensorValueU32*)this);
	else if(mType.numType==SensorDef::S64)
		*((SensorValueS64*)v)=*((SensorValueS64*)this);
	else if(mType.numType==SensorDef::U64)
		*((SensorValueU64*)v)=*((SensorValueU64*)this);
	else if(mType.numType==SensorDef::F32)
		*((SensorValueF32*)v)=*((SensorValueF32*)this);
	else if(mType.numType==SensorDef::F64)
		*((SensorValueF64*)v)=*((SensorValueF64*)this);
	else *((SensorValueText*)v)=*((SensorValueText*)this);
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
	createDataFrom(&t);
	return *this;
}

void SensorValue::copyMem(const void *from,void *to,std::size_t sz)
{
	for(std::size_t i=0;i<sz;++i)
		((char*)to)[i]=((const char*)from)[i];
}

bool SensorValue::isEqMem(const void *m1,const void *m2,std::size_t sz)
{
	for(std::size_t i=0;i<sz;++i)
		if(((char*)m1)[i]!=((const char*)m2)[i])return false;
	return true;
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

QByteArray SensorValueText::get(quint32 dimIndex,quint32 packIndex)const
{
	return ((QByteArray*)mData)[packIndex*mType.dim+dimIndex];
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
	return parseDataFromBinary(QByteArray::fromBase64(data));
}

QByteArrayList SensorValue::dumpToMsgArgs()const
{
	QByteArrayList retVal;
	if(mType.tsType!=SensorDef::NO_TIME)
		retVal.append(QByteArray::number(mTime));
	retVal.append(dumpToMsgArgsNoTime());
	return retVal;
}

QByteArrayList SensorValue::dumpToMsgArgsNoTime() const
{
	QByteArrayList retVal;
	quint32 totalValuesCount=mType.dim*mPacketsCount;
	for(quint32 i=0;i<totalValuesCount;++i)
		retVal.append(valueToStr(i));
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

QByteArray SensorValue::dumpToBinaryNoTime()const
{
	if(mType.numType==SensorDef::TEXT)return QByteArray();
	quint32 totalDataSize=mType.dim*mPacketsCount*mType.valueSizeInBytes();
	return QByteArray((const char*)mData,totalDataSize);
}

QByteArray SensorValue::valueToString(quint32 dimIndex,quint32 packIndex)const
{
	return valueToStr(packIndex*mType.dim+dimIndex);
}

quint32 SensorValue::packetsCount()const
{
	return mPacketsCount;
}

const SensorDef::Type& SensorValue::type()const
{
	return mType;
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

double SensorValue::valueToDouble(quint32 dimIndex,quint32 packIndex)const
{
	if(mType.numType==SensorDef::TEXT)
		return 0;
	double v=0;
	if(mType.numType==SensorDef::S8)
		v=((const SensorValueS8*)this)->get(dimIndex,packIndex);
	else if(mType.numType==SensorDef::U8)
		v=((const SensorValueU8*)this)->get(dimIndex,packIndex);
	else if(mType.numType==SensorDef::S16)
		v=((const SensorValueS16*)this)->get(dimIndex,packIndex);
	else if(mType.numType==SensorDef::U16)
		v=((const SensorValueU16*)this)->get(dimIndex,packIndex);
	else if(mType.numType==SensorDef::S32)
		v=((const SensorValueS32*)this)->get(dimIndex,packIndex);
	else if(mType.numType==SensorDef::U32)
		v=((const SensorValueU32*)this)->get(dimIndex,packIndex);
	else if(mType.numType==SensorDef::S64)
		v=((const SensorValueS64*)this)->get(dimIndex,packIndex);
	else if(mType.numType==SensorDef::U64)
		v=((const SensorValueU64*)this)->get(dimIndex,packIndex);
	else if(mType.numType==SensorDef::F32)
		v=((const SensorValueF32*)this)->get(dimIndex,packIndex);
	else if(mType.numType==SensorDef::F64)
		v=((const SensorValueF64*)this)->get(dimIndex,packIndex);
	return v;
}

bool SensorValue::parseDataFromMsgArgs(const QByteArrayList &args)
{
	freeData();
	createData();
	quint32 dataOffset=(mType.tsType==SensorDef::NO_TIME)?0:1;
	quint32 totalValuesCount=mType.dim*mPacketsCount;
	for(quint32 i=0;i<totalValuesCount;++i)
		if(!valueFromStr(i,args[i+dataOffset]))return false;
	return true;
}

bool SensorValue::parseDataFromBinary(const QByteArray &data)
{
	freeData();
	createData();
	quint32 dataOffset=(mType.tsType==SensorDef::NO_TIME)?0:sizeof(qint64);
	quint32 totalDataSize=mType.dim*mPacketsCount*mType.valueSizeInBytes();
	const char *dataWithOffset=data.constData()+dataOffset;
	copyMem(dataWithOffset,mData,totalDataSize);
	return true;
}

bool SensorValue::operator==(const SensorValue &t)const
{
	if(!(mType==t.mType&&mTime==t.mTime&&mPacketsCount==t.mPacketsCount))return false;
	quint32 totalValuesCount=mType.dim*mPacketsCount;
	for(quint32 i=0;i<totalValuesCount;++i)
		if(!valueIsEqual(&t,i))return false;
	return true;
}

bool SensorValueF32::valueFromStr(quint32 index,const QByteArray &data)
{
	bool ok=false;
	((float*)mData)[index]=data.toFloat(&ok);
	return ok;
}

QByteArray SensorValueF32::valueToStr(quint32 index)const
{
	return QByteArray::number(((float*)mData)[index],'g',17);
}

bool SensorValueF64::valueFromStr(quint32 index,const QByteArray &data)
{
	bool ok=false;
	((double*)mData)[index]=data.toDouble(&ok);
	return ok;
}

QByteArray SensorValueF64::valueToStr(quint32 index)const
{
	return QByteArray::number(((double*)mData)[index],'g',17);
}

bool SensorValueS8::valueFromStr(quint32 index,const QByteArray &data)
{
	bool ok=false;
	((qint8*)mData)[index]=(qint8)data.toShort(&ok);
	return ok;
}

QByteArray SensorValueS8::valueToStr(quint32 index)const
{
	return QByteArray::number(((qint8*)mData)[index]);
}

bool SensorValueU8::valueFromStr(quint32 index,const QByteArray &data)
{
	bool ok=false;
	((quint8*)mData)[index]=(qint8)data.toUShort(&ok);
	return ok;
}

QByteArray SensorValueU8::valueToStr(quint32 index)const
{
	return QByteArray::number(((quint8*)mData)[index]);
}

bool SensorValueS16::valueFromStr(quint32 index,const QByteArray &data)
{
	bool ok=false;
	((qint16*)mData)[index]=data.toShort(&ok);
	return ok;
}

QByteArray SensorValueS16::valueToStr(quint32 index)const
{
	return QByteArray::number(((qint16*)mData)[index]);
}

bool SensorValueU16::valueFromStr(quint32 index,const QByteArray &data)
{
	bool ok=false;
	((quint16*)mData)[index]=data.toUShort(&ok);
	return ok;
}

QByteArray SensorValueU16::valueToStr(quint32 index)const
{
	return QByteArray::number(((quint16*)mData)[index]);
}

bool SensorValueS32::valueFromStr(quint32 index,const QByteArray &data)
{
	bool ok=false;
	((qint32*)mData)[index]=data.toLong(&ok);
	return ok;
}

QByteArray SensorValueS32::valueToStr(quint32 index)const
{
	return QByteArray::number(((qint32*)mData)[index]);
}

bool SensorValueU32::valueFromStr(quint32 index,const QByteArray &data)
{
	bool ok=false;
	((quint32*)mData)[index]=data.toULong(&ok);
	return ok;
}

QByteArray SensorValueU32::valueToStr(quint32 index)const
{
	return QByteArray::number(((quint32*)mData)[index]);
}

bool SensorValueS64::valueFromStr(quint32 index,const QByteArray &data)
{
	bool ok=false;
	((qint64*)mData)[index]=data.toLongLong(&ok);
	return ok;
}

QByteArray SensorValueS64::valueToStr(quint32 index)const
{
	return QByteArray::number(((qint64*)mData)[index]);
}

bool SensorValueU64::valueFromStr(quint32 index,const QByteArray &data)
{
	bool ok=false;
	((quint64*)mData)[index]=data.toULongLong(&ok);
	return ok;
}

QByteArray SensorValueU64::valueToStr(quint32 index)const
{
	return QByteArray::number(((quint64*)mData)[index]);
}

bool SensorValueText::valueFromStr(quint32 index,const QByteArray &data)
{
	((QByteArray*)mData)[index]=data;
	return true;
}

QByteArray SensorValueText::valueToStr(quint32 index)const
{
	return ((QByteArray*)mData)[index];
}

void SensorValueText::createData()
{
	mData=new QByteArray[mType.dim*mPacketsCount];
}

void SensorValueText::freeData()
{
	delete[] (QByteArray*)mData;
}

void SensorValueText::createDataFrom(const SensorValue *from)
{
	SensorValueText *tt=(SensorValueText*)from;
	mData=new QByteArray[mType.dim*mPacketsCount];
	for(quint32 i=0;i<mType.dim*mPacketsCount;++i)
		((QByteArray*)mData)[i]=((const QByteArray*)tt->mData)[i];
}

bool SensorValueText::valueIsEqual(const SensorValue *t,quint32 index)const
{
	SensorValueText *tt=(SensorValueText*)t;
	return ((QByteArray*)mData)[index]==((QByteArray*)tt->mData)[index];
}
