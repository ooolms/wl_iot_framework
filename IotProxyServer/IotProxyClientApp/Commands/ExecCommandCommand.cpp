#include "ExecCommandCommand.h"
#include "../StdQFile.h"
#include <QDebug>

ExecCommandCommand::ExecCommandCommand(const CmdArgParser &p,ARpcOutsideDevice *d)
	:IClientCommand(p,d)
{
	connect(d,&ARpcOutsideDevice::rawMessage,this,&ExecCommandCommand::onRawMessage);
}

bool ExecCommandCommand::evalCommand()
{
	bool sync=parser.hasKey("sync");
	if(parser.getArgs().count()<2)
	{
		//TODO help
		return false;
	}
	QStringList args=parser.getArgs();
	QString devIdOrName=args[0];
	args.removeFirst();
	return dev->writeMsg(IClientCommand::execCommandCommand,QStringList()<<devIdOrName<<(sync?"1":"0")<<args);
}

void ExecCommandCommand::onRawMessage(const ARpcMessage &m)
{
	QDebug d(StdQFile::inst().stdout());
	if(m.title==ARpcConfig::srvCmdDataMsg)
		QDebug(StdQFile::inst().stdout())<<"Command result: "<<m.args.join(ARpcConfig::argDelim);
}
