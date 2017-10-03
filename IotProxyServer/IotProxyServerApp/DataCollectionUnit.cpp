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

const QString DataCollectionUnit::dataTranslatorTypeKey="dataTranslator_type";
const QString DataCollectionUnit::dataTranslatorConfigKey="dataTranslator_config";

DataCollectionUnit::DataCollectionUnit(ARpcRealDevice *dev,ARpcISensorStorage *stor,const ARpcSensor &sensorDescr,
	QObject *parent)
	:QObject(parent)
	,sensorDescriptor(sensorDescr)
{
	device=dev;
	translator=0;
	storeMode=stor->getStoreMode();
	if(!stor->isOpened())
		stor->open();
	if(storeMode==ARpcISensorStorage::CONTINUOUS)
		stors.contStor=(ARpcContinuousStorage*)stor;
	else if(storeMode==ARpcISensorStorage::AUTO_SESSIONS||storeMode==ARpcISensorStorage::MANUAL_SESSIONS)
		stors.sessStor=(ARpcSessionStorage*)stor;
	else if(storeMode==ARpcISensorStorage::LAST_N_VALUES)
		stors.lastNStor=(ARpcLastNValuesStorage*)stor;
	else if(storeMode==ARpcISensorStorage::LAST_VALUE_IN_MEMORY)
		stors.lastMemStor=(ARpcLastValueInMemoryStorage*)stor;
	if(storeMode==ARpcISensorStorage::AUTO_SESSIONS)
	{
		QUuid id;
		stors.sessStor->createSession(QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss"),id);
		stors.sessStor->openMainWriteSession(id);
	}
	setupSensorDataTranslator();
	connect(device,&ARpcDevice::rawMessage,this,&DataCollectionUnit::onRawMsg);
}

DataCollectionUnit::~DataCollectionUnit()
{
	if(storeMode==ARpcISensorStorage::AUTO_SESSIONS||storeMode==ARpcISensorStorage::MANUAL_SESSIONS)
		if(stors.sessStor->isMainWriteSessionOpened())
			stors.sessStor->closeMainWriteSession();
}

void DataCollectionUnit::onRawMsg(const ARpcMessage &m)
{
	if(m.title==ARpcConfig::measurementMsg&&m.args.count()>0&&m.args[0]==sensorDescriptor.name)
		processMeasurementMsg(m);
	else if(m.title==ARpcConfig::infoMsg)
		emit infoMessage("FROM DEVICE: "+m.args.join("|"));
}

void DataCollectionUnit::processMeasurementMsg(const ARpcMessage &m)
{
	QStringList args=m.args;
	args.removeFirst();
	parseValueFromStrList(args);
}

void DataCollectionUnit::setupSensorDataTranslator()
{
	if(translator)
		delete translator;
	ARpcISensorStorage *st=(ARpcISensorStorage*)stors.contStor;
	QString translType=st->readAttribute(dataTranslatorTypeKey).toString();
	QVariantMap translConfig=st->readAttribute(dataTranslatorConfigKey).toMap();
	translator=ISensorDataTranslator::makeTranslator(translType,translConfig);
	if(translator)
		translator->setParent(this);
}

bool DataCollectionUnit::parseValueFromStrList(const QStringList &args)
{
	quint32 dims=1;
	if(sensorDescriptor.constraints.contains("dims"))
		dims=sensorDescriptor.constraints["dims"].toUInt();
	if(dims==0)
		dims=1;
	ARpcISensorValue *v=sensorDescriptor.makeEmptySensorValue();
	if(!v)
		return false;
	QScopedPointer<ARpcISensorValue> value(v);
	if(!value->parse(args))
	{
		emit errorMessage("Device: "+device->id().toString()+"; sensor: "+sensorDescriptor.name+": bad value");
		return false;
	}
	UdpDataExport::writeMeasurement(device->id(),sensorDescriptor.name,args);
	if(storeMode==ARpcISensorStorage::CONTINUOUS)
		stors.contStor->writeSensorValue(value.data());
	else if(storeMode==ARpcISensorStorage::LAST_N_VALUES)
		stors.lastNStor->writeSensorValue(value.data());
	else if(storeMode==ARpcISensorStorage::LAST_VALUE_IN_MEMORY)
		stors.lastMemStor->writeSensorValue(value.data());
	else if(storeMode==ARpcISensorStorage::AUTO_SESSIONS||storeMode==ARpcISensorStorage::MANUAL_SESSIONS)
	{
		if(stors.sessStor->isMainWriteSessionOpened())
			stors.sessStor->writeSensorValue(value.data());
	}
	if(translator)
		translator->writeSensorValue(value.data());
	emit infoMessage("SENSOR VALUE WRITTEN: "+device->id().toString()+"|"+sensorDescriptor.name+"|"+args.join("|"));
	return true;
}
