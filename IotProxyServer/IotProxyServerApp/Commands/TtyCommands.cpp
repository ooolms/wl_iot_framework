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

#include "TtyCommands.h"
#include "../IotProxyInstance.h"
#include "StandardErrors.h"
#include <QSerialPortInfo>

TtyCommands::TtyCommands(ARpcOutsideDevice *d)
	:ICommand(d)
{
}

bool TtyCommands::processCommand(const ARpcMessage &m)
{
	if(m.title=="list_tty")
		return listTtyDevices(m);
	else if(m.title=="identify_tty")
		return identifyTtyDevice(m);
	else return false;
}

QStringList TtyCommands::acceptedCommands()
{
	return QStringList()<<"list_tty"<<"identify_tty";
}

bool TtyCommands::listTtyDevices(const ARpcMessage &m)
{
	Q_UNUSED(m)
	QList<QSerialPortInfo> ports=QSerialPortInfo::availablePorts();
	for(auto &p:ports)
	{
		LsTtyUsbDevices::DeviceInfo info;
		IotProxyInstance::inst().findUsbTtyDeviceByPortName(p.portName(),info);
		auto ttyDev=IotProxyInstance::inst().findTtyDevByPortName(p.portName());
		if(ttyDev&&ttyDev->isIdentified())clientDev->writeMsg(ARpcConfig::srvCmdDataMsg,
			QStringList()<<p.portName()<<p.serialNumber()<<info.manufacturerString<<
			info.vendorId<<info.productId<<ttyDev->id().toString()<<ttyDev->name());
		else clientDev->writeMsg(ARpcConfig::srvCmdDataMsg,
			QStringList()<<p.portName()<<p.serialNumber()<<info.manufacturerString<<
			info.vendorId<<info.productId);
	}
	return true;
}

bool TtyCommands::identifyTtyDevice(const ARpcMessage &m)
{
	if(m.args.count()<1)
	{
		lastErrorStr=StandardErrors::invalidAgruments;
		return false;
	}
	QString portName=m.args[0];
	ARpcTtyDevice *dev=IotProxyInstance::inst().findTtyDevByPortName(portName);
	if(!dev)
	{
		lastErrorStr=StandardErrors::noDeviceWithId.arg(portName);
		return false;
	}
	if(!dev->identify())
	{
		lastErrorStr=StandardErrors::deviceNotIdentified;
		return false;
	}
	clientDev->writeMsg(ARpcConfig::srvCmdDataMsg,QStringList()<<dev->id().toString()<<dev->name());
	return true;
}
