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

#ifndef ACCESSPOLICYCOMMANDS_H
#define ACCESSPOLICYCOMMANDS_H

#include <QObject>
#include <QUuid>
#include "wliot/client/ServerDataTypes.h"
#include "wliot/client/ServerConnection.h"

namespace WLIOTClient
{
	class AccessPolicyCommands
		:public QObject
	{
		Q_OBJECT
	public:
		class UserCommands
		{
		public:
			explicit UserCommands(AccessPolicyCommands *bPtr);
			bool list(QList<ApmUser> &list);
			bool create(const QByteArray &userName,const QByteArray &password,ApmIdType &uid);
			bool passwd(const QByteArray &oldPasswd,const QByteArray &newPasswd);
			bool chpass(const QByteArray &userName,const QByteArray &newPasswd);
			bool uid(const QByteArray &userName,ApmIdType &uid);
			bool name(ApmIdType uid,QByteArray &userName);

		private:
			AccessPolicyCommands *base;
		};

		class GroupCommands
		{
		public:
			explicit GroupCommands(AccessPolicyCommands *bPtr);
			bool list(QList<ApmUsersGroup> &list);
			bool create(const QByteArray &groupName,ApmIdType &gid);
			bool del(const QByteArray &groupName);
			bool listUsers(const QByteArray &groupName,QByteArrayList &userNames);
			bool addUser(const QByteArray &groupName,const QByteArray &userName);
			bool delUser(const QByteArray &groupName,const QByteArray &userName);
			bool getModer(const QByteArray &groupName,QByteArray &userName);
			bool setModer(const QByteArray &groupName,const QByteArray &userName);
			bool gid(const QByteArray &groupName,ApmIdType &gid);
			bool name(ApmIdType gid,QByteArray &groupName);

		private:
			AccessPolicyCommands *base;
		};

		class DevCommands
		{
		public:
			explicit DevCommands(AccessPolicyCommands *bPtr);
			bool list(QList<QUuid> &list);
			bool listRules(const QUuid &devId,QList<ApmDevicePolicyNote> &notes);
			bool setRule(const QUuid &devId,const ApmDevicePolicyNote &rule);
			bool changeDevOwner(const QUuid &devId,const QByteArray &newOwnerName=QByteArray());

		private:
			AccessPolicyCommands *base;
		};

	public:
		explicit AccessPolicyCommands(ServerConnection *conn);
		UserCommands& users();
		GroupCommands& groups();
		DevCommands& devices();

	private:
		ServerConnection *srvConn;
		UserCommands userCommands;
		GroupCommands groupCommands;
		DevCommands devicesCommands;
	};
}

#endif // ACCESSPOLICYCOMMANDS_H
