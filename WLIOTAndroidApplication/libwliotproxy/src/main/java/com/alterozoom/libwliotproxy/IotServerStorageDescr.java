package com.alterozoom.libwliotproxy;

import java.util.UUID;

public class IotServerStorageDescr
{
	UUID deviceId;
	ByteArray deviceName;
	SensorDef sensor;
	ISensorStorage.StoreMode mode;
	ISensorStorage.TimestampRule tsRule;
	SensorDef.Type storedValuesType;//sensor.type + tsRule effect = storedValuesType (or effectiveValuesType)
}
