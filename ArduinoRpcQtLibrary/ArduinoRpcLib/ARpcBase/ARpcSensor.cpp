#include "ARpcSensor.h"
#include "ARpcCommonRc.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDomDocument>
#include <QDomElement>
#include <QDomNamedNodeMap>
#include <QXmlSchema>
#include <QXmlSchemaValidator>
#include <QFile>

QString ARpcSensor::typeToString(ARpcSensor::Type t)
{
	if(t==SINGLE)return "single";
	else if(t==SINGLE_LT)return "single_lt";
	else if(t==SINGLE_GT)return "single_gt";
	else if(t==TEXT)return "text";
	else if(t==PACKET)return "packet";
	else if(t==PACKET_LT)return "packet_lt";
	else if(t==PACKET_GT)return "packet_gt";
	else return QString();
}

ARpcSensor::Type ARpcSensor::typeFromString(const QString &s)
{
	if(s=="single")return SINGLE;
	else if(s=="single_lt")return SINGLE_LT;
	else if(s=="single_gt")return SINGLE_GT;
	else if(s=="text")return TEXT;
	else if(s=="packet")return PACKET;
	else if(s=="packet_lt")return PACKET_LT;
	else if(s=="packet_gt")return PACKET_GT;
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
	ARpcCommonRc::initRc();
	QXmlSchema schema;
	QFile file(":/ARpc/sensors.xsd");
	file.open(QIODevice::ReadOnly);
	if(schema.load(&file))
	{
		file.close();
		QXmlSchemaValidator validator(schema);
		if(!validator.validate(data.toUtf8()))
			return false;
	}
	else file.close();
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
