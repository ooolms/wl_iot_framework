#ifndef IOTSERVERTYPES_H
#define IOTSERVERTYPES_H

#include <QUuid>
#include "ARpcBase/ARpcSensorDef.h"
#include "ARpcStorages/ARpcBaseFSSensorStorage.h"

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
	QByteArray type;
	QByteArray portOrName;
};

struct IotServerStorageDescr
{
	QUuid deviceId;
	QByteArray deviceName;
	ARpcSensorDef sensor;
	ARpcISensorStorage::StoreMode mode;
	ARpcISensorStorage::TimestampRule tsRule;
	ARpcSensorDef::Type storedValuesType;//sensor.type + tsRule effect = storedValuesType (or effectiveValuesType)
};

#endif // IOTSERVERTYPES_H
