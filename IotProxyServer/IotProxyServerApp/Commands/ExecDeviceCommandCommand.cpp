#include "ExecDeviceCommandCommand.h"
#include "../IotProxyInstance.h"
#include "ARpcBase/ARpcSyncCall.h"
#include "StandardErrors.h"

ExecDeviceCommandCommand::ExecDeviceCommandCommand(ARpcOutsideDevice *d)
	:ICommand(d)
{
}

bool ExecDeviceCommandCommand::processCommand(const ARpcMessage &m)
{
	if(m.title!="exec_command")return false;
	if(m.args.count()<3)
	{
		lastErrorStr=StandardErrors::invalidAgruments;
		return false;
	}
	bool useSync=!(m.args[1]=="1");
	QString cmd=m.args[2];
	QStringList cmdArgs=m.args;
	cmdArgs.removeAt(0);
	cmdArgs.removeAt(0);
	cmdArgs.removeAt(0);
	ARpcDevice *dev=IotProxyInstance::inst().deviceByIdOrName(m.args[0]);
	if(!dev)
	{
		lastErrorStr=StandardErrors::noDeviceWithId.arg(m.args[0]);
		return false;
	}
	if(useSync)
	{
		ARpcSyncCall call;
		QStringList retVal;
		if(!call.call(dev,cmd,cmdArgs,retVal))
		{
			lastErrorStr="command execution error";
			return false;
		}
		clientDev->writeMsg(ARpcConfig::srvCmdDataMsg,retVal);
	}
	else dev->writeMsg(ARpcConfig::funcCallMsg,QStringList()<<cmd<<cmdArgs);
	return true;
}

QStringList ExecDeviceCommandCommand::acceptedCommands()
{
	return QStringList()<<"exec_command";
}
