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

#include "wliot/client/ConfigurationCommands.h"

using namespace WLIOT;
using namespace WLIOTClient;

ConfigurationCommands::ConfigurationCommands(ServerConnection *conn)
	:QObject(conn)
{
	srvConn=conn;
}

bool ConfigurationCommands::getTtyFilterByPortName(QByteArray &filter)
{
	QByteArrayList retVal;
	if(!srvConn->execCommand("devices_config",QByteArrayList()<<"get_tty_by_port_name",retVal)||retVal.isEmpty())
		return false;
	filter=retVal[0];
	return true;
}

bool ConfigurationCommands::setTtyFilterByPortName(const QByteArray &filter)
{
	return srvConn->execCommand("devices_config",QByteArrayList()<<"set_tty_by_port_name"<<filter);
}

bool ConfigurationCommands::getTtyFilterByPidVid(QByteArray &filter)
{
	QByteArrayList retVal;
	if(!srvConn->execCommand("devices_config",QByteArrayList()<<"get_tty_by_vid_pid",retVal)||retVal.isEmpty())
		return false;
	filter=retVal[0];
	return true;
}

bool ConfigurationCommands::setTtyFilterByPidVid(const QByteArray &filter)
{
	return srvConn->execCommand("devices_config",QByteArrayList()<<"set_tty_by_vid_pid"<<filter);
}

bool ConfigurationCommands::getTcpFilter(QByteArray &filter)
{
	QByteArrayList retVal;
	if(!srvConn->execCommand("devices_config",QByteArrayList()<<"get_tcp_by_address",retVal)||retVal.isEmpty())
		return false;
	filter=retVal[0];
	return true;
}

bool ConfigurationCommands::setTcpFilter(const QByteArray &filter)
{
	return srvConn->execCommand("devices_config",QByteArrayList()<<"set_tcp_by_address"<<filter);
}

bool ConfigurationCommands::setDetectTcpDevices(bool enable)
{
	return srvConn->execCommand("devices_config",QByteArrayList()<<"set_detect_tcp_devices"<<(enable?"1":"0"));
}
