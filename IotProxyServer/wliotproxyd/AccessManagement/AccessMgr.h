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
	bool addUser(const QByteArray &userName,IdType &userId);
	IdType userId(const QByteArray &userName);
	void rmUser(const QByteArray &userName);
	bool userSetPass(const QByteArray &userName,const QByteArray &pass);

private:
	AccessMgr();
	~AccessMgr();
	AccessMgr(const AccessMgr &)=delete;
	AccessMgr& operator=(const AccessMgr&)=delete;
	bool readUsers();
	bool writeUsers();
	bool readUserGroups();
	bool writeUserGroups();
	int userFindByName(const QByteArray &userName)const;
	int userFindByUid(IdType userId)const;
	bool readConfigFile(const QString &filePath,int fieldsCount,
		std::function<bool(const QByteArrayList&)> lineParseFunc);

private:
	friend class IotProxyConfig;
	bool ready;
	QList<User> users;
	QList<UsersGroup> userGroups;
	QMap<QUuid,IdType> deviceOwners;
	CompiledUserPolicy usersPolicy;
	IdType maxUserId,maxUserGroupId;
};

#endif // ACCESSMGR_H
