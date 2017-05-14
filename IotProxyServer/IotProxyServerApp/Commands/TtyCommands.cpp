#include "TtyCommands.h"
#include "../IotProxyInstance.h"
#include <QSerialPortInfo>

TtyCommands::TtyCommands(ARpcOutsideDevice *d)
	:ICommand(d)
{
}

bool TtyCommands::processCommand(const ARpcMessage &m)
{
	if(m.title=="list_tty")
	{
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
	else return false;
}
