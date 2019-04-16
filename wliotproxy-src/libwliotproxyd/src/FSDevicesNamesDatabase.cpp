#include "wliot/FSDevicesNamesDatabase.h"
#include <QDomDocument>
#include <QDomElement>
#include <QVariant>
#include <QFile>

FSDevicesNamesDatabase::FSDevicesNamesDatabase(QObject *parent)
	:IDevicesNamesDatabase(parent)
{
}

FSDevicesNamesDatabase::~FSDevicesNamesDatabase()
{
}

bool FSDevicesNamesDatabase::initDb(const QString &path)
{
	dbPath=path;
	autoDevNames.clear();
	manualDevNames.clear();
	reverseMap.clear();
	QFile file(dbPath);
	if(!file.open(QIODevice::ReadOnly))
		return true;
	QDomDocument doc;
	if(!doc.setContent(&file))
	{
		file.close();
		return false;
	}
	file.close();
	QDomElement rootElem=doc.firstChildElement("devnames").toElement();
	if(rootElem.isNull())
		return false;
	QDomElement autoElem=rootElem.firstChildElement("auto").toElement();
	QDomElement manualElem=rootElem.firstChildElement("manual").toElement();
	if(autoElem.isNull())
		return false;
	for(int i=0;i<autoElem.childNodes().count();++i)
	{
		QDomElement elem=autoElem.childNodes().at(i).toElement();
		if(elem.isNull()||elem.nodeName()!="dev")
			continue;
		QUuid uid=QUuid(elem.attribute("uid"));
		QByteArray name=elem.attribute("name").toUtf8();
		if(uid.isNull()||name.isEmpty())continue;
		autoDevNames[uid]=name;
		reverseMap[name]=uid;
	}
	for(int i=0;i<manualElem.childNodes().count();++i)
	{
		QDomElement elem=manualElem.childNodes().at(i).toElement();
		if(elem.isNull()||elem.nodeName()!="dev")
			continue;
		QUuid uid=QUuid(elem.attribute("uid"));
		QByteArray name=elem.attribute("name").toUtf8();
		if(uid.isNull()||name.isEmpty())continue;
		manualDevNames[uid]=name;
		reverseMap[name]=uid;
	}
	return true;
}

QByteArray FSDevicesNamesDatabase::deviceName(const QUuid &uid)
{
	if(dbPath.isEmpty())return QByteArray();
	if(manualDevNames.contains(uid))
		return manualDevNames[uid];
	return autoDevNames.value(uid);
}

QByteArrayList FSDevicesNamesDatabase::devicesNames(const QList<QUuid> &uids)
{
	if(dbPath.isEmpty())return QByteArrayList();
	QByteArrayList retVal;
	for(const QUuid &id:uids)
		retVal.append(deviceName(id));
	return retVal;
}

QUuid FSDevicesNamesDatabase::findDevice(const QByteArray &name)
{
	if(dbPath.isEmpty())return QByteArray();
	return reverseMap.value(name);
}

bool FSDevicesNamesDatabase::setManualDevName(const QUuid &uid,const QByteArray &name)
{
	QByteArray oldName=deviceName(uid);
	if(!oldName.isEmpty())
		reverseMap.remove(oldName);
	manualDevNames[uid]=name;
	reverseMap[name]=uid;
	return writeCfg();
}

void FSDevicesNamesDatabase::onDeviceIdentified(const QUuid &uid,const QByteArray &name)
{
	if(dbPath.isEmpty())return;
	if(!manualDevNames.contains(uid))
	{
		if(autoDevNames.contains(uid))
			reverseMap.remove(autoDevNames[uid]);
		autoDevNames[uid]=name;
		reverseMap[name]=uid;
	}
	else autoDevNames[uid]=name;
	writeCfg();
}

bool FSDevicesNamesDatabase::writeCfg()
{
	if(dbPath.isEmpty())return false;
	QDomDocument doc;
	QDomElement rootElem=doc.createElement("devnames");
	doc.appendChild(rootElem);
	QDomElement autoElem=doc.createElement("auto");
	QDomElement manualElem=doc.createElement("manual");
	rootElem.appendChild(manualElem);
	rootElem.appendChild(autoElem);
	for(auto i=manualDevNames.begin();i!=manualDevNames.end();++i)
	{
		QDomElement elem=doc.createElement("dev");
		manualElem.appendChild(elem);
		elem.setAttribute("uid",i.key().toString());
		elem.setAttribute("name",QString::fromUtf8(i.value()));
	}
	for(auto i=autoDevNames.begin();i!=autoDevNames.end();++i)
	{
		QDomElement elem=doc.createElement("dev");
		autoElem.appendChild(elem);
		elem.setAttribute("uid",i.key().toString());
		elem.setAttribute("name",QString::fromUtf8(i.value()));
	}
	QFile file(dbPath);
	if(!file.open(QIODevice::WriteOnly))
		return false;
	file.write(doc.toByteArray());
	file.close();
	return true;
}
