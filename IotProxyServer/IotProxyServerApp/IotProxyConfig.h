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

#ifndef IOTPROXYCONFIG_H
#define IOTPROXYCONFIG_H

#include <QStringList>
#include <tuple>
#include "CmdArgParser.h"

class IotProxyConfig
{
public:
	static bool readConfig(const CmdArgParser &p);
	static bool setTtyByNameFilters(const QString &filtersList);
	static bool setTtyByVidPidFilters(const QString &filtersList);
	static bool setTcpByAddressFitlers(const QString &filtersList);
	static bool setDetectTcpDevices(bool d);
	static QString dumpTtyVidPidFilters();

private:
	static bool writeDevicesConfig();

public://config vars;
	struct VidPidPair
	{
		QString vid;
		QString pid;
	};

	static QString serverProcessUserName;
	static QString serverProcessGroupName;
	static QStringList ttyPortNames;
	static QList<VidPidPair> ttyByVidPid;//vid,pid, if empty - take all devs
	static QStringList tcpAddresses;
	static QString dataUdpExportAddress;
	static bool detectTcpDevices;

private:
	static bool ready;
};

#endif // IOTPROXYCONFIG_H
