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

#include "ARpcSensorDef.h"
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

static int metaTypeId=qRegisterMetaType<ARpcSensorDef::Type>();

ARpcSensorDef::ARpcSensorDef(ARpcSensorDef::Type t,const QByteArray &n,const QByteArray &tl,
	const QByteArray &u,const QMap<QByteArray,QByteArray> &attrs)
{
	type=t;
	name=n;
	title=tl;
	unit=u;
	attributes=attrs;
}

QByteArray ARpcSensorDef::nameOrTitle()const
{
	if(title.isEmpty())return name;
	return title;
}

bool ARpcSensorDef::parseJsonDescription(const QByteArray &data,QList<ARpcSensorDef> &sensors)
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
		ARpcSensorDef s;
		if(name.isEmpty()||!s.type.fromString(sensorObject[shortStrings?"t":"type"].toString().toUtf8()))
			return false;
		s.name=name;
		if(sensorObject.contains(shortStrings?"tl":"title"))
			s.title=sensorObject[shortStrings?"tl":"title"].toString().toUtf8();
		if(sensorObject.contains(shortStrings?"u":"unit"))
			s.unit=sensorObject[shortStrings?"u":"unit"].toString().toUtf8();
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

bool ARpcSensorDef::parseXmlDescription(const QByteArray &data,QList<ARpcSensorDef> &sensors)
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
		ARpcSensorDef s;
		if(name.isEmpty()||!s.type.fromString(elem.attribute(shortStrings?"t":"type").toUtf8()))
			return false;
		s.name=name;
		if(elem.hasAttribute(shortStrings?"tl":"title"))
			s.title=elem.attribute(shortStrings?"tl":"title").toUtf8();
		if(elem.hasAttribute(shortStrings?"u":"unit"))
			s.unit=elem.attribute(shortStrings?"u":"unit").toUtf8();
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

void ARpcSensorDef::dumpToJson(QByteArray &data,const QList<ARpcSensorDef> &sensors)
{
	QJsonDocument doc;
	QJsonObject rootObj;
	QJsonArray sensorsArr;
	for(const ARpcSensorDef &s:sensors)
	{
		if(s.type.numType==BAD_TYPE||s.name.isEmpty())
			continue;
		QJsonObject sensorObj;
		sensorObj["name"]=QString::fromUtf8(s.name);
		sensorObj["type"]=QString::fromUtf8(s.type.toString());
		if(!s.title.isEmpty())
			sensorObj["title"]=QString::fromUtf8(s.title);
		if(!s.unit.isEmpty())
			sensorObj["unit"]=QString::fromUtf8(s.unit);
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

void ARpcSensorDef::dumpToXml(QByteArray &data,const QList<ARpcSensorDef> &sensors)
{
	QDomDocument doc;
	QDomElement rootElem=doc.createElement("sensors");
	doc.appendChild(rootElem);
	for(const ARpcSensorDef &s:sensors)
	{
		QDomElement elem=doc.createElement("sensor");
		rootElem.appendChild(elem);
		elem.setAttribute("name",QString::fromUtf8(s.name));
		elem.setAttribute("type",QString::fromUtf8(s.type.toString()));
		if(!s.title.isEmpty())
			elem.setAttribute("title",QString::fromUtf8(s.title));
		if(!s.unit.isEmpty())
			elem.setAttribute("unit",QString::fromUtf8(s.unit));
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

int ARpcSensorDef::findByName(const QList<ARpcSensorDef> &sensors,const QByteArray &name)
{
	for(int i=0;i<sensors.count();++i)
		if(sensors[i].name==name)
			return i;
	return -1;
}

bool ARpcSensorDef::operator==(const ARpcSensorDef &t) const
{
	return name==t.name&&title==t.title&&unit==t.unit&&type==t.type&&attributes==t.attributes;
}

ARpcSensorDef::Type::Type(ARpcSensorDef::NumType nt,ARpcSensorDef::ValPackType pt,
	ARpcSensorDef::TimestampType tt,quint32 d)
{
	numType=nt;
	packType=pt;
	tsType=tt;
	dim=d;
}

bool ARpcSensorDef::Type::operator==(const ARpcSensorDef::Type &t)const
{
	return numType==t.numType&&packType==t.packType&&tsType==t.tsType&&dim==t.dim;
}

bool ARpcSensorDef::Type::operator!=(const ARpcSensorDef::Type &t)const
{
	return !(*this==t);
}

bool ARpcSensorDef::Type::isValid()const
{
	return numType!=BAD_TYPE&&dim>0;
}

QByteArray ARpcSensorDef::Type::toString()const
{
	QByteArray s;
	if(!isValid())return s;
	//numType
	if(numType==F32)
		s+="f32";
	else if(numType==F64)
		s+="f64";
	else if(numType==S8)
		s+="s8";
	else if(numType==U8)
		s+="u8";
	else if(numType==S16)
		s+="s16";
	else if(numType==U16)
		s+="u16";
	else if(numType==S32)
		s+="s32";
	else if(numType==U32)
		s+="u32";
	else if(numType==S64)
		s+="s64";
	else if(numType==U64)
		s+="u64";
	else //TEXT
		s+="txt";
	//dim
	if(dim!=1)
		s+="_d"+QByteArray::number(dim);
	//packType
	if(packType==SINGLE)
		s+="_sv";
	else s+="_pv";
	//timestamp
	if(tsType==LOCAL_TIME)
		s+="_lt";
	else if(tsType==GLOBAL_TIME)
		s+="_gt";
	else s+="_nt";
	return s;
}

bool ARpcSensorDef::Type::fromString(const QByteArray &str)
{
	NumType newNumType=BAD_TYPE;
	ValPackType newPackType=SINGLE;
	int newDim=1;
	TimestampType newTsType=NO_TIME;
	QByteArrayList list=str.split('_');
	QRegExp dimRegExp("^d(\\d+)$");
	for(QByteArray &el:list)
	{
		//numType
		if(el=="f32")
			newNumType=F32;
		else if(el=="f64")
			newNumType=F64;
		else if(el=="s8")
			newNumType=S8;
		else if(el=="u8")
			newNumType=U8;
		else if(el=="s16")
			newNumType=S16;
		else if(el=="u16")
			newNumType=U16;
		else if(el=="s32")
			newNumType=S32;
		else if(el=="u32")
			newNumType=U32;
		else if(el=="s64")
			newNumType=S64;
		else if(el=="u64")
			newNumType=U64;
		else if(el=="txt")
			newNumType=TEXT;
		//packType
		else if(el=="sv")
			newPackType=SINGLE;
		else if(el=="pv")
			newPackType=PACKET;
		//tsType
		else if(el=="nt")
			newTsType=NO_TIME;
		else if(el=="lt")
			newTsType=LOCAL_TIME;
		else if(el=="gt")
			newTsType=GLOBAL_TIME;
		//dim
		else if(dimRegExp.exactMatch(el))
		{
			bool ok=false;
			newDim=dimRegExp.cap(1).toUInt(&ok);
			if(!ok)return false;
		}
		else return false;//unknown element
	}
	if(newNumType==BAD_TYPE)return false;
	numType=newNumType;
	dim=newDim;
	packType=newPackType;
	tsType=newTsType;
	return true;
}

bool ARpcSensorDef::Type::hasFixedSize()const
{
	return packType==SINGLE&&numType!=TEXT;
}

quint32 ARpcSensorDef::Type::fixedSizeForBinaryValuePackaging()const
{
	if(!hasFixedSize())return 0;
	return (tsType==NO_TIME?0:sizeof(qint64))+dim*valueSizeInBytes();
}

quint32 ARpcSensorDef::Type::valueSizeInBytes()const
{
	if(numType==S16||numType==U16)return 2;
	else if(numType==S32||numType==U32||numType==F32)
		return 4;
	else if(numType==S64||numType==U64||numType==F64)
		return 8;
	else if(numType==S8||numType==U8)return 1;
	else return 0;
}