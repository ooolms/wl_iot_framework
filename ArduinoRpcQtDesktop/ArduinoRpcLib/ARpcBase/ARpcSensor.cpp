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

#include "ARpcSensor.h"
#include "ARpcCommonRc.h"
#include "ARpcAllSensorValues.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDomDocument>
#include <QDomElement>
#include <QDomNamedNodeMap>
#include <QXmlSchema>
#include <QXmlSchemaValidator>
#include <QFile>

QByteArray ARpcSensor::typeToString(ARpcSensor::Type t)
{
	if(t==SINGLE)
		return "single";
	else if(t==SINGLE_LT)
		return "single_lt";
	else if(t==SINGLE_GT)
		return "single_gt";
	else if(t==TEXT)
		return "text";
	else if(t==PACKET)
		return "packet";
	else if(t==PACKET_LT)
		return "packet_lt";
	else if(t==PACKET_GT)
		return "packet_gt";
	else
		return QByteArray();
}

ARpcSensor::Type ARpcSensor::typeFromString(const QByteArray &s)
{
	if(s=="single"||s=="s")
		return SINGLE;
	else if(s=="single_lt"||s=="slt")
		return SINGLE_LT;
	else if(s=="single_gt"||s=="sgt")
		return SINGLE_GT;
	else if(s=="text"||s=="t")
		return TEXT;
	else if(s=="packet"||s=="p")
		return PACKET;
	else if(s=="packet_lt"||s=="plt")
		return PACKET_LT;
	else if(s=="packet_gt"||s=="pgt")
		return PACKET_GT;
	else
		return BAD_TYPE;
}

bool ARpcSensor::parseJsonDescription(const QByteArray &data,QList<ARpcSensor> &sensors)
{
	QJsonDocument doc=QJsonDocument::fromJson(data);
	if(!doc.isObject())
		return false;
	bool shortStrings=false;
	if(!doc.object().contains("sensors"))
	{
		if(!doc.object().contains("sl"))
			return false;
		shortStrings=true;
	}
	if(!doc.object()[shortStrings?"sl":"sensors"].isArray())
		return false;
	QJsonArray sensorsArray=doc.object()[shortStrings?"sl":"sensors"].toArray();
	for(int i=0;i<sensorsArray.count();++i)
	{
		if(!sensorsArray[i].isObject())
			return false;
		QJsonObject sensorObject=sensorsArray[i].toObject();
		QByteArray name=sensorObject[shortStrings?"n":"name"].toString().toUtf8();
		ARpcSensor::Type type=ARpcSensor::typeFromString(sensorObject[shortStrings?"t":"type"].toString().toUtf8());
		if(name.isEmpty()||type==ARpcSensor::BAD_TYPE)
			return false;
		ARpcSensor s;
		s.name=name;
		s.type=type;
		if(sensorObject.contains(shortStrings?"a":"attributes"))
		{
			QJsonObject attr=sensorObject[shortStrings?"a":"attributes"].toObject();
			for(auto i=attr.begin();i!=attr.end();++i)
			{
				if(i.value().isDouble())
					s.attributes[i.key().toUtf8()]=QByteArray::number(i.value().toDouble());
				else
					s.attributes[i.key().toUtf8()]=i.value().toString().toUtf8();
			}
		}
		sensors.append(s);
	}
	return true;
}

bool ARpcSensor::parseXmlDescription(const QByteArray &data,QList<ARpcSensor> &sensors)
{
	ARpcCommonRc::initRc();
	//TODO return checks later
//	QXmlSchema schema;
//	QFile file(":/ARpc/sensors.xsd");
//	file.open(QIODevice::ReadOnly);
//	if(schema.load(&file))
//	{
//		file.close();
//		QXmlSchemaValidator validator(schema);
//		if(!validator.validate(data))
//			return false;
//	}
//	else
//		file.close();
	QDomDocument doc;
	if(!doc.setContent(data))
		return false;
	bool shortStrings=false;
	QDomElement rootElem=doc.firstChildElement("sensors");
	if(rootElem.isNull())
	{
		rootElem=doc.firstChildElement("sl");
		if(rootElem.isNull())
			return false;
		shortStrings=true;
	}
	for(int i=0;i<rootElem.childNodes().count();++i)
	{
		QDomElement elem=rootElem.childNodes().at(i).toElement();
		if(elem.isNull())
			return false;
		if(shortStrings)
		{
			if(elem.nodeName()!="s")
				return false;
		}
		else if(elem.nodeName()!="sensor")
			return false;
		QByteArray name=elem.attribute(shortStrings?"n":"name").toUtf8();
		ARpcSensor::Type type=ARpcSensor::typeFromString(elem.attribute(shortStrings?"t":"type").toUtf8());
		if(name.isEmpty()||type==ARpcSensor::BAD_TYPE)
			return false;
		ARpcSensor s;
		s.name=name;
		s.type=type;
		QDomElement attrElem=elem.firstChildElement(shortStrings?"a":"attributes");
		if(!attrElem.isNull())
		{
			QDomNamedNodeMap xmlAttrs=attrElem.attributes();
			for(int i=0;i<xmlAttrs.count();++i)
				s.attributes[xmlAttrs.item(i).nodeName().toUtf8()]=xmlAttrs.item(i).nodeValue().toUtf8();
		}
		sensors.append(s);
	}
	return true;
}

void ARpcSensor::dumpToJson(QByteArray &data,const QList<ARpcSensor> &sensors)
{
	QJsonDocument doc;
	QJsonObject rootObj;
	QJsonArray sensorsArr;
	for(const ARpcSensor &s:sensors)
	{
		if(s.type==ARpcSensor::BAD_TYPE)
			continue;
		QJsonObject sensorObj;
		sensorObj["name"]=QString::fromUtf8(s.name);
		sensorObj["type"]=QString::fromUtf8(ARpcSensor::typeToString(s.type));
		if(!s.attributes.isEmpty())
		{
			QJsonObject attrObj;
			for(auto i=s.attributes.begin();i!=s.attributes.end();++i)
				attrObj[QString::fromUtf8(i.key())]=QString::fromUtf8(i.value());
			sensorObj["attributes"]=attrObj;
		}
		sensorsArr.append(sensorObj);
	}
	rootObj["sensors"]=sensorsArr;
	doc.setObject(rootObj);
	data=doc.toJson();
}

void ARpcSensor::dumpToXml(QByteArray &data,const QList<ARpcSensor> &sensors)
{
	QDomDocument doc;
	QDomElement rootElem=doc.createElement("sensors");
	doc.appendChild(rootElem);
	for(const ARpcSensor &s:sensors)
	{
		QDomElement elem=doc.createElement("sensor");
		rootElem.appendChild(elem);
		elem.setAttribute("name",QString::fromUtf8(s.name));
		elem.setAttribute("type",QString::fromUtf8(ARpcSensor::typeToString(s.type)));
		if(!s.attributes.isEmpty())
		{
			QDomElement aElem=doc.createElement("attributes");
			elem.appendChild(aElem);
			for(auto i=s.attributes.begin();i!=s.attributes.end();++i)
				aElem.setAttribute(QString::fromUtf8(i.key()),QString::fromUtf8(i.value()));
		}
	}
	data=doc.toByteArray(-1);
}

int ARpcSensor::findByName(const QList<ARpcSensor> &sensors,const QByteArray &name)
{
	for(int i=0;i<sensors.count();++i)
		if(sensors[i].name==name)
			return i;
	return -1;
}

ARpcISensorValue* ARpcSensor::makeEmptySensorValue()
{
	quint32 dims=1;
	if(attributes.contains("dims"))
		dims=attributes["dims"].toUInt();
	if(type==SINGLE)
		return new ARpcSingleSensorValue(dims);
	else if(type==SINGLE_GT)
		return new ARpcSingleSensorValue(dims,false);
	else if(type==SINGLE_LT)
		return new ARpcSingleSensorValue(dims,true);
	else if(type==PACKET)
		return new ARpcPacketSensorValue(dims);
	else if(type==PACKET_GT)
		return new ARpcPacketSensorValue(dims,false);
	else if(type==PACKET_LT)
		return new ARpcPacketSensorValue(dims,true);
	else if(type==TEXT)
		return new ARpcTextSensorValue;
	else return 0;
}

bool ARpcSensor::operator==(const ARpcSensor &t)const
{
	return name==t.name&&type==t.type&&attributes==t.attributes;
}
