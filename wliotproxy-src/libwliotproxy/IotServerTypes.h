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
#include <QSet>
#include "wliot/devices/SensorDef.h"
#include "wliot/storages/ISensorStorage.h"
#include "GDIL/core/Program.h"
#include "GDIL/core/TimerBlock.h"
#include <functional>

typedef qint32 IotServerApmIdType;
static const IotServerApmIdType nullId=-1;
static const IotServerApmIdType anyId=-2;
static const IotServerApmIdType rootUid=0;

struct IotServerDataExportServiceDescr
{
	QUuid uid;
	QByteArray name;
	QByteArrayList paramNames;
};

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

enum class IotServerUserPolicyFlag
{
	EMPTY=0x0,
	CAN_MANAGE_USER_GROUPS=0x1,
	CAN_CATCH_DEVICES=0x2
};

Q_DECLARE_FLAGS(IotServerUserPolicyFlags,IotServerUserPolicyFlag)
Q_DECLARE_OPERATORS_FOR_FLAGS(IotServerUserPolicyFlags)

class IotServerUser
{
public:
	IotServerUser()
	{
		uid=nullId;
	}

	IotServerUser(const IotServerApmIdType &id,const QByteArray &name)
	{
		uid=id;
		userName=name;
	}

public:
	IotServerApmIdType uid;
	QByteArray userName;
};

class IotServerUsersGroup
{
public:
	IotServerUsersGroup()
	{
		gid=moderatorUid=nullId;
	}

public:
	IotServerApmIdType gid;
	IotServerApmIdType moderatorUid;
	QByteArray groupName;
	QSet<IotServerApmIdType> uids;
};

enum class IotServerDevicePolicyFlag:uint16_t
{
	NO_RULE=0x0,
	READ_STORAGES=0x1,
	SETUP_STORAGES=0x2,
	READ_STATE=0x4,
	EXECUTE_COMMANDS=0x8,
	ANY=0xffff
};

Q_DECLARE_FLAGS(IotServerDevicePolicyFlags,IotServerDevicePolicyFlag)
Q_DECLARE_OPERATORS_FOR_FLAGS(IotServerDevicePolicyFlags)

class IotServerDevicePolicyNote
{
public:
	IotServerDevicePolicyNote()
	{
		pol=IotServerDevicePolicyFlag::NO_RULE;
		userPolicy=true;
	}
	QString polToStr()const;

public:
	bool userPolicy;//false - group policy
	QByteArray targetName;
	IotServerDevicePolicyFlags pol;
};

typedef std::function<bool(const QByteArrayList&)> CmDataCallback;

class IotServerGDILConfigOption
{
public:
	IotServerGDILConfigOption()
	{
		id.blockId=0;
	}

	IotServerGDILConfigOption(const ConfigOptionId i,const QByteArray &bn,const DataUnit &v,const TypeConstraints &c)
	{
		id=i;
		blockName=bn;
		value=v;
		constr=c;
	}

public:
	ConfigOptionId id;
	QByteArray blockName;
	DataUnit value;
	TypeConstraints constr;
};

class IotServerGDILTimer
{
public:
	IotServerGDILTimer()
	{
		blockId=0;
	}

	IotServerGDILTimer(quint32 bId,const QByteArray &bn,const TimerBlock::TimerConfig &c)
	{
		blockId=bId;
		blockName=bn;
		config=c;
	}

public:
	quint32 blockId;
	QByteArray blockName;
	TimerBlock::TimerConfig config;
};

#endif // IOTSERVERTYPES_H
