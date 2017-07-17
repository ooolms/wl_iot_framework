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
		auto ttyDev=IotProxyInstance::inst().findTtyDevByPortName(p.portName());
		if(ttyDev&&ttyDev->isIdentified())clientDev->writeMsg(ARpcConfig::srvCmdDataMsg,
			QStringList()<<p.portName()<<p.serialNumber()<<p.manufacturer()<<p.description()<<
			ttyDev->id().toString()<<ttyDev->name());
		else clientDev->writeMsg(ARpcConfig::srvCmdDataMsg,
			QStringList()<<p.portName()<<p.serialNumber()<<p.manufacturer()<<p.description());
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
