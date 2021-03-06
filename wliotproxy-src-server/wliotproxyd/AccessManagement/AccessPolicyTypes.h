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

#ifndef ACCESSPOLICYTYPES_H
#define ACCESSPOLICYTYPES_H

#include <QByteArray>
#include <QUuid>
#include <QMap>
#include <QSet>

typedef qint32 IdType;

static const IdType nullId=-1;
static const IdType anyId=-2;
static const IdType rootUid=0;
static const IdType rootGid=0;
static const IdType maxSystemId=999;

enum class UserPolicyFlag
{
	EMPTY=0x0,
	CAN_MANAGE_USER_GROUPS=0x1,
	CAN_CATCH_DEVICES=0x2
};

Q_DECLARE_FLAGS(UserPolicyFlags,UserPolicyFlag)
Q_DECLARE_OPERATORS_FOR_FLAGS(UserPolicyFlags)

class User
{
public:
	User()
	{
		uid=nullId;
		policy=UserPolicyFlag::EMPTY;
	}

public:
	IdType uid;
	QByteArray userName;
	QByteArray passwdHash;
	UserPolicyFlags policy;
};

class UsersGroup
{
public:
	UsersGroup()
	{
		gid=moderatorUid=nullId;
	}

public:
	IdType gid;
	IdType moderatorUid;
	QByteArray groupName;
	QSet<IdType> uids;
};

class DevicesGroup
{
public:
	DevicesGroup()
	{
		gid=ownerUid=nullId;
	}

public:
	IdType gid;
	IdType ownerUid;
	QByteArray groupName;
	QSet<QUuid> deviceIds;
};

enum class DevicePolicyActionFlag:uint16_t
{
	NO_RULE=0x0,
	READ_STORAGES=0x1,
	SETUP_STORAGES=0x2,
	READ_STATE=0x4,
	EXECUTE_COMMANDS=0x8,
	ANY=0xffff
};

Q_DECLARE_FLAGS(DevicePolicyActionFlags,DevicePolicyActionFlag)
Q_DECLARE_OPERATORS_FOR_FLAGS(DevicePolicyActionFlags)

#endif // ACCESSPOLICYTYPES_H

