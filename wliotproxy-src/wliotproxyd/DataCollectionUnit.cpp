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

#include "DataCollectionUnit.h"
#include "wliot/storages/AllStorages.h"
#include "wliot/WLIOTProtocolDefs.h"
#include "UdpDataExport.h"
#include "ServerInstance.h"
#include <QDateTime>

const QByteArray DataCollectionUnit::dataTranslatorTypeKey="dataTranslator_type";
const QByteArray DataCollectionUnit::dataTranslatorConfigKey="dataTranslator_config";

DataCollectionUnit::DataCollectionUnit(RealDevice *dev,ISensorStorage *stor,QObject *parent)
	:QObject(parent)
{
	device=dev;
	storeMode=stor->storeMode();
	storage=stor;
	if(!stor->isOpened())
		stor->open();
	if(storeMode==ISensorStorage::AUTO_SESSIONS)
	{
		QUuid id;
		reinterpret_cast<SessionStorage*>(storage)->createSession(
			QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss").toUtf8(),id);
		reinterpret_cast<SessionStorage*>(storage)->openMainWriteSession(id);
	}
	setupSensorDataTranslators();
	connect(device,&RealDevice::newMessageFromDevice,this,&DataCollectionUnit::onNewMessage);
}

DataCollectionUnit::~DataCollectionUnit()
{
	if(storeMode==ISensorStorage::AUTO_SESSIONS||storeMode==ISensorStorage::MANUAL_SESSIONS)
		if(reinterpret_cast<SessionStorage*>(storage)->isMainWriteSessionOpened())
			reinterpret_cast<SessionStorage*>(storage)->closeMainWriteSession();
}

void DataCollectionUnit::onNewMessage(const Message &m)
{
	if(m.title==WLIOTProtocolDefs::measurementMsg&&m.args.count()>0&&m.args[0]==storage->sensor().name)
	{
		QByteArrayList args=m.args;
		args.removeFirst();
		parseValueFromStrList(args,TEXT);
	}
	else if(m.title==WLIOTProtocolDefs::measurementBMsg&&m.args.count()==2&&m.args[0]==storage->sensor().name)
	{
		QByteArrayList args=m.args;
		args.removeFirst();
		parseValueFromStrList(args,BINARY);
	}
}

void DataCollectionUnit::setupSensorDataTranslators()
{
	for(auto i:translators)
		delete i;
	translators.clear();
	for(const QByteArray &serviceType:storage->allDataExportServices())
	{
		ISensorStorage::DataExportConfig cfg=storage->getDataExportConfig(serviceType);
		ISensorDataTranslator *transl=ISensorDataTranslator::makeTranslator(
			serviceType,storage->deviceId(),storage->deviceName(),storage->sensor(),cfg);
		if(transl)
			translators.append(transl);
	}
}

bool DataCollectionUnit::parseValueFromStrList(const QByteArrayList &args,ValueRepresentation vr)
{
	SensorValue *v=SensorValue::createSensorValue(storage->sensor().type);
	if(!v)
		return false;
	QScopedPointer<SensorValue> value(v);
	if(vr==BINARY)
	{
		if(!value->parseBinary(args[0]))
		{
			emit errorMessage("Device: "+device->id().toByteArray()+"; sensor: "+storage->sensor().name+": bad value");
			return false;
		}
	}
	else //TEXT
	{
		if(!value->parseMsgArgs(args))
		{
			emit errorMessage("Device: "+device->id().toByteArray()+"; sensor: "+storage->sensor().name+": bad value");
			return false;
		}
	}
	UdpDataExport::writeMeasurement(device->id(),storage->sensor().name,args);
	if(storeMode==ISensorStorage::AUTO_SESSIONS||storeMode==ISensorStorage::MANUAL_SESSIONS)
	{
		if(!reinterpret_cast<SessionStorage*>(storage)->isMainWriteSessionOpened())
			return false;
	}
	storage->writeSensorValue(value.data());
	emit infoMessage("VALUE: "+device->name()+"|"+
		device->id().toByteArray()+"|"+storage->sensor().name+"|"+value->dumpToMsgArgs().join("|"));
	for(auto t:translators)
		t->writeSensorValue(value.data());
	return true;
}
