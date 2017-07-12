#include "IotProxyConfig.h"
#include <QFileInfo>
#include <QSettings>
#include <QDebug>

QString IotProxyConfig::serverProcessUserName=QString();
QString IotProxyConfig::serverProcessGroupName=QString();
QStringList IotProxyConfig::ttyPortNames=QStringList();
QStringList IotProxyConfig::ttyManufacturerRegExps=QStringList();
QStringList IotProxyConfig::tcpAddresses=QStringList();

bool IotProxyConfig::readConfig(const QString &cfgDir,const CmdArgParser &p)
{
	serverProcessUserName.clear();
	ttyPortNames.clear();
	tcpAddresses.clear();

	QFileInfo info(cfgDir);
	if(!QFile(info.absoluteFilePath()+"/WLIotProxyServer.ini").exists())return false;
	QSettings settings(info.absoluteFilePath()+"/WLIotProxyServer.ini",QSettings::IniFormat);
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
	ttyManufacturerRegExps=settings.value("by_manufacturer").toStringList();
	settings.endGroup();
	return settings.status()==QSettings::NoError;
}

