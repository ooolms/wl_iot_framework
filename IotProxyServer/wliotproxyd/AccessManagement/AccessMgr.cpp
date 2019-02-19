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

#include "AccessMgr.h"
#include <QFile>
#include <QCryptographicHash>
#include <QSet>
#include <QDir>

AccessMgr::AccessMgr()
{
	ready=false;
	maxUserId=maxUserGroupId=1;
	mUsersCanManageDevices=false;
	mUsersCanManageGroups=false;
}

AccessMgr::~AccessMgr()
{
}

bool AccessMgr::readUsers()
{
	users.clear();
	maxUserId=minFreeId;
	if(!QFile::exists("/var/lib/wliotproxyd/users/users"))
	{
		User u;
		u.uid=rootUid;
		u.userName="root";
		users.append(u);
		return writeUsers();
	}
	QSet<QByteArray> userNames;
	QSet<IdType> userIds;
	bool hasRoot=false;
	std::function<bool(const QByteArrayList&)> f=[this,&userIds,&userNames,&hasRoot](const QByteArrayList &fields)->bool
	{
		bool ok=false;
		User u;
		u.uid=fields[0].toULong(&ok);
		if(!ok||u.uid<rootUid)return false;
		u.userName=fields[1];
		if(u.userName.isEmpty())return false;
		if(userNames.contains(u.userName)||userIds.contains(u.uid))
			return false;

		u.passwdHash=QByteArray::fromBase64(fields[2]);
		if(fields[3]=="1")
			u.policy|=UserPolicyFlag::CAN_MANAGE_USER_GROUPS;
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
		u.uid=rootUid;
		u.userName="root";
		users.prepend(u);
		return writeUsers();
	}
	return true;
}

bool AccessMgr::writeUsers()
{
	QFile file("/var/lib/wliotproxyd/users/users");
	if(!file.open(QIODevice::WriteOnly))
		return false;
	for(User &u:users)
	{
		file.write(QByteArray::number(u.uid));
		file.write(":");
		file.write(u.userName);
		file.write(":");
		file.write(u.passwdHash.toBase64());
		file.write(":");
		file.write((u.policy&UserPolicyFlag::CAN_MANAGE_USER_GROUPS)?"1":"0");
		file.write(":");
		file.write((u.policy&UserPolicyFlag::CAN_CATCH_DEVICES)?"1":"0");
		file.write("\n");
	}
	file.close();
	return true;
}

bool AccessMgr::readUserGroups()
{
	userGroups.clear();
	maxUserGroupId=minFreeId;
	if(!QFile::exists("/var/lib/wliotproxyd/users/groups"))return true;
	QSet<QByteArray> userGroupNames;
	QSet<IdType> userGroupIds;
	std::function<bool(const QByteArrayList&)> f=
		[this,&userGroupIds,&userGroupNames](const QByteArrayList &fields)->bool
	{
		bool ok=false;
		UsersGroup g;
		g.gid=fields[0].toLong(&ok);
		if(!ok||g.gid<minFreeId)return false;
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
			g.uids.insert(uid);
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

bool AccessMgr::writeUserGroups()
{
	QFile file("/var/lib/wliotproxyd/users/groups");
	if(!file.open(QIODevice::WriteOnly))
		return false;
	for(UsersGroup &g:userGroups)
	{
		file.write(QByteArray::number(g.gid));
		file.write(":");
		file.write(g.groupName);
		file.write(":");
		file.write(QByteArray::number(g.moderatorUid));
		file.write(":");
		file.write(idsSetToString(g.uids));
		file.write("\n");
	}
	file.close();
	return true;
}

bool AccessMgr::readDeviceOwners()
{
	deviceOwners.clear();
	if(!QFile::exists("/var/lib/wliotproxyd/devices/owners"))return true;
	std::function<bool(const QByteArrayList&)> f=
		[this](const QByteArrayList &fields)->bool
	{
		bool ok=false;
		QUuid id(fields[0]);
		IdType uid=fields[1].toLong(&ok);
		if(id.isNull()||!ok||userFindByUid(uid)==-1)return false;
		deviceOwners[id]=uid;
		return true;
	};
	return readConfigFile("/var/lib/wliotproxyd/devices/owners",2,f);
}

bool AccessMgr::writeDeviceOwners()
{
	QFile file("/var/lib/wliotproxyd/devices/owners");
	if(!file.open(QIODevice::WriteOnly))
		return false;
	for(auto i=deviceOwners.begin();i!=deviceOwners.end();++i)
	{
		file.write(i.key().toByteArray());
		file.write(":");
		file.write(QByteArray::number(i.value()));
		file.write("\n");
	}
	file.close();
	return true;
}

bool AccessMgr::readSingleDevicePolicies()
{
	devicesPolicy.clear();
	compiledUsersPolicy.clear();
	QStringList policyFiles=QDir("/var/lib/wliotproxyd/devices/single_policies/").entryList(QDir::Files);
	for(QString &f:policyFiles)
	{
		QUuid devId(f);
		if(devId.isNull())continue;
		devicesPolicy[devId]=DevicePolicy();
		DevicePolicy &pol=devicesPolicy[devId];
		std::function<bool(const QByteArrayList&)> func=
			[this,&pol](const QByteArrayList &fields)->bool
		{
			bool ok=false;
			IdType id=fields[1].toLong(&ok);
			if(!ok||id<minFreeId)return false;
			DevicePolicyActionFlags flags=DevicePolicyActionFlag::NO_RULE;
			for(char c:fields[2])
			{
				if(c=='r')
					flags|=DevicePolicyActionFlag::READ_STORAGES;
				else if(c=='m')
					flags|=DevicePolicyActionFlag::SETUP_STORAGES;
				else if(c=='s')
					flags|=DevicePolicyActionFlag::READ_STATE;
				else if(c=='e')
					flags|=DevicePolicyActionFlag::EXECUTE_COMMANDS;
				else return false;
			}

			if(fields[0]=="u")
			{
				if(userFindByUid(id)==-1)return true;//ignore error
				pol.userFlags[id]=flags;
			}
			else if(fields[1]=="g")
			{
				if(usersGroupFindByUid(id)==-1)return true;//ignore error
				pol.groupFlags[id]=flags;
			}
			else return false;
			return true;
		};
		return readConfigFile("/var/lib/wliotproxyd/devices/single_policies/"+f,4,func);
	}
	compileUsersPolicy();
	return true;
}

bool AccessMgr::writeSingleDevicePolicy(const QUuid &id)
{
	if(!devicesPolicy.contains(id))
	{
		QFile::remove("/var/lib/wliotproxyd/devices/single_policies/"+id.toString());
		return true;
	}
	QFile file("/var/lib/wliotproxyd/devices/single_policies/"+id.toString());
	if(!file.open(QIODevice::WriteOnly))
		return false;
	DevicePolicy &pol=devicesPolicy[id];
	for(auto i=pol.groupFlags.begin();i!=pol.groupFlags.end();++i)
	{
		DevicePolicyActionFlags flags=i.value();
		QByteArray rulesStr;
		if(flags&DevicePolicyActionFlag::READ_STORAGES)
			rulesStr.append('r');
		if(flags&DevicePolicyActionFlag::SETUP_STORAGES)
			rulesStr.append('m');
		if(flags&DevicePolicyActionFlag::READ_STATE)
			rulesStr.append('s');
		if(flags&DevicePolicyActionFlag::EXECUTE_COMMANDS)
			rulesStr.append('e');
		file.write("g:");
		file.write(QByteArray::number(i.key()));
		file.write(":");
		file.write(rulesStr);
		file.write("\n");
	}
	file.close();
	return true;
}

int AccessMgr::groupFindByGid(IdType gid) const
{
	for(int i=0;i<userGroups.count();++i)
		if(userGroups[i].gid==gid)
			return i;
	return -1;
}

int AccessMgr::groupFindByName(const QByteArray &groupName)const
{
	for(int i=0;i<userGroups.count();++i)
		if(userGroups[i].groupName==groupName)
			return i;
	return -1;
}

bool AccessMgr::createUser(const QByteArray &userName,IdType &uid)
{
	if(!ready)return false;
	if(userFindByName(userName)!=-1||userName.isEmpty())
		return false;
	static const QByteArray validChars="qwertyuioplkjhgfdsazxcvbnmQWERTYUIOPLKJHGFDSAZXCVBNM1234567890._-";
	static const QByteArray validChars1Sym="qwertyuioplkjhgfdsazxcvbnmQWERTYUIOPLKJHGFDSAZXCVBNM";
	if(!validChars1Sym.contains(userName[0]))return false;
	for(char c:userName)
		if(!validChars.contains(c))
			return false;
	User u;
	u.userName=userName;
	u.uid=++maxUserId;
	users.append(u);
	if(!writeUsers())
	{
		users.removeLast();
		--maxUserId;
		return false;
	}
	uid=u.uid;
	return true;
}

IdType AccessMgr::userId(const QByteArray &userName)
{
	if(!ready)return -1;
	int index=userFindByName(userName);
	if(index==-1)return -1;
	return users[index].uid;
}

bool AccessMgr::delUser(const QByteArray &userName)
{
	if(!ready)return false;
	Q_UNUSED(userName)
	return false;//now not supported
	//IMPL
//	int index=userFindByName(userName);
//	if(index==-1)return;
//	users.removeAt(index);
//	//TODO clear groups, policies
//	writeUsers();
}

bool AccessMgr::userSetPass(IdType uid,const QByteArray &pass)
{
	if(!ready)return false;
	int index=userFindByUid(uid);
	if(index==-1)return false;
	QCryptographicHash hash(QCryptographicHash::Sha512);
	hash.addData(pass);
	hash.addData((const char*)&users[index].uid,sizeof(User::uid));
	users[index].passwdHash=hash.result();
	writeUsers();
	return true;
}

const QList<User>& AccessMgr::allUsers()
{
	return users;
}

QByteArray AccessMgr::userName(IdType uid)
{
	int index=userFindByUid(uid);
	if(index==-1)return QByteArray();
	return users[index].userName;
}

bool AccessMgr::createUsersGroup(const QByteArray &groupName,IdType moderatorUid,IdType &gid)
{
	if(groupFindByName(groupName)!=-1||userFindByUid(moderatorUid)==-1)return false;
	UsersGroup grp;
	grp.gid=++maxUserGroupId;
	grp.groupName=groupName;
	grp.moderatorUid=moderatorUid;
	grp.uids.insert(moderatorUid);
	userGroups.append(grp);
	if(!writeUserGroups())
	{
		--maxUserGroupId;
		userGroups.removeLast();
		return false;
	}
	gid=grp.gid;
	return true;
}

bool AccessMgr::addUserToGroup(IdType gid,IdType uid)
{
	if(userFindByUid(uid)==-1)return false;
	int index=groupFindByGid(gid);
	if(index==-1)return false;
	UsersGroup &grp=userGroups[index];
	if(grp.uids.contains(uid))return false;
	grp.uids.insert(uid);
	if(!writeUserGroups())
	{
		grp.uids.remove(uid);
		return false;
	}
	return true;
}

bool AccessMgr::delUserFromGroup(IdType gid,IdType uid)
{
	if(userFindByUid(uid)==-1)return false;
	int index=groupFindByGid(gid);
	if(index==-1)return false;
	UsersGroup &grp=userGroups[index];
	if(!grp.uids.contains(uid))
		return true;
	if(grp.moderatorUid==uid)
		return false;
	grp.uids.remove(uid);
	if(!writeUserGroups())
	{
		grp.uids.insert(uid);
		return false;
	}
	return true;
}

bool AccessMgr::changeUsersGroupModerator(IdType gid,IdType uid)
{
	if(userFindByUid(uid)==-1)return false;
	int index=groupFindByGid(gid);
	if(index==-1)return false;
	UsersGroup &grp=userGroups[index];
	if(uid!=rootUid&&!grp.uids.contains(uid))
		return false;
	IdType currId=grp.moderatorUid;
	grp.moderatorUid=uid;
	if(!writeUserGroups())
	{
		grp.moderatorUid=currId;
		return false;
	}
	return true;
}

bool AccessMgr::delUsersGroup(IdType gid)
{
	if(!mUsersCanManageGroups)return false;
	int index=groupFindByGid(gid);
	if(index==-1)return false;
	UsersGroup grp=userGroups[index];
	userGroups.removeAt(index);
	if(!writeUserGroups())
	{
		userGroups.insert(index,grp);
		return false;
	}
	bool policyModified=false;
	for(auto i=devicesPolicy.begin();i!=devicesPolicy.end();++i)
	{
		DevicePolicy &pol=i.value();
		if(pol.groupFlags.contains(gid))
		{
			pol.groupFlags.remove(gid);
			writeSingleDevicePolicy(i.key());
			policyModified=true;
		}
	}
	if(policyModified)
		compileUsersPolicy();
	return true;
}

IdType AccessMgr::usersGroupId(const QByteArray &groupName)
{
	int index=groupFindByName(groupName);
	if(index==-1)return nullId;
	return userGroups[index].gid;
}

const QList<UsersGroup> &AccessMgr::allUsersGroups()
{
	return userGroups;
}

IdType AccessMgr::moderatorId(IdType gid)const
{
	int index=groupFindByGid(gid);
	if(index==-1)return nullId;
	return userGroups[index].moderatorUid;
}

bool AccessMgr::userCanManageUsersInUsersGroup(IdType uid,IdType gid)const
{
	if(uid==nullId)return false;
	int index=groupFindByGid(gid);
	if(index==-1)return false;
	return uid==rootUid||(uid==userGroups[index].moderatorUid);
}

bool AccessMgr::userCanCreateUsersGroup(IdType uid)const
{
	if(uid==nullId)return false;
	if(uid==rootUid)return true;
	int index=userFindByUid(uid);
	if(index==-1)return false;
	return mUsersCanManageGroups||(users[index].policy&UserPolicyFlag::CAN_MANAGE_USER_GROUPS);
}

bool AccessMgr::userCanManageUsersGroup(IdType uid,IdType gid)const
{
	if(uid==nullId)return false;
	if(uid==rootUid)return true;
	int index=groupFindByGid(gid);
	if(index==-1)return false;
	if(userGroups[index].moderatorUid!=uid)return false;
	index=userFindByUid(uid);
	if(index==-1)return false;
	return mUsersCanManageGroups||(users[index].policy&UserPolicyFlag::CAN_MANAGE_USER_GROUPS);
}

IdType AccessMgr::groupId(const QByteArray &groupName)
{
	int index=groupFindByName(groupName);
	if(index==-1)return nullId;
	return userGroups[index].gid;
}

QSet<IdType> AccessMgr::groupUsers(IdType gid)
{
	int index=groupFindByGid(gid);
	if(index==-1)return QSet<IdType>();
	return userGroups[index].uids;
}

IdType AccessMgr::devOwner(const QUuid &devId)
{
	return deviceOwners.value(devId,nullId);
}

bool AccessMgr::setDevOwner(const QUuid &devId,IdType uid)
{
	if(!ready)return false;
	if(userFindByUid(uid)==-1)return false;
	deviceOwners[devId]=uid;
	writeDeviceOwners();//TODO process errors
	if(devicesPolicy.contains(devId))
	{
		devicesPolicy.remove(devId);
		writeSingleDevicePolicy(devId);
		compileUsersPolicy();
	}
	return true;
	//TODO remove from devices groups
}

bool AccessMgr::setDevicePolicyForUser(const QUuid &devId,IdType uid,DevicePolicyActionFlags flags)
{
	if(!ready)return false;
	if(userFindByUid(uid)==-1)return false;
	DevicePolicy &dPol=devicesPolicy[devId];
	if(flags==0)
		dPol.userFlags.remove(uid);
	else dPol.userFlags[uid]=flags;
	CompiledUserPolicy uPol=compiledUsersPolicy[uid];
	uPol.groupPolicy.remove(devId);
	uPol.selfPolicy[devId]=flags;
	writeSingleDevicePolicy(devId);
	return true;
}

bool AccessMgr::setDevicePolicyForUsersGroup(const QUuid &devId,IdType gid,DevicePolicyActionFlags flags)
{
	if(!ready)return false;
	int index=usersGroupFindByUid(gid);
	if(index==-1)return false;
	UsersGroup &gr=userGroups[index];
	DevicePolicy &dPol=devicesPolicy[devId];
	if(flags==0)
		dPol.groupFlags.remove(gid);
	else dPol.groupFlags[gid]=flags;
	for(IdType uid:gr.uids)
	{
		CompiledUserPolicy uPol=compiledUsersPolicy[uid];
		if(!uPol.selfPolicy.contains(devId))
			compiledUsersPolicy[uid].groupPolicy[devId]=flags;
	}
	writeSingleDevicePolicy(devId);
	return true;
}

bool AccessMgr::userCanAccessDevice(const QUuid &devId,IdType uid,DevicePolicyActionFlag flag)
{
	if(uid==nullId)return false;
	if(uid==rootUid)return true;
	if(devOwner(devId)==uid)return true;
	if(compiledUsersPolicy.contains(uid))
	{
		CompiledUserPolicy &pol=compiledUsersPolicy[uid];
		if(pol.selfPolicy.contains(devId))
			return pol.selfPolicy[devId]&flag;
		if(pol.groupPolicy.contains(devId))
			return pol.groupPolicy[devId]&flag;
	}
	return false;
}

bool AccessMgr::userCanChangeDeviceOwner(const QUuid &devId,IdType uid)
{
	if(uid==nullId)return false;
	if(uid==rootUid)return true;
	IdType ownUid=devOwner(devId);
	int index=userFindByUid(uid);
	if(index==-1)return false;
	if(ownUid==uid)
		return mUsersCanManageDevices;
	if(ownUid!=nullId)return false;
	return mUsersCanManageDevices||(users[index].policy&UserPolicyFlag::CAN_CATCH_DEVICES);
}

bool AccessMgr::readConfig()
{
	QDir dir("/");
	dir.mkpath("/var/lib/wliotproxyd/users");
	dir.mkpath("/var/lib/wliotproxyd/devices");
	dir.mkpath("/var/lib/wliotproxyd/devices/single_policies");
	dir.mkpath("/var/lib/wliotproxyd/devices/group_policies");
	if(!readUsers())return false;
	if(!readUserGroups())return false;
	if(!readDeviceOwners())return false;
	if(!readSingleDevicePolicies())return false;
	ready=true;
	return true;
}

IdType AccessMgr::authentificateUser(const QByteArray &userName,const QByteArray &pass)
{
	if(!ready)return nullId;
	int index=userFindByName(userName);
	if(index==-1)return nullId;
	QCryptographicHash hash(QCryptographicHash::Sha512);
	hash.addData(pass);
	hash.addData((const char*)&users[index].uid,sizeof(User::uid));
	if(users[index].passwdHash!=hash.result())
		return nullId;
	return users[index].uid;
}

void AccessMgr::setUsersCanCaptureDevices(bool en)
{
	mUsersCanManageDevices=en;
}

void AccessMgr::setUsersCanManageGroups(bool en)
{
	mUsersCanManageGroups=en;
}

bool AccessMgr::usersCanManageGroups()
{
	return mUsersCanManageGroups;
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

int AccessMgr::usersGroupFindByUid(IdType gid)const
{
	for(int i=0;i<userGroups.count();++i)
		if(userGroups[i].gid==gid)return i;
	return -1;
}

void AccessMgr::compileUsersPolicy()
{
	compiledUsersPolicy.clear();
	for(auto i=devicesPolicy.begin();i!=devicesPolicy.end();++i)
	{
		QUuid devId=i.key();
		DevicePolicy &dPol=i.value();
		for(auto j=dPol.groupFlags.begin();j!=dPol.groupFlags.end();++j)
		{
			IdType gid=j.key();
			UsersGroup &gr=userGroups[usersGroupFindByUid(gid)];
			for(IdType uid:gr.uids)
			{
				DevicePolicyActionFlags &flags=compiledUsersPolicy[uid].groupPolicy[devId];
				flags=j.value();
				if(flags&DevicePolicyActionFlag::EXECUTE_COMMANDS)
					flags|=DevicePolicyActionFlag::READ_STATE;
				if(flags&DevicePolicyActionFlag::SETUP_STORAGES)
					flags|=DevicePolicyActionFlag::READ_STORAGES;
			}
		}
	}
	for(auto i=devicesPolicy.begin();i!=devicesPolicy.end();++i)
	{
		QUuid devId=i.key();
		DevicePolicy &dPol=i.value();
		for(auto j=dPol.userFlags.begin();j!=dPol.userFlags.end();++j)
		{
			IdType uid=j.key();
			CompiledUserPolicy &uPol=compiledUsersPolicy[uid];
			uPol.groupPolicy.remove(devId);
			DevicePolicyActionFlags &flags=compiledUsersPolicy[uid].selfPolicy[devId];
			flags=j.value();
			if(flags&DevicePolicyActionFlag::EXECUTE_COMMANDS)
				flags|=DevicePolicyActionFlag::READ_STATE;
			if(flags&DevicePolicyActionFlag::SETUP_STORAGES)
				flags|=DevicePolicyActionFlag::READ_STORAGES;
		}
	}
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

QByteArray AccessMgr::idsListToString(const QList<IdType> &ids)
{
	QByteArray retVal;
	for(const IdType &id:ids)
	{
		retVal.append(QByteArray::number(id));
		retVal.append(",");
	}
	if(!retVal.isEmpty())
		retVal.chop(1);
	return retVal;
}

QByteArray AccessMgr::idsSetToString(const QSet<IdType> &ids)
{
	QByteArray retVal;
	for(const IdType &id:ids)
	{
		retVal.append(QByteArray::number(id));
		retVal.append(",");
	}
	if(!retVal.isEmpty())
		retVal.chop(1);
	return retVal;
}
