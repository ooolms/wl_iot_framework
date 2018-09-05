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
#include "ARpcStorages/ARpcAllStorages.h"
#include "ARpcBase/ARpcServerConfig.h"

StoragesCommands::StoragesCommands(ARpcOutsideDevice *d,IotProxyCommandProcessor *p)
	:ICommand(d,p)
{
}

QByteArrayList StoragesCommands::storageToMsgArguments(ARpcISensorStorage *s)
{
	QByteArray mode=ARpcISensorStorage::storeModeToString(s->storeMode());
	QByteArray tsRule=ARpcISensorStorage::timestampRuleToString(s->timestampRule());
	QByteArray sensorValuesType=s->sensor().type.toString();
	QByteArray effectiveValuesType=s->storedValuesType().toString();
	QByteArray constraintsStr;
	for(auto i=s->sensor().attributes.begin();i!=s->sensor().attributes.end();++i)
		constraintsStr+=i.key()+"="+i.value()+";";
	constraintsStr.chop(1);
	return QByteArrayList()<<s->deviceId().toByteArray()<<s->deviceName()<<s->sensor().name<<sensorValuesType<<
		constraintsStr<<mode<<tsRule<<effectiveValuesType;
}

bool StoragesCommands::processCommand(const QByteArray &cmd,const QByteArrayList &args,QByteArrayList &retVal)
{
	if(cmd=="list_storages")
		return listStorages(retVal);
	else if(cmd=="add_storage")
		return addStorage(args,retVal);
	else if(cmd=="add_storage_manual")
		return addStorageManual(args,retVal);
	else if(cmd=="remove_storage")
		return removeStorage(args,retVal);
	else if(cmd=="session_list")
		return listSessions(args,retVal);
	return false;
}

QByteArrayList StoragesCommands::acceptedCommands()
{
	return QByteArrayList()<<"list_storages"<<"add_storage"<<"remove_storage"<<"session_list"<<"add_storage_manual";
}

bool StoragesCommands::listStorages(QByteArrayList &retVal)
{
	QList<ARpcStorageId> sensors;
	ARpcLocalDatabase *localDb=IotProxyInstance::inst().sensorsStorage();
	if(!localDb->listSensors(sensors))
	{
		retVal.append("error accessing database");
		return false;
	}
	for(ARpcStorageId &id:sensors)
	{
		ARpcISensorStorage *stor=localDb->existingStorage(id);
		if(!stor)continue;
		writeCmdataMsg(storageToMsgArguments(stor));
	}
	return true;
}

bool StoragesCommands::addStorage(const QByteArrayList &args,QByteArrayList &retVal)
{
	if(args.count()<4||args[0].isEmpty()||args[1].isEmpty())
	{
		retVal.append(StandardErrors::invalidAgruments);
		return false;
	}
	QByteArray devIdOrName=args[0];
	QByteArray sensorName=args[1];
	ARpcISensorStorage::StoreMode mode=ARpcISensorStorage::storeModeFromString(args[2]);
	int valuesCount=1;
	if(mode==ARpcISensorStorage::LAST_N_VALUES||mode==ARpcISensorStorage::LAST_N_VALUES_IN_MEMORY)
	{
		if(args.count()<5)
		{
			retVal.append(StandardErrors::invalidAgruments);
			return false;
		}
		bool ok=false;
		valuesCount=args[4].toInt(&ok);
		if((!ok)||valuesCount==0)
		{
			retVal.append(StandardErrors::invalidAgruments);
			return false;
		}
	}
	ARpcISensorStorage::TimestampRule tsRule;
	if(!ARpcISensorStorage::timestampRuleFromString(args[3],tsRule))
	{
		retVal.append(StandardErrors::invalidAgruments);
		return false;
	}
	ARpcRealDevice *dev=IotProxyInstance::inst().devices()->deviceByIdOrName(devIdOrName);
	if(!dev)
	{
		retVal.append(QByteArray(StandardErrors::noDeviceFound).replace("%1",devIdOrName));
		return false;
	}
	QList<ARpcSensorDef> sensors;
	if(!dev->getSensorsDescription(sensors))
	{
		retVal.append("no sensor for device");
		return false;
	}
	ARpcSensorDef sensor;
	bool sensorFound=false;
	for(ARpcSensorDef &s:sensors)
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
	ARpcLocalDatabase *localSensorsDb=IotProxyInstance::inst().sensorsStorage();
	ARpcISensorStorage *stor=localSensorsDb->create(dev->id(),dev->name(),mode,sensor,tsRule,valuesCount);
	if(!stor)
	{
		retVal.append("can't create storage");
		return false;
	}
	return true;
}

bool StoragesCommands::addStorageManual(const QByteArrayList &args,QByteArrayList &retVal)
{
	if(args.count()<6||args[0].isEmpty()||args[1].isEmpty())
	{
		retVal.append(StandardErrors::invalidAgruments);
		return false;
	}
	QByteArray devIdOrName=args[0];
	ARpcSensorDef sensor;
	sensor.name=args[1];
	if(!sensor.type.fromString(args[2]))
	{
		retVal.append(StandardErrors::invalidAgruments);
		return false;
	}
	quint32 dims=args[3].toUInt();
	if(dims==0)dims=1;
	sensor.attributes["dims"]=QByteArray::number(dims);
	ARpcISensorStorage::StoreMode mode=ARpcISensorStorage::storeModeFromString(args[4]);
	int nForLastNValues=1;
	if(mode==ARpcISensorStorage::LAST_N_VALUES)
	{
		if(args.count()<7)
		{
			retVal.append(StandardErrors::invalidAgruments);
			return false;
		}
		bool ok=false;
		nForLastNValues=args[6].toInt(&ok);
		if((!ok)||nForLastNValues==0)
		{
			retVal.append(StandardErrors::invalidAgruments);
			return false;
		}
	}
	ARpcISensorStorage::TimestampRule tsRule;
	if(!ARpcISensorStorage::timestampRuleFromString(args[5],tsRule))
	{
		retVal.append(StandardErrors::invalidAgruments);
		return false;
	}
	ARpcRealDevice *dev=IotProxyInstance::inst().devices()->deviceByIdOrName(devIdOrName);
	if(!dev)
	{
		retVal.append(QByteArray(StandardErrors::noDeviceFound).replace("%1",devIdOrName));
		return false;
	}
	ARpcLocalDatabase *localSensorsDb=IotProxyInstance::inst().sensorsStorage();
	ARpcISensorStorage *stor=localSensorsDb->create(dev->id(),dev->name(),mode,sensor,tsRule,nForLastNValues);
	if(!stor)
	{
		retVal.append("can't create storage");
		return false;
	}
	return true;
}

bool StoragesCommands::removeStorage(const QByteArrayList &args,QByteArrayList &retVal)
{
	if(args.count()<2||args[0].isEmpty()||args[1].isEmpty())
	{
		retVal.append(StandardErrors::invalidAgruments);
		return false;
	}
	QByteArray devIdOrName=args[0];
	QByteArray sensorName=args[1];
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

bool StoragesCommands::listSessions(const QByteArrayList &args,QByteArrayList &retVal)
{
	if(args.count()<2||args[0].isEmpty()||args[1].isEmpty())
	{
		retVal.append(StandardErrors::invalidAgruments);
		return false;
	}
	QByteArray devIdOrName=args[0];
	QByteArray sensorName=args[1];
	ARpcLocalDatabase *localSensorsDb=IotProxyInstance::inst().sensorsStorage();
	QUuid devId;
	ARpcISensorStorage *st=localSensorsDb->findStorageForDevice(devIdOrName,sensorName,devId);
	if(!st)
	{
		retVal.append("no storage found");
		return false;
	}
	if(st->storeMode()!=ARpcISensorStorage::AUTO_SESSIONS&&st->storeMode()!=ARpcISensorStorage::MANUAL_SESSIONS)
	{
		retVal.append("not a session storage");
		return false;
	}
	ARpcSessionStorage *sSt=reinterpret_cast<ARpcSessionStorage*>(st);
	QList<QUuid> ids;
	QByteArrayList titles;
	sSt->listSessions(ids,titles);
	for(int i=0;i<ids.count();++i)
		writeCmdataMsg(QByteArrayList()<<ids[i].toByteArray()<<titles[i]);
	return true;
}
