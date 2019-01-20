#include "AccessMgr.h"
#include <QFile>
#include <QCryptographicHash>
#include <QSet>
#include <QDir>

/*
Структура каталогов
/var/lib/wliotproxyd/access_policy/
	users/
		users
		groups
	devices/
		owners
		groups
		single_policies/
			{uuid} - файлы с политиками доступа для отдельных устройств
		group_policies/
			{gid} - файлы с политиками для групп устройств

Все файлы организованы как набор строк-записей.
Каждая запись представляет собой набор полей, разделенных символом ":".
Количество полей в каждой записи одинаковое.
Ниже описаны поля для каждого из файлов конфигурации в порядке следования в записи.
Все идентификаторы начинаются с 1, за исключение uid суперпользователя, равного 0.
Идентикаторы устройств - в формате uuid
Во всех файлах разрешены комментарии - строки, начинающиеся с "#"

Поля файла users/users:
	uid - идентификатор пользователя (уникальный)
	user_name - имя пользователя (уникальное)
	password_hash - sha-512 хэш пароля в base64 кодировке
	can_create_user_groups_flag - если 1, пользователь может создавать группы пользователей, если 0 или пустой - не может
	can_catch_devices_flag - если 1, пользователь может захватывать ничейные устройства, если 0 или пустой - не может

Поля файла users/groups:
	gid - идентификатор группы
	group_name - название группы (уникальное)
	moderatorUid - идентификатор пользователя-модератора группы
	uids - список идентификаторов пользователей в группе, через ","

Поля файла devices/owners:
	deviceId - идентификатор устройства
	uid - идентификатор пользователя-владельца

Поля файла devices/groups:
	gid - идентификатор группы
	uid - идентификатор пользователя
	group_name - название группы
	devicesIds - список идентификаторов устройств в группе, через ","

Поля файла политик для устройств:
	actor_flag - "g", если политика применяется для группы пользователей, "u" - если для одного пользователя
	id - идентификатор пользователя или группы пользователей
	allow_flag - "a", если политика разрешающая, "d" - если запрещающая
	action - строка, описывающая действия, попадающие под политику.
		Предатсвляет собой комбинацию из символов "r", "m", "s" и "e". "r" (read) - чтение данных из хранилищ устройства,
		"m" (modify) - управление хранилищами устройства, "s" (state) - запрос состояния устройства, "e" (execute) -
		отправка команд устройству.

Политика "m" включает в себя политику "r". Таким образом, если разрешено управление хранилищами, значит разрешено и
чтение данных. И наоборот, если запрещено чтение данных, то запрещено и управление хранилищами. Аналогично политика "e"
включает в себя политику "s".
*/

AccessMgr::AccessMgr()
{
	ready=false;
	maxUserId=maxUserGroupId=1;
}

AccessMgr::~AccessMgr()
{
}

bool AccessMgr::readUsers()
{
	users.clear();
	maxUserId=1;
	if(!QFile::exists("/var/lib/wliotproxyd/users/users"))
	{
		User u;
		u.uid=0;
		u.userName="root";
		users.append(u);
		writeUsers();
		return true;
	}
	QSet<QByteArray> userNames;
	QSet<IdType> userIds;
	bool hasRoot=false;
	std::function<bool(const QByteArrayList&)> f=[this,&userIds,&userNames,&hasRoot](const QByteArrayList &fields)->bool
	{
		bool ok=false;
		User u;
		u.uid=fields[0].toULong(&ok);
		if(!ok||u.uid<0)return false;
		u.userName=fields[1];
		if(u.userName.isEmpty())return false;
		if(userNames.contains(u.userName)||userIds.contains(u.uid))
			return false;

		u.passwdHash=QByteArray::fromBase64(fields[2]);
		if(fields[3]=="1")
			u.policy|=UserPolicyFlag::CAN_CREATE_USER_GROUPS;
		else if(!fields[3].isEmpty()&&fields[3]!="0")return false;
		if(fields[4]=="1")
			u.policy|=UserPolicyFlag::CAN_CATCH_DEVICES;
		else if(!fields[4].isEmpty()&&fields[4]!="0")return false;

		users.append(u);
		userNames.insert(u.userName);
		userIds.insert(u.uid);
		if(u.uid==0)
			hasRoot=true;
		if(u.uid>maxUserId)
			maxUserId=u.uid;
		return true;
	};
	if(!readConfigFile("/var/lib/wliotproxyd/users/users",5,f))return false;
	if(!hasRoot)
	{
		User u;
		u.uid=0;
		u.userName="root";
		users.prepend(u);
		writeUsers();
	}
	return true;
}

bool AccessMgr::writeUsers()
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

bool AccessMgr::readUserGroups()
{
	userGroups.clear();
	maxUserGroupId=1;
	if(!QFile::exists("/var/lib/wliotproxyd/users/groups"))return true;
	QSet<QByteArray> userGroupNames;
	QSet<IdType> userGroupIds;
	std::function<bool(const QByteArrayList&)> f=
		[this,&userGroupIds,&userGroupNames](const QByteArrayList &fields)->bool
	{
		bool ok=false;
		UsersGroup g;
		g.gid=fields[0].toLong(&ok);
		if(!ok||g.gid<=0)return false;
		g.groupName=fields[1];
		if(g.groupName.isEmpty())return false;
		if(userGroupNames.contains(g.groupName)||userGroupIds.contains(g.gid))
			return false;

		g.moderatorUid=fields[2].toULong(&ok);
		if(!ok)return false;
		if(userFindByUid(g.moderatorUid)==-1)return false;
		QByteArrayList uidsStrs=fields[3].split(',');
		for(QByteArray &uS:uidsStrs)
		{
			IdType uid=uS.toLong(&ok);
			if(!ok||userFindByUid(uid)==-1)return false;
			g.uids.append(uid);
		}

		userGroups.append(g);
		userGroupNames.insert(g.groupName);
		userGroupIds.insert(g.gid);
		if(g.gid>maxUserGroupId)
			maxUserGroupId=g.gid;
		return true;
	};
	return readConfigFile("/var/lib/wliotproxyd/users/groups",4,f);
}

bool AccessMgr::addUser(const QByteArray &userName, IdType &uid)
{
	if(!ready)return false;
	if(userId(userName)!=-1)
		return false;
	uid=++maxUserId;
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

IdType AccessMgr::userId(const QByteArray &userName)
{
	if(!ready)return -1;
	int index=userFindByName(userName);
	if(index==-1)return -1;
	return users[index].uid;
}

void AccessMgr::rmUser(const QByteArray &userName)
{
	if(!ready)return;
	int index=userFindByName(userName);
	if(index==-1)return;
	users.removeAt(index);
	writeUsers();
}

bool AccessMgr::userSetPass(const QByteArray &userName,const QByteArray &pass)
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

bool AccessMgr::readConfig()
{
	QDir("/var/lib/wliotproxyd/users/").mkpath(".");
	QDir("/var/lib/wliotproxyd/devices/").mkpath(".");
	QDir("/var/lib/wliotproxyd/devices/single_policies").mkpath(".");
	QDir("/var/lib/wliotproxyd/devices/group_policies").mkpath(".");
	if(!readUsers())return false;
	ready=true;
	return true;
}

IdType AccessMgr::authentificateUser(const QByteArray &userName,const QByteArray &pass)
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

int AccessMgr::userFindByName(const QByteArray &userName)const
{
	for(int i=0;i<users.count();++i)
		if(users[i].userName==userName)return i;
	return -1;
}

int AccessMgr::userFindByUid(IdType uid)const
{
	for(int i=0;i<users.count();++i)
		if(users[i].uid==uid)return i;
	return -1;
}

bool AccessMgr::readConfigFile(const QString &filePath,int fieldsCount,
	std::function<bool(const QByteArrayList &)>lineParseFunc)
{
	QFile file(filePath);
	if(!file.open(QIODevice::ReadOnly))
		return false;
	while(!file.atEnd())
	{
		QByteArray line=file.readLine().trimmed();
		if(line.startsWith("#"))continue;
		if(line.endsWith('\n'))
			line.chop(1);
		QByteArrayList fields=line.split(':');
		if(fields.count()==0||(fields.count()==1&&fields[0].isEmpty()))continue;
		if(fields.count()!=fieldsCount)return false;
		if(!lineParseFunc(fields))return false;
	}
	file.close();
	return true;
}
