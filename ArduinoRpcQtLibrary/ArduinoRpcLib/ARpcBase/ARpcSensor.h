#ifndef ARPCSENSOR_H
#define ARPCSENSOR_H

#include <QVariantMap>

//обсудить еще раз

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
	inline bool isSingle(){return type&singleValueFlag;}
	inline bool isPacket(){return type&packetValueFlag;}
	inline bool isText(){return type&textValueFlag;}
	inline bool isNTValue(){return !(type&0x30);}//no time
	inline bool isLTValue(){return type&ltFlag;}
	inline bool isGTValue(){return type&gtFlag;}
	static inline bool isSingle(Type type){return type&singleValueFlag;}
	static inline bool isPacket(Type type){return type&packetValueFlag;}
	static inline bool isText(Type type){return type&textValueFlag;}
	static inline bool isNTValue(Type type){return !(type&0x30);}//no time
	static inline bool isLTValue(Type type){return type&ltFlag;}
	static inline bool isGTValue(Type type){return type&gtFlag;}

public:
	QString name;
	Type type=BAD_TYPE;
	QVariantMap constraints;
};

#endif // ARPCSENSOR_H
