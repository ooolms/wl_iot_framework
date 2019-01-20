#ifndef AUTHBASETYPES
#define AUTHBASETYPES

#include <QByteArray>
#include <QList>
#include <QUuid>
#include <QMap>

typedef qint32 IdType;

static const IdType nullId=-1;

enum class UserPolicyFlag
{
	EMPTY=0x0,
	CAN_CREATE_USER_GROUPS=0x1,
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

struct UsersGroup
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
	QList<IdType> uids;
};

struct DevicesGroup
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
	QList<QUuid> deviceIds;
};

enum class DevicePolicyActor
{
	USER,
	USERS_GROUP
};

enum class DevicePolicyTarget
{
	DEVICE,
	DEVICES_GROUP
};

enum class DevicePolicyActionFlag
{
	NO_RULE=0x0,
	READ_STORAGES=0x1,
	SETUP_STORAGES=0x2,
	READ_STATE=0x4,
	EXECUTE_COMMANDS=0x8
};

Q_DECLARE_FLAGS(DevicePolicyActionFlags,DevicePolicyActionFlag)
Q_DECLARE_OPERATORS_FOR_FLAGS(DevicePolicyActionFlags)

enum class DevicePolicyRule
{
	ALLOW,
	DENY
};

typedef QMap<IdType,DevicePolicyActionFlags> CompiledUserPolicy;

#endif // AUTHBASETYPES

