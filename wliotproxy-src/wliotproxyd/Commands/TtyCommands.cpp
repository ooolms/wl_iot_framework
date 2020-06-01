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
#include "../ServerInstance.h"
#include "StandardErrors.h"
#include <QSerialPortInfo>
#include "wliot/WLIOTServerProtocolDefs.h"

using namespace WLIOT;

TtyCommands::TtyCommands(CommandProcessor *p)
	:ICommand(p)
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
		ServerInstance::inst().devices()->usbTtyDeviceByPortName(p.portName(),info);
		writeCmdataMsg(ctx.callId,
			QByteArrayList()<<p.portName().toUtf8()<<p.serialNumber().toUtf8()<<info.manufacturerString.toUtf8()<<
			info.vendorId.toUtf8()<<info.productId.toUtf8());
	}
	return true;
}

bool TtyCommands::identifyTtyDevice(CallContext &ctx)
{
	if(ctx.args.count()<1)
	{
		ctx.retVal.append(StandardErrors::invalidAgruments());
		return false;
	}
	QString portName=QString::fromUtf8(ctx.args[0]);
	RealDevice *dev=ServerInstance::inst().devices()->ttyDeviceByPortName(portName);
	if(!dev)
	{
		dev=ServerInstance::inst().devices()->addTtyDeviceByPortName(portName);
		if(!dev)
		{
			ctx.retVal.append(StandardErrors::noDeviceFound(portName.toUtf8()));
			return false;
		}
	}
	if(!dev->isReady()&&dev->identify()!=RealDevice::OK)
	{
		ctx.retVal.append(StandardErrors::deviceNotIdentified("tty:"+ctx.args[0]));
		return false;
	}
	ctx.retVal<<dev->id().toByteArray()<<dev->name();
	return true;
}
