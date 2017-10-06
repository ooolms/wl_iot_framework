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

#include "StoragesCommands.h"
#include "../IotProxyInstance.h"
#include "StandardErrors.h"
#include "ARpcBase/ARpcAllSensorValues.h"
#include "ARpcLocalStorage/ARpcAllStorages.h"
#include "ARpcBase/ARpcServerConfig.h"

StoragesCommands::StoragesCommands(ARpcOutsideDevice *d,IotProxyCommandProcessor *p)
	:ICommand(d,p)
{
}

bool StoragesCommands::processCommand(const ARpcMessage &m,QStringList &retVal)
{
	if(m.title=="list_storages")
		return listStorages(m,retVal);
	else if(m.title=="add_sensor")
		return addSensor(m,retVal);
	else if(m.title=="remove_sensor")
		return removeSensor(m,retVal);
	return false;
}

QStringList StoragesCommands::acceptedCommands()
{
	return QStringList()<<"list_storages"<<"add_sensor"<<"remove_sensor";
}

bool StoragesCommands::listStorages(const ARpcMessage &m,QStringList &retVal)
{
	Q_UNUSED(m)
	QList<DeviceAndSensorId> sensors;
	ARpcLocalDatabase *localDb=IotProxyInstance::inst().sensorsStorage();
	if(!localDb->listSensors(sensors))
	{
		retVal.append("error accessing database");
		return false;
	}
	for(DeviceAndSensorId &id:sensors)
	{
		ARpcISensorStorage *stor=localDb->existingStorage(id);
		if(!stor)continue;
		QString mode=ARpcISensorStorage::storeModeToString(stor->getStoreMode());
		QString tsRule=ARpcISensorStorage::timestampRuleToString(stor->getTimestampRule());
		QString sensorValuesType=ARpcSensor::typeToString(stor->sensor().type);
		QString constraintsStr;
		for(auto i=stor->sensor().constraints.begin();i!=stor->sensor().constraints.end();++i)
			constraintsStr+=i.key()+"="+i.value()+";";
		constraintsStr.chop(1);
		clientDev->writeMsg(ARpcServerConfig::srvCmdDataMsg,QStringList()<<id.deviceId.toString()<<
			stor->deviceName()<<id.sensorName<<sensorValuesType<<constraintsStr<<mode<<tsRule);
	}
	return true;
}

bool StoragesCommands::addSensor(const ARpcMessage &m,QStringList &retVal)
{
	if(m.args.count()<4||m.args[0].isEmpty()||m.args[1].isEmpty())
	{
		retVal.append(StandardErrors::invalidAgruments);
		return false;
	}
	QString devIdOrName=m.args[0];
	QString sensorName=m.args[1];
	ARpcISensorStorage::StoreMode mode=ARpcISensorStorage::storeModeFromString(m.args[2]);
	int nForLastNValues=1;
	if(mode==ARpcISensorStorage::MANUAL_SESSIONS)
	{
		retVal.append(StandardErrors::invalidAgruments);
		return false;
	}
	else if(mode==ARpcISensorStorage::LAST_N_VALUES)
	{
		if(m.args.count()<5)
		{
			retVal.append(StandardErrors::invalidAgruments);
			return false;
		}
		bool ok=false;
		nForLastNValues=m.args[4].toInt(&ok);
		if((!ok)||nForLastNValues==0)
		{
			retVal.append(StandardErrors::invalidAgruments);
			return false;
		}
	}
	ARpcISensorStorage::TimestampRule tsRule;
	if(!ARpcISensorStorage::timestampRuleFromString(m.args[3],tsRule))
	{
		retVal.append(StandardErrors::invalidAgruments);
		return false;
	}
	ARpcRealDevice *dev=IotProxyInstance::inst().deviceByIdOrName(devIdOrName);
	if(!dev)
	{
		retVal.append(StandardErrors::noDeviceWithId.arg(devIdOrName));
		return false;
	}
	QList<ARpcSensor> sensors;
	if(!dev->getSensorsDescription(sensors))
	{
		retVal.append("no sensor for device");
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
		retVal.append("no sensor for device");
		return false;
	}
	quint32 dims=1;
	if(sensor.constraints.contains("dims"))dims=sensor.constraints["dims"].toUInt();
	if(dims==0)dims=1;
	ARpcLocalDatabase *localSensorsDb=IotProxyInstance::inst().sensorsStorage();
	DeviceAndSensorId id={dev->id(),sensorName};
	ARpcISensorStorage *stor=localSensorsDb->create(id,dev->name(),mode,sensor,tsRule,nForLastNValues);
	if(!stor)
	{
		retVal.append("can't create storage");
		return false;
	}
	return true;
}

bool StoragesCommands::removeSensor(const ARpcMessage &m,QStringList &retVal)
{
	if(m.args.count()<2||m.args[0].isEmpty()||m.args[1].isEmpty())
	{
		retVal.append(StandardErrors::invalidAgruments);
		return false;
	}
	QString devIdOrName=m.args[0];
	QString sensorName=m.args[1];
	ARpcLocalDatabase *localSensorsDb=IotProxyInstance::inst().sensorsStorage();
	QUuid devId;
	ARpcISensorStorage *st=localSensorsDb->findStorageForDevice(devIdOrName,sensorName,devId);
	if(!st)
	{
		retVal.append("no storage found");
		return false;
	}
	localSensorsDb->removeStorage({devId,sensorName});
	return true;
}
