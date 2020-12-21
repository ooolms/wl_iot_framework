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
#include "AlterozoomAuthTypes.h"

class AlterozoomAuthentificationStorage
{
public:
	static bool readConfig(const QString &path);
	static void storeConfig();
	static void clearAuth(const QByteArray &host,const QByteArray &email);
	static void setAuth(const QByteArray &host,const QByteArray &email,quint64 userId,const QByteArray &token);
	static const QMap<AlterozoomAuthKey,AlterozoomAuthValue>& getAuthMap();
	static const QByteArray& getDefaultHost();
	static void setDefaultHost(const QByteArray &h);
	static QByteArray hostFromStr(QByteArray s);

private:
	static QString cfgPath,proxyCfgPath;
	static QMap<AlterozoomAuthKey,AlterozoomAuthValue> authData;
	static QByteArray defaultHost;
};

#endif // ALTEROZOOMAUTHENTIFICATIONSTORAGE_H
