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
#include "../IotProxyConfig.h"
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

bool StoragesCommands::processCommand(CallContext &ctx)
{
	if(ctx.cmd=="list_storages")
		return listStorages(ctx);
	else if(ctx.cmd=="add_storage")
		return addStorage(ctx);
	else if(ctx.cmd=="remove_storage")
		return removeStorage(ctx);
	else if(ctx.cmd=="storage_add_data_export")
		return addDataExport(ctx);
	else if(ctx.cmd=="storage_get_data_export")
		return getDataExport(ctx);
	else if(ctx.cmd=="storage_get_data_export_list")
		return allDataExports(ctx);
	else if(ctx.cmd=="storage_get_attr")
		return getAttr(ctx);
	else if(ctx.cmd=="storage_set_attr")
		return setAttr(ctx);
	return false;
}

QByteArrayList StoragesCommands::acceptedCommands()
{
	return QByteArrayList()<<"list_storages"<<"add_storage"<<"remove_storage"<<
		"storage_add_data_export"<<"storage_get_attr"<<"storage_set_attr"<<"storage_get_data_export"<<
		"storage_get_data_export_list";
}

bool StoragesCommands::listStorages(CallContext &ctx)
{
	QList<ARpcStorageId> sensors;
	ARpcFSStoragesDatabase *localDb=IotProxyInstance::inst().storages();
	if(!localDb->listStorages(sensors))
	{
		ctx.retVal.append("error accessing database");
		return false;
	}
	for(ARpcStorageId &id:sensors)
	{
		ARpcISensorStorage *stor=localDb->existingStorage(id);
		if(!stor)continue;
		if(!IotProxyConfig::accessManager.userCanAccessDevice(
			id.deviceId,proc->uid(),DevicePolicyActionFlag::READ_STORAGES))
				continue;
		writeCmdataMsg(ctx.callId,storageToMsgArguments(stor));
	}
	return true;
}

bool StoragesCommands::addStorage(CallContext &ctx)
{
	if(ctx.args.count()<4||ctx.args[0].isEmpty()||ctx.args[1].isEmpty())
	{
		ctx.retVal.append(StandardErrors::invalidAgruments);
		return false;
	}
	QByteArray devIdOrName=ctx.args[0];
	QByteArray sensorName=ctx.args[1];
	ARpcISensorStorage::StoreMode mode=ARpcISensorStorage::storeModeFromString(ctx.args[2]);
	int valuesCount=1;
	if(mode==ARpcISensorStorage::LAST_N_VALUES||mode==ARpcISensorStorage::LAST_N_VALUES_IN_MEMORY)
	{
		if(ctx.args.count()<5)
		{
			ctx.retVal.append(StandardErrors::invalidAgruments);
			return false;
		}
		bool ok=false;
		valuesCount=ctx.args[4].toInt(&ok);
		if((!ok)||valuesCount==0)
		{
			ctx.retVal.append(StandardErrors::invalidAgruments);
			return false;
		}
	}
	ARpcISensorStorage::TimestampRule tsRule;
	if(!ARpcISensorStorage::timestampRuleFromString(ctx.args[3],tsRule))
	{
		ctx.retVal.append(StandardErrors::invalidAgruments);
		return false;
	}
	ARpcRealDevice *dev=IotProxyInstance::inst().devices()->deviceByIdOrName(devIdOrName);
	if(!dev||!dev->isConnected())
	{
		ctx.retVal.append(QByteArray(StandardErrors::noDeviceFound).replace("%1",devIdOrName));
		return false;
	}
	if(!IotProxyConfig::accessManager.userCanAccessDevice(dev->id(),proc->uid(),DevicePolicyActionFlag::SETUP_STORAGES))
	{
		ctx.retVal.append(QByteArray(StandardErrors::noDeviceFound).replace("%1",devIdOrName));
		return false;
	}
	QList<ARpcSensorDef> sensors;
	if(!dev->getSensorsDescription(sensors))
	{
		ctx.retVal.append("no sensor for device");
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
		ctx.retVal.append("no sensor for device");
		return false;
	}
	ARpcFSStoragesDatabase *localSensorsDb=IotProxyInstance::inst().storages();
	ARpcISensorStorage *stor=localSensorsDb->create(dev->id(),dev->name(),mode,sensor,tsRule,valuesCount);
	if(!stor)
	{
		ctx.retVal.append("can't create storage");
		return false;
	}
	return true;
}

bool StoragesCommands::removeStorage(CallContext &ctx)
{
	if(ctx.args.count()<2||ctx.args[0].isEmpty()||ctx.args[1].isEmpty())
	{
		ctx.retVal.append(StandardErrors::invalidAgruments);
		return false;
	}
	QByteArray devIdOrName=ctx.args[0];
	QByteArray sensorName=ctx.args[1];
	ARpcFSStoragesDatabase *localSensorsDb=IotProxyInstance::inst().storages();
	QUuid devId;
	ARpcISensorStorage *st=localSensorsDb->findStorageForDevice(devIdOrName,sensorName,devId);
	if(!st)
	{
		ctx.retVal.append("no storage found");
		return false;
	}
	if(!IotProxyConfig::accessManager.userCanAccessDevice(devId,proc->uid(),DevicePolicyActionFlag::SETUP_STORAGES))
	{
		ctx.retVal.append("no storage found");
		return false;
	}
	localSensorsDb->removeStorage({devId,sensorName});
	return true;
}

bool StoragesCommands::addDataExport(CallContext &ctx)
{
	if(ctx.args.count()<3)
	{
		ctx.retVal.append(StandardErrors::invalidAgruments);
		return false;
	}
	QByteArray devNameOrId=ctx.args[0];
	QByteArray sensorName=ctx.args[1];
	QByteArray serviceType=ctx.args[2];
	ARpcISensorStorage::DataExportConfig cfg;
	for(int i=3;i<ctx.args.count();++i)
	{
		QByteArray arg=ctx.args[i];
		int ind=arg.indexOf(':');
		if(ind==-1)continue;
		QByteArray n=arg.mid(0,ind);
		QByteArray v=arg.mid(ind+1);
		if(n.isEmpty()||v.isEmpty())continue;
		cfg[n]=v;
	}
	QUuid devId;
	ARpcISensorStorage *st=IotProxyInstance::inst().storages()->findStorageForDevice(
		devNameOrId,sensorName,devId);
	if(!st)
	{
		ctx.retVal.append("no storage found");
		return false;
	}
	if(!IotProxyConfig::accessManager.userCanAccessDevice(devId,proc->uid(),DevicePolicyActionFlag::SETUP_STORAGES))
	{
		ctx.retVal.append("no storage found");
		return false;
	}
	if(cfg.isEmpty())
		st->removeDataExportConfig(serviceType);
	else
	{
		QScopedPointer<ISensorDataTranslator> tr(ISensorDataTranslator::makeTranslator(
			serviceType,st->deviceId(),st->sensor(),cfg));
		if(!tr.data()||!tr.data()->checkConfig(cfg))
		{
			ctx.retVal.append(StandardErrors::invalidAgruments);
			return false;
		}
		st->addDataExportConfig(serviceType,cfg);
	}
	DataCollectionUnit *unit=IotProxyInstance::inst().collectionUnit(devId,sensorName);
	if(unit)unit->setupSensorDataTranslators();
	return true;
}

bool StoragesCommands::getDataExport(CallContext &ctx)
{
	if(ctx.args.count()<3)
	{
		ctx.retVal.append(StandardErrors::invalidAgruments);
		return false;
	}
	QByteArray devNameOrId=ctx.args[0];
	QByteArray sensorName=ctx.args[1];
	QByteArray serviceType=ctx.args[2];
	ARpcISensorStorage::DataExportConfig cfg;
	QUuid devId;
	ARpcISensorStorage *st=IotProxyInstance::inst().storages()->findStorageForDevice(
		devNameOrId,sensorName,devId);
	if(!st)
	{
		ctx.retVal.append("no storage found");
		return false;
	}
	if(!IotProxyConfig::accessManager.userCanAccessDevice(devId,proc->uid(),DevicePolicyActionFlag::SETUP_STORAGES))
	{
		ctx.retVal.append("no storage found");
		return false;
	}
	cfg=st->getDataExportConfig(serviceType);
	for(auto i=cfg.begin();i!=cfg.end();++i)
		ctx.retVal.append(i.key()+":"+i.value());
	return true;
}

bool StoragesCommands::allDataExports(CallContext &ctx)
{
	if(ctx.args.count()<2)
	{
		ctx.retVal.append(StandardErrors::invalidAgruments);
		return false;
	}
	QByteArray devNameOrId=ctx.args[0];
	QByteArray sensorName=ctx.args[1];
	QUuid devId;
	ARpcISensorStorage *st=IotProxyInstance::inst().storages()->findStorageForDevice(
		devNameOrId,sensorName,devId);
	if(!st)
	{
		ctx.retVal.append("no storage found");
		return false;
	}
	if(!IotProxyConfig::accessManager.userCanAccessDevice(devId,proc->uid(),DevicePolicyActionFlag::SETUP_STORAGES))
	{
		ctx.retVal.append("no storage found");
		return false;
	}
	ctx.retVal=st->allDataExportServices();
	return true;
}

bool StoragesCommands::getAttr(CallContext &ctx)
{
	if(ctx.args.count()<3)
	{
		ctx.retVal.append(StandardErrors::invalidAgruments);
		return false;
	}
	QByteArray devNameOrId=ctx.args[0];
	QByteArray sensorName=ctx.args[1];
	QByteArray attr=ctx.args[2];
	QUuid devId;
	ARpcISensorStorage *st=IotProxyInstance::inst().storages()->findStorageForDevice(
		devNameOrId,sensorName,devId);
	if(!st)
	{
		ctx.retVal.append("no storage found");
		return false;
	}
	if(!IotProxyConfig::accessManager.userCanAccessDevice(devId,proc->uid(),DevicePolicyActionFlag::SETUP_STORAGES))
	{
		ctx.retVal.append("no storage found");
		return false;
	}
	ctx.retVal.append(st->readAttribute(attr));
	return true;
}

bool StoragesCommands::setAttr(CallContext &ctx)
{
	if(ctx.args.count()<4)
	{
		ctx.retVal.append(StandardErrors::invalidAgruments);
		return false;
	}
	QByteArray devNameOrId=ctx.args[0];
	QByteArray sensorName=ctx.args[1];
	QByteArray attr=ctx.args[2];
	QUuid devId;
	ARpcISensorStorage *st=IotProxyInstance::inst().storages()->findStorageForDevice(
		devNameOrId,sensorName,devId);
	if(!st)
	{
		ctx.retVal.append("no storage found");
		return false;
	}
	if(!IotProxyConfig::accessManager.userCanAccessDevice(devId,proc->uid(),DevicePolicyActionFlag::SETUP_STORAGES))
	{
		ctx.retVal.append("no storage found");
		return false;
	}
	st->writeAttribute(attr,ctx.args[3]);
	return true;
}
