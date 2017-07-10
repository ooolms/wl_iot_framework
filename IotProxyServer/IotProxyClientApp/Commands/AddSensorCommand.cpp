#include "AddSensorCommand.h"
#include "ARpcLocalStorage/ARpcISensorStorage.h"
#include "../StdQFile.h"
#include <QDebug>

AddSensorCommand::AddSensorCommand(const CmdArgParser &p,ARpcOutsideDevice *d)
	:IClientCommand(p,d)
{
}

bool AddSensorCommand::evalCommand()
{
	if(parser.getArgs().count()!=4)
	{
		//TODO help
		return false;
	}
	int nForLastNValues=1;
	ARpcISensorStorage::StoreMode mode=ARpcISensorStorage::storeModeFromString(parser.getArgs()[2]);
	if(mode==ARpcISensorStorage::BAD_MODE)
	{
		//TODO help
		return false;
	}
	else if(mode==ARpcISensorStorage::LAST_N_VALUES&&parser.hasVar("N"))
	{
		bool ok=false;
		nForLastNValues=parser.getVarSingle("N").toInt(&ok);
		if(!ok||nForLastNValues==0)nForLastNValues=1;
	}
	ARpcISensorStorage::TimestampRule tsRule;
	if(!ARpcISensorStorage::timestampRuleFromString(parser.getArgs()[3],tsRule))
	{
		//TODO help
		return false;
	}
	if(mode==ARpcISensorStorage::LAST_N_VALUES)
		return dev->writeMsg(IClientCommand::addSensorCommand,parser.getArgs()+
			QStringList(QString::number(nForLastNValues)));
	else return dev->writeMsg(IClientCommand::addSensorCommand,parser.getArgs());
}
