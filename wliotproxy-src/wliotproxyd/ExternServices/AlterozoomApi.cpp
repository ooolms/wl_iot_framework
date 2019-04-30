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

#include "AlterozoomApi.h"
#include "AlterozoomAuthentificationStorage.h"
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrlQuery>
#include <QEventLoop>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QDomDocument>
#include <QDomElement>
#include "AlterozoomNoCookieJar.h"
#include <QDebug>

static const QByteArray AZ_CLIENT_HEADER="X-AzIotServer";
static const QByteArray AZ_CLIENT_VERSION="1.0";

AlterozoomApi::AlterozoomApi(QObject *parent)
	:QObject(parent)
{
	mgr.setCookieJar(new AlterozoomNoCookieJar(&mgr));
}

void AlterozoomApi::setStoredUser(QByteArray host,QByteArray email,QByteArray token)
{
	QNetworkRequest rq=makeRequest(QUrl("https://"+host+"/iot/user_info"),token);
	QNetworkReply *reply=mgr.get(rq);
	connect(this,&AlterozoomApi::destroyed,reply,&QNetworkReply::abort);
	connect(reply,&QNetworkReply::finished,[this,reply,host,email,token]()
	{
		quint64 userId=0;
		if(reply->error()!=QNetworkReply::NoError)
		{
			qDebug()<<"Alterozoom authentication failed: "<<reply->errorString();
			emit authenticationComplete(false,host,email);
			return;
		}
		QByteArray data=reply->readAll();
		QJsonDocument doc=QJsonDocument::fromJson(data);
		if(!doc.isObject())
		{
			qDebug()<<"Alterozoom authentication failed: bad answer: "<<data;
			emit authenticationComplete(false,host,email);
			return;
		}
		QJsonObject userObj=doc.object();
		if(!userObj.contains("email")||!userObj.contains("id"))
		{
			emit authenticationComplete(false,host,email);
			return;
		}
		QJsonValue idElem=userObj["id"];
		QJsonValue emailElem=userObj["email"];
		if(!idElem.isDouble()||!emailElem.isString()||emailElem.toString().toUtf8()!=email)
		{
			emit authenticationComplete(false,host,email);
			return;
		}
		userId=idElem.toDouble();
		AlterozoomAuthKey k={host,email};
		AlterozoomAuthValue v={userId,token};
		authData[k]=v;
		qDebug()<<"Alterozoom authentication complete: "<<userId<<":"<<host<<":"<<email;
		emit authenticationComplete(true,host,email);
	});
}

void AlterozoomApi::authenticate(QByteArray host,QByteArray email,const QByteArray &password)
{
	QNetworkRequest rq=makeRequest(QUrl("https://"+host+"/users/sign_in/token"),"");
	QJsonDocument doc;
	QJsonObject rqObj;
	QJsonObject paramsObj;
	paramsObj["email"]=QString::fromUtf8(email);
	paramsObj["password"]=QString::fromUtf8(password);
	rqObj["user"]=paramsObj;
	rqObj["scope"]="iot";
//	rqObj["clientId"]="";
	doc.setObject(rqObj);
	QByteArray data=doc.toJson(QJsonDocument::Compact);
	rq.setHeader(QNetworkRequest::ContentTypeHeader,"application/json");
	QNetworkReply *reply=mgr.post(rq,data);
	connect(this,&AlterozoomApi::destroyed,reply,&QNetworkReply::abort);
	connect(reply,&QNetworkReply::finished,[this,reply,host,email]()
	{
		reply->deleteLater();
		if(reply->error()!=QNetworkReply::NoError)
		{
			emit authenticationComplete(false,host,email);
			return;
		}
		QByteArray data=reply->readAll();
		QJsonDocument doc=QJsonDocument::fromJson(data);
		if(!doc.isObject())
		{
			emit authenticationComplete(false,host,email);
			return;
		}
		QJsonObject paramsObj=doc.object();
		if(!paramsObj.contains("user_id")||!paramsObj.contains("email")||
			!paramsObj.contains("access_token"))
		{
			emit authenticationComplete(false,host,email);
			return;
		}
		QByteArray tempLogin=paramsObj["email"].toString().toUtf8();
		if(tempLogin!=email)
		{
			emit authenticationComplete(false,host,email);
			return;
		}
		quint64 userId=(quint64)paramsObj["user_id"].toDouble();
		if(userId==0)
		{
			emit authenticationComplete(false,host,email);
			return;
		}
		QByteArray token=paramsObj["access_token"].toString().toUtf8();
		AlterozoomAuthKey k={host,email};
		AlterozoomAuthValue v={userId,token};
		authData[k]=v;
		qDebug()<<"Alterozoom authentication complete: "<<userId<<":"<<host<<":"<<email;
		emit authenticationComplete(true,host,email);
	});
}

bool AlterozoomApi::isAuthenticated(const QByteArray &host,const QByteArray &email)
{
	return authData.contains({host,email});
}

void AlterozoomApi::createSensor(QByteArray host,QByteArray email,QUuid devId,
	QByteArray devName,SensorDef sensor)
{
	AlterozoomAuthKey k={host,email};
	AlterozoomAuthValue v;
	{
		if(!authData.contains(k))
		{
			qDebug()<<"Alterozoom sensor setup: no user";
			emit sensorCreated(false,host,email,devId,sensor.name);
			return;
		}
		v=authData[k];
	}
	QByteArray srvDevId=QByteArray::number(v.userId)+devId.toString().toUtf8();
	QNetworkRequest rq=makeRequest(QUrl("https://"+host+"/iot/devices/"+srvDevId),v.token);
	QNetworkReply *reply=mgr.get(rq);
	connect(this,&AlterozoomApi::destroyed,reply,&QNetworkReply::abort);
	connect(reply,&QNetworkReply::finished,[this,reply,host,email,devId,devName,sensor,v]()
	{
		QByteArray srvDevId=QByteArray::number(v.userId)+devId.toString().toUtf8();
		QString typeStr=(sensor.type.numType==SensorDef::TEXT)?"Text":"Float";
		QString formatStr=sensor.type.packType==SensorDef::PACKET?"Packet":"Single";
		QString tsRuleStr=sensor.type.tsType==SensorDef::GLOBAL_TIME?"Global":
			(sensor.type.tsType==SensorDef::LOCAL_TIME?"Local":"No");
		reply->deleteLater();
		qDebug()<<"Alterozoom devices list loaded";
		if(reply->error()!=QNetworkReply::NoError)
		{
			if(reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt()!=404)
			{
				emit sensorCreated(false,host,email,devId,sensor.name);
				return;
			}
			QNetworkRequest rq=makeRequest(QUrl("https://"+host+"/iot/devices"),v.token);
			rq.setHeader(QNetworkRequest::ContentTypeHeader,"application/json");
			QJsonDocument doc;
			QJsonObject docObj;
			docObj["id"]=QString::fromUtf8(srvDevId);
			docObj["name"]=QString::fromUtf8(devName);
			docObj["components"]=QJsonArray();
			doc.setObject(docObj);
			QNetworkReply *reply=mgr.post(rq,doc.toJson());
			connect(this,&AlterozoomApi::destroyed,reply,&QNetworkReply::abort);
			connect(reply,&QNetworkReply::finished,[this,reply,host,email,devId,devName,sensor]()
			{
				reply->deleteLater();
				if(reply->error()==QNetworkReply::NoError)
					makeCreateSensorRequest(host,email,devId,sensor);
			});
		}
		else
		{
			QJsonDocument doc=QJsonDocument::fromJson(reply->readAll());
			if(!doc.isObject())
			{
				emit sensorCreated(false,host,email,devId,sensor.name);
				return;
			}
			QJsonObject docObj=doc.object();
			if(docObj["id"].toString().toUtf8()!=srvDevId)
			{
				emit sensorCreated(false,host,email,devId,sensor.name);
				return;
			}
			QJsonArray compList=docObj["components"].toArray();
			QJsonObject compObj;
			bool found=false;
			for(int i=0;i<compList.count();++i)
			{
				QJsonObject o=compList[i].toObject();
				if(o["id"].toString().toUtf8()==sensor.name)
				{
					compObj=o;
					found=true;
					break;
				}
			}
			if(!found)
			{
				qDebug()<<"Alterozoom sensor not found, creating";
				makeCreateSensorRequest(host,email,devId,sensor);
				return;
			}
			if(compObj["format"].toString()!=formatStr||compObj["timestampType"].toString()!=tsRuleStr)
			{
				qDebug()<<"Alterozoom sensor setup: wrong format";
				emit sensorCreated(false,host,email,devId,sensor.name);
				return;
			}
			quint32 fieldsCount=sensor.type.dim+(sensor.type.tsType==SensorDef::LOCAL_TIME?1:0);
			QJsonArray fieldsArr=compObj["fields"].toArray();
			if((quint32)fieldsArr.count()!=fieldsCount)
			{
				qDebug()<<"Alterozoom sensor setup: wrong format";
				emit sensorCreated(false,host,email,devId,sensor.name);
				return;
			}
			for(quint32 i=0;i<fieldsCount;++i)
			{
				QJsonObject fieldObj=fieldsArr.at(i).toObject();
				if(fieldObj["name"].toString()=="lt")
				{
					if(fieldObj["type"].toString()!="Float"||sensor.type.tsType!=SensorDef::LOCAL_TIME)
					{
						qDebug()<<"Alterozoom sensor setup: wrong format";
						emit sensorCreated(false,host,email,devId,sensor.name);
						return;
					}
				}
				else
				{
					bool ok=false;
					quint32 num=fieldObj["name"].toString().toUtf8().toULong(&ok);
					if(!ok||num>=sensor.type.dim)
					{
						qDebug()<<"Alterozoom sensor setup: wrong format";
						emit sensorCreated(false,host,email,devId,sensor.name);
						return;
					}
				}
				if(fieldObj["type"].toString()!=typeStr)
				{
					qDebug()<<"Alterozoom sensor setup: wrong format";
					emit sensorCreated(false,host,email,devId,sensor.name);
					return;
				}
				qDebug()<<"Alterozoom sensor setup: sensor found";
				emit sensorCreated(true,host,email,devId,sensor.name);
			}
		}
	});
}

void AlterozoomApi::postMeasurement(QByteArray host,QByteArray email,QUuid devId,
	QByteArray sensorName,const SensorValue *val,QUuid sessionId)
{
	AlterozoomAuthKey k={host,email};
	if(!authData.contains(k))return;
	AlterozoomAuthValue v=authData[k];
	QByteArray srvDevId=QByteArray::number(v.userId)+devId.toString().toUtf8();
	QJsonDocument doc;
	QJsonArray valList;
	if(val->type().tsType==SensorDef::LOCAL_TIME)
		valList.append(val->time());
	if(val->type().numType==SensorDef::TEXT)
	{
		const SensorValueText *tVal=(const SensorValueText*)val;
		for(quint32 i=0;i<tVal->packetsCount();++i)
			for(quint32 j=0;j<tVal->type().dim;++j)
				valList.append(QString::fromUtf8(tVal->get(j,i)));
	}
	else
	{
		for(quint32 i=0;i<val->packetsCount();++i)
			for(quint32 j=0;j<val->type().dim;++j)
				valList.append(val->valueToDouble(j,i));
	}
	QJsonObject measObj;
	if(val->type().tsType==SensorDef::GLOBAL_TIME)
		measObj["timestamp"]=val->time();
	if(!sessionId.isNull())
		measObj["sessionId"]=sessionId.toString();
	measObj["values"]=valList;
	QJsonArray measArr;
	measArr.append(measObj);
	QJsonObject compObj;
	compObj[sensorName]=measArr;
	QJsonObject devObj;
	devObj[srvDevId]=compObj;
	doc.setObject(devObj);
	QNetworkRequest rq=makeRequest(QUrl("https://"+host+"/iot/measurements"),v.token);
	rq.setHeader(QNetworkRequest::ContentTypeHeader,"application/json");
	QNetworkReply *reply=mgr.post(rq,doc.toJson());
	connect(this,&AlterozoomApi::destroyed,reply,&QNetworkReply::abort);
	connect(reply,&QNetworkReply::finished,reply,&QNetworkReply::deleteLater);
}

AlterozoomAuthValue AlterozoomApi::authCred(const QByteArray &host,const QByteArray &email)
{
	AlterozoomAuthKey k={host,email};
	if(!authData.contains(k))
		return AlterozoomAuthValue();
	return authData[k];
}

QNetworkRequest AlterozoomApi::makeRequest(const QUrl &url,const QByteArray &tok)
{
	QNetworkRequest r(url);
	r.setRawHeader(AZ_CLIENT_HEADER,AZ_CLIENT_VERSION);
	r.setRawHeader("Authorization","Bearer "+tok);
	return r;
}

void AlterozoomApi::execSync(QNetworkReply *r)
{
	QEventLoop loop;
	connect(r,&QNetworkReply::finished,&loop,&QEventLoop::quit);
	loop.exec(QEventLoop::ExcludeUserInputEvents);
	r->deleteLater();
}

void AlterozoomApi::makeCreateSensorRequest(const QByteArray &host,const QByteArray &email,
	const QUuid &devId,const SensorDef &sensor)
{
	AlterozoomAuthKey k={host,email};
	if(!authData.contains(k))return;
	AlterozoomAuthValue v=authData[k];
	QByteArray srvDevId=QByteArray::number(v.userId)+devId.toString().toUtf8();
	QString typeStr=(sensor.type.numType==SensorDef::TEXT)?"Text":"Float";
	QString formatStr=sensor.type.packType==SensorDef::PACKET?"Packet":"Single";
	QString tsRuleStr=sensor.type.tsType==SensorDef::GLOBAL_TIME?"Global":
		(sensor.type.tsType==SensorDef::LOCAL_TIME?"Local":"No");
	QJsonDocument doc;
	QJsonObject docObj;
	docObj["id"]=QString::fromUtf8(sensor.name);
	docObj["name"]=QString::fromUtf8(sensor.title);
	docObj["format"]=formatStr;
	docObj["timestampType"]=tsRuleStr;
	docObj["type"]=typeStr;
	if(!sensor.unit.isEmpty())
		docObj["unit"]=QString::fromUtf8(sensor.unit);
	QJsonArray fieldsArr;
	if(sensor.type.tsType==SensorDef::LOCAL_TIME)
	{
		QJsonObject fieldObj;
		fieldObj["name"]="lt";
		fieldObj["type"]="Float";
		fieldObj["unit"]="";
		fieldsArr.append(fieldObj);
	}
	for(quint32 i=0;i<sensor.type.dim;++i)
	{
		QJsonObject fieldObj;
		fieldObj["name"]=QString::fromUtf8(QByteArray::number(i));
		fieldObj["type"]=typeStr;
		fieldObj["unit"]="";
		fieldsArr.append(fieldObj);
	}
	docObj["fields"]=fieldsArr;
	doc.setObject(docObj);
	QNetworkRequest rq=makeRequest(QUrl("https://"+host+"/iot/components/"+srvDevId),v.token);
	rq.setHeader(QNetworkRequest::ContentTypeHeader,"application/json");
	QNetworkReply *reply=mgr.post(rq,doc.toJson());
	connect(this,&AlterozoomApi::destroyed,reply,&QNetworkReply::abort);
	connect(reply,&QNetworkReply::finished,reply,&QNetworkReply::deleteLater);
}
