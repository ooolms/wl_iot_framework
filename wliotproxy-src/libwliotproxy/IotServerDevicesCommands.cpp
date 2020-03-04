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

IotServerDevicesCommands::IotServerDevicesCommands(IotServerConnection *conn)
	:QObject(conn)
{
	srvConn=conn;
}

bool IotServerDevicesCommands::listTty(QList<IotServerTtyPortDescr> &ttyList)
{
	CmDataCallback cb=[&ttyList](const QByteArrayList &args)
	{
		if(args.count()<5)
			return false;
		IotServerTtyPortDescr d;
		d.portName=args[0];
		d.serialNumber=args[1];
		d.manufacturer=args[2];
		d.usbVendorId=args[3];
		d.usbProductId=args[4];
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
	CmDataCallback cb=[&devs](const QByteArrayList &args)
	{
		if(args.count()<4)return false;
		IotServerIdentifiedDeviceDescr d={QUuid(args[0]),args[1],args[2],args[3]};
		if(d.id.isNull())return false;
		devs.append(d);
		return true;
	};
	return srvConn->execCommand("list_identified",QByteArrayList(),cb);
}

bool IotServerDevicesCommands::listSensors(const QByteArray &idOrName,QList<SensorDef> &sensors)
{
	QByteArrayList retVal;
	if(!srvConn->execCommand("list_sensors",QByteArrayList()<<idOrName,retVal)||retVal.isEmpty())return false;
	if(retVal[0].startsWith("{"))
		return SensorDef::parseJsonDescription(retVal[0],sensors);
	else return SensorDef::parseXmlDescription(retVal[0],sensors);
}

bool IotServerDevicesCommands::listControls(const QByteArray &idOrName,ControlsGroup &controls)
{
	QByteArrayList retVal;
	if(!srvConn->execCommand("list_controls",QByteArrayList()<<idOrName,retVal)||retVal.isEmpty())return false;
	if(retVal[0].startsWith("{"))
		return ControlsGroup::parseJsonDescription(retVal[0],controls);
	else return ControlsGroup::parseXmlDescription(retVal[0],controls);
}

bool IotServerDevicesCommands::getDevState(const QByteArray &idOrName,DeviceState &state)
{
	QByteArrayList retVal;
	if(!srvConn->execCommand("get_dev_state",QByteArrayList()<<idOrName,retVal))return false;
	return state.parseMsgArgs(retVal);
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
	if(command==WLIOTProtocolDefs::getSensorsCommand)
		return srvConn->execCommand("list_sensors",QByteArrayList()<<idOrName,retVal);
	else if(command==WLIOTProtocolDefs::getControlsCommand)
		return srvConn->execCommand("list_controls",QByteArrayList()<<idOrName,retVal);
	else if(command==WLIOTProtocolDefs::getStateCommand)
		return srvConn->execCommand("get_dev_state",QByteArrayList()<<idOrName,retVal);
	else return srvConn->execCommand("exec_command",QByteArrayList()<<idOrName<<command<<args,retVal);
}

bool IotServerDevicesCommands::registerVirtualDevice(
	const QUuid &deviceId,const QByteArray &deviceName,const QUuid &typeId)
{
	return srvConn->execCommand("register_virtual_device",
		QByteArrayList()<<deviceId.toByteArray()<<deviceName<<typeId.toByteArray());
}

bool IotServerDevicesCommands::sendVDevMeasurement(
	const QUuid &deviceId,const QByteArray &sensorName,const QByteArrayList &measArgs)
{
	return srvConn->execCommand("vdev_meas",
		QByteArrayList()<<deviceId.toByteArray()<<sensorName<<measArgs);
}

bool IotServerDevicesCommands::sendVDevMeasurementB(
	const QUuid &deviceId,const QByteArray &sensorName,const QByteArray &data)
{
	return srvConn->execCommand("vdev_measb",
		QByteArrayList()<<deviceId.toByteArray()<<sensorName<<data);
}

bool IotServerDevicesCommands::devNames(const QList<QUuid> &ids,QByteArrayList &names)
{
	QByteArrayList idsList;
	for(const QUuid &id:ids)
		idsList.append(id.toByteArray());
	return srvConn->execCommand("dev_names",idsList,names);
}

bool IotServerDevicesCommands::setDevName(const QUuid &devId,const QByteArray &devName)
{
	if(devName.isEmpty())
		return srvConn->execCommand("set_dev_name",QByteArrayList()<<devId.toByteArray());
	else return srvConn->execCommand("set_dev_name",QByteArrayList()<<devId.toByteArray()<<devName);
}
