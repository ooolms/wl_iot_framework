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

#include "wliot/devices/RealDevice.h"
#include "wliot/storages/BaseFSSensorStorage.h"
#include "wliot/devices/SimpleMsgDispatch.h"
#include "ISensorDataTranslator.h"
#include <QObject>

namespace WLIOT
{
	class ContinuousStorage;
	class SessionStorage;
	class LastNValuesStorage;
	class LastNValuesInMemoryStorage;
}

class DataCollectionUnit
	:public QObject
{
	Q_OBJECT
public:
	enum ValueRepresentation
	{
		TEXT,
		BINARY
	};

public:
	explicit DataCollectionUnit(WLIOT::RealDevice *dev,WLIOT::ISensorStorage *stor,QObject *parent=0);
	virtual ~DataCollectionUnit();
	void setupSensorDataTranslators();
	bool parseValueFromStrList(const QByteArrayList &args,ValueRepresentation vr=TEXT);

private slots:
	void onNewMessage(const WLIOT::Message &m);

public:
	static const QByteArray dataTranslatorTypeKey;
	static const QByteArray dataTranslatorConfigKey;

private:
	WLIOT::ISensorStorage *storage;
	WLIOT::ISensorStorage::StoreMode storeMode;
	WLIOT::RealDevice *device;
	QList<ISensorDataTranslator*> translators;
};

#endif // DATACOLLECTIONUNIT_H
