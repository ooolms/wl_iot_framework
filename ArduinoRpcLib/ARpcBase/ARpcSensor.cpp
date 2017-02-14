#include "ARpcSensor.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDomDocument>
#include <QDomElement>
#include <QDomNamedNodeMap>

QString ARpcSensor::typeToString(ARpcSensor::Type t)
{
	if(t==INTEGER)return "integer";
	else if(t==FLOAT)return "float";
	else if(t==TEXT)return "text";
	else if(t==INTEGER_ARRAY)return "integer_array";
	else if(t==FLOAT_ARRAY)return "float_array";
	else return QString();
}

ARpcSensor::Type ARpcSensor::typeFromString(const QString &s)
{
	if(s=="integer")return INTEGER;
	else if(s=="float")return FLOAT;
	else if(s=="text")return TEXT;
	else if(s=="integer_array")return INTEGER_ARRAY;
	else if(s=="float_array")return FLOAT_ARRAY;
	else return BAD_TYPE;
}

bool ARpcSensor::parseJsonDescription(const QString &data,QList<ARpcSensor> &sensors)
{
	QJsonDocument doc=QJsonDocument::fromJson(data.toUtf8());
	if(!doc.isObject())return false;
	if(!doc.object().contains("sensors"))return false;
	if(!doc.object()["sensors"].isArray())return false;
	QJsonArray sensorsArray=doc.object()["sensors"].toArray();
	for(int i=0;i<sensorsArray.count();++i)
	{
		if(!sensorsArray[i].isObject())return false;
		QJsonObject sensorObject=sensorsArray[i].toObject();
		QString name=sensorObject["name"].toString();
		ARpcSensor::Type type=ARpcSensor::typeFromString(sensorObject["type"].toString());
		if(name.isEmpty()||type==ARpcSensor::BAD_TYPE)return false;
		ARpcSensor s;
		s.name=name;
		s.type=type;
		if(sensorObject.contains("constraints"))
		{
			QJsonObject constr=sensorObject["constraints"].toObject();
			for(auto i=constr.begin();i!=constr.end();++i)
			{
				if(i.value().isDouble())
					s.constraints[i.key()]=QString::fromUtf8(QByteArray::number(i.value().toDouble()));
				else s.constraints[i.key()]=i.value().toString();
			}
		}
		sensors.append(s);
	}
	return true;
}

bool ARpcSensor::parseXmlDescription(const QString &data,QList<ARpcSensor> &sensors)
{
	QDomDocument doc;
	if(!doc.setContent(data))return false;
	QDomElement rootElem=doc.firstChildElement("sensors");
	if(rootElem.isNull())return false;
	for(int i=0;i<rootElem.childNodes().count();++i)
	{
		QDomElement elem=rootElem.childNodes().at(i).toElement();
		if(elem.isNull()||elem.nodeName()!="sensor")return false;
		QString name=elem.attribute("name");
		ARpcSensor::Type type=ARpcSensor::typeFromString(elem.attribute("type"));
		if(name.isEmpty()||type==ARpcSensor::BAD_TYPE)return false;
		ARpcSensor s;
		s.name=name;
		s.type=type;
		QDomElement constrElem=elem.firstChildElement("constraints");
		if(!constrElem.isNull())
		{
			QDomNamedNodeMap attrs=constrElem.attributes();
			for(int i=0;i<attrs.count();++i)
				s.constraints[attrs.item(i).nodeName()]=attrs.item(i).nodeValue();
		}
		sensors.append(s);
	}
	return true;
}
