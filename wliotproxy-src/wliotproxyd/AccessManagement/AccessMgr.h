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
#include "AcsMgrBaseTypes.h"
#include <functional>

class AccessMgr
{
public:
	bool readConfig();
	IdType authentificateUser(const QByteArray &userName,const QByteArray &pass);
	void setUsersCanCaptureDevices(bool en);
	void setUsersCanManageGroups(bool en);
	bool usersCanManageGroups();

public://users management
	bool createUser(const QByteArray &userName,IdType &userId);
	IdType userId(const QByteArray &userName);
	bool delUser(const QByteArray &userName);
	bool userSetPass(IdType uid,const QByteArray &pass);
	const QList<User>& allUsers();
	QByteArray userName(IdType uid);
	bool hasUser(IdType uid);

public://groups management
	bool createUsersGroup(const QByteArray &groupName,IdType moderatorUid,IdType &gid);
	bool addUserToGroup(IdType gid,IdType uid);
	bool delUserFromGroup(IdType gid,IdType uid);
	bool changeUsersGroupModerator(IdType gid,IdType uid);
	bool delUsersGroup(IdType gid);
	IdType usersGroupId(const QByteArray &groupName);
	const QList<UsersGroup>& allUsersGroups();
	IdType moderatorId(IdType gid)const;
	bool userCanManageUsersInUsersGroup(IdType uid,IdType gid)const;
	bool userCanCreateUsersGroup(IdType uid)const;
	bool userCanManageUsersGroup(IdType uid,IdType gid)const;
	IdType groupId(const QByteArray &groupName);
	QSet<IdType> groupUsers(IdType gid);

public://manage devices
	IdType devOwner(const QUuid &devId);
	bool setDevOwner(const QUuid &devId,IdType uid);
	bool setDevicePolicyForUser(const QUuid &devId,IdType uid,DevicePolicyActionFlags flags);
	bool setDevicePolicyForUsersGroup(const QUuid &devId,IdType gid,DevicePolicyActionFlags flags);
	bool userCanAccessDevice(const QUuid &devId,IdType uid,DevicePolicyActionFlag flag);
	bool userCanChangeDeviceOwner(const QUuid &devId,IdType uid);
	bool userCanManageDevicePolicy(const QUuid &devId,IdType uid);
	bool userCanRegisterVirtualDevice(const QUuid &devId,IdType uid);

private:
	explicit AccessMgr();
	~AccessMgr();
	AccessMgr(const AccessMgr &)=delete;
	AccessMgr& operator=(const AccessMgr&)=delete;
	bool readUsers();
	bool writeUsers();
	bool readUserGroups();
	bool writeUserGroups();
	bool readDeviceOwners();
	bool writeDeviceOwners();
	bool readSingleDevicePolicies();
	bool writeSingleDevicePolicy(const QUuid &id);
	int groupFindByGid(IdType gid)const;
	int groupFindByName(const QByteArray &groupName)const;
	int userFindByUid(IdType uid)const;
	int userFindByName(const QByteArray &userName)const;
	int usersGroupFindByUid(IdType gid)const;
	void compileUsersPolicy();
	bool readConfigFile(const QString &filePath,int fieldsCount,
		std::function<bool(const QByteArrayList&)> lineParseFunc);
	static QByteArray idsListToString(const QList<IdType> &ids);
	static QByteArray idsSetToString(const QSet<IdType> &ids);

private:
	friend class MainServerConfig;
	bool ready;
	QList<User> users;
	QList<UsersGroup> userGroups;
	QMap<QUuid,IdType> deviceOwners;
	QMap<QUuid,DevicePolicy> devicesPolicy;
	QMap<IdType,CompiledUserPolicy> compiledUsersPolicy;
	IdType maxUserId,maxUserGroupId;
	bool mUsersCanManageGroups;
	bool mUsersCanManageDevices;
};

#endif // ACCESSMGR_H
