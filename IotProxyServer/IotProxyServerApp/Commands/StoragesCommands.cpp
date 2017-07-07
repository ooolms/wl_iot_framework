#include "StoragesCommands.h"
#include "../IotProxyInstance.h"

StoragesCommands::StoragesCommands(ARpcOutsideDevice *d)
	:ICommand(d)
{
}

bool StoragesCommands::processCommand(const ARpcMessage &m)
{
	if(m.title=="list_storages")
		return listStorages(m);
	else if(m.title=="add_sensors")
		return addSensor(m);
	else if(m.title=="remove_sensor")
		return removeSensor(m);
	return false;
}

QStringList StoragesCommands::acceptedCommands()
{
	return QStringList()<<"list_storages"<<"add_sensor"<<"remove_sensor";
}

bool StoragesCommands::listStorages(const ARpcMessage &m)
{
	QList<DeviceAndSensorId> sensors;
	ARpcLocalDatabase *localDb=IotProxyInstance::inst().getSensorsDb();
	if(!localDb->listSensors(sensors))
	{
		lastErrorStr="error accessing database";
		return false;
	}
	for(DeviceAndSensorId &id:sensors)
	{
		ARpcISensorStorage *stor=localDb->existingStorage(id);
		if(!stor)continue;
		QString mode=ARpcISensorStorage::storeModeToString(stor->getStoreMode());
		QString tsRule=ARpcISensorStorage::timestampRuleToString(stor->getTimestampRule());
		QString sensorValuesType=ARpcSensor::typeToString(stor->sensorValuesType());
		clientDev->writeMsg(ARpcConfig::srvCmdDataMsg,QStringList()<<id.deviceId.toString()<<
			stor->getDeviceName()<<id.sensorName<<mode<<tsRule<<sensorValuesType);
	}
	return true;
}
