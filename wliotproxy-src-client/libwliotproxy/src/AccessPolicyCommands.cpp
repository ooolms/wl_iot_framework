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

#include "wliot/client/AccessPolicyCommands.h"

using namespace WLIOT;
using namespace WLIOTClient;

AccessPolicyCommands::AccessPolicyCommands(ServerConnection *conn)
	:QObject(conn)
	,userCommands(this)
	,groupCommands(this)
	,devicesCommands(this)
{
	srvConn=conn;
}

AccessPolicyCommands::UserCommands& AccessPolicyCommands::users()
{
	return userCommands;
}

AccessPolicyCommands::GroupCommands& AccessPolicyCommands::groups()
{
	return groupCommands;
}

AccessPolicyCommands::DevCommands& AccessPolicyCommands::devices()
{
	return devicesCommands;
}

AccessPolicyCommands::UserCommands::UserCommands(AccessPolicyCommands *bPtr)
{
	base=bPtr;
}

bool AccessPolicyCommands::UserCommands::list(QList<ApmUser> &list)
{
	list.clear();
	auto onCmData=[&list](const QByteArrayList &args)->bool
	{
		if(args.count()!=2)return false;
		bool ok=false;
		ApmIdType uid=args[0].toLong(&ok);
		if(!ok||uid<=nullId||args[1].isEmpty())return false;
		list.append(ApmUser(uid,args[1]));
		return true;
	};
	if(!base->srvConn->execCommand("apm",QByteArrayList()<<"user"<<"list",onCmData))
		return false;
	return true;
}

bool AccessPolicyCommands::UserCommands::create(
	const QByteArray &userName,const QByteArray &password,ApmIdType &uid)
{
	QByteArrayList retVal;
	if(!base->srvConn->execCommand("apm",QByteArrayList()<<"user"<<"create"<<userName<<password,retVal)||
		retVal.size()!=1)
		return false;
	bool ok=false;
	uid=retVal[0].toLong(&ok);
	return ok;
}

bool AccessPolicyCommands::UserCommands::passwd(const QByteArray &oldPasswd,const QByteArray &newPasswd)
{
	return base->srvConn->execCommand("apm",QByteArrayList()<<"user"<<"passwd"<<oldPasswd<<newPasswd);
}

bool AccessPolicyCommands::UserCommands::chpass(const QByteArray &userName,const QByteArray &newPasswd)
{
	return base->srvConn->execCommand("apm",QByteArrayList()<<"user"<<"passwd"<<userName<<newPasswd);
}

bool AccessPolicyCommands::UserCommands::uid(const QByteArray &userName,ApmIdType &uid)
{
	QByteArrayList retVal;
	if(!base->srvConn->execCommand("apm",QByteArrayList()<<"user"<<"uid"<<userName,retVal)||retVal.size()!=1)
		return false;
	bool ok=false;
	uid=retVal[0].toLong(&ok);
	return ok;
}

bool AccessPolicyCommands::UserCommands::name(ApmIdType uid,QByteArray &userName)
{
	QByteArrayList retVal;
	if(!base->srvConn->execCommand("apm",QByteArrayList()<<"user"<<"name"<<QByteArray::number(uid),retVal)||
		retVal.size()!=1||retVal[0].isEmpty())
		return false;
	userName=retVal[0];
	return true;
}

AccessPolicyCommands::GroupCommands::GroupCommands(AccessPolicyCommands *bPtr)
{
	base=bPtr;
}

bool AccessPolicyCommands::GroupCommands::list(QList<ApmUsersGroup> &list)
{
	list.clear();
	auto onCmData=[&list](const QByteArrayList &args)->bool
	{
		if(args.count()!=4)return false;
		bool ok=false;
		ApmUsersGroup g;
		g.gid=args[0].toLong(&ok);
		g.groupName=args[1];
		g.moderatorUid=args[2].toLong(&ok);
		if(!ok||g.gid<=nullId||g.groupName.isEmpty())return false;
		for(QByteArray &s:args[3].split(','))
		{
			g.uids.insert(s.toLong(&ok));
			if(!ok)return false;
		}
		list.append(g);
		return true;
	};
	if(!base->srvConn->execCommand("apm",QByteArrayList()<<"grp"<<"list",onCmData))
		return false;
	return true;
}

bool AccessPolicyCommands::GroupCommands::create(const QByteArray &groupName,ApmIdType &gid)
{
	QByteArrayList retVal;
	if(!base->srvConn->execCommand("apm",QByteArrayList()<<"grp"<<"create"<<groupName,retVal)||retVal.size()!=1)
		return false;
	bool ok=false;
	gid=retVal[0].toLong(&ok);
	return ok;
}

bool AccessPolicyCommands::GroupCommands::del(const QByteArray &groupName)
{
	return base->srvConn->execCommand("apm",QByteArrayList()<<"grp"<<"del"<<groupName);
}

bool AccessPolicyCommands::GroupCommands::listUsers(const QByteArray &groupName,QByteArrayList &userNames)
{
	QByteArrayList retVal;
	if(!base->srvConn->execCommand("apm",QByteArrayList()<<"grp"<<"list_users"<<groupName,retVal))
		return false;
	userNames=retVal;
	return true;
}

bool AccessPolicyCommands::GroupCommands::addUser(const QByteArray &groupName,const QByteArray &userName)
{
	return base->srvConn->execCommand("apm",QByteArrayList()<<"grp"<<"add_user"<<groupName<<userName);
}

bool AccessPolicyCommands::GroupCommands::delUser(const QByteArray &groupName,const QByteArray &userName)
{
	return base->srvConn->execCommand("apm",QByteArrayList()<<"grp"<<"del_user"<<groupName<<userName);
}

bool AccessPolicyCommands::GroupCommands::getModer(const QByteArray &groupName,QByteArray &userName)
{
	QByteArrayList retVal;
	if(!base->srvConn->execCommand("apm",QByteArrayList()<<"grp"<<"get_moder"<<groupName,retVal)||retVal.size()!=1)
		return false;
	userName=retVal[0];
	return true;
}

bool AccessPolicyCommands::GroupCommands::setModer(const QByteArray &groupName,const QByteArray &userName)
{
	return base->srvConn->execCommand("apm",QByteArrayList()<<"grp"<<"set_moder"<<groupName<<userName);
}

bool AccessPolicyCommands::GroupCommands::gid(const QByteArray &groupName,ApmIdType &gid)
{
	QByteArrayList retVal;
	if(!base->srvConn->execCommand("apm",QByteArrayList()<<"grp"<<"uid"<<groupName,retVal)||retVal.size()!=1)
		return false;
	bool ok=false;
	gid=retVal[0].toLong(&ok);
	return ok;
}

bool AccessPolicyCommands::GroupCommands::name(ApmIdType gid,QByteArray &groupName)
{
	QByteArrayList retVal;
	if(!base->srvConn->execCommand("apm",QByteArrayList()<<"grp"<<"name"<<QByteArray::number(gid),retVal)||
		retVal.size()!=1||retVal[0].isEmpty())
		return false;
	groupName=retVal[0];
	return true;
}

AccessPolicyCommands::DevCommands::DevCommands(AccessPolicyCommands *bPtr)
{
	base=bPtr;
}

bool AccessPolicyCommands::DevCommands::list(QList<QUuid> &list)
{
	QByteArrayList retVal;
	if(!base->srvConn->execCommand("apm",QByteArrayList()<<"dev"<<"list",retVal))
		return false;
	list.clear();
	for(QByteArray &s:retVal)
	{
		QUuid id(s);
		if(s.isNull())return false;
		list.append(id);
	}
	return true;
}

bool AccessPolicyCommands::DevCommands::listRules(const QUuid &devId,QList<ApmDevicePolicyNote> &notes)
{
	notes.clear();
	auto onCmData=[&notes](const QByteArrayList &args)->bool
	{
		if(args.count()!=3)return false;
		ApmDevicePolicyNote n;
		if(args[0]=="u")
			n.userPolicy=true;
		else if(args[0]=="g")
			n.userPolicy=false;
		else return false;
		n.targetName=args[1];
		if(n.targetName.isEmpty())return false;
		for(QChar c:args[2])
		{
			if(c=='r')
				n.pol|=ApmDevicePolicyFlag::READ_STORAGES;
			else if(c=='m')
				n.pol|=ApmDevicePolicyFlag::SETUP_STORAGES;
			else if(c=='s')
				n.pol|=ApmDevicePolicyFlag::READ_STATE;
			else if(c=='e')
				n.pol|=ApmDevicePolicyFlag::EXECUTE_COMMANDS;
			else return false;
		}
		notes.append(n);
		return true;
	};
	return base->srvConn->execCommand("apm",QByteArrayList()<<"dev"<<"list_rules"<<devId.toByteArray(),onCmData);
}

bool AccessPolicyCommands::DevCommands::setRule(const QUuid &devId,const ApmDevicePolicyNote &rule)
{
	QByteArray polStr=rule.polToStr().toUtf8();
	return base->srvConn->execCommand("apm",QByteArrayList()<<"dev"<<"set_rule"<<devId.toByteArray()<<
		(rule.userPolicy?"u":"g")<<rule.targetName<<polStr);
}

bool AccessPolicyCommands::DevCommands::changeDevOwner(const QUuid &devId,const QByteArray &newOwnerName)
{
	if(newOwnerName.isEmpty())
		return base->srvConn->execCommand("apm",QByteArrayList()<<"dev"<<"chown"<<devId.toByteArray());
	else return base->srvConn->execCommand("apm",QByteArrayList()<<"dev"<<"chown"<<devId.toByteArray()<<newOwnerName);
}
