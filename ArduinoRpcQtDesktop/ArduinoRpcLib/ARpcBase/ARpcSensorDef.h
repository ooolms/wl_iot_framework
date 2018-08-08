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

#ifndef ARPCSENSORDEF_H
#define ARPCSENSORDEF_H

#include <QVariantMap>

class ARpcSensorDef
{
public:
	enum NumType : unsigned char
	{
		BAD_TYPE,
		F32,//float
		F64,//double
		S8,//signed char
		U8,//unsigned char
		S16,//signed short
		U16,//unsigned short
		S32,//signed long
		U32,//unsigned long
		S64,//signed long long
		U64,//unsigned long long
		TEXT//char* UTF-8
	};

	enum ValPackType : unsigned char
	{
		SINGLE,
		PACKET
	};

	enum TimestampType : unsigned char
	{
		NO_TIME,
		LOCAL_TIME,
		GLOBAL_TIME
	};

	class Type
	{
	public:
		Type()=default;
		Type(NumType nt,ValPackType pt,TimestampType tt,quint32 d);

	public:
		bool operator==(const Type &t)const;
		bool operator!=(const Type &t)const;
		bool isValid()const;
		QByteArray toString()const;
		bool fromString(const QByteArray &str);
		bool hasFixedSize()const;
		quint32 fixedSizeForBinaryValuePackaging()const;
		quint32 valueSizeInBytes()const;

	public:
		NumType numType=BAD_TYPE;
		ValPackType packType=SINGLE;
		TimestampType tsType=NO_TIME;
		quint32 dim=1;
	};

public:
	ARpcSensorDef()=default;
	ARpcSensorDef(Type t,const QByteArray &n,const QByteArray &tl=QByteArray(),const QByteArray &u=QByteArray(),
		const QMap<QByteArray,QByteArray> &attrs=QMap<QByteArray,QByteArray>());
	QByteArray nameOrTitle()const;
	static int findByName(const QList<ARpcSensorDef> &sensors,const QByteArray &name);
	bool operator==(const ARpcSensorDef &t)const;

public:
	static bool parseJsonDescription(const QByteArray &data,QList<ARpcSensorDef> &sensors);
	static bool parseXmlDescription(const QByteArray &data,QList<ARpcSensorDef> &sensors);
	static void dumpToJson(QByteArray &data,const QList<ARpcSensorDef> &sensors);
	static void dumpToXml(QByteArray &data,const QList<ARpcSensorDef> &sensors);

public:
	QByteArray name;
	QByteArray title;
	QByteArray unit;
	Type type;
	QMap<QByteArray,QByteArray> attributes;
};

Q_DECLARE_METATYPE(ARpcSensorDef::Type)

#endif // ARPCSENSORDEF_H
