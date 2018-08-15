#include "AlterozoomAuthentificationStorage.h"
#include <QDomDocument>
#include <QDomElement>
#include <QFile>
#include <QUrl>

QString AlterozoomAuthentificationStorage::cfgPath;
QMap<AlterozoomAuthentificationStorage::AuthKey,AlterozoomAuthentificationStorage::AuthValue>
	AlterozoomAuthentificationStorage::authData;
QByteArray AlterozoomAuthentificationStorage::defaultHost="alterozoom.com";

bool AlterozoomAuthentificationStorage::readConfig(const QString &path)
{
	QFile file(path);
	if(!file.open(QIODevice::ReadOnly))return false;
	QByteArray data=file.readAll();
	file.close();
	QDomDocument doc;
	if(!doc.setContent(data))return false;
	QDomElement rootElem=doc.firstChildElement("alterozoom-auth");
	if(rootElem.isNull())return false;
	defaultHost=rootElem.attribute("defaultHost").toUtf8();
	if(defaultHost.isEmpty())
		defaultHost="alterozoom.com";
	cfgPath=path;
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
		AuthKey k={host,email};
		AuthValue v={id,token};
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
	AuthKey key={host,email};
	if(!authData.contains(key))return;
	authData.remove(key);
	storeConfig();
}

void AlterozoomAuthentificationStorage::setAuth(const QByteArray &host,const QByteArray &email,
	quint64 userId,const QByteArray &token)
{
	AuthKey key={host,email};
	AuthValue val={userId,token};
	if(authData.contains(key)&&authData[key]==val)return;
	authData[key]=val;
	storeConfig();
}

const QMap<AlterozoomAuthentificationStorage::AuthKey,AlterozoomAuthentificationStorage::AuthValue>&
AlterozoomAuthentificationStorage::getAuthMap()
{
	return authData;
}

void AlterozoomAuthentificationStorage::setDefaultHost(const QByteArray &h)
{
	if(cfgPath.isEmpty())return;
	defaultHost=hostFromStr(h);
	storeConfig();
}

QByteArray AlterozoomAuthentificationStorage::hostFromStr(const QByteArray &s)
{
	QUrl u(QString::fromUtf8(s));
	if(u.port()==443)
		u.setPort(-1);
	if(u.port()!=-1)
		return u.host().toUtf8()+":"+QByteArray::number(u.port());
	else return u.host().toUtf8();
}
