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

static const QByteArray AZ_CLIENT_HEADER="X-AzIotServer";
static const QByteArray AZ_CLIENT_VERSION="1.0";

AlterozoomApi::AlterozoomApi(QObject *parent)
	:QObject(parent)
{
	mgr.setCookieJar(new AlterozoomNoCookieJar(&mgr));
}

bool AlterozoomApi::setStoredUser(const QByteArray &host,const QByteArray &tok,QByteArray &email,quint64 &userId)
{
	return loadUserInfo(host,tok,email,userId);
}

bool AlterozoomApi::authentificate(const QByteArray &host,const QByteArray &email,
	const QByteArray &password,QByteArray &token,quint64 &userId)
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
	execSync(reply);
	if(reply->error()!=QNetworkReply::NoError)
		return false;
	data=reply->readAll();
	doc=QJsonDocument::fromJson(data);
	if(!doc.isObject())
		return false;
	paramsObj=doc.object();
	if(!paramsObj.contains("user_id")||!paramsObj.contains("email")||
		!paramsObj.contains("access_token"))
		return false;
	QByteArray tempLogin=paramsObj["email"].toString().toUtf8();
	if(tempLogin!=email)
		return false;
	userId=(quint64)paramsObj["user_id"].toDouble();
	if(userId==0)return false;
	token=paramsObj["access_token"].toString().toUtf8();
	AuthKey k={host,email};
	AuthValue v={userId,token};
	authData[k]=v;
	return true;
}

bool AlterozoomApi::isAuthentificated(const QByteArray &host,const QByteArray &email)
{
	return authData.contains({host,email});
}

bool AlterozoomApi::createSensor(const QByteArray &host,const QByteArray &email,const QUuid &devId,
	const QByteArray &devName,const ARpcSensorDef &sensor,bool &wasNewCreated)
{
	AuthKey k={host,email};
	if(!authData.contains(k))return false;
	AuthValue &v=authData[k];
	QByteArray srvDevId=QByteArray::number(v.userId)+devId.toString().toUtf8();
	QNetworkRequest rq=makeRequest(QUrl("https://"+host+"/iot/devices/"+srvDevId),v.token);
	QNetworkReply *reply=mgr.get(rq);
	execSync(reply);
	QString typeStr=(sensor.type.numType==ARpcSensorDef::TEXT)?"Text":"Float";
	QString formatStr=sensor.type.packType==ARpcSensorDef::PACKET?"Packet":"Single";
	QString tsRuleStr=sensor.type.tsType==ARpcSensorDef::GLOBAL_TIME?"Global":
		(sensor.type.tsType==ARpcSensorDef::LOCAL_TIME?"Local":"No");
	if(reply->error()!=QNetworkReply::NoError)
	{
		if(reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt()!=404)
			return false;
		rq=makeRequest(QUrl("https://"+host+"/iot/devices"),v.token);
		rq.setHeader(QNetworkRequest::ContentTypeHeader,"application/json");
		QJsonDocument doc;
		QJsonObject docObj;
		docObj["id"]=QString::fromUtf8(srvDevId);
		docObj["name"]=QString::fromUtf8(devName);
		docObj["components"]=QJsonArray();
		doc.setObject(docObj);
		reply=mgr.post(rq,doc.toJson());
		execSync(reply);
	}
	else
	{
		QJsonDocument doc=QJsonDocument::fromJson(reply->readAll());
		if(!doc.isObject())return false;
		QJsonObject docObj=doc.object();
		if(docObj["id"].toString().toUtf8()!=srvDevId)return false;
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
		if(found)
		{
			if(compObj["format"].toString()!=formatStr||compObj["timestampType"].toString()!=tsRuleStr)
				return false;
			quint32 fieldsCount=sensor.type.dim+(sensor.type.tsType==ARpcSensorDef::LOCAL_TIME?1:0);
			QJsonArray fieldsArr=compObj["fields"].toArray();
			if((quint32)fieldsArr.count()!=fieldsCount)return false;
			for(quint32 i=0;i<fieldsCount;++i)
			{
				QJsonObject fieldObj=fieldsArr.at(i).toObject();
				if(fieldObj["name"].toString()=="lt")
				{
					if(fieldObj["type"].toString()!="Float"||sensor.type.tsType!=ARpcSensorDef::LOCAL_TIME)
						return false;
				}
				else
				{
					bool ok=false;
					quint32 num=fieldObj["name"].toString().toUtf8().toULong(&ok);
					if(!ok||num>=sensor.type.dim)return false;
				}
				if(fieldObj["type"].toString()!=typeStr)return false;
			}
			wasNewCreated=false;
			return true;
		}
	}
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
	if(sensor.type.tsType==ARpcSensorDef::LOCAL_TIME)
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
	rq=makeRequest(QUrl("https://"+host+"/iot/components/"+srvDevId),v.token);
	rq.setHeader(QNetworkRequest::ContentTypeHeader,"application/json");
	reply=mgr.post(rq,doc.toJson());
	execSync(reply);
	if(reply->error()==QNetworkReply::NoError)
	{
		wasNewCreated=true;
		return true;
	}
	else qDebug()<<reply->readAll();
	return false;
}

bool AlterozoomApi::postMeasurement(const QByteArray &host,const QByteArray &email,const QUuid &devId,
	const QByteArray &sensorName,const ARpcSensorValue *val,const QUuid &sessionId)
{
	AuthKey k={host,email};
	if(!authData.contains(k))return false;
	AuthValue &v=authData[k];
	QByteArray token=v.token;
	QByteArray srvDevId=QByteArray::number(v.userId)+devId.toString().toUtf8();
	QJsonDocument doc;
	QJsonArray valList;
	if(val->type().tsType==ARpcSensorDef::LOCAL_TIME)
		valList.append(val->time());
	if(val->type().numType==ARpcSensorDef::TEXT)
	{
		const ARpcSensorValueText *tVal=(const ARpcSensorValueText*)val;
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
	if(val->type().tsType==ARpcSensorDef::GLOBAL_TIME)
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
	QNetworkRequest rq=makeRequest(QUrl("https://"+host+"/iot/measurements"),token);
	rq.setHeader(QNetworkRequest::ContentTypeHeader,"application/json");
	QNetworkReply *reply=mgr.post(rq,doc.toJson());
	connect(reply,&QNetworkReply::finished,reply,&QNetworkReply::deleteLater);
	return true;
	/*execSync(reply);
	if(reply->error()==QNetworkReply::NoError)return true;
	qDebug()<<reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt()<<": "<<reply->readAll();
	return false;*/
}

bool AlterozoomApi::loadUserInfo(const QByteArray &host,const QByteArray &token,QByteArray &email,quint64 &userId)
{
	QNetworkRequest rq=makeRequest(QUrl("https://"+host+"/iot/user_info"),token);
	QNetworkReply *reply=mgr.get(rq);
	execSync(reply);
	if(reply->error()!=QNetworkReply::NoError)
		return false;
	QByteArray data=reply->readAll();
	QJsonDocument doc=QJsonDocument::fromJson(data);
	if(!doc.isObject())return false;
	QJsonObject userObj=doc.object();
	if(!userObj.contains("email")||!userObj.contains("id"))return false;
	QJsonValue idElem=userObj["id"];
	QJsonValue emailElem=userObj["email"];
	if(!idElem.isDouble()||!emailElem.isString())
		return false;
	email=emailElem.toString().toUtf8();
	userId=idElem.toDouble();
	AuthKey k={host,email};
	AuthValue v={userId,token};
	authData[k]=v;
	return true;
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
