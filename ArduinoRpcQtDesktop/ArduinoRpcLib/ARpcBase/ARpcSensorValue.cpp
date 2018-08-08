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

#include "ARpcSensorValue.h"

ARpcSensorValue* ARpcSensorValue::mkCopy()const
{
	ARpcSensorValue *v=createSensorValue(mType);
	if(mType.numType==ARpcSensorDef::S8)
		*((ARpcSensorValueS8*)v)=*((ARpcSensorValueS8*)this);
	else if(mType.numType==ARpcSensorDef::U8)
		*((ARpcSensorValueU8*)v)=*((ARpcSensorValueU8*)this);
	else if(mType.numType==ARpcSensorDef::S16)
		*((ARpcSensorValueS16*)v)=*((ARpcSensorValueS16*)this);
	else if(mType.numType==ARpcSensorDef::U16)
		*((ARpcSensorValueU16*)v)=*((ARpcSensorValueU16*)this);
	else if(mType.numType==ARpcSensorDef::S32)
		*((ARpcSensorValueS32*)v)=*((ARpcSensorValueS32*)this);
	else if(mType.numType==ARpcSensorDef::U32)
		*((ARpcSensorValueU32*)v)=*((ARpcSensorValueU32*)this);
	else if(mType.numType==ARpcSensorDef::S64)
		*((ARpcSensorValueS64*)v)=*((ARpcSensorValueS64*)this);
	else if(mType.numType==ARpcSensorDef::U64)
		*((ARpcSensorValueU64*)v)=*((ARpcSensorValueU64*)this);
	else if(mType.numType==ARpcSensorDef::F32)
		*((ARpcSensorValueF32*)v)=*((ARpcSensorValueF32*)this);
	else if(mType.numType==ARpcSensorDef::F64)
		*((ARpcSensorValueF64*)v)=*((ARpcSensorValueF64*)this);
	else *((ARpcSensorValueText*)v)=*((ARpcSensorValueText*)this);
	return v;
}

ARpcSensorValue* ARpcSensorValue::createSensorValue(ARpcSensorDef::Type t)
{
	if(t.numType==ARpcSensorDef::S8)
		return (ARpcSensorValue*)new ARpcSensorValueS8(t);
	else if(t.numType==ARpcSensorDef::U8)
		return (ARpcSensorValue*)new ARpcSensorValueU8(t);
	else if(t.numType==ARpcSensorDef::S16)
		return (ARpcSensorValue*)new ARpcSensorValueS16(t);
	else if(t.numType==ARpcSensorDef::U16)
		return (ARpcSensorValue*)new ARpcSensorValueU16(t);
	else if(t.numType==ARpcSensorDef::S32)
		return (ARpcSensorValue*)new ARpcSensorValueS32(t);
	else if(t.numType==ARpcSensorDef::U32)
		return (ARpcSensorValue*)new ARpcSensorValueU32(t);
	else if(t.numType==ARpcSensorDef::S64)
		return (ARpcSensorValue*)new ARpcSensorValueS64(t);
	else if(t.numType==ARpcSensorDef::U64)
		return (ARpcSensorValue*)new ARpcSensorValueU64(t);
	else if(t.numType==ARpcSensorDef::F32)
		return (ARpcSensorValue*)new ARpcSensorValueF32(t);
	else if(t.numType==ARpcSensorDef::F64)
		return (ARpcSensorValue*)new ARpcSensorValueF64(t);
	else if(t.numType==ARpcSensorDef::TEXT)
		return (ARpcSensorValue*)new ARpcSensorValueText(t);
	else return 0;
}

ARpcSensorValue::ARpcSensorValue(ARpcSensorDef::Type t)
{
	mType=t;
	mTime=0;
	mPacketsCount=1;
}

ARpcSensorValue::~ARpcSensorValue()
{
}

bool ARpcSensorValue::isEmpty()const
{
	return mType.dim==0||mPacketsCount==0;
}

ARpcSensorValue& ARpcSensorValue::operator=(const ARpcSensorValue &t)
{
	mType=t.mType;
	mTime=t.mTime;
	mPacketsCount=t.mPacketsCount;
	freeData();
	createDataFrom(&t);
	return *this;
}

void ARpcSensorValue::copyMem(const void *from,void *to,std::size_t sz)
{
	for(std::size_t i=0;i<sz;++i)
		((char*)to)[i]=((const char*)from)[i];
}

bool ARpcSensorValue::isEqMem(const void *m1,const void *m2,std::size_t sz)
{
	for(std::size_t i=0;i<sz;++i)
		if(((char*)m1)[i]!=((const char*)m2)[i])return false;
	return true;
}

ARpcSensorValueText::ARpcSensorValueText(ARpcSensorDef::Type t)
	:ARpcSensorValue(t)
{
	createData();
}

ARpcSensorValueText::~ARpcSensorValueText()
{
	freeData();
}

ARpcSensorValueText::ARpcSensorValueText(const ARpcSensorValueText &t)
	:ARpcSensorValue(t)
{
	createData();
}

QByteArray ARpcSensorValueText::get(quint32 dimIndex,quint32 packIndex)const
{
	return ((QByteArray*)mData)[packIndex*mType.dim+dimIndex];
}

bool ARpcSensorValue::parseMsgArgs(const QByteArrayList &args)
{
	quint32 argsCount=args.count();
	if(mType.numType==ARpcSensorDef::BAD_TYPE||mType.dim==0)return false;
	quint32 dataOffset=(mType.tsType==ARpcSensorDef::NO_TIME)?0:1;
	if(argsCount<dataOffset)return false;
	if(mType.packType==ARpcSensorDef::SINGLE)
	{
		if(argsCount!=(mType.dim+dataOffset))return false;
		mPacketsCount=1;
	}
	else
	{
		if((argsCount-dataOffset)%mType.dim!=0)return false;
		mPacketsCount=(argsCount-dataOffset)/mType.dim;
	}
	if(mType.tsType!=ARpcSensorDef::NO_TIME)
	{
		bool ok=false;
		mTime=args[0].toLongLong(&ok);
		if(!ok)return false;
	}
	return parseDataFromMsgArgs(args);
}

bool ARpcSensorValue::parseBinary(const QByteArray &data)
{
	quint32 dataSize=data.size();
	if(mType.numType==ARpcSensorDef::BAD_TYPE||mType.dim==0||mType.numType==ARpcSensorDef::TEXT)
		return false;
	quint32 dataOffset=(mType.tsType==ARpcSensorDef::NO_TIME)?0:sizeof(qint64);
	if(dataSize<dataOffset)return false;
	quint32 valSize=mType.valueSizeInBytes();
	if(mType.packType==ARpcSensorDef::SINGLE)
	{
		if(dataSize!=(mType.dim*valSize+dataOffset))return false;
		mPacketsCount=1;
	}
	else
	{
		if((dataSize-dataOffset)%(mType.dim*valSize)!=0)return false;
		mPacketsCount=(dataSize-dataOffset)/(mType.dim*valSize);
	}
	if(mType.tsType!=ARpcSensorDef::NO_TIME)
		mTime=*((qint64*)data.constData());
	return parseDataFromBinary(data);
}

bool ARpcSensorValue::parseBase64(const QByteArray &data)
{
	return parseDataFromBinary(QByteArray::fromBase64(data));
}

QByteArrayList ARpcSensorValue::dumpToMsgArgs()const
{
	QByteArrayList retVal;
	if(mType.tsType!=ARpcSensorDef::NO_TIME)
		retVal.append(QByteArray::number(mTime));
	retVal.append(dumpToMsgArgsNoTime());
	return retVal;
}

QByteArrayList ARpcSensorValue::dumpToMsgArgsNoTime() const
{
	QByteArrayList retVal;
	quint32 totalValuesCount=mType.dim*mPacketsCount;
	for(quint32 i=0;i<totalValuesCount;++i)
		retVal.append(valueToStr(i));
	return retVal;
}

QByteArray ARpcSensorValue::dumpToBinary()const
{
	if(mType.numType==ARpcSensorDef::TEXT)return QByteArray();
	QByteArray retVal;
	if(mType.tsType!=ARpcSensorDef::NO_TIME)
		retVal.append((const char*)&mTime,sizeof(qint64));
	retVal.append(dumpToBinaryNoTime());
	return retVal;
}

QByteArray ARpcSensorValue::dumpToBinaryNoTime()const
{
	if(mType.numType==ARpcSensorDef::TEXT)return QByteArray();
	quint32 totalDataSize=mType.dim*mPacketsCount*mType.valueSizeInBytes();
	return QByteArray((const char*)mData,totalDataSize);
}

QByteArray ARpcSensorValue::valueToString(quint32 dimIndex,quint32 packIndex)const
{
	return valueToStr(packIndex*mType.dim+dimIndex);
}

quint32 ARpcSensorValue::packetsCount()const
{
	return mPacketsCount;
}

const ARpcSensorDef::Type& ARpcSensorValue::type()const
{
	return mType;
}

qint64 ARpcSensorValue::time()const
{
	return mTime;
}

void ARpcSensorValue::setTime(qint64 t)
{
	mTime=t;
}

bool ARpcSensorValue::isDataEqual(const ARpcSensorValue &t)
{
	if(mType.numType!=t.mType.numType||mType.dim!=t.mType.dim||mPacketsCount!=t.mPacketsCount)return false;
	quint32 valCount=mType.dim*mPacketsCount;
	for(quint32 i=0;i<valCount;++i)
		if(!valueIsEqual(&t,i))return false;
	return true;
}

bool ARpcSensorValue::parseDataFromMsgArgs(const QByteArrayList &args)
{
	freeData();
	createData();
	quint32 dataOffset=(mType.tsType==ARpcSensorDef::NO_TIME)?0:1;
	quint32 totalValuesCount=mType.dim*mPacketsCount;
	for(quint32 i=0;i<totalValuesCount;++i)
		if(!valueFromStr(i,args[i+dataOffset]))return false;
	return true;
}

bool ARpcSensorValue::parseDataFromBinary(const QByteArray &data)
{
	freeData();
	createData();
	quint32 dataOffset=(mType.tsType==ARpcSensorDef::NO_TIME)?0:sizeof(qint64);
	quint32 totalDataSize=mType.dim*mPacketsCount*mType.valueSizeInBytes();
	const char *dataWithOffset=data.constData()+dataOffset;
	copyMem(dataWithOffset,mData,totalDataSize);
	return true;
}

bool ARpcSensorValue::operator==(const ARpcSensorValue &t)const
{
	if(!(mType==t.mType&&mTime==t.mTime&&mPacketsCount==t.mPacketsCount))return false;
	quint32 totalValuesCount=mType.dim*mPacketsCount;
	for(quint32 i=0;i<totalValuesCount;++i)
		if(!valueIsEqual(&t,i))return false;
	return true;
}

bool ARpcSensorValueF32::valueFromStr(quint32 index,const QByteArray &data)
{
	bool ok=false;
	((float*)mData)[index]=data.toFloat(&ok);
	return ok;
}

QByteArray ARpcSensorValueF32::valueToStr(quint32 index)const
{
	return QByteArray::number(((float*)mData)[index],'g',17);
}

bool ARpcSensorValueF64::valueFromStr(quint32 index,const QByteArray &data)
{
	bool ok=false;
	((double*)mData)[index]=data.toDouble(&ok);
	return ok;
}

QByteArray ARpcSensorValueF64::valueToStr(quint32 index)const
{
	return QByteArray::number(((double*)mData)[index],'g',17);
}

bool ARpcSensorValueS8::valueFromStr(quint32 index,const QByteArray &data)
{
	bool ok=false;
	((qint8*)mData)[index]=(qint8)data.toShort(&ok);
	return ok;
}

QByteArray ARpcSensorValueS8::valueToStr(quint32 index)const
{
	return QByteArray::number(((qint8*)mData)[index]);
}

bool ARpcSensorValueU8::valueFromStr(quint32 index,const QByteArray &data)
{
	bool ok=false;
	((quint8*)mData)[index]=(qint8)data.toUShort(&ok);
	return ok;
}

QByteArray ARpcSensorValueU8::valueToStr(quint32 index)const
{
	return QByteArray::number(((quint8*)mData)[index]);
}

bool ARpcSensorValueS16::valueFromStr(quint32 index,const QByteArray &data)
{
	bool ok=false;
	((qint16*)mData)[index]=data.toShort(&ok);
	return ok;
}

QByteArray ARpcSensorValueS16::valueToStr(quint32 index)const
{
	return QByteArray::number(((qint16*)mData)[index]);
}

bool ARpcSensorValueU16::valueFromStr(quint32 index,const QByteArray &data)
{
	bool ok=false;
	((quint16*)mData)[index]=data.toUShort(&ok);
	return ok;
}

QByteArray ARpcSensorValueU16::valueToStr(quint32 index)const
{
	return QByteArray::number(((quint16*)mData)[index]);
}

bool ARpcSensorValueS32::valueFromStr(quint32 index,const QByteArray &data)
{
	bool ok=false;
	((qint32*)mData)[index]=data.toLong(&ok);
	return ok;
}

QByteArray ARpcSensorValueS32::valueToStr(quint32 index)const
{
	return QByteArray::number(((qint32*)mData)[index]);
}

bool ARpcSensorValueU32::valueFromStr(quint32 index,const QByteArray &data)
{
	bool ok=false;
	((quint32*)mData)[index]=data.toULong(&ok);
	return ok;
}

QByteArray ARpcSensorValueU32::valueToStr(quint32 index)const
{
	return QByteArray::number(((quint32*)mData)[index]);
}

bool ARpcSensorValueS64::valueFromStr(quint32 index,const QByteArray &data)
{
	bool ok=false;
	((qint64*)mData)[index]=data.toLongLong(&ok);
	return ok;
}

QByteArray ARpcSensorValueS64::valueToStr(quint32 index)const
{
	return QByteArray::number(((qint64*)mData)[index]);
}

bool ARpcSensorValueU64::valueFromStr(quint32 index,const QByteArray &data)
{
	bool ok=false;
	((quint64*)mData)[index]=data.toULongLong(&ok);
	return ok;
}

QByteArray ARpcSensorValueU64::valueToStr(quint32 index)const
{
	return QByteArray::number(((quint64*)mData)[index]);
}

bool ARpcSensorValueText::valueFromStr(quint32 index,const QByteArray &data)
{
	((QByteArray*)mData)[index]=data;
	return true;
}

QByteArray ARpcSensorValueText::valueToStr(quint32 index)const
{
	return ((QByteArray*)mData)[index];
}

void ARpcSensorValueText::createData()
{
	mData=new QByteArray[mType.dim*mPacketsCount];
}

void ARpcSensorValueText::freeData()
{
	delete[] (QByteArray*)mData;
}

void ARpcSensorValueText::createDataFrom(const ARpcSensorValue *from)
{
	ARpcSensorValueText *tt=(ARpcSensorValueText*)from;
	mData=new QByteArray[mType.dim*mPacketsCount];
	for(quint32 i=0;i<mType.dim*mPacketsCount;++i)
		((QByteArray*)mData)[i]=((const QByteArray*)tt->mData)[i];
}

bool ARpcSensorValueText::valueIsEqual(const ARpcSensorValue *t,quint32 index)const
{
	ARpcSensorValueText *tt=(ARpcSensorValueText*)t;
	return ((QByteArray*)mData)[index]==((QByteArray*)tt->mData)[index];
}
