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
#include "wliot/ServerConfig.h"

TtyCommands::TtyCommands(QtIODeviceWrap *d,IotProxyCommandProcessor *p)
	:ICommand(d,p)
{
}

bool TtyCommands::processCommand(CallContext &ctx)
{
	if(ctx.cmd=="list_tty")
		return listTtyDevices(ctx);
	else if(ctx.cmd=="identify_tty")
		return identifyTtyDevice(ctx);
	else return false;
}

QByteArrayList TtyCommands::acceptedCommands()
{
	return QByteArrayList()<<"list_tty"<<"identify_tty";
}

bool TtyCommands::listTtyDevices(CallContext &ctx)
{
	QList<QSerialPortInfo> ports=QSerialPortInfo::availablePorts();
	for(auto &p:ports)
	{
		LsTtyUsbDevices::DeviceInfo info;
		IotProxyInstance::inst().devices()->usbTtyDeviceByPortName(p.portName(),info);
		auto ttyDev=IotProxyInstance::inst().devices()->ttyDeviceByPortName(p.portName());
		if(ttyDev&&ttyDev->isIdentified())writeCmdataMsg(ctx.callId,
			QByteArrayList()<<p.portName().toUtf8()<<p.serialNumber().toUtf8()<<info.manufacturerString.toUtf8()<<
			info.vendorId.toUtf8()<<info.productId.toUtf8()<<ttyDev->id().toByteArray()<<ttyDev->name());
		else writeCmdataMsg(ctx.callId,
			QByteArrayList()<<p.portName().toUtf8()<<p.serialNumber().toUtf8()<<info.manufacturerString.toUtf8()<<
			info.vendorId.toUtf8()<<info.productId.toUtf8());
	}
	return true;
}

bool TtyCommands::identifyTtyDevice(CallContext &ctx)
{
	if(ctx.args.count()<1)
	{
		ctx.retVal.append(StandardErrors::invalidAgruments);
		return false;
	}
	QString portName=QString::fromUtf8(ctx.args[0]);
	SerialDevice *dev=IotProxyInstance::inst().devices()->ttyDeviceByPortName(portName);
	if(!dev)
	{
		dev=IotProxyInstance::inst().devices()->addTtyDeviceByPortName(portName);
		if(!dev)
		{
			ctx.retVal.append(QByteArray(StandardErrors::noDeviceFound).replace("%1",portName.toUtf8()));
			return false;
		}
	}
	if(!dev->isIdentified()&&dev->identify()!=RealDevice::OK)
	{
		ctx.retVal.append(StandardErrors::deviceNotIdentified);
		return false;
	}
	ctx.retVal<<dev->id().toByteArray()<<dev->name();
	return true;
}
