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
public://users management
	bool readConfig();
	IdType authentificateUser(const QByteArray &userName,const QByteArray &pass);
	bool addUser(const QByteArray &userName,IdType &userId);
	IdType userId(const QByteArray &userName);
	void rmUser(const QByteArray &userName);
	bool userSetPass(const QByteArray &userName,const QByteArray &pass);

public://manage devices
	bool setDevOwner(const QUuid &devId,IdType uid);
	bool setDevicePolicyForUser(const QUuid &devId,IdType uid,DevicePolicyActionFlags flags);
	bool setDevicePolicyForUsersGroup(const QUuid &devId,IdType gid,DevicePolicyActionFlags flags);

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
	int userFindByName(const QByteArray &userName)const;
	int userFindByUid(IdType uid)const;
	int usersGroupFindByUid(IdType gid)const;
	bool readConfigFile(const QString &filePath,int fieldsCount,
		std::function<bool(const QByteArrayList&)> lineParseFunc);
	static QByteArray idsListToString(const QList<IdType> &ids);

private:
	friend class IotProxyConfig;
	bool ready;
	QList<User> users;
	QList<UsersGroup> userGroups;
	QMap<QUuid,IdType> deviceOwners;
	QMap<QUuid,DevicePolicy> devicesPolicy;
	QMap<IdType,CompiledUserPolicy> usersPolicy;
	IdType maxUserId,maxUserGroupId;
};

#endif // ACCESSMGR_H
