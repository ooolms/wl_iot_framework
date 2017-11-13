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

#ifndef ARPCSENSOR_H
#define ARPCSENSOR_H

#include <QVariantMap>

//обсудить еще раз
class ARpcISensorValue;

class ARpcSensor
{
public:
	enum Type
	{
		BAD_TYPE=0x0,
		SINGLE=0x1,
		SINGLE_LT=0x11,
		SINGLE_GT=0x21,
		PACKET=0x2,
		PACKET_LT=0x12,
		PACKET_GT=0x22,
		TEXT=0x24
	};
	static const int singleValueFlag=0x1;
	static const int packetValueFlag=0x2;
	static const int textValueFlag=0x4;
	static const int ltFlag=0x10;
	static const int gtFlag=0x20;

public:
	static QString typeToString(Type t);
	static Type typeFromString(const QString &s);
	static bool parseJsonDescription(const QString &data,QList<ARpcSensor> &sensors);
	static bool parseXmlDescription(const QString &data,QList<ARpcSensor> &sensors);
	static void dumpToJson(QString &data,const QList<ARpcSensor> &sensors);
	static void dumpToXml(QString &data,const QList<ARpcSensor> &sensors);
	static int findByName(const QList<ARpcSensor> &sensors,const QString &name);
	inline bool isSingle()const{return type&singleValueFlag;}
	inline bool isPacket()const{return type&packetValueFlag;}
	inline bool isText()const{return type&textValueFlag;}
	inline bool isNTValue()const{return !(type&0x30);}//no time
	inline bool isLTValue()const{return type&ltFlag;}
	inline bool isGTValue()const{return type&gtFlag;}
	static inline bool isSingle(Type type){return type&singleValueFlag;}
	static inline bool isPacket(Type type){return type&packetValueFlag;}
	static inline bool isText(Type type){return type&textValueFlag;}
	static inline bool isNTValue(Type type){return !(type&0x30);}//no time
	static inline bool isLTValue(Type type){return type&ltFlag;}
	static inline bool isGTValue(Type type){return type&gtFlag;}
	ARpcISensorValue* makeEmptySensorValue();
	bool operator==(const ARpcSensor &t)const;

public:
	QString name;
	Type type;
	QMap<QString,QString> constraints;
};

#endif // ARPCSENSOR_H
