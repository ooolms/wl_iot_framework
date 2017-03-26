#ifndef ARPCSENSOR_H
#define ARPCSENSOR_H

#include <QVariantMap>

//обсудить еще раз

class ARpcSensor
{
public:
	enum Type
	{
		BAD_TYPE,
		SINGLE,
		SINGLE_LT,
		SINGLE_GT,
		TEXT,
		PACKET,
		PACKET_LT,
		PACKET_GT
	};

public:
	static QString typeToString(Type t);
	static Type typeFromString(const QString &s);
	static bool parseJsonDescription(const QString &data,QList<ARpcSensor> &sensors);
	static bool parseXmlDescription(const QString &data,QList<ARpcSensor> &sensors);

public:
	QString name;
	Type type=BAD_TYPE;
	QVariantMap constraints;
};

#endif // ARPCSENSOR_H
