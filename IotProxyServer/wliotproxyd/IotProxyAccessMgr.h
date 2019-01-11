#ifndef IOTPROXYACCESSMGR_H
#define IOTPROXYACCESSMGR_H

#include <QByteArray>
#include <QList>

class IotProxyAccessMgr
{
public:
	struct User
	{
		quint32 uid;
		QByteArray userName;
		QByteArray passwdHash;
	};
	struct Group
	{
		quint32 uid;
		QByteArray userName;
		QList<quint32> members;
	};

public:
	bool readUsers();
	bool writeUsers();
	qint32 chkUser(const QByteArray &userName,const QByteArray &pass);
	bool addUser(const QByteArray &userName,qint32 &userId);
	qint32 userId(const QByteArray &userName);
	void rmUser(const QByteArray &userName);
	bool userSetPass(const QByteArray &userName,const QByteArray &pass);

private:
	IotProxyAccessMgr();
	~IotProxyAccessMgr();
	IotProxyAccessMgr(const IotProxyAccessMgr &)=delete;
	IotProxyAccessMgr& operator=(const IotProxyAccessMgr&)=delete;
	int userFindByName(const QByteArray &userName)const;
	int userFindByUid(quint32 userId)const;

private:
	friend class IotProxyConfig;
	bool ready;
	QList<User> users;
	quint32 maxUid;
};

#endif // IOTPROXYACCESSMGR_H
