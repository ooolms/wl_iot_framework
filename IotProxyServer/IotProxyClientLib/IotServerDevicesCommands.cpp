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

#include "IotServerDevicesCommands.h"

IotServerDevicesCommands::IotServerDevicesCommands(IotServerConnection *conn,QObject *parent)
	:QObject(parent)
{
	srvConn=conn;
}

bool IotServerDevicesCommands::listTty(QList<IotServerTtyPortDescr> &ttyList)
{
	IotServerConnection::CmDataCallback cb=[this,&ttyList](const QByteArrayList &args)
	{
		if(args.count()<5)
			return false;
		IotServerTtyPortDescr d;
		d.portName=args[0];
		d.serialNumber=args[1];
		d.manufacturer=args[2];
		d.usbVendorId=args[3];
		d.usbVendorId=args[4];
		ttyList.append(d);
		return true;
	};
	return srvConn->execCommand("list_tty",QByteArrayList(),cb);
}

bool IotServerDevicesCommands::identifyTty(const QByteArray &portName)
{
	return srvConn->execCommand("identify_tty",QByteArrayList()<<portName);
}

bool IotServerDevicesCommands::identifyTcp(const QByteArray &host)
{
	return srvConn->execCommand("identify_tcp",QByteArrayList()<<host);
}

bool IotServerDevicesCommands::listIdentified(QList<IotServerIdentifiedDeviceDescr> &devs)
{
	IotServerConnection::CmDataCallback cb=[this,&devs](const QByteArrayList &args)
	{
		if(args.count()<4)return false;
		IotServerIdentifiedDeviceDescr d={QUuid(args[0]),args[1],args[2],args[3]};
		if(d.id.isNull())return false;
		devs.append(d);
		return true;
	};
	return srvConn->execCommand("list_identified",QByteArrayList(),cb);
}

bool IotServerDevicesCommands::listSensors(const QByteArray &idOrName,QList<ARpcSensorDef> &sensors)
{
	QByteArrayList retVal;
	if(!srvConn->execCommand("list_sensors",QByteArrayList()<<idOrName,retVal)||retVal.isEmpty())return false;
	if(retVal[0].startsWith("{"))
		return ARpcSensorDef::parseJsonDescription(retVal[0],sensors);
	else return ARpcSensorDef::parseXmlDescription(retVal[0],sensors);
}

bool IotServerDevicesCommands::listControls(const QByteArray &idOrName,ARpcControlsGroup &controls)
{
	QByteArrayList retVal;
	if(!srvConn->execCommand("list_controls",QByteArrayList()<<idOrName,retVal)||retVal.isEmpty())return false;
	if(retVal[0].startsWith("{"))
		return ARpcControlsGroup::parseJsonDescription(retVal[0],controls);
	else return ARpcControlsGroup::parseXmlDescription(retVal[0],controls);
}

bool IotServerDevicesCommands::deviceId(const QByteArray &idOrName,QUuid &id)
{
	QByteArrayList retVal;
	if(!srvConn->execCommand("device_id",QByteArrayList()<<idOrName,retVal)||retVal.isEmpty())return false;
	id=QUuid(retVal[0]);
	return !id.isNull();
}

bool IotServerDevicesCommands::execDeviceCommand(
	const QByteArray &idOrName,const QByteArray &command,const QByteArrayList &args,QByteArrayList &retVal)
{
	return srvConn->execCommand("exec_command",QByteArrayList()<<idOrName<<command<<args,retVal);
}
