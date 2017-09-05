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

QString IotProxyConfig::serverProcessUserName;
QString IotProxyConfig::serverProcessGroupName;
QString IotProxyConfig::dataUdpExportAddress;
QStringList IotProxyConfig::ttyPortNames;
QStringList IotProxyConfig::tcpAddresses;
QList<IotProxyConfig::VidPidPair> IotProxyConfig::ttyByVidPid;
bool IotProxyConfig::ready=false;
bool IotProxyConfig::detectTcpDevices=false;

bool IotProxyConfig::readConfig(const CmdArgParser &p)
{
	serverProcessUserName.clear();
	ttyPortNames.clear();
	tcpAddresses.clear();

	if(!QFile("/etc/wliotproxyd.ini").exists())return false;
//	if(!QFile("/var/lib/wliotproxyd/devices.ini").exists())return false;

	{//global daemon settings
		QSettings settings("/etc/wliotproxyd.ini",QSettings::IniFormat);
		settings.sync();
		if(settings.status()!=QSettings::NoError)return false;

		settings.beginGroup("common");
		if(settings.contains("user"))
			serverProcessUserName=settings.value("user").toString();
		if(settings.contains("group"))
			serverProcessGroupName=settings.value("group").toString();
		if(!p.getVarSingle("user").isEmpty())
			serverProcessUserName=p.getVarSingle("user");
		if(!p.getVarSingle("group").isEmpty())
			serverProcessGroupName=p.getVarSingle("group");
		dataUdpExportAddress=settings.value("data_udp_export_address").toString();
		settings.endGroup();
	}

	{//devices config
		QSettings settings("/var/lib/wliotproxyd/devices.ini",QSettings::IniFormat);
		settings.sync();
		if(settings.status()!=QSettings::NoError)return false;

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
			else vid=s;
			if(!vid.isEmpty())ttyByVidPid.append({vid,pid});
		}
		settings.endGroup();

		qDebug()<<"Tty port names from config: "<<ttyPortNames;
		qDebug()<<"Tty vid/pid pairs from config: "<<pidVidPairs;
	}
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
	//TODO reparse config from IotProxyInstance
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
		else vid=s.trimmed();
		if(!vid.isEmpty())ttyByVidPid.append({vid,pid});
	}
	if(!writeDevicesConfig())
	{
		ttyByVidPid=bkp;
		return false;
	}
	//TODO reparse config from IotProxyInstance
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
	//TODO reparse config from IotProxyInstance
	return true;
}

bool IotProxyConfig::setDetectTcpDevices(bool d)
{
	if(detectTcpDevices==d)return true;
	detectTcpDevices=d;
	if(!writeDevicesConfig())
	{
		detectTcpDevices=!detectTcpDevices;
		return false;
	}
	//TODO reparse config from IotProxyInstance
	return true;
}

QString IotProxyConfig::dumpTtyVidPidFilters()
{
	QStringList retVal;
	for(VidPidPair &p:ttyByVidPid)
	{
		if(p.pid.isEmpty())
			retVal.append(p.vid);
		else retVal.append(p.vid+":"+p.pid);
	}
	return retVal.join(',');
}

bool IotProxyConfig::writeDevicesConfig()
{
	if(!ready)return false;
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
		else pidVidPairs.append(s.vid+":"+s.pid);
	}
	settings.setValue("by_vid_pid",pidVidPairs);
	settings.endGroup();
	settings.sync();
	return settings.status()==QSettings::NoError;
}
