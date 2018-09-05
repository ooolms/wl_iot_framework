#include "IotServerConfigurationCommands.h"

IotServerConfigurationCommands::IotServerConfigurationCommands(IotServerConnection *conn,QObject *parent)
	:QObject(parent)
{
	srvConn=conn;
}

bool IotServerConfigurationCommands::getTtyFilterByPortName(QByteArray &filter)
{
	QByteArrayList retVal;
	if(!srvConn->execCommand("devices_config",QByteArrayList()<<"get_tty_by_port_name",retVal)||retVal.isEmpty())
		return false;
	filter=retVal[0];
	return true;
}

bool IotServerConfigurationCommands::setTtyFilterByPortName(const QByteArray &filter)
{
	return srvConn->execCommand("devices_config",QByteArrayList()<<"set_tty_by_port_name"<<filter);
}

bool IotServerConfigurationCommands::getTtyFilterByPidVid(QByteArray &filter)
{
	QByteArrayList retVal;
	if(!srvConn->execCommand("devices_config",QByteArrayList()<<"get_tty_by_vid_pid",retVal)||retVal.isEmpty())
		return false;
	filter=retVal[0];
	return true;
}

bool IotServerConfigurationCommands::setTtyFilterByPidVid(const QByteArray &filter)
{
	return srvConn->execCommand("devices_config",QByteArrayList()<<"set_tty_by_vid_pid"<<filter);
}

bool IotServerConfigurationCommands::getTcpFilter(QByteArray &filter)
{
	QByteArrayList retVal;
	if(!srvConn->execCommand("devices_config",QByteArrayList()<<"get_tcp_by_address",retVal)||retVal.isEmpty())
		return false;
	filter=retVal[0];
	return true;
}

bool IotServerConfigurationCommands::setTcpFilter(const QByteArray &filter)
{
	return srvConn->execCommand("devices_config",QByteArrayList()<<"set_tcp_by_address"<<filter);
}

bool IotServerConfigurationCommands::setDetectTcpDevices(bool enable)
{
	return srvConn->execCommand("devices_config",QByteArrayList()<<"set_detect_tcp_devices"<<(enable?"1":"0"));
}
