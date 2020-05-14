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

#include "MainServerConfig.h"
#include "SysLogWrapper.h"
#include <QFileInfo>
#include <QDomDocument>
#include <QDomElement>
#include <QSettings>
#include <QDebug>
#include <QUuid>

QString MainServerConfig::serverProcessUserName;
QString MainServerConfig::serverProcessGroupName;
QString MainServerConfig::dataUdpExportAddress;
QStringList MainServerConfig::ttyPortNames;
QStringList MainServerConfig::tcpAddresses;
QString MainServerConfig::serverName;
QUuid MainServerConfig::serverId;
QList<QSslCertificate> MainServerConfig::networkCrtChain;
QSslKey MainServerConfig::networkKey;
QList<MainServerConfig::VidPidPair> MainServerConfig::ttyByVidPid;
bool MainServerConfig::ready=false;
bool MainServerConfig::detectTcpDevices=false;
AccessMgr MainServerConfig::accessManager;
CustomNetworkProxyFactory *MainServerConfig::proxy=0;

bool MainServerConfig::readConfig(const CmdArgParser &p)
{
	if(ready)return false;
	proxy=new CustomNetworkProxyFactory;
	if(!readEtcConfig(p))
		return false;
	if(!readDevicesConfig())
		return false;
	if(!readServerId())
		return false;
	if(!readProxies())
		return false;
	if(!accessManager.readConfig())
		return false;
	ready=true;
	return true;
}

bool MainServerConfig::setTtyByNameFilters(const QString &filtersList)
{
	QStringList bkp=ttyPortNames;
	ttyPortNames=filtersList.split(',',QString::SkipEmptyParts);
	if(!writeDevicesConfig())
	{
		ttyPortNames=bkp;
		return false;
	}
	return true;
}

bool MainServerConfig::setTtyByVidPidFilters(const QString &filtersList)
{
	QList<VidPidPair> bkp=ttyByVidPid;
	QStringList pidVidPairs=filtersList.split(',',QString::SkipEmptyParts);
	ttyByVidPid.clear();
	for(QString &s:pidVidPairs)
	{
		QString vid,pid;
		if(s.indexOf(':')!=-1)
		{
			vid=s.mid(0,s.indexOf(':')).trimmed();
			pid=s.mid(s.indexOf(':')+1).trimmed();
		}
		else
			vid=s.trimmed();
		if(!vid.isEmpty())
			ttyByVidPid.append({vid,pid});
	}
	if(!writeDevicesConfig())
	{
		ttyByVidPid=bkp;
		return false;
	}
	return true;
}

bool MainServerConfig::setTcpByAddressFitlers(const QString &filtersList)
{
	QStringList bkp=tcpAddresses;
	tcpAddresses=filtersList.split(',',QString::SkipEmptyParts);
	if(!writeDevicesConfig())
	{
		tcpAddresses=bkp;
		return false;
	}
	return true;
}

bool MainServerConfig::setDetectTcpDevices(bool d)
{
	if(detectTcpDevices==d)
		return true;
	detectTcpDevices=d;
	if(!writeDevicesConfig())
	{
		detectTcpDevices=!detectTcpDevices;
		return false;
	}
	return true;
}

QString MainServerConfig::dumpTtyVidPidFilters()
{
	QStringList retVal;
	for(VidPidPair &p:ttyByVidPid)
	{
		if(p.pid.isEmpty())
			retVal.append(p.vid);
		else
			retVal.append(p.vid+":"+p.pid);
	}
	return retVal.join(',');
}

bool MainServerConfig::writeDevicesConfig()
{
	if(!ready)
		return false;
	QSettings settings("/var/lib/wliotproxyd/devices.ini",QSettings::IniFormat);

	settings.beginGroup("tcp_devices");
	settings.setValue("by_address",tcpAddresses);
	settings.setValue("detect_tcp_devices",detectTcpDevices?"1":"0");
	settings.endGroup();

	settings.beginGroup("tty_devices");
	settings.setValue("by_name",ttyPortNames);
	QStringList pidVidPairs;
	for(VidPidPair &s:ttyByVidPid)
	{
		if(s.pid.isEmpty())
			pidVidPairs.append(s.vid);
		else
			pidVidPairs.append(s.vid+":"+s.pid);
	}
	settings.setValue("by_vid_pid",pidVidPairs);
	settings.endGroup();
	settings.sync();
	return settings.status()==QSettings::NoError;
}

bool MainServerConfig::readEtcConfig(const CmdArgParser &p)
{
	if(!QFile("/etc/wliotproxyd.ini").exists())
		return false;
	QSettings settings("/etc/wliotproxyd.ini",QSettings::IniFormat);
	settings.sync();
	if(settings.status()!=QSettings::NoError)
		return false;
	settings.beginGroup("common");
	serverName=settings.value("server_name").toString();
	if(settings.contains("user"))
		serverProcessUserName=settings.value("user").toString();
	if(settings.contains("group"))
		serverProcessGroupName=settings.value("group").toString();
	if(!p.getVarSingle("user").isEmpty())
		serverProcessUserName=p.getVarSingle("user");
	if(!p.getVarSingle("group").isEmpty())
		serverProcessGroupName=p.getVarSingle("group");
	dataUdpExportAddress=settings.value("data_udp_export_address").toString();

	QString crtFilePath=settings.value("networkCrt").toString();
	QString keyFilePath=settings.value("networkKey").toString();

//	accessManager.setUsersCanCaptureDevices(settings.value("usersCanManageDevices","1")=="1");
//	accessManager.setUsersCanManageGroups(settings.value("usersCanManageGroups","1")=="1");

	QFile file(crtFilePath);
	if(file.open(QIODevice::ReadOnly))
	{
		networkCrtChain=QSslCertificate::fromDevice(&file);
		file.close();
	}
	file.setFileName(keyFilePath);
	if(file.open(QIODevice::ReadOnly))
	{
		networkKey=QSslKey(&file,QSsl::Rsa);
		file.close();
	}
	settings.endGroup();
	return true;
}

bool MainServerConfig::readDevicesConfig()
{
	QSettings settings("/var/lib/wliotproxyd/devices.ini",QSettings::IniFormat);
	settings.sync();
	if(settings.status()!=QSettings::NoError)
		return false;
	settings.beginGroup("tcp_devices");
	tcpAddresses=settings.value("by_address").toStringList();
	tcpAddresses.removeDuplicates();
	detectTcpDevices=(settings.value("detect_tcp_devices").toString()=="1");
	settings.endGroup();
	qDebug()<<"Tcp addresses from config: "<<tcpAddresses;

	settings.beginGroup("tty_devices");
	ttyPortNames=settings.value("by_name").toStringList();
	ttyPortNames.removeDuplicates();
	QStringList pidVidPairs=settings.value("by_vid_pid").toStringList();
	ttyByVidPid.clear();
	for(QString &s:pidVidPairs)
	{
		QString vid,pid;
		if(s.indexOf(':')!=-1)
		{
			vid=s.mid(0,s.indexOf(':'));
			pid=s.mid(s.indexOf(':')+1);
		}
		else
			vid=s;
		if(!vid.isEmpty())
			ttyByVidPid.append({vid,pid});
	}
	settings.endGroup();
	qDebug()<<"Tty port names from config: "<<ttyPortNames;
	qDebug()<<"Tty vid/pid pairs from config: "<<pidVidPairs;
	return true;
}

bool MainServerConfig::readServerId()
{
	QFile file("/var/lib/wliotproxyd/server.id");
	if(file.exists())
	{
		if(!file.open(QIODevice::ReadOnly))
			return false;
		serverId=QUuid(QString::fromUtf8(file.readAll()));
		file.close();
		return !serverId.isNull();
	}
	serverId=QUuid::createUuid();
	if(serverId.isNull())
		return false;
	if(!file.open(QIODevice::WriteOnly))
		return false;
	file.write(serverId.toString().toUtf8());
	file.close();
	return true;
}

bool MainServerConfig::readProxies()
{
	proxy->proxyMap.clear();
	proxy->defaultProxy=CustomNetworkProxyFactory::noProxy;
	QFile file("/var/lib/wliotproxyd/proxies.xml");
	if(!file.open(QIODevice::ReadOnly))return true;
	QByteArray data=file.readAll();
	file.close();
	QDomDocument doc;
	if(!doc.setContent(data))return false;
	QDomElement rootElem=doc.firstChildElement("proxies");
	if(rootElem.isNull())return false;
	for(int i=0;i<rootElem.childNodes().count();++i)
	{
		QDomElement elem=rootElem.childNodes().at(i).toElement();
		if(elem.isNull()||elem.nodeName()!="proxy")continue;
		bool ok=false;
		QString host=elem.attribute("host");
		quint16 port=elem.attribute("port").toUShort(&ok);
		QString user=elem.attribute("user");
		QString password=elem.attribute("password");
		QNetworkProxy::ProxyType type=QNetworkProxy::NoProxy;
		if(elem.attribute("type")=="http")
			type=QNetworkProxy::HttpProxy;
		else if(elem.attribute("type")=="socks5")
			type=QNetworkProxy::Socks5Proxy;
		QString useForHost=elem.attribute("use-for");
		if(!ok||host.isEmpty()||port==0)continue;
		if(elem.hasAttribute("default"))
			proxy->defaultProxy=QNetworkProxy(type,host,port,user,password);
		else if(!useForHost.isEmpty())
			proxy->proxyMap[useForHost]=QNetworkProxy(type,host,port,user,password);
	}
	return true;
}

bool MainServerConfig::chkPassStrength(const QString &pass)
{
	if(pass.length()<7)return false;
	if(!pass.contains(QRegExp("[a-z]")))return false;
	if(!pass.contains(QRegExp("[A-Z]")))return false;
	if(!pass.contains(QRegExp("[0-9]")))return false;
	return true;
}
