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

#ifndef DATACOLLECTIONUNIT_H
#define DATACOLLECTIONUNIT_H

#include "ARpcBase/ARpcRealDevice.h"
#include "ARpcLocalStorage/ARpcISensorStorage.h"
#include "ARpcBase/ARpcSimpleMsgDispatch.h"
#include "ISensorDataTranslator.h"
#include <QObject>

class ARpcContinuousStorage;
class ARpcSessionStorage;
class ARpcLastNValuesStorage;
class ARpcLastValueInMemoryStorage;

class DataCollectionUnit
	:public QObject
{
	Q_OBJECT
public:
	enum ValueRepresentation
	{
		TEXT,
		FLOAT,
		DOUBLE
	};

public:
	explicit DataCollectionUnit(ARpcRealDevice *dev,ARpcISensorStorage *stor,
		const ARpcSensor &sensorDescr,QObject *parent=0);
	virtual ~DataCollectionUnit();
	void setupSensorDataTranslator();
	bool parseValueFromStrList(const QByteArrayList &args,ValueRepresentation vr=TEXT);

signals:
	void errorMessage(const QString &msg);
	void infoMessage(const QString &msg);

private slots:
	void onRawMsg(const ARpcMessage &m);

public:
	static const QByteArray dataTranslatorTypeKey;
	static const QByteArray dataTranslatorConfigKey;

private:
	ARpcISensorStorage *storage;
	ARpcISensorStorage::StoreMode storeMode;
	ARpcRealDevice *device;
	ISensorDataTranslator *translator;
	ARpcSensor sensorDescriptor;
};

#endif // DATACOLLECTIONUNIT_H
