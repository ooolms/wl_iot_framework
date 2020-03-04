#ifndef IOTSERVERACCESSPOLICYCOMMANDS_H
#define IOTSERVERACCESSPOLICYCOMMANDS_H

#include <QObject>
#include <QUuid>
#include "IotServerTypes.h"
#include "IotServerConnection.h"

class IotServerAccessPolicyCommands
	:public QObject
{
	Q_OBJECT
public:
	class UserCommands
	{
	public:
		explicit UserCommands(IotServerAccessPolicyCommands *bPtr);
		bool list(QList<IotServerUser> &list);
		bool create(const QByteArray &userName,const QByteArray &password,IotServerApmIdType &uid);
		bool passwd(const QByteArray &oldPasswd,const QByteArray &newPasswd);
		bool chpass(const QByteArray &userName,const QByteArray &newPasswd);
		bool uid(const QByteArray &userName,IotServerApmIdType &uid);
		bool name(IotServerApmIdType uid,QByteArray &userName);

	private:
		IotServerAccessPolicyCommands *base;
	};

	class GroupCommands
	{
	public:
		explicit GroupCommands(IotServerAccessPolicyCommands *bPtr);
		bool list(QList<IotServerUsersGroup> &list);
		bool create(const QByteArray &groupName,IotServerApmIdType &gid);
		bool del(const QByteArray &groupName);
		bool listUsers(const QByteArray &groupName,QByteArrayList &userNames);
		bool addUser(const QByteArray &groupName,const QByteArray &userName);
		bool delUser(const QByteArray &groupName,const QByteArray &userName);
		bool getModer(const QByteArray &groupName,QByteArray &userName);
		bool setModer(const QByteArray &groupName,const QByteArray &userName);
		bool gid(const QByteArray &groupName,IotServerApmIdType &gid);
		bool name(IotServerApmIdType gid,QByteArray &groupName);

	private:
		IotServerAccessPolicyCommands *base;
	};

	class DevCommands
	{
	public:
		explicit DevCommands(IotServerAccessPolicyCommands *bPtr);
		bool list(QList<QUuid> &list);
		bool listRules(const QUuid &devId,QList<IotServerDevicePolicyNote> &notes);
		bool setRule(const QUuid &devId,const IotServerDevicePolicyNote &rule);
		bool changeDevOwner(const QUuid &devId,const QByteArray &newOwnerName=QByteArray());

	private:
		IotServerAccessPolicyCommands *base;
	};

public:
	explicit IotServerAccessPolicyCommands(IotServerConnection *conn);
	UserCommands& users();
	GroupCommands& groups();
	DevCommands& devices();

private:
	IotServerConnection *srvConn;
	UserCommands userCommands;
	GroupCommands groupCommands;
	DevCommands devicesCommands;
};

#endif // IOTSERVERACCESSPOLICYCOMMANDS_H
