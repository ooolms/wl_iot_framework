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

#ifndef SENSORVALUE_H
#define SENSORVALUE_H

#include "wliot/devices/SensorDef.h"
#include "wliot/devices/Message.h"
#include <QVector>

//TODO implicit sharing

class SensorValue
{
public:
	static SensorValue* createSensorValue(SensorDef::Type t);
	SensorValue* mkCopy()const;
	virtual ~SensorValue();
	bool isEmpty()const;
	bool operator==(const SensorValue &t)const;
	bool parseMsgArgs(const QByteArrayList &args);//msg args contain sensor value
	bool parseBinary(const QByteArray &data);//msg args contain sensor value
	bool parseBase64(const QByteArray &data);//msg args contain sensor value
	QByteArrayList dumpToMsgArgs()const;//to msg args
	QByteArrayList dumpToMsgArgsNoTime()const;
	QByteArray dumpToBinary()const;
	QByteArray dumpToBinaryNoTime()const;
	QByteArray valueToString(quint32 dimIndex,quint32 packIndex=0)const;
	quint32 packetsCount()const;
	const SensorDef::Type& type()const;
	qint64 time()const;
	void setTime(qint64 t);
	bool isDataEqual(const SensorValue &t);
	double valueToDouble(quint32 dimIndex,quint32 packIndex=0)const;

protected:
	explicit SensorValue(SensorDef::Type t);
	SensorValue& operator=(const SensorValue &t);
	virtual bool valueFromStr(quint32 index,const QByteArray &data)=0;
	virtual QByteArray valueToStr(quint32 index)const=0;
	virtual void createData()=0;
	virtual void freeData()=0;
	virtual void createDataFrom(const SensorValue *from)=0;
	virtual bool valueIsEqual(const SensorValue *t,quint32 index)const=0;
	static void copyMem(const void *from,void *to,std::size_t sz);
	static bool isEqMem(const void *m1,const void *m2,std::size_t sz);

private:
	bool parseDataFromMsgArgs(const QByteArrayList &args);
	bool parseDataFromBinary(const QByteArray &data);

protected:
	SensorDef::Type mType;
	qint64 mTime;
	void* mData;
	quint32 mPacketsCount;
};

template<typename T>
class SensorValueNumeric
	:public SensorValue
{
protected:
	explicit SensorValueNumeric(SensorDef::Type t)
		:SensorValue(t)
	{
		createData();
	}

	SensorValueNumeric(const SensorValueNumeric &t)
		:SensorValue(t)
	{}

	virtual ~SensorValueNumeric()
	{
		freeData();
	}

public:
	T get(quint32 dimIndex,quint32 packIndex=0)const
	{
		return ((T*)mData)[packIndex*mType.dim+dimIndex];
	}

	QVector<T> getSample(quint32 packIndex=0)
	{
		QVector<T> v;
		v.resize(mType.dim);
		T *mDataTmp=((T*)mData)+(packIndex*mType.dim);
		for(quint32 i=0;i<mType.dim;++i)
			v[i]=mDataTmp[i];
		return v;
	}

protected:
	virtual void createData()override
	{
		mData=new T[mType.dim*mPacketsCount];
	}

	virtual void freeData()override
	{
		delete[] (T*)mData;
	}

	virtual void createDataFrom(const SensorValue *from)override
	{
		const SensorValueNumeric<T> *tt=(const SensorValueNumeric<T>*)from;
		mData=new T[mType.dim*mPacketsCount];
		copyMem(tt->mData,mData,mType.dim*mPacketsCount*sizeof(T));
	}
	virtual bool valueIsEqual(const SensorValue *t,quint32 index)const override
	{
		const SensorValueNumeric<T> *tt=(const SensorValueNumeric<T>*)t;
		return isEqMem(((const char*)mData)+index*sizeof(T),((const char*)tt->mData)+index*sizeof(T),sizeof(T));
	}
};

class SensorValueF32
	:public SensorValueNumeric<float>
{
public:
	explicit SensorValueF32(SensorDef::Type t):SensorValueNumeric<float>(t){}
	SensorValueF32(const SensorValueF32 &t):SensorValueNumeric<float>(t){}

protected:
	virtual bool valueFromStr(quint32 index,const QByteArray &data)override;
	virtual QByteArray valueToStr(quint32 index)const override;
};

class SensorValueF64
	:public SensorValueNumeric<double>
{
public:
	explicit SensorValueF64(SensorDef::Type t):SensorValueNumeric<double>(t){}
	SensorValueF64(const SensorValueF64 &t):SensorValueNumeric<double>(t){}

protected:
	virtual bool valueFromStr(quint32 index,const QByteArray &data)override;
	virtual QByteArray valueToStr(quint32 index)const override;
};

class SensorValueS8
	:public SensorValueNumeric<qint8>
{
public:
	explicit SensorValueS8(SensorDef::Type t):SensorValueNumeric<qint8>(t){}
	SensorValueS8(const SensorValueS8 &t):SensorValueNumeric<qint8>(t){}

protected:
	virtual bool valueFromStr(quint32 index,const QByteArray &data)override;
	virtual QByteArray valueToStr(quint32 index)const override;
};

class SensorValueU8
	:public SensorValueNumeric<quint8>
{
public:
	explicit SensorValueU8(SensorDef::Type t):SensorValueNumeric<quint8>(t){}
	SensorValueU8(const SensorValueU8 &t):SensorValueNumeric<quint8>(t){}

protected:
	virtual bool valueFromStr(quint32 index,const QByteArray &data)override;
	virtual QByteArray valueToStr(quint32 index)const override;
};

class SensorValueS16
	:public SensorValueNumeric<qint16>
{
public:
	explicit SensorValueS16(SensorDef::Type t):SensorValueNumeric<qint16>(t){}
	SensorValueS16(const SensorValueS16 &t):SensorValueNumeric<qint16>(t){}

protected:
	virtual bool valueFromStr(quint32 index,const QByteArray &data)override;
	virtual QByteArray valueToStr(quint32 index)const override;
};

class SensorValueU16
	:public SensorValueNumeric<quint16>
{
public:
	explicit SensorValueU16(SensorDef::Type t):SensorValueNumeric<quint16>(t){}
	SensorValueU16(const SensorValueU16 &t):SensorValueNumeric<quint16>(t){}

protected:
	virtual bool valueFromStr(quint32 index,const QByteArray &data)override;
	virtual QByteArray valueToStr(quint32 index)const override;
};

class SensorValueS32
	:public SensorValueNumeric<qint32>
{
public:
	explicit SensorValueS32(SensorDef::Type t):SensorValueNumeric<qint32>(t){}
	SensorValueS32(const SensorValueS32 &t):SensorValueNumeric<qint32>(t){}

protected:
	virtual bool valueFromStr(quint32 index,const QByteArray &data)override;
	virtual QByteArray valueToStr(quint32 index)const override;
};

class SensorValueU32
	:public SensorValueNumeric<quint32>
{
public:
	explicit SensorValueU32(SensorDef::Type t):SensorValueNumeric<quint32>(t){}
	SensorValueU32(const SensorValueU32 &t):SensorValueNumeric<quint32>(t){}

protected:
	virtual bool valueFromStr(quint32 index,const QByteArray &data)override;
	virtual QByteArray valueToStr(quint32 index)const override;
};

class SensorValueS64
	:public SensorValueNumeric<qint64>
{
public:
	explicit SensorValueS64(SensorDef::Type t):SensorValueNumeric<qint64>(t){}
	SensorValueS64(const SensorValueS64 &t):SensorValueNumeric<qint64>(t){}

protected:
	virtual bool valueFromStr(quint32 index,const QByteArray &data)override;
	virtual QByteArray valueToStr(quint32 index)const override;
};

class SensorValueU64
	:public SensorValueNumeric<quint64>
{
public:
	explicit SensorValueU64(SensorDef::Type t):SensorValueNumeric<quint64>(t){}
	SensorValueU64(const SensorValueU64 &t):SensorValueNumeric<quint64>(t){}

protected:
	virtual bool valueFromStr(quint32 index,const QByteArray &data)override;
	virtual QByteArray valueToStr(quint32 index)const override;
};

class SensorValueText
	:public SensorValue
{
public:
	explicit SensorValueText(SensorDef::Type t);
	virtual ~SensorValueText();
	SensorValueText(const SensorValueText &t);
	QByteArray get(quint32 dimIndex,quint32 packIndex=0)const;

protected:
	virtual bool valueFromStr(quint32 index,const QByteArray &data)override;
	virtual QByteArray valueToStr(quint32 index)const override;
	virtual void createData()override;
	virtual void freeData()override;
	virtual void createDataFrom(const SensorValue *from)override;
	virtual bool valueIsEqual(const SensorValue *t,quint32 index)const override;
};

#endif // SENSORVALUE_H
