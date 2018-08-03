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

//CRIT implicit sharing

class ARpcSensorValue
{
public:
	explicit ARpcSensorValue(ARpcSensorDef::Type t);
	ARpcSensorValue(const ARpcSensorValue &t);
	~ARpcSensorValue();
	ARpcSensorValue& operator=(const ARpcSensorValue &t);

public:
	qint64 time()const;
	void setTime(qint64 t);
	bool isNull()const;
	bool parseMsgArgs(const QByteArrayList &mData);//msg args contain sensor value
	bool parseBinary(const QByteArray &mData);//msg args contain sensor value
	bool parseBase64(const QByteArray &mData);//msg args contain sensor value
	QByteArrayList dumpToMsgArgs()const;//to msg args
	QByteArray dumpToBinary()const;
	QByteArray dumpToBinaryNoTime()const;
	const ARpcSensorDef::Type& type()const;
	quint32 packetValuesCount()const;

public:
	qint8 getS8(quint32 dimIndex,quint32 packIndex=0)const;
	quint8 getU8(quint32 dimIndex,quint32 packIndex=0)const;
	qint16 getS16(quint32 dimIndex,quint32 packIndex=0)const;
	quint16 getU16(quint32 dimIndex,quint32 packIndex=0)const;
	qint32 getS32(quint32 dimIndex,quint32 packIndex=0)const;
	quint32 getU32(quint32 dimIndex,quint32 packIndex=0)const;
	qint64 getS64(quint32 dimIndex,quint32 packIndex=0)const;
	quint64 getU64(quint32 dimIndex,quint32 packIndex=0)const;
	float getF32(quint32 dimIndex,quint32 packIndex=0)const;
	double getF64(quint32 dimIndex,quint32 packIndex=0)const;
	const char* getText(quint32 dimIndex,quint32 packIndex=0)const;

private:
	bool parseDataFromMsgArgs(const QByteArrayList &args);
	bool parseDataFromBinary(const QByteArray &data);
	void freeDataMem();

protected:
	ARpcSensorDef::Type mType;
	qint64 mTime;
	union
	{
		char *numData;
		char **strData;
	}mData;
	quint32 mPacketValuesCount;
};

#endif // ARPCSENSORVALUE_H
