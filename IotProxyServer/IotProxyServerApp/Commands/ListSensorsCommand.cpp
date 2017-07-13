#include "ListSensorsCommand.h"
#include "../IotProxyInstance.h"
#include "StandardErrors.h"

ListSensorsCommand::ListSensorsCommand(ARpcOutsideDevice *d)
	:ICommand(d)
{
}

bool ListSensorsCommand::processCommand(const ARpcMessage &m)
{
	if(m.title=="list_sensors")
	{
		if(m.args.count()<1)
		{
			lastErrorStr=StandardErrors::invalidAgruments;
			return false;
		}
		ARpcDevice *dev=IotProxyInstance::inst().deviceByIdOrName(m.args[0]);
		if(dev==0)
		{
			lastErrorStr=StandardErrors::noDeviceWithId.arg(m.args[0]);
			return false;
		}
		QList<ARpcSensor> sensors;
		if(!dev->getSensorsDescription(sensors))
		{
			lastErrorStr="can't get sensors from device";
			return false;
		}
		QString xmlData;
		ARpcSensor::dumpToXml(xmlData,sensors);
		clientDev->writeMsg(ARpcConfig::srvCmdDataMsg,QStringList()<<xmlData);
		return true;
	}
	return false;
}

QStringList ListSensorsCommand::acceptedCommands()
{
	return QStringList()<<"list_sensors";
}

