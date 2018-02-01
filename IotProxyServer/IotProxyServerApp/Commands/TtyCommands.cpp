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
#include "ARpcBase/ARpcServerConfig.h"

TtyCommands::TtyCommands(ARpcOutsideDevice *d,IotProxyCommandProcessor *p)
	:ICommand(d,p)
{
}

bool TtyCommands::processCommand(const ARpcMessage &m, QByteArrayList &retVal)
{
	if(m.title=="list_tty")
		return listTtyDevices(m);
	else if(m.title=="identify_tty")
		return identifyTtyDevice(m,retVal);
	else return false;
}

QByteArrayList TtyCommands::acceptedCommands()
{
	return QByteArrayList()<<"list_tty"<<"identify_tty";
}

bool TtyCommands::listTtyDevices(const ARpcMessage &m)
{
	Q_UNUSED(m)
	QList<QSerialPortInfo> ports=QSerialPortInfo::availablePorts();
	for(auto &p:ports)
	{
		LsTtyUsbDevices::DeviceInfo info;
		IotProxyInstance::inst().devices()->usbTtyDeviceByPortName(p.portName(),info);
		auto ttyDev=IotProxyInstance::inst().devices()->ttyDeviceByPortName(p.portName());
		if(ttyDev&&ttyDev->isIdentified())clientDev->writeMsg(ARpcServerConfig::srvCmdDataMsg,
			QByteArrayList()<<p.portName().toUtf8()<<p.serialNumber().toUtf8()<<info.manufacturerString.toUtf8()<<
			info.vendorId.toUtf8()<<info.productId.toUtf8()<<ttyDev->id().toByteArray()<<ttyDev->name());
		else clientDev->writeMsg(ARpcServerConfig::srvCmdDataMsg,
			QByteArrayList()<<p.portName().toUtf8()<<p.serialNumber().toUtf8()<<info.manufacturerString.toUtf8()<<
			info.vendorId.toUtf8()<<info.productId.toUtf8());
	}
	return true;
}

bool TtyCommands::identifyTtyDevice(const ARpcMessage &m,QByteArrayList &retVal)
{
	if(m.args.count()<1)
	{
		retVal.append(StandardErrors::invalidAgruments);
		return false;
	}
	QString portName=QString::fromUtf8(m.args[0]);
	ARpcTtyDevice *dev=IotProxyInstance::inst().devices()->ttyDeviceByPortName(portName);
	if(!dev)
	{
		dev=IotProxyInstance::inst().devices()->addTtyDeviceByPortName(portName);
		if(!dev)
		{
			retVal.append(QByteArray(StandardErrors::noDeviceWithId).replace("%1",portName.toUtf8()));
			return false;
		}
	}
	if(!dev->isIdentified()&&!dev->identify())
	{
		retVal.append(StandardErrors::deviceNotIdentified);
		return false;
	}
	retVal<<dev->id().toByteArray()<<dev->name();
	return true;
}
