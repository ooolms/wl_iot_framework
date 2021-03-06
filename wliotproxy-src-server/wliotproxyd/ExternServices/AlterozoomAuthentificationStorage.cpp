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

#include "AlterozoomAuthentificationStorage.h"
#include <QDomDocument>
#include <QDomElement>
#include <QFile>
#include <QUrl>
#include <QDebug>

QString AlterozoomAuthentificationStorage::cfgPath;
QString AlterozoomAuthentificationStorage::proxyCfgPath;
QMap<AlterozoomAuthKey,AlterozoomAuthValue>AlterozoomAuthentificationStorage::authData;
QByteArray AlterozoomAuthentificationStorage::defaultHost="alterozoom.com";

bool AlterozoomAuthentificationStorage::readConfig(const QString &path)
{
	QFile file(path);
	if(!file.open(QIODevice::ReadOnly))
	{
		cfgPath=path;
		defaultHost="alterozoom.com";
		authData.clear();
		return true;
	}
	QByteArray data=file.readAll();
	file.close();
	QDomDocument doc;
	if(!doc.setContent(data))return false;
	QDomElement rootElem=doc.firstChildElement("alterozoom-auth");
	if(rootElem.isNull())return false;
	cfgPath=path;
	defaultHost=rootElem.attribute("defaultHost").toUtf8();
	if(defaultHost.isEmpty())
		defaultHost="alterozoom.com";
	authData.clear();
	for(int i=0;i<rootElem.childNodes().count();++i)
	{
		QDomElement elem=rootElem.childNodes().at(i).toElement();
		if(elem.isNull()||elem.nodeName()!="auth")continue;
		bool ok=false;
		QByteArray host=elem.attribute("host").toUtf8();
		QByteArray email=elem.attribute("email").toUtf8();
		quint64 id=elem.attribute("userId").toULongLong(&ok);
		QByteArray token=elem.text().toUtf8();
		if(!ok)continue;
		if(host.isEmpty()||email.isEmpty()||token.isEmpty())continue;
		AlterozoomAuthKey k={host,email};
		AlterozoomAuthValue v={id,token};
		authData[k]=v;
	}
	return true;
}

void AlterozoomAuthentificationStorage::storeConfig()
{
	if(cfgPath.isEmpty())return;
	QDomDocument doc;
	QDomElement rootElem=doc.createElement("alterozoom-auth");
	doc.appendChild(rootElem);
	rootElem.setAttribute("defaultHost",QString::fromUtf8(defaultHost));
	for(auto i=authData.begin();i!=authData.end();++i)
	{
		QDomElement elem=doc.createElement("auth");
		rootElem.appendChild(elem);
		elem.setAttribute("host",QString::fromUtf8(i.key().host));
		elem.setAttribute("email",QString::fromUtf8(i.key().email));
		elem.setAttribute("userId",i.value().userId);
		elem.appendChild(doc.createTextNode(QString::fromUtf8(i.value().token)));
	}
	QFile f(cfgPath);
	if(!f.open(QIODevice::WriteOnly))return;
	f.write(doc.toByteArray());
	f.close();
}

void AlterozoomAuthentificationStorage::clearAuth(const QByteArray &host,const QByteArray &email)
{
	AlterozoomAuthKey key={host,email};
	if(!authData.contains(key))return;
	authData.remove(key);
	storeConfig();
}

void AlterozoomAuthentificationStorage::setAuth(const QByteArray &host,const QByteArray &email,
	quint64 userId,const QByteArray &token)
{
	AlterozoomAuthKey key={host,email};
	AlterozoomAuthValue val={userId,token};
	if(authData.contains(key)&&authData[key]==val)return;
	authData[key]=val;
	storeConfig();
}

const QMap<AlterozoomAuthKey,AlterozoomAuthValue> &AlterozoomAuthentificationStorage::getAuthMap()
{
	return authData;
}

const QByteArray& AlterozoomAuthentificationStorage::getDefaultHost()
{
	return defaultHost;
}

void AlterozoomAuthentificationStorage::setDefaultHost(const QByteArray &h)
{
	if(cfgPath.isEmpty())return;
	defaultHost=hostFromStr(h);
	storeConfig();
}

QByteArray AlterozoomAuthentificationStorage::hostFromStr(QByteArray s)
{
	if(s.startsWith("http://"))
		return QByteArray();
	if(!s.startsWith("https://"))
		s.prepend("https://");
	QUrl u(QString::fromUtf8(s),QUrl::StrictMode);
	if(u.port()==443)
		u.setPort(-1);
	if(u.port()!=-1)
		return u.host().toUtf8()+":"+QByteArray::number(u.port());
	else return u.host().toUtf8();
}
