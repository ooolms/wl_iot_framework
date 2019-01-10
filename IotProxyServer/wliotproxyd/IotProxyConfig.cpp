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

#include "IotProxyConfig.h"
#include "SysLogWrapper.h"
#include <QFileInfo>
#include <QSettings>
#include <QDebug>
#include <QUuid>

QString IotProxyConfig::serverProcessUserName;
QString IotProxyConfig::serverProcessGroupName;
QString IotProxyConfig::dataUdpExportAddress;
QString IotProxyConfig::networkAccessKey;
QStringList IotProxyConfig::ttyPortNames;
QStringList IotProxyConfig::tcpAddresses;
QString IotProxyConfig::serverName;
QUuid IotProxyConfig::serverId;
QSslCertificate IotProxyConfig::networkCrt;
QSslKey IotProxyConfig::networkKey;
QList<IotProxyConfig::VidPidPair> IotProxyConfig::ttyByVidPid;
QList<IotProxyConfig::User> IotProxyConfig::users;
bool IotProxyConfig::ready=false;
bool IotProxyConfig::detectTcpDevices=false;
quint32 IotProxyConfig::maxUid=0;

bool IotProxyConfig::readConfig(const CmdArgParser &p)
{
	if(ready)return false;
	if(!readEtcConfig(p))
		return false;
	if(!readDevicesConfig())
		return false;
	if(!readServerId())
		return false;
	if(!readUsers())
		return false;
	ready=true;
	return true;
}

bool IotProxyConfig::setTtyByNameFilters(const QString &filtersList)
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

bool IotProxyConfig::setTtyByVidPidFilters(const QString &filtersList)
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

bool IotProxyConfig::setTcpByAddressFitlers(const QString &filtersList)
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

bool IotProxyConfig::setDetectTcpDevices(bool d)
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

QString IotProxyConfig::dumpTtyVidPidFilters()
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

bool IotProxyConfig::writeDevicesConfig()
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

bool IotProxyConfig::readEtcConfig(const CmdArgParser &p)
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
	networkAccessKey=settings.value("networkAccessKey").toString();
	if(!chkPassStrength(networkAccessKey))
	{
		qDebug()<<"Network access key is too sick";
		networkAccessKey.clear();
	}

	QString crtFilePath=settings.value("networkCrt").toString();
	QString keyFilePath=settings.value("networkKey").toString();

	QFile file(crtFilePath);
	if(file.open(QIODevice::ReadOnly))
	{
		networkCrt=QSslCertificate(&file);
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

bool IotProxyConfig::readDevicesConfig()
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

bool IotProxyConfig::readServerId()
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
	QFile rndFile("/dev/random");
	if(!rndFile.open(QIODevice::ReadOnly|QIODevice::Unbuffered))
		return false;
	QByteArray rndData;
	for(int i=0;i<128;++i)
		rndData.append(rndFile.read(1));
	rndFile.close();
	serverId=QUuid::createUuidV5(QUuid(),rndData);
	if(serverId.isNull())
		return false;
	if(!file.open(QIODevice::WriteOnly))
		return false;
	file.write(serverId.toString().toUtf8());
	file.close();
	return true;
}

bool IotProxyConfig::readUsers()
{
	QFile file("/var/lib/wliotproxyd/passwd");
	users.clear();
	maxUid=1000;
	if(!file.open(QIODevice::ReadOnly))
	{
		User u;
		u.uid=0;
		u.userName="root";
		users.append(u);
		if(u.uid>maxUid)
			maxUid=u.uid;
		return true;
	}
	bool hasRoot=false;
	QSet<QString> userNames;
	QSet<quint32> userIds;
	while(!file.atEnd())
	{
		QByteArray line=file.readLine();
		if(line.endsWith('\n'))
			line.chop(1);
		QByteArrayList fields=line.split(':');
		if(fields.count()!=3)continue;
		bool ok=false;
		User u;
		u.userName=fields[0];
		u.passwdHash=fields[1];
		u.uid=fields[2].toULong(&ok);
		if(u.userName.isEmpty()||!ok)continue;
		if(userNames.contains(u.userName)||userIds.contains(u.uid))
			return false;
		users.append(u);
		userNames.insert(u.userName);
		userIds.insert(u.uid);
		if(u.uid==0)
			hasRoot=true;
	}
	file.close();
	if(!hasRoot)
	{
		User u;
		u.uid=0;
		u.userName="root";
		users.prepend(u);
		writeUsers();
	}
	return true;
}

bool IotProxyConfig::writeUsers()
{
	QFile file("/var/lib/wliotproxyd/passwd");
	if(!file.open(QIODevice::WriteOnly))
		return false;
	for(User &u:users)
	{
		file.write(u.userName);
		file.write(":");
		file.write(u.passwdHash);
		file.write(":");
		file.write(QByteArray::number(u.uid));
		file.write("\n");
	}
	file.close();
	return true;
}

bool IotProxyConfig::chkPassStrength(const QString &pass)
{
	if(pass.length()<7)return false;
	if(!pass.contains(QRegExp("[a-z]")))return false;
	if(!pass.contains(QRegExp("[A-Z]")))return false;
	if(!pass.contains(QRegExp("[0-9]")))return false;
	return true;
}

bool IotProxyConfig::chkUser(const QByteArray &userName,const QByteArray &pass,quint32 &uid)
{
	QCryptographicHash hash(QCryptographicHash::Sha512);
	hash.addData(pass);
	for(User &u:users)
	{
		if(u.userName==userName)
		{
			if(u.passwdHash!=hash.result())return false;
			uid=u.uid;
			return true;
		}
	}
	return false;
}

bool IotProxyConfig::addUser(const QByteArray &userName,quint32 &uid)
{
	if(hasUser(userName,uid))
		return false;
	uid=maxUid++;
	User u;
	u.userName=userName;
	static const QByteArray validChars="abcdefghijklmnopqstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890";
	for(char c:u.userName)
		if(!validChars.contains(c))
			return false;
	u.uid=uid;
	users.append(u);
	return writeUsers();
}

bool IotProxyConfig::hasUser(const QByteArray &userName,quint32 &uid)
{
	for(User &u:users)
	{
		if(u.userName==userName)
		{
			uid=u.uid;
			return true;
		}
	}
	return false;
}

void IotProxyConfig::rmUser(const QByteArray &userName)
{
	for(int i=0;i<users.count();++i)
	{
		if(users[i].userName==userName)
		{
			users.removeAt(i);
			writeUsers();
			return;
		}
	}
}

bool IotProxyConfig::userSetPass(const QByteArray &userName,const QByteArray &pass)
{
	QCryptographicHash hash(QCryptographicHash::Sha512);
	hash.addData(pass);
	QByteArray passHash=hash.result();
	for(User &u:users)
	{
		if(u.userName==userName)
		{
			u.passwdHash=passHash;
			writeUsers();
			return true;
		}
	}
	return false;
}