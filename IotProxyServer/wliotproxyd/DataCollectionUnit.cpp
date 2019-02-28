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
#include "ARpcStorages/ARpcAllStorages.h"
#include "UdpDataExport.h"
#include <QDateTime>

const QByteArray DataCollectionUnit::dataTranslatorTypeKey="dataTranslator_type";
const QByteArray DataCollectionUnit::dataTranslatorConfigKey="dataTranslator_config";

DataCollectionUnit::DataCollectionUnit(ARpcRealDevice *dev,ARpcISensorStorage *stor,QObject *parent)
	:QObject(parent)
{
	device=dev;
	storeMode=stor->storeMode();
	storage=stor;
	if(!stor->isOpened())
		stor->open();
	if(storeMode==ARpcISensorStorage::AUTO_SESSIONS)
	{
		QUuid id;
		reinterpret_cast<ARpcSessionStorage*>(storage)->createSession(
			QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss").toUtf8(),id);
		reinterpret_cast<ARpcSessionStorage*>(storage)->openMainWriteSession(id);
	}
	setupSensorDataTranslators();
	connect(device,&ARpcDevice::newMessage,this,&DataCollectionUnit::onNewMessage);
}

DataCollectionUnit::~DataCollectionUnit()
{
	if(storeMode==ARpcISensorStorage::AUTO_SESSIONS||storeMode==ARpcISensorStorage::MANUAL_SESSIONS)
		if(reinterpret_cast<ARpcSessionStorage*>(storage)->isMainWriteSessionOpened())
			reinterpret_cast<ARpcSessionStorage*>(storage)->closeMainWriteSession();
}

void DataCollectionUnit::onNewMessage(const ARpcMessage &m)
{
	if(m.title==ARpcConfig::measurementMsg&&m.args.count()>0&&m.args[0]==storage->sensor().name)
	{
		QByteArrayList args=m.args;
		args.removeFirst();
		parseValueFromStrList(args,TEXT);
	}
	else if(m.title==ARpcConfig::measurementBMsg&&m.args.count()==2&&m.args[0]==storage->sensor().name)
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
		ARpcISensorStorage::DataExportConfig cfg=storage->getDataExportConfig(serviceType);
		ISensorDataTranslator *transl=ISensorDataTranslator::makeTranslator(
			serviceType,storage->deviceId(),storage->sensor(),cfg);
		if(transl)
			translators.append(transl);
	}
}

bool DataCollectionUnit::parseValueFromStrList(const QByteArrayList &args,ValueRepresentation vr)
{
	ARpcSensorValue *v=ARpcSensorValue::createSensorValue(storage->sensor().type);
	if(!v)
		return false;
	QScopedPointer<ARpcSensorValue> value(v);
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
	if(storeMode==ARpcISensorStorage::AUTO_SESSIONS||storeMode==ARpcISensorStorage::MANUAL_SESSIONS)
	{
		if(!reinterpret_cast<ARpcSessionStorage*>(storage)->isMainWriteSessionOpened())
			return false;
	}
	storage->writeSensorValue(value.data());
	emit infoMessage("VALUE: "+device->name()+"|"+
		device->id().toByteArray()+"|"+storage->sensor().name+"|"+value->dumpToMsgArgs().join("|"));
	for(auto t:translators)
		t->writeSensorValue(value.data());
	return true;
}
