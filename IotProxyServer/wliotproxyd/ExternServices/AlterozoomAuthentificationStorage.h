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

#ifndef ALTEROZOOMAUTHENTIFICATIONSTORAGE_H
#define ALTEROZOOMAUTHENTIFICATIONSTORAGE_H

#include <QString>
#include <QMap>
#include <QNetworkProxy>

class AlterozoomAuthentificationStorage
{
public:
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

public:
	static bool readConfig(const QString &path);
	static bool readProxies(const QString &path);
	static void storeConfig();
	static void clearAuth(const QByteArray &host,const QByteArray &email);
	static void setAuth(const QByteArray &host,const QByteArray &email,quint64 userId,const QByteArray &token);
	static const QMap<AuthKey,AuthValue>& getAuthMap();
	static const QByteArray& getDefaultHost();
	static void setDefaultHost(const QByteArray &h);
	static QByteArray hostFromStr(QByteArray s);

public:
	static QMap<QString,QNetworkProxy> proxyMap;
	static QNetworkProxy defaultProxy;

private:
	static QString cfgPath,proxyCfgPath;
	static QMap<AuthKey,AuthValue> authData;
	static QByteArray defaultHost;
};

#endif // ALTEROZOOMAUTHENTIFICATIONSTORAGE_H
