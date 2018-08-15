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

#include "BindStorageCommand.h"
#include "../IotProxyInstance.h"
#include "../ISensorDataTranslator.h"
#include "ARpcBase/ARpcSyncCall.h"
#include "StandardErrors.h"

BindStorageCommand::BindStorageCommand(ARpcOutsideDevice *d,IotProxyCommandProcessor *p)
	:ICommand(d,p)
{
}

bool BindStorageCommand::processCommand(const ARpcMessage &m,QByteArrayList &retVal)
{
	if(m.title!="bind_storage")return false;
	if(m.args.count()<3)
	{
		retVal.append(StandardErrors::invalidAgruments);
		return false;
	}
	QByteArray devNameOrId=m.args[0];
	QByteArray sensorName=m.args[1];
	QByteArray serviceType=m.args[2];
	ARpcISensorStorage::DataExportConfig cfg;
	for(int i=3;i<m.args.count();++i)
	{
		QByteArray arg=m.args[i];
		int ind=arg.indexOf(':');
		if(ind==-1)continue;
		QByteArray n=arg.mid(0,ind);
		QByteArray v=arg.mid(ind+1);
		if(n.isEmpty()||v.isEmpty())continue;
		cfg[n]=v;
	}
	QUuid devId;
	ARpcISensorStorage *st=IotProxyInstance::inst().sensorsStorage()->findStorageForDevice(devNameOrId,sensorName,devId);
	if(!st)
	{
		retVal.append("no storage found");
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
			retVal.append(StandardErrors::invalidAgruments);
			return false;
		}
		st->addDataExportConfig(serviceType,cfg);
	}
	DataCollectionUnit *unit=IotProxyInstance::inst().collectionUnit(devId,sensorName);
	if(unit)unit->setupSensorDataTranslators();
	Q_UNUSED(retVal)
	return true;
}

QByteArrayList BindStorageCommand::acceptedCommands()
{
	return QByteArrayList()<<"bind_storage";
}
