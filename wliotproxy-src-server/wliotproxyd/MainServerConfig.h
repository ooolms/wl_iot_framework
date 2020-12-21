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

#ifndef MAINSERVERCONFIG_H
#define MAINSERVERCONFIG_H

#include <QStringList>
#include <QSslCertificate>
#include <QSslKey>
#include <QUuid>
#include <sys/stat.h>
#include "CmdArgParser.h"
#include "CustomNetworkProxyFactory.h"
#include "AccessManagement/AccessMgr.h"

class MainServerConfig
{
public:
	static bool readConfig(const CmdArgParser &p);
	static bool readEtcConfig(const CmdArgParser &p);//loads only main wliotproxyd.ini config options
	static bool setTtyByNameFilters(const QString &filtersList);
	static bool setTtyByVidPidFilters(const QString &filtersList);
	static bool setTcpByAddressFitlers(const QString &filtersList);
	static bool setDetectTcpDevices(bool d);
	static QString dumpTtyVidPidFilters();

private:
	static bool writeDevicesConfig();
	static bool readDevicesConfig();
	static bool readServerId();
	static bool readProxies();
	static bool chkPassStrength(const QString &pass);

public://config vars;
	struct VidPidPair
	{
		QString vid;
		QString pid;
	};

	static QString configPath;
	static QString serverProcessUserName;
	static QString serverProcessGroupName;
	static QStringList ttyPortNames;
	static QList<VidPidPair> ttyByVidPid;//vid,pid, if empty - take all devs
	static QStringList tcpAddresses;
	static QString dataUdpExportAddress;
	static QList<QSslCertificate> networkCrtChain;
	static QSslKey networkKey;
	static QString serverName;
	static QString vdilPluginsPath;
	static QUuid serverId;
	static bool detectTcpDevices;
	static AccessMgr accessManager;
	static __mode_t unixSocketAccessMask;//see $man umask
	static bool unixSocketNeedsAuth;

private:
	static bool ready;
	static CustomNetworkProxyFactory *proxy;
};

#endif // MAINSERVERCONFIG_H
