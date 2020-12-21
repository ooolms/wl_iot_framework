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

#include "wliot/devices/SensorDef.h"
#include "wliot/devices/SensorValue.h"
#include "AlterozoomAuthTypes.h"
#include <QObject>
#include <QNetworkAccessManager>
#include <QUuid>
#include <QMap>

class AlterozoomApi
	:public QObject
{
	Q_OBJECT
public:
	explicit AlterozoomApi(QObject *parent=nullptr);
	void setStoredUser(QByteArray host,QByteArray email,QByteArray token);
	void authenticate(QByteArray host,QByteArray email,const QByteArray &password);
	bool isAuthenticated(const QByteArray &host,const QByteArray &email);
	void createSensor(QByteArray host,QByteArray email,QUuid devId,QByteArray devName,
		WLIOT::SensorDef sensor);
	void postMeasurement(QByteArray host,QByteArray email,QUuid devId,
		QByteArray sensorName,const WLIOT::SensorValue *val,QUuid sessionId=QUuid());
	AlterozoomAuthValue authCred(const QByteArray &host,const QByteArray &email);

signals:
	void authenticationComplete(bool ok,const QByteArray &host,const QByteArray &email);
	void sensorCreated(bool ok,const QByteArray &host,const QByteArray &email,
		const QUuid &devId,const QByteArray &sensorName);

private:
	bool loadUserInfo(const QByteArray &host,const QByteArray &token,QByteArray &email,quint64 &userId);
	QNetworkRequest makeRequest(const QUrl &url,const QByteArray &tok);
	void execSync(QNetworkReply *r);
	void makeCreateSensorRequest(const QByteArray &host,const QByteArray &email,
		const QUuid &devId,const WLIOT::SensorDef &sensor);

private:
	QMap<AlterozoomAuthKey,AlterozoomAuthValue> authData;
	QNetworkAccessManager mgr;
};

#endif // ALTEROZOOMAPI_H
