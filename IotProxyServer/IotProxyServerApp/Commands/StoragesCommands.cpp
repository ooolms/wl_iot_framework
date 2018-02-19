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

QByteArrayList StoragesCommands::storageToMsgArguments(ARpcISensorStorage *s)
{
	QByteArray mode=ARpcISensorStorage::storeModeToString(s->getStoreMode());
	QByteArray tsRule=ARpcISensorStorage::timestampRuleToString(s->getTimestampRule());
	QByteArray sensorValuesType=ARpcSensor::typeToString(s->sensor().type);
	QByteArray effectiveValuesType=ARpcSensor::typeToString(s->effectiveValuesType());
	QByteArray constraintsStr;
	for(auto i=s->sensor().constraints.begin();i!=s->sensor().constraints.end();++i)
		constraintsStr+=i.key()+"="+i.value()+";";
	constraintsStr.chop(1);
	return QByteArrayList()<<s->deviceId().toByteArray()<<s->deviceName()<<s->sensor().name<<sensorValuesType<<
		constraintsStr<<mode<<tsRule<<effectiveValuesType;
}

bool StoragesCommands::processCommand(const ARpcMessage &m,QByteArrayList &retVal)
{
	if(m.title=="list_storages")
		return listStorages(m,retVal);
	else if(m.title=="add_storage")
		return addStorage(m,retVal);
	else if(m.title=="remove_storage")
		return removeStorage(m,retVal);
	else if(m.title=="session_list")
		return listSessions(m,retVal);
	return false;
}

QByteArrayList StoragesCommands::acceptedCommands()
{
	return QByteArrayList()<<"list_storages"<<"add_storage"<<"remove_storage"<<"session_list";
}

bool StoragesCommands::listStorages(const ARpcMessage &m, QByteArrayList &retVal)
{
	Q_UNUSED(m)
	QList<DeviceStorageId> sensors;
	ARpcLocalDatabase *localDb=IotProxyInstance::inst().sensorsStorage();
	if(!localDb->listSensors(sensors))
	{
		retVal.append("error accessing database");
		return false;
	}
	for(DeviceStorageId &id:sensors)
	{
		ARpcISensorStorage *stor=localDb->existingStorage(id);
		if(!stor)continue;
		clientDev->writeMsg(ARpcServerConfig::srvCmdDataMsg,storageToMsgArguments(stor));
	}
	return true;
}

bool StoragesCommands::addStorage(const ARpcMessage &m, QByteArrayList &retVal)
{
	if(m.args.count()<4||m.args[0].isEmpty()||m.args[1].isEmpty())
	{
		retVal.append(StandardErrors::invalidAgruments);
		return false;
	}
	QByteArray devIdOrName=m.args[0];
	QByteArray sensorName=m.args[1];
	ARpcISensorStorage::StoreMode mode=ARpcISensorStorage::storeModeFromString(m.args[2]);
	int nForLastNValues=1;
	if(mode==ARpcISensorStorage::LAST_N_VALUES)
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
	ARpcRealDevice *dev=IotProxyInstance::inst().devices()->deviceByIdOrName(devIdOrName);
	if(!dev)
	{
		retVal.append(QByteArray(StandardErrors::noDeviceWithId).replace("%1",devIdOrName));
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
	ARpcISensorStorage *stor=localSensorsDb->create(dev->id(),dev->name(),mode,sensor,tsRule,nForLastNValues);
	if(!stor)
	{
		retVal.append("can't create storage");
		return false;
	}
	return true;
}

bool StoragesCommands::removeStorage(const ARpcMessage &m, QByteArrayList &retVal)
{
	if(m.args.count()<2||m.args[0].isEmpty()||m.args[1].isEmpty())
	{
		retVal.append(StandardErrors::invalidAgruments);
		return false;
	}
	QByteArray devIdOrName=m.args[0];
	QByteArray sensorName=m.args[1];
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

bool StoragesCommands::listSessions(const ARpcMessage &m, QByteArrayList &retVal)
{
	if(m.args.count()<2||m.args[0].isEmpty()||m.args[1].isEmpty())
	{
		retVal.append(StandardErrors::invalidAgruments);
		return false;
	}
	QByteArray devIdOrName=m.args[0];
	QByteArray sensorName=m.args[1];
	ARpcLocalDatabase *localSensorsDb=IotProxyInstance::inst().sensorsStorage();
	QUuid devId;
	ARpcISensorStorage *st=localSensorsDb->findStorageForDevice(devIdOrName,sensorName,devId);
	if(!st)
	{
		retVal.append("no storage found");
		return false;
	}
	if(st->getStoreMode()!=ARpcISensorStorage::AUTO_SESSIONS&&st->getStoreMode()!=ARpcISensorStorage::MANUAL_SESSIONS)
	{
		retVal.append("not a session storage");
		return false;
	}
	ARpcSessionStorage *sSt=(ARpcSessionStorage*)st;
	QList<QUuid> ids;
	QByteArrayList titles;
	sSt->listSessions(ids,titles);
	for(int i=0;i<ids.count();++i)
		clientDev->writeMsg(ARpcServerConfig::srvCmdDataMsg,QByteArrayList()<<ids[i].toByteArray()<<titles[i]);
	return true;
}
