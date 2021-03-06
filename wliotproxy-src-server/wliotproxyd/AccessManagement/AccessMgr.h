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

#ifndef ACCESSMGR_H
#define ACCESSMGR_H

#include <QByteArray>
#include <QList>
#include "AccessPolicyTypes.h"
#include <functional>
#include <QObject>

class AccessMgr
	:public QObject
{
private:
	class DevicePolicy
	{
	public:
		QMap<IdType,DevicePolicyActionFlags> userFlags;
		QMap<IdType,DevicePolicyActionFlags> groupFlags;
	};

	class CompiledUserPolicy
	{
	public:
		QMap<QUuid,DevicePolicyActionFlags> selfPolicy;
		QMap<QUuid,DevicePolicyActionFlags> groupPolicy;
	};


public:
	bool readConfig();
	IdType authenticateUser(const QByteArray &userName,const QByteArray &pass);
	void setUsersCanCaptureDevices(bool en);
	void setUsersCanManageGroups(bool en);
	bool usersCanManageGroups();

public://users management
	bool createUser(const QByteArray &userName,IdType &userId);
	IdType userId(const QByteArray &userName);
	bool delUser(const QByteArray &userName);
	bool userSetPass(IdType uid,const QByteArray &pass);
	const QMap<IdType,User>& allUsers();
	QByteArray userName(IdType uid);
	bool hasUser(IdType uid);

public://groups management
	bool createUsersGroup(const QByteArray &groupName,IdType moderatorUid,IdType &gid);
	bool addUserToGroup(IdType gid,IdType uid);
	bool delUserFromGroup(IdType gid,IdType uid);
	bool changeUsersGroupModerator(IdType gid,IdType uid);
	bool delUsersGroup(IdType gid);
	IdType usersGroupId(const QByteArray &groupName);
	QByteArray usersGroupName(IdType gid);
	const QMap<IdType,UsersGroup>& allUsersGroups();
	IdType moderatorId(IdType gid)const;
	bool userCanManageUsersInUsersGroup(IdType uid,IdType gid)const;
	bool userCanCreateUsersGroup(IdType uid)const;
	bool userCanManageUsersGroup(IdType uid,IdType gid)const;
	QSet<IdType> groupUsers(IdType gid);
	bool leaveGroup(IdType gid,IdType uid);

public://manage devices
	IdType devOwner(const QUuid &devId);
	bool setDevOwner(const QUuid &devId,IdType uid);
	bool setDevicePolicyForUser(const QUuid &devId,IdType uid,DevicePolicyActionFlags flags);
	bool setDevicePolicyForUsersGroup(const QUuid &devId,IdType gid,DevicePolicyActionFlags flags);
	bool userCanAccessDevice(const QUuid &devId,IdType uid,DevicePolicyActionFlag flag);
	bool userCanChangeDeviceOwner(const QUuid &devId,IdType uid);
	bool userCanManageDevicePolicy(const QUuid &devId,IdType uid);
	QSet<QUuid> allUserDevices(IdType uid);
	bool userCanRegisterVirtualDevice(const QUuid &devId,IdType uid);
	void listDeviceUserPolicies(const QUuid &devId,QMap<IdType,DevicePolicyActionFlags> &rules);
	void listDeviceUserGroupPolicies(const QUuid &devId,QMap<IdType,DevicePolicyActionFlags> &rules);
	QByteArray polToStr(DevicePolicyActionFlags pol);
	bool polFromStr(const QByteArray &str,DevicePolicyActionFlags &pol);

private:
	explicit AccessMgr();
	~AccessMgr();
	AccessMgr(const AccessMgr &)=delete;
	AccessMgr& operator=(const AccessMgr&)=delete;
	void readMaxUidGid();
	bool writeMaxUidGid();
	bool readUsers();
	bool writeUsers(bool checkIfReady=true);
	bool readUserGroups();
	bool writeUserGroups();
	bool readDeviceOwners();
	bool writeDeviceOwners();
	bool readSingleDevicePolicies();
	bool writeSingleDevicePolicy(const QUuid &id);
	void compileUsersPolicy();
	bool readConfigFile(const QString &filePath,int fieldsCount,
		std::function<bool(const QByteArrayList&)> lineParseFunc);
	static QByteArray idsListToString(const QList<IdType> &ids);
	static QByteArray idsSetToString(const QSet<IdType> &ids);

private:
	friend class MainServerConfig;
	bool ready;
	QMap<IdType,User> users;
	QMap<IdType,UsersGroup> userGroups;
	QMap<QUuid,IdType> deviceOwners;
	QMap<QUuid,DevicePolicy> devicesPolicy;
	QMap<IdType,CompiledUserPolicy> compiledUsersPolicy;
	IdType maxUserId,maxUserGroupId;
	bool mUsersCanManageGroups;
	bool mUsersCanManageDevices;
};

#endif // ACCESSMGR_H
