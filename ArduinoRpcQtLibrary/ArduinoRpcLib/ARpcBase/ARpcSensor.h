#ifndef ARPCSENSOR_H
#define ARPCSENSOR_H

#include <QVariantMap>

class ARpcSensor
{
public:
	enum Type
	{
		BAD_TYPE,
		INTEGER,
		FLOAT,
		TEXT,
		INTEGER_ARRAY,
		FLOAT_ARRAY
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
