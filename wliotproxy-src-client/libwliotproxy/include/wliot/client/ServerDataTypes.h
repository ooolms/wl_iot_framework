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

#ifndef SERVERDATATYPES_H
#define SERVERDATATYPES_H

#include <QUuid>
#include <QSet>
#include "wliot/devices/SensorDef.h"
#include "wliot/storages/ISensorStorage.h"
#include "VDIL/core/Program.h"
#include "VDIL/core/TimerBlock.h"
#include <functional>

namespace WLIOTClient
{
	typedef qint32 ApmIdType;
	static const ApmIdType nullId=-1;
	static const ApmIdType anyId=-2;
	static const ApmIdType rootUid=0;

	struct DataExportServiceDescr
	{
		QUuid uid;
		QByteArray name;
		QByteArrayList paramNames;
	};

	struct TtyPortDescr
	{
		QByteArray portName;
		QByteArray serialNumber;
		QByteArray manufacturer;
		QByteArray usbVendorId;
		QByteArray usbProductId;
	};

	struct IdentifiedDeviceDescr
	{
		QUuid id;
		QByteArray name;
		QUuid typeId;
		QByteArray portOrName;
	};

	struct StorageDescr
	{
		QUuid deviceId;
		QByteArray deviceName;
		WLIOT::SensorDef sensor;
		WLIOT::ISensorStorage::StoreMode mode;
		WLIOT::ISensorStorage::TimestampRule tsRule;
		WLIOT::SensorDef::Type storedValuesType;//sensor.type + tsRule effect = storedValuesType (or effectiveValuesType)
	};

	enum class ApmUserPolicyFlag
	{
		EMPTY=0x0,
		CAN_MANAGE_USER_GROUPS=0x1,
		CAN_CATCH_DEVICES=0x2
	};

	Q_DECLARE_FLAGS(ApmUserPolicyFlags,ApmUserPolicyFlag)
	Q_DECLARE_OPERATORS_FOR_FLAGS(ApmUserPolicyFlags)

	class ApmUser
	{
	public:
		ApmUser()
		{
			uid=nullId;
		}

		ApmUser(const ApmIdType &id,const QByteArray &name)
		{
			uid=id;
			userName=name;
		}

	public:
		ApmIdType uid;
		QByteArray userName;
	};

	class ApmUsersGroup
	{
	public:
		ApmUsersGroup()
		{
			gid=moderatorUid=nullId;
		}

	public:
		ApmIdType gid;
		ApmIdType moderatorUid;
		QByteArray groupName;
		QSet<ApmIdType> uids;
	};

	enum class ApmDevicePolicyFlag:uint16_t
	{
		NO_RULE=0x0,
		READ_STORAGES=0x1,
		SETUP_STORAGES=0x2,
		READ_STATE=0x4,
		EXECUTE_COMMANDS=0x8,
		ANY=0xffff
	};

	Q_DECLARE_FLAGS(ApmDevicePolicyFlags,ApmDevicePolicyFlag)
	Q_DECLARE_OPERATORS_FOR_FLAGS(ApmDevicePolicyFlags)

	class ApmDevicePolicyNote
	{
	public:
		ApmDevicePolicyNote()
		{
			pol=ApmDevicePolicyFlag::NO_RULE;
			userPolicy=true;
		}
		QString polToStr()const;

	public:
		bool userPolicy;//false - group policy
		QByteArray targetName;
		ApmDevicePolicyFlags pol;
	};

	typedef std::function<bool(const QByteArrayList&)> CmDataCallback;

	class VDILConfigOption
	{
	public:
		VDILConfigOption()
		{
			id.blockId=0;
		}

		VDILConfigOption(const WLIOTVDIL::ConfigOptionId i,const QByteArray &bn,const WLIOTVDIL::DataUnit &v)
		{
			id=i;
			blockName=bn;
			value=v;
		}

	public:
		WLIOTVDIL::ConfigOptionId id;
		QByteArray blockName;
		WLIOTVDIL::DataUnit value;
	};

	class VDILTimer
	{
	public:
		VDILTimer()
		{
			blockId=0;
		}

		VDILTimer(quint32 bId,const QByteArray &bn,const WLIOTVDIL::TimerBlock::TimerConfig &c)
		{
			blockId=bId;
			blockName=bn;
			config=c;
		}

	public:
		quint32 blockId;
		QByteArray blockName;
		WLIOTVDIL::TimerBlock::TimerConfig config;
	};
}

#endif // SERVERDATATYPES_H
