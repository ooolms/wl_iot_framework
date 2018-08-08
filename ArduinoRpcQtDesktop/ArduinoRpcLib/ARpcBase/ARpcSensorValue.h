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

#ifndef ARPCSENSORVALUE_H
#define ARPCSENSORVALUE_H

#include "ARpcBase/ARpcSensorDef.h"
#include "ARpcBase/ARpcMessage.h"
#include <QVector>

//CRIT implicit sharing

class ARpcSensorValue
{
public:
	static ARpcSensorValue* createSensorValue(ARpcSensorDef::Type t);
	ARpcSensorValue* mkCopy()const;
	virtual ~ARpcSensorValue();
	bool isEmpty()const;
	bool operator==(const ARpcSensorValue &t)const;
	bool parseMsgArgs(const QByteArrayList &args);//msg args contain sensor value
	bool parseBinary(const QByteArray &data);//msg args contain sensor value
	bool parseBase64(const QByteArray &data);//msg args contain sensor value
	QByteArrayList dumpToMsgArgs()const;//to msg args
	QByteArrayList dumpToMsgArgsNoTime()const;
	QByteArray dumpToBinary()const;
	QByteArray dumpToBinaryNoTime()const;
	QByteArray valueToString(quint32 dimIndex,quint32 packIndex=0)const;
	quint32 packetsCount()const;
	const ARpcSensorDef::Type& type()const;
	qint64 time()const;
	void setTime(qint64 t);
	bool isDataEqual(const ARpcSensorValue &t);
	double valueToDouble(quint32 dimIndex,quint32 packIndex=0)const;

protected:
	explicit ARpcSensorValue(ARpcSensorDef::Type t);
	ARpcSensorValue& operator=(const ARpcSensorValue &t);
	virtual bool valueFromStr(quint32 index,const QByteArray &data)=0;
	virtual QByteArray valueToStr(quint32 index)const=0;
	virtual void createData()=0;
	virtual void freeData()=0;
	virtual void createDataFrom(const ARpcSensorValue *from)=0;
	virtual bool valueIsEqual(const ARpcSensorValue *t,quint32 index)const=0;
	static void copyMem(const void *from,void *to,std::size_t sz);
	static bool isEqMem(const void *m1,const void *m2,std::size_t sz);

private:
	bool parseDataFromMsgArgs(const QByteArrayList &args);
	bool parseDataFromBinary(const QByteArray &data);

protected:
	ARpcSensorDef::Type mType;
	qint64 mTime;
	void* mData;
	quint32 mPacketsCount;
};

template<typename T>
class ARpcSensorValueNumeric
	:public ARpcSensorValue
{
protected:
	explicit ARpcSensorValueNumeric(ARpcSensorDef::Type t)
		:ARpcSensorValue(t)
	{
		createData();
	}

	ARpcSensorValueNumeric(const ARpcSensorValueNumeric &t)
		:ARpcSensorValue(t)
	{}

	virtual ~ARpcSensorValueNumeric()
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

	virtual void createDataFrom(const ARpcSensorValue *from)override
	{
		const ARpcSensorValueNumeric<T> *tt=(const ARpcSensorValueNumeric<T>*)from;
		mData=new T[mType.dim*mPacketsCount];
		copyMem(tt->mData,mData,mType.dim*mPacketsCount*sizeof(T));
	}
	virtual bool valueIsEqual(const ARpcSensorValue *t,quint32 index)const override
	{
		const ARpcSensorValueNumeric<T> *tt=(const ARpcSensorValueNumeric<T>*)t;
		return isEqMem(((const char*)mData)+index*sizeof(T),((const char*)tt->mData)+index*sizeof(T),sizeof(T));
	}
};

class ARpcSensorValueF32
	:public ARpcSensorValueNumeric<float>
{
public:
	explicit ARpcSensorValueF32(ARpcSensorDef::Type t):ARpcSensorValueNumeric<float>(t){}
	ARpcSensorValueF32(const ARpcSensorValueF32 &t):ARpcSensorValueNumeric<float>(t){}

protected:
	virtual bool valueFromStr(quint32 index,const QByteArray &data)override;
	virtual QByteArray valueToStr(quint32 index)const override;
};

class ARpcSensorValueF64
	:public ARpcSensorValueNumeric<double>
{
public:
	explicit ARpcSensorValueF64(ARpcSensorDef::Type t):ARpcSensorValueNumeric<double>(t){}
	ARpcSensorValueF64(const ARpcSensorValueF64 &t):ARpcSensorValueNumeric<double>(t){}

protected:
	virtual bool valueFromStr(quint32 index,const QByteArray &data)override;
	virtual QByteArray valueToStr(quint32 index)const override;
};

class ARpcSensorValueS8
	:public ARpcSensorValueNumeric<qint8>
{
public:
	explicit ARpcSensorValueS8(ARpcSensorDef::Type t):ARpcSensorValueNumeric<qint8>(t){}
	ARpcSensorValueS8(const ARpcSensorValueS8 &t):ARpcSensorValueNumeric<qint8>(t){}

protected:
	virtual bool valueFromStr(quint32 index,const QByteArray &data)override;
	virtual QByteArray valueToStr(quint32 index)const override;
};

class ARpcSensorValueU8
	:public ARpcSensorValueNumeric<quint8>
{
public:
	explicit ARpcSensorValueU8(ARpcSensorDef::Type t):ARpcSensorValueNumeric<quint8>(t){}
	ARpcSensorValueU8(const ARpcSensorValueU8 &t):ARpcSensorValueNumeric<quint8>(t){}

protected:
	virtual bool valueFromStr(quint32 index,const QByteArray &data)override;
	virtual QByteArray valueToStr(quint32 index)const override;
};

class ARpcSensorValueS16
	:public ARpcSensorValueNumeric<qint16>
{
public:
	explicit ARpcSensorValueS16(ARpcSensorDef::Type t):ARpcSensorValueNumeric<qint16>(t){}
	ARpcSensorValueS16(const ARpcSensorValueS16 &t):ARpcSensorValueNumeric<qint16>(t){}

protected:
	virtual bool valueFromStr(quint32 index,const QByteArray &data)override;
	virtual QByteArray valueToStr(quint32 index)const override;
};

class ARpcSensorValueU16
	:public ARpcSensorValueNumeric<quint16>
{
public:
	explicit ARpcSensorValueU16(ARpcSensorDef::Type t):ARpcSensorValueNumeric<quint16>(t){}
	ARpcSensorValueU16(const ARpcSensorValueU16 &t):ARpcSensorValueNumeric<quint16>(t){}

protected:
	virtual bool valueFromStr(quint32 index,const QByteArray &data)override;
	virtual QByteArray valueToStr(quint32 index)const override;
};

class ARpcSensorValueS32
	:public ARpcSensorValueNumeric<qint32>
{
public:
	explicit ARpcSensorValueS32(ARpcSensorDef::Type t):ARpcSensorValueNumeric<qint32>(t){}
	ARpcSensorValueS32(const ARpcSensorValueS32 &t):ARpcSensorValueNumeric<qint32>(t){}

protected:
	virtual bool valueFromStr(quint32 index,const QByteArray &data)override;
	virtual QByteArray valueToStr(quint32 index)const override;
};

class ARpcSensorValueU32
	:public ARpcSensorValueNumeric<quint32>
{
public:
	explicit ARpcSensorValueU32(ARpcSensorDef::Type t):ARpcSensorValueNumeric<quint32>(t){}
	ARpcSensorValueU32(const ARpcSensorValueU32 &t):ARpcSensorValueNumeric<quint32>(t){}

protected:
	virtual bool valueFromStr(quint32 index,const QByteArray &data)override;
	virtual QByteArray valueToStr(quint32 index)const override;
};

class ARpcSensorValueS64
	:public ARpcSensorValueNumeric<qint64>
{
public:
	explicit ARpcSensorValueS64(ARpcSensorDef::Type t):ARpcSensorValueNumeric<qint64>(t){}
	ARpcSensorValueS64(const ARpcSensorValueS64 &t):ARpcSensorValueNumeric<qint64>(t){}

protected:
	virtual bool valueFromStr(quint32 index,const QByteArray &data)override;
	virtual QByteArray valueToStr(quint32 index)const override;
};

class ARpcSensorValueU64
	:public ARpcSensorValueNumeric<quint64>
{
public:
	explicit ARpcSensorValueU64(ARpcSensorDef::Type t):ARpcSensorValueNumeric<quint64>(t){}
	ARpcSensorValueU64(const ARpcSensorValueU64 &t):ARpcSensorValueNumeric<quint64>(t){}

protected:
	virtual bool valueFromStr(quint32 index,const QByteArray &data)override;
	virtual QByteArray valueToStr(quint32 index)const override;
};

class ARpcSensorValueText
	:public ARpcSensorValue
{
public:
	explicit ARpcSensorValueText(ARpcSensorDef::Type t);
	virtual ~ARpcSensorValueText();
	ARpcSensorValueText(const ARpcSensorValueText &t);
	QByteArray get(quint32 dimIndex,quint32 packIndex=0)const;

protected:
	virtual bool valueFromStr(quint32 index,const QByteArray &data)override;
	virtual QByteArray valueToStr(quint32 index)const override;
	virtual void createData()override;
	virtual void freeData()override;
	virtual void createDataFrom(const ARpcSensorValue *from)override;
	virtual bool valueIsEqual(const ARpcSensorValue *t,quint32 index)const override;
};

#endif // ARPCSENSORVALUE_H
