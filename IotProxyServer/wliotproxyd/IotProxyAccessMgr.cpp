#include "IotProxyAccessMgr.h"
#include <QFile>
#include <QCryptographicHash>
#include <QSet>

IotProxyAccessMgr::IotProxyAccessMgr()
{
	ready=false;
	maxUid=1000;
}

IotProxyAccessMgr::~IotProxyAccessMgr()
{
}

bool IotProxyAccessMgr::readUsers()
{
	QFile file("/var/lib/wliotproxyd/passwd");
	users.clear();
	maxUid=1000;
	if(!file.exists())
	{
		User u;
		u.uid=0;
		u.userName="root";
		users.append(u);
		writeUsers();
		ready=true;
		return true;
	}
	if(!file.open(QIODevice::ReadOnly))
		return false;
	bool hasRoot=false;
	QSet<QString> userNames;
	QSet<quint32> userIds;
	while(!file.atEnd())
	{
		QByteArray line=file.readLine();
		if(line.endsWith('\n'))
			line.chop(1);
		QByteArrayList fields=line.split(':');
		if(fields.count()!=3)continue;
		bool ok=false;
		User u;
		u.userName=fields[0];
		u.passwdHash=QByteArray::fromBase64(fields[1]);
		u.uid=fields[2].toULong(&ok);
		if(u.userName.isEmpty()||!ok)continue;
		if(userNames.contains(u.userName)||userIds.contains(u.uid))
			return false;
		users.append(u);
		userNames.insert(u.userName);
		userIds.insert(u.uid);
		if(u.uid==0)
			hasRoot=true;
		if(u.uid>maxUid)
			maxUid=u.uid;
	}
	file.close();
	if(!hasRoot)
	{
		User u;
		u.uid=0;
		u.userName="root";
		users.prepend(u);
		writeUsers();
	}
	ready=true;
	return true;
}

bool IotProxyAccessMgr::writeUsers()
{
	QFile file("/var/lib/wliotproxyd/passwd");
	if(!file.open(QIODevice::WriteOnly))
		return false;
	for(User &u:users)
	{
		file.write(u.userName);
		file.write(":");
		file.write(u.passwdHash.toBase64());
		file.write(":");
		file.write(QByteArray::number(u.uid));
		file.write("\n");
	}
	file.close();
	return true;
}

bool IotProxyAccessMgr::addUser(const QByteArray &userName,qint32 &uid)
{
	if(!ready)return false;
	if(userId(userName)!=-1)
		return false;
	uid=++maxUid;
	User u;
	u.userName=userName;
	static const QByteArray validChars="qwertyuioplkjhgfdsazxcvbnmQWERTYUIOPLKJHGFDSAZXCVBNM1234567890._-";
	for(char c:u.userName)
		if(!validChars.contains(c))
			return false;
	u.uid=uid;
	users.append(u);
	return writeUsers();
}

qint32 IotProxyAccessMgr::userId(const QByteArray &userName)
{
	if(!ready)return -1;
	int index=userFindByName(userName);
	if(index==-1)return -1;
	return users[index].uid;
}

void IotProxyAccessMgr::rmUser(const QByteArray &userName)
{
	if(!ready)return;
	int index=userFindByName(userName);
	if(index==-1)return;
	users.removeAt(index);
	writeUsers();
}

bool IotProxyAccessMgr::userSetPass(const QByteArray &userName,const QByteArray &pass)
{
	if(!ready)return false;
	int index=userFindByName(userName);
	if(index==-1)return false;
	QCryptographicHash hash(QCryptographicHash::Sha512);
	hash.addData(pass);
	hash.addData((const char*)&users[index].uid,sizeof(User::uid));
	users[index].passwdHash=hash.result();
	writeUsers();
	return true;
}

qint32 IotProxyAccessMgr::chkUser(const QByteArray &userName,const QByteArray &pass)
{
	if(!ready)return -1;
	int index=userFindByName(userName);
	if(index==-1)return -1;
	QCryptographicHash hash(QCryptographicHash::Sha512);
	hash.addData(pass);
	hash.addData((const char*)&users[index].uid,sizeof(User::uid));
	if(users[index].passwdHash!=hash.result())return -1;
	return users[index].uid;
}

int IotProxyAccessMgr::userFindByName(const QByteArray &userName)const
{
	for(int i=0;i<users.count();++i)
		if(users[i].userName==userName)return i;
	return -1;
}

int IotProxyAccessMgr::userFindByUid(quint32 uid)const
{
	for(int i=0;i<users.count();++i)
		if(users[i].uid==uid)return i;
	return -1;
}
