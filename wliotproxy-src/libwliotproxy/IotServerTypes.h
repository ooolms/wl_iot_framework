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

#ifndef IOTSERVERTYPES_H
#define IOTSERVERTYPES_H

#include <QUuid>
#include "wliot/devices/SensorDef.h"
#include "wliot/storages/ISensorStorage.h"
#include <functional>

struct IotServerTtyPortDescr
{
	QByteArray portName;
	QByteArray serialNumber;
	QByteArray manufacturer;
	QByteArray usbVendorId;
	QByteArray usbProductId;
};

struct IotServerIdentifiedDeviceDescr
{
	QUuid id;
	QByteArray name;
	QUuid typeId;
	QByteArray portOrName;
};

struct IotServerStorageDescr
{
	QUuid deviceId;
	QByteArray deviceName;
	SensorDef sensor;
	ISensorStorage::StoreMode mode;
	ISensorStorage::TimestampRule tsRule;
	SensorDef::Type storedValuesType;//sensor.type + tsRule effect = storedValuesType (or effectiveValuesType)
};

typedef std::function<bool(const QByteArrayList&)> CmDataCallback;

#endif // IOTSERVERTYPES_H
