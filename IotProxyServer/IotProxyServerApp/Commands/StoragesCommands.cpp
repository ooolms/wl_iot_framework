#include "StoragesCommands.h"
#include "../IotProxyInstance.h"
#include "StandardErrors.h"
#include "ARpcBase/ARpcAllSensorValues.h"
#include "ARpcLocalStorage/ARpcAllStorages.h"

StoragesCommands::StoragesCommands(ARpcOutsideDevice *d)
	:ICommand(d)
{
}

bool StoragesCommands::processCommand(const ARpcMessage &m)
{
	if(m.title=="list_storages")
		return listStorages(m);
	else if(m.title=="add_sensor")
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

bool StoragesCommands::addSensor(const ARpcMessage &m)
{
	if(m.args.count()<4||m.args[0].isEmpty()||m.args[1].isEmpty())
	{
		lastErrorStr=StandardErrors::invalidAgruments;
		return false;
	}
	QString devIdOrName=m.args[0];
	QString sensorName=m.args[1];
	ARpcISensorStorage::StoreMode mode=ARpcISensorStorage::storeModeFromString(m.args[2]);
	int nForLastNValues=1;
	if(mode==ARpcISensorStorage::MANUAL_SESSIONS)
	{
		lastErrorStr=StandardErrors::invalidAgruments;
		return false;
	}
	else if(mode==ARpcISensorStorage::LAST_N_VALUES)
	{
		if(m.args.count()<5)
		{
			lastErrorStr=StandardErrors::invalidAgruments;
			return false;
		}
		bool ok=false;
		nForLastNValues=m.args[4].toInt(&ok);
		if((!ok)||nForLastNValues==0)
		{
			lastErrorStr=StandardErrors::invalidAgruments;
			return false;
		}
	}
	ARpcISensorStorage::TimestampRule tsRule;
	if(!ARpcISensorStorage::timestampRuleFromString(m.args[3],tsRule))
	{
		lastErrorStr=StandardErrors::invalidAgruments;
		return false;
	}
	ARpcDevice *dev=IotProxyInstance::inst().deviceByIdOrName(devIdOrName);
	if(!dev)
	{
		lastErrorStr=StandardErrors::noDeviceWithId.arg(devIdOrName);
		return false;
	}
	QList<ARpcSensor> sensors;
	if(!dev->getSensorsDescription(sensors))
	{
		lastErrorStr="no sensor for device";
		return false;
	}
	ARpcSensor sensor;
	bool sensorFound=false;
	for(ARpcSensor &s:sensors)
	{
		if(s.name==sensorName)
		{
			sensor=s;
			sensorFound=true;
			break;
		}
	}
	if(!sensorFound)
	{
		lastErrorStr="no sensor for device";
		return false;
	}
	quint32 dims=1;
	if(sensor.constraints.contains("dims"))dims=sensor.constraints["dims"].toUInt();
	if(dims==0)dims=1;
	ARpcLocalDatabase *localSensorsDb=IotProxyInstance::inst().getSensorsDb();
	DeviceAndSensorId id={dev->id(),sensorName};
	ARpcISensorStorage *stor=localSensorsDb->preCreate(id,mode,sensor.type,tsRule);
	if(!stor)
	{
		lastErrorStr="can't create storage";
		return false;
	}
	if(mode==ARpcISensorStorage::CONTINUOUS)
	{
		if(sensor.type==ARpcSensor::SINGLE)
			((ARpcContinuousStorage*)stor)->createAsFixedBlocksDb(ARpcSingleSensorValue(dims),true);
		else if(sensor.type==ARpcSensor::SINGLE_LT)
			((ARpcContinuousStorage*)stor)->createAsFixedBlocksDb(ARpcSingleSensorValue(dims,true),true);
		else if(sensor.type==ARpcSensor::SINGLE_GT)
			((ARpcContinuousStorage*)stor)->createAsFixedBlocksDb(ARpcSingleSensorValue(dims,false),true);
		else ((ARpcContinuousStorage*)stor)->createAsChainedBlocksDb(true);
	}
	else if(mode==ARpcISensorStorage::AUTO_SESSIONS||mode==ARpcISensorStorage::MANUAL_SESSIONS)
	{
		if(sensor.type==ARpcSensor::SINGLE)
			((ARpcSessionStorage*)stor)->createAsFixedBlocksDb(ARpcSingleSensorValue(dims),true);
		else if(sensor.type==ARpcSensor::SINGLE_LT)
			((ARpcSessionStorage*)stor)->createAsFixedBlocksDb(ARpcSingleSensorValue(dims,true),true);
		else if(sensor.type==ARpcSensor::SINGLE_GT)
			((ARpcSessionStorage*)stor)->createAsFixedBlocksDb(ARpcSingleSensorValue(dims,false),true);
		else ((ARpcSessionStorage*)stor)->createAsChainedBlocksDb(true);
	}
	else //last N values
	{
		if(sensor.type==ARpcSensor::TEXT)
			((ARpcLastNValuesStorage*)stor)->create(nForLastNValues,ARpcTextSensorValue());
		else if(sensor.type==ARpcSensor::SINGLE)
			((ARpcLastNValuesStorage*)stor)->create(nForLastNValues,ARpcSingleSensorValue(dims));
		else if(sensor.type==ARpcSensor::SINGLE_LT)
			((ARpcLastNValuesStorage*)stor)->create(nForLastNValues,ARpcSingleSensorValue(dims,true));
		else if(sensor.type==ARpcSensor::SINGLE_GT)
			((ARpcLastNValuesStorage*)stor)->create(nForLastNValues,ARpcSingleSensorValue(dims,false));
		else if(sensor.type==ARpcSensor::PACKET)
			((ARpcLastNValuesStorage*)stor)->create(nForLastNValues,ARpcPacketSensorValue(dims));
		else if(sensor.type==ARpcSensor::PACKET_LT)
			((ARpcLastNValuesStorage*)stor)->create(nForLastNValues,ARpcPacketSensorValue(dims,true));
		else if(sensor.type==ARpcSensor::PACKET_GT)
			((ARpcLastNValuesStorage*)stor)->create(nForLastNValues,ARpcPacketSensorValue(dims,false));
	}
	stor->setDeviceName(dev->name());
	localSensorsDb->creationFinished(id);
	return true;
}

bool StoragesCommands::removeSensor(const ARpcMessage &m)
{
	if(m.args.count()<2||m.args[0].isEmpty()||m.args[1].isEmpty())
	{
		lastErrorStr=StandardErrors::invalidAgruments;
		return false;
	}
	QString devIdOrName=m.args[0];
	QString sensorName=m.args[1];
	ARpcLocalDatabase *localSensorsDb=IotProxyInstance::inst().getSensorsDb();
	QList<DeviceAndSensorId> ids;
	if(!localSensorsDb->listSensors(ids))
	{
		lastErrorStr="no storage found";
		return false;
	}
	for(DeviceAndSensorId &id:ids)
	{
		ARpcISensorStorage *stor=localSensorsDb->existingStorage(id);
		if(!stor)continue;
		if((id.deviceId.toString()==devIdOrName||stor->getDeviceName()==devIdOrName)&&id.sensorName==sensorName)
		{
			localSensorsDb->removeStorage(id);
			return true;
		}
	}
	lastErrorStr="no storage found";
	return false;
}
