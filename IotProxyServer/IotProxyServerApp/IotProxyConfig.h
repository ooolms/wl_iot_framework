#ifndef IOTPROXYCONFIG_H
#define IOTPROXYCONFIG_H

#include <QStringList>
#include "CmdArgParser.h"

class IotProxyConfig
{
public:
	static bool readConfig(const QString &cfgDir,const CmdArgParser &p);

public://config vars;
	static QString serverProcessUserName;
	static QString serverProcessGroupName;
	static QStringList ttyDevices;
	static QStringList tcpDevices;
};

#endif // IOTPROXYCONFIG_H
