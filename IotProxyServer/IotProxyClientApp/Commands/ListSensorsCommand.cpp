#include "ListSensorsCommand.h"
#include "../StdQFile.h"
#include "ARpcBase/ARpcSensor.h"
#include <QCoreApplication>
#include <QDebug>

ListSensorsCommand::ListSensorsCommand(const CmdArgParser &p,ARpcOutsideDevice *d)
	:IClientCommand(p,d)
{
	connect(d,&ARpcOutsideDevice::rawMessage,this,&ListSensorsCommand::onRawMessage);
}

bool ListSensorsCommand::evalCommand()
{
	if(parser.getArgs().count()!=1)
	{
		//TODO help
		return false;
	}
	return dev->writeMsg(IClientCommand::listSensorsCommand,parser.getArgs());
}

void ListSensorsCommand::onRawMessage(const ARpcMessage &m)
{
	if(m.title==ARpcConfig::srvCmdDataMsg)
	{
		if(m.args.count()<1)return;
		QList<ARpcSensor> sensors;
		if(!ARpcSensor::parseXmlDescription(m.args[0],sensors))
		{
			QDebug(StdQFile::inst().stderr())<<"error: can't parse sensors xml";
			qApp->exit(1);
		}
		QDebug d(StdQFile::inst().stderr());
		for(ARpcSensor &s:sensors)
		{
			d<<"Sensor:";
			d<<"\n\tName:"<<s.name;
			d<<"\n\tType:"<<ARpcSensor::typeToString(s.type);
			d<<"\n\tConstrains:"<<s.constraints;
		}
	}
}
