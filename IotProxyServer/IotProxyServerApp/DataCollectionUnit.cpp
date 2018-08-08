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
#include "ARpcLocalStorage/ARpcAllStorages.h"
#include "ARpcBase/ARpcPacketSensorValue.h"
#include "ARpcBase/ARpcSingleSensorValue.h"
#include "ARpcBase/ARpcTextSensorValue.h"
#include "UdpDataExport.h"
#include <QDateTime>

const QByteArray DataCollectionUnit::dataTranslatorTypeKey="dataTranslator_type";
const QByteArray DataCollectionUnit::dataTranslatorConfigKey="dataTranslator_config";

DataCollectionUnit::DataCollectionUnit(ARpcRealDevice *dev,ARpcISensorStorage *stor,
	const ARpcSensorDef &sensorDescr,QObject *parent)
	:QObject(parent)
	,sensorDescriptor(sensorDescr)
{
	device=dev;
	translator=0;
	storeMode=stor->getStoreMode();
	storage=stor;
	if(!stor->isOpened())
		stor->open();
	if(storeMode==ARpcISensorStorage::AUTO_SESSIONS)
	{
		QUuid id;
		((ARpcSessionStorage*)storage)->createSession(
			QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss").toUtf8(),id);
		((ARpcSessionStorage*)storage)->openMainWriteSession(id);
	}
	setupSensorDataTranslator();
	connect(device,&ARpcDevice::rawMessage,this,&DataCollectionUnit::onRawMsg);
}

DataCollectionUnit::~DataCollectionUnit()
{
	if(storeMode==ARpcISensorStorage::AUTO_SESSIONS||storeMode==ARpcISensorStorage::MANUAL_SESSIONS)
		if(((ARpcSessionStorage*)storage)->isMainWriteSessionOpened())
			((ARpcSessionStorage*)storage)->closeMainWriteSession();
}

void DataCollectionUnit::onRawMsg(const ARpcMessage &m)
{
	if(m.title==ARpcConfig::measurementMsg&&m.args.count()>0&&m.args[0]==sensorDescriptor.name)
	{
		QByteArrayList args=m.args;
		args.removeFirst();
		parseValueFromStrList(args,TEXT);
	}
	else if(m.title==ARpcConfig::measurementBMsg&&m.args.count()==2&&m.args[0]==sensorDescriptor.name)
	{
		QByteArrayList args=m.args;
		args.removeFirst();
		parseValueFromStrList(args,BINARY);
	}
}

void DataCollectionUnit::setupSensorDataTranslator()
{
	if(translator)
		delete translator;
	QString translType=storage->readAttribute(dataTranslatorTypeKey).toString();
	QVariantMap translConfig=storage->readAttribute(dataTranslatorConfigKey).toMap();
	translator=ISensorDataTranslator::makeTranslator(translType,translConfig);
	if(translator)
		translator->setParent(this);
}

bool DataCollectionUnit::parseValueFromStrList(const QByteArrayList &args,ValueRepresentation vr)
{
	ARpcSensorValue *v=ARpcSensorValue::createSensorValue(sensorDescriptor.type);
	if(!v)
		return false;
	QScopedPointer<ARpcSensorValue> value(v);
	if(vr==BINARY)
	{
		if(!value->parseBinary(args[0]))
		{
			emit errorMessage("Device: "+device->id().toString()+"; sensor: "+sensorDescriptor.name+": bad value");
			return false;
		}
	}
	else //TEXT
	{
		if(!value->parseMsgArgs(args))
		{
			emit errorMessage("Device: "+device->id().toString()+"; sensor: "+sensorDescriptor.name+": bad value");
			return false;
		}
	}
	UdpDataExport::writeMeasurement(device->id(),sensorDescriptor.name,args);
	if(storeMode==ARpcISensorStorage::AUTO_SESSIONS||storeMode==ARpcISensorStorage::MANUAL_SESSIONS)
	{
		if(!((ARpcSessionStorage*)storage)->isMainWriteSessionOpened())
			return false;
	}
	storage->writeSensorValue(value.data());
	if(translator)
		translator->writeSensorValue(value.data());
//	emit infoMessage("SENSOR VALUE WRITTEN: "+device->id().toString()+"|"+sensorDescriptor.name+"|"+args.join("|"));
	return true;
}
