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

#ifndef ALTEROZOOMAPI_H
#define ALTEROZOOMAPI_H

#include "ARpcBase/ARpcSensorDef.h"
#include "ARpcBase/ARpcSensorValue.h"
#include "AlterozoomProxyFactory.h"
#include <QObject>
#include <QNetworkAccessManager>
#include <QUuid>
#include <QMap>

class AlterozoomApi
	:public QObject
{
	struct AuthKey
	{
		QByteArray host;
		QByteArray email;
		bool operator==(const AuthKey &t)const{return host==t.host&&email==t.email;}
		bool operator<(const AuthKey &t)const{return host<t.host||(host==t.host&&email<t.email);}
	};
	struct AuthValue
	{
		quint64 userId;
		QByteArray token;
		bool operator==(const AuthValue &t)const{return userId==t.userId&&token==t.token;}
	};
	Q_OBJECT
public:
	explicit AlterozoomApi(QObject *parent=nullptr);
	bool setStoredUser(const QByteArray &host,const QByteArray &token,QByteArray &email,quint64 &userId);
	bool authentificate(const QByteArray &host,const QByteArray &email,
		const QByteArray &password,QByteArray &token,quint64 &userId);
	bool isAuthentificated(const QByteArray &host,const QByteArray &email);
	bool createSensor(const QByteArray &host,const QByteArray &email,const QUuid &devId,const QByteArray &devName,
		const ARpcSensorDef &sensor,bool &wasNewCreated);
	bool postMeasurement(const QByteArray &host,const QByteArray &email,const QUuid &devId,
		const QByteArray &sensorName,const ARpcSensorValue *val,const QUuid &sessionId=QUuid());

private:
	bool loadUserInfo(const QByteArray &host,const QByteArray &token,QByteArray &email,quint64 &userId);
	QNetworkRequest makeRequest(const QUrl &url,const QByteArray &tok);
	void execSync(QNetworkReply *r);

private:
	QMap<AuthKey,AuthValue> authData;
	AlteroozomProxyFactory *factory;
	QNetworkAccessManager mgr;
};

#endif // ALTEROZOOMAPI_H
