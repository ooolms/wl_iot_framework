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
#include "ARpcPacketSensorValue.h"
#include "ARpcSingleSensorValue.h"
#include "ARpcTextSensorValue.h"

ARpcSensorValue::ARpcSensorValue(ARpcSensorDef::Type t)
{
	mTime=0;
	mType=t;
	mData.numData=0;
	mPacketValuesCount=1;
}

ARpcSensorValue::ARpcSensorValue(const ARpcSensorValue &t)
{
	mData.numData=0;
	*this=t;
}

ARpcSensorValue::~ARpcSensorValue()
{
	freeDataMem();
}

ARpcSensorValue& ARpcSensorValue::operator=(const ARpcSensorValue &t)
{
	freeDataMem();
	mTime=t.mTime;
	mType=t.mType;
	mPacketValuesCount=t.mPacketValuesCount;
	if(mType.numType!=ARpcSensorDef::Type::TEXT)
	{
		quint32 totalDataSize=mType.dim*mPacketValuesCount*mType.valueSizeInBytes();
		mData.numData=new char[totalDataSize];
		for(quint32 i=0;i<totalDataSize;++i)
			mData.numData[i]=t.mData.numData[i];
	}
	else
	{
		quint32 totalValuesCount=mType.dim*mPacketValuesCount;
		mData.strData=new char*[totalValuesCount];
		for(quint32 i=0;i<totalValuesCount;++i)
		{
			mData.strData[i]=new char[qstrlen(t.mData.strData[i])+1];
			qstrcpy(mData.strData[i],t.mData.strData[i]);
		}
	}
	return *this;
}

qint64 ARpcSensorValue::time()const
{
	return mTime;
}

void ARpcSensorValue::setTime(qint64 t)
{
	mTime=t;
}

bool ARpcSensorValue::isNull()const
{
	return mData.numData==0;
}

bool ARpcSensorValue::parseMsgArgs(const QByteArrayList &args)
{
	quint32 argsCount=args.count();
	if(mType.numType==ARpcSensorDef::Type::BAD_TYPE||mType.dim==0)return false;
	quint32 dataOffset=(mType.tsType==ARpcSensorDef::Type::NO_TIME)?0:1;
	if(argsCount<dataOffset)return false;
	if(mType.packType==ARpcSensorDef::Type::SINGLE)
	{
		if(argsCount!=(mType.dim+dataOffset))return false;
		mPacketValuesCount=1;
	}
	else
	{
		if((argsCount-dataOffset)%mType.dim!=0)return false;
		mPacketValuesCount=(argsCount-dataOffset)/mType.dim;
	}
	if(mType.tsType!=ARpcSensorDef::Type::NO_TIME)
	{
		bool ok=false;
		if(mType.tsType!=ARpcSensorDef::Type::NO_TIME)
		{
			mTime=args[0].toLongLong(&ok);
			if(!ok)return false;
		}
	}
	return parseDataFromMsgArgs(args);
}

bool ARpcSensorValue::parseBinary(const QByteArray &data)
{
	quint32 dataSize=data.size();
	if(mType.numType==ARpcSensorDef::Type::BAD_TYPE||mType.dim==0||mType.numType==ARpcSensorDef::Type::TEXT)
		return false;
	quint32 dataOffset=(mType.tsType==ARpcSensorDef::Type::NO_TIME)?0:sizeof(qint64);
	if(dataSize<dataOffset)return false;
	quint32 valSize=mType.valueSizeInBytes();
	if(mType.packType==ARpcSensorDef::Type::SINGLE)
	{
		if(dataSize!=(mType.dim*valSize+dataOffset))return false;
		mPacketValuesCount=1;
	}
	else
	{
		if((dataSize-dataOffset)%(mType.dim*valSize)!=0)return false;
		mPacketValuesCount=(dataSize-dataOffset)/(mType.dim*valSize);
	}
	if(mType.tsType!=ARpcSensorDef::Type::NO_TIME)
	{
		bool ok=false;
		if(mType.tsType!=ARpcSensorDef::Type::NO_TIME)
		{
			mTime=*((qint64*)data.constData());
			if(!ok)return false;
		}
	}
	return parseDataFromBinary(data);
}

bool ARpcSensorValue::parseBase64(const QByteArray &mData)
{
	return parseDataFromBinary(QByteArray::fromBase64(mData));
}

QByteArrayList ARpcSensorValue::dumpToMsgArgs()const
{
	QByteArrayList retVal;
	if(mType.tsType!=ARpcSensorDef::Type::NO_TIME)
		retVal.append(QByteArray::number(mTime));
	for(quint32 i=0;i<mPacketValuesCount;++i)
	{
		for(quint32 j=0;j<mType.dim;++j)
		{
			if(mType.numType==ARpcSensorDef::Type::S8)
				retVal.append(QByteArray::number(getS8(j,i)));
			else if(mType.numType==ARpcSensorDef::Type::U8)
				retVal.append(QByteArray::number(getU8(j,i)));
			else if(mType.numType==ARpcSensorDef::Type::S16)
				retVal.append(QByteArray::number(getS16(j,i)));
			else if(mType.numType==ARpcSensorDef::Type::U16)
				retVal.append(QByteArray::number(getU16(j,i)));
			else if(mType.numType==ARpcSensorDef::Type::S32)
				retVal.append(QByteArray::number(getS32(j,i)));
			else if(mType.numType==ARpcSensorDef::Type::U32)
				retVal.append(QByteArray::number(getU32(j,i)));
			else if(mType.numType==ARpcSensorDef::Type::S64)
				retVal.append(QByteArray::number(getS64(j,i)));
			else if(mType.numType==ARpcSensorDef::Type::U64)
				retVal.append(QByteArray::number(getU64(j,i)));
			else if(mType.numType==ARpcSensorDef::Type::F32)
				retVal.append(QByteArray::number(getF32(j,i)));
			else if(mType.numType==ARpcSensorDef::Type::F64)
				retVal.append(QByteArray::number(getF64(j,i)));
			else retVal.append(QByteArray(getText(j,i)));
		}
	}
	return retVal;
}

QByteArray ARpcSensorValue::dumpToBinary()const
{
	if(isNull()||mType.numType==ARpcSensorDef::Type::TEXT)return QByteArray();
	QByteArray retVal;
	if(mType.tsType!=ARpcSensorDef::Type::NO_TIME)
		retVal.append((const char*)&mTime,sizeof(qint64));
	retVal.append(dumpToBinaryNoTime());
	return retVal;
}

QByteArray ARpcSensorValue::dumpToBinaryNoTime()const
{
	if(isNull()||mType.numType==ARpcSensorDef::Type::TEXT)return QByteArray();
	quint32 totalDataSize=mType.dim*mPacketValuesCount*mType.valueSizeInBytes();
	return QByteArray(mData.numData,totalDataSize);
}

const ARpcSensorDef::Type& ARpcSensorValue::type()const
{
	return mType;
}

quint32 ARpcSensorValue::packetValuesCount()const
{
	return mPacketValuesCount;
}

qint8 ARpcSensorValue::getS8(quint32 dimIndex,quint32 packIndex)const
{
	return *((qint8*)&mData.numData[packIndex*mType.dim+dimIndex]);
}

quint8 ARpcSensorValue::getU8(quint32 dimIndex,quint32 packIndex)const
{
	return *((quint8*)&mData.numData[packIndex*mType.dim+dimIndex]);
}

qint16 ARpcSensorValue::getS16(quint32 dimIndex,quint32 packIndex)const
{
	return *((qint16*)&mData.numData[(packIndex*mType.dim+dimIndex)<<1]);
}

quint16 ARpcSensorValue::getU16(quint32 dimIndex,quint32 packIndex)const
{
	return *((quint16*)&mData.numData[(packIndex*mType.dim+dimIndex)<<1]);
}

qint32 ARpcSensorValue::getS32(quint32 dimIndex,quint32 packIndex)const
{
	return *((qint32*)&mData.numData[(packIndex*mType.dim+dimIndex)<<2]);
}

quint32 ARpcSensorValue::getU32(quint32 dimIndex,quint32 packIndex)const
{
	return *((quint32*)&mData.numData[(packIndex*mType.dim+dimIndex)<<2]);
}

qint64 ARpcSensorValue::getS64(quint32 dimIndex, quint32 packIndex)const
{
	return *((qint64*)&mData.numData[(packIndex*mType.dim+dimIndex)<<3]);
}

quint64 ARpcSensorValue::getU64(quint32 dimIndex,quint32 packIndex)const
{
	return *((quint64*)&mData.numData[(packIndex*mType.dim+dimIndex)<<3]);
}

float ARpcSensorValue::getF32(quint32 dimIndex,quint32 packIndex)const
{
	return *((float*)&mData.numData[(packIndex*mType.dim+dimIndex)<<2]);
}

double ARpcSensorValue::getF64(quint32 dimIndex,quint32 packIndex)const
{
	return *((double*)&mData.numData[(packIndex*mType.dim+dimIndex)<<3]);
}

const char *ARpcSensorValue::getText(quint32 dimIndex,quint32 packIndex)const
{
	return mData.strData[packIndex*mType.dim+dimIndex];
}

bool ARpcSensorValue::parseDataFromMsgArgs(const QByteArrayList &args)
{
	freeDataMem();
	bool ok=false;
	quint32 dataOffset=(mType.tsType==ARpcSensorDef::Type::NO_TIME)?0:1;
	quint32 totalValuesCount=mType.dim*mPacketValuesCount;
	if(mType.numType==ARpcSensorDef::Type::TEXT)
	{
		mData.strData=new char*[totalValuesCount];
		for(quint32 i=0;i<totalValuesCount;++i)
		{
			mData.strData[i]=new char[args[i+dataOffset].count()+1];
			qstrcpy(mData.strData[i],args[i+dataOffset].constData());
		}
		return true;
	}
	if(mType.numType==ARpcSensorDef::Type::S16||mType.numType==ARpcSensorDef::Type::U16)
		mData.numData=new char[totalValuesCount<<1];
	else if(mType.numType==ARpcSensorDef::Type::S32||mType.numType==ARpcSensorDef::Type::U32||
		mType.numType==ARpcSensorDef::Type::F32)
		mData.numData=new char[totalValuesCount<<2];
	else if(mType.numType==ARpcSensorDef::Type::S64||mType.numType==ARpcSensorDef::Type::U64||
		mType.numType==ARpcSensorDef::Type::F64)
		mData.numData=new char[totalValuesCount<<3];
	else mData.numData=new char[totalValuesCount];
	for(quint32 i=0;i<totalValuesCount;++i)
	{
		ok=false;
		if(mType.numType==ARpcSensorDef::Type::S8)
			*((qint8*)&mData.numData[i])=(qint8)args[i+dataOffset].toShort(&ok);
		else if(mType.numType==ARpcSensorDef::Type::U8)
			*((quint8*)&mData.numData[i])=(qint8)args[i+dataOffset].toUShort(&ok);
		else if(mType.numType==ARpcSensorDef::Type::S16)
			*((qint16*)&mData.numData[i<<1])=args[i+dataOffset].toShort(&ok);
		else if(mType.numType==ARpcSensorDef::Type::U16)
			*((quint16*)&mData.numData[i<<1])=args[i+dataOffset].toUShort(&ok);
		else if(mType.numType==ARpcSensorDef::Type::S32)
			*((qint32*)&mData.numData[i<<2])=args[i+dataOffset].toLong(&ok);
		else if(mType.numType==ARpcSensorDef::Type::U32)
			*((quint32*)&mData.numData[i<<2])=args[i+dataOffset].toULong(&ok);
		else if(mType.numType==ARpcSensorDef::Type::S64)
			*((qint64*)&mData.numData[i<<3])=args[i+dataOffset].toLongLong(&ok);
		else if(mType.numType==ARpcSensorDef::Type::U64)
			*((quint64*)&mData.numData[i<<3])=args[i+dataOffset].toULongLong(&ok);
		else if(mType.numType==ARpcSensorDef::Type::F32)
			*((float*)&mData.numData[i<<2])=args[i+dataOffset].toFloat(&ok);
		else if(mType.numType==ARpcSensorDef::Type::F64)
			*((double*)&mData.numData[i<<3])=args[i+dataOffset].toDouble(&ok);
		if(!ok)return false;
	}
	return true;
}

bool ARpcSensorValue::parseDataFromBinary(const QByteArray &data)
{
	freeDataMem();
	quint32 dataOffset=(mType.tsType==ARpcSensorDef::Type::NO_TIME)?0:sizeof(qint64);
	quint32 totalDataSize=mType.dim*mPacketValuesCount*mType.valueSizeInBytes();
	mData.numData=new char[totalDataSize];
	const char *dataWithOffset=data.constData()+dataOffset;
	for(quint32 i=0;i<totalDataSize;++i)
		mData.numData[i]=dataWithOffset[i+dataOffset];
	return true;
}

void ARpcSensorValue::freeDataMem()
{
	if(!mData.numData)return;
	if(mType.numType==ARpcSensorDef::Type::TEXT)
	{
		for(quint32 i=0;i<mPacketValuesCount*mType.dim;++i)
			delete[] mData.strData[i];
		delete[] mData.strData;
	}
	else delete[] mData.numData;
	mData.numData=0;
}
