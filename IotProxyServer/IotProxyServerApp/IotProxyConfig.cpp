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
#include <QFileInfo>
#include <QSettings>
#include <QDebug>

QString IotProxyConfig::serverProcessUserName=QString();
QString IotProxyConfig::serverProcessGroupName=QString();
QStringList IotProxyConfig::ttyPortNames=QStringList();
QStringList IotProxyConfig::tcpAddresses=QStringList();
QList<IotProxyConfig::VidPidPair> IotProxyConfig::ttyByVidPid=QList<IotProxyConfig::VidPidPair>();

bool IotProxyConfig::readConfig(const QString &cfgDir,const CmdArgParser &p)
{
	serverProcessUserName.clear();
	ttyPortNames.clear();
	tcpAddresses.clear();

	QFileInfo info(cfgDir);
	if(!QFile(info.absoluteFilePath()+"/wliotproxyd.ini").exists())return false;
	QSettings settings(info.absoluteFilePath()+"/wliotproxyd.ini",QSettings::IniFormat);
	settings.sync();
	if(settings.status()!=QSettings::NoError)return false;

	if(settings.contains("user"))
		serverProcessUserName=settings.value("user").toString();
	if(settings.contains("group"))
		serverProcessGroupName=settings.value("group").toString();
	if(!p.getVarSingle("user").isEmpty())
		serverProcessUserName=p.getVarSingle("user");
	if(!p.getVarSingle("group").isEmpty())
		serverProcessGroupName=p.getVarSingle("group");

	settings.beginGroup("tcp_devices");
	QStringList keys=settings.childKeys();
	for(QString &s:keys)
		tcpAddresses.append(settings.value(s).toString());
	settings.endGroup();

	settings.beginGroup("tty_devices");
	ttyPortNames=settings.value("by_name").toStringList();
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
		if(!pid.isEmpty()||!vid.isEmpty())ttyByVidPid.append({vid,pid});
	}
	settings.endGroup();
	return settings.status()==QSettings::NoError;
}

