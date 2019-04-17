#include "IotServerAccessPolicyCommands.h"

IotServerAccessPolicyCommands::IotServerAccessPolicyCommands(IotServerConnection *conn,QObject *parent)
	:QObject(parent)
	,userCommands(this)
	,groupCommands(this)
	,devicesCommands(this)
{
	srvConn=conn;
}

IotServerAccessPolicyCommands::UserCommands& IotServerAccessPolicyCommands::users()
{
	return userCommands;
}

IotServerAccessPolicyCommands::GroupCommands& IotServerAccessPolicyCommands::groups()
{
	return groupCommands;
}

IotServerAccessPolicyCommands::DevCommands& IotServerAccessPolicyCommands::devices()
{
	return devicesCommands;
}

IotServerAccessPolicyCommands::UserCommands::UserCommands(IotServerAccessPolicyCommands *bPtr)
{
	base=bPtr;
}

bool IotServerAccessPolicyCommands::UserCommands::list(QList<IotServerUser> &list)
{
	list.clear();
	auto onCmData=[&list](const QByteArrayList &args)->bool
	{
		if(args.count()!=2)return false;
		bool ok=false;
		IotServerApmIdType uid=args[0].toLong(&ok);
		if(!ok||uid<=nullId||args[1].isEmpty())return false;
		list.append(IotServerUser(uid,args[1]));
		return true;
	};
	if(!base->srvConn->execCommand("apm",QByteArrayList()<<"user"<<"list",onCmData))
		return false;
	return true;
}

bool IotServerAccessPolicyCommands::UserCommands::create(
	const QByteArray &userName,const QByteArray &password,IotServerApmIdType &uid)
{
	QByteArrayList retVal;
	if(!base->srvConn->execCommand("apm",QByteArrayList()<<"user"<<"create"<<userName<<password,retVal)||
		retVal.size()!=1)
		return false;
	bool ok=false;
	uid=retVal[0].toLong(&ok);
	return ok;
}

bool IotServerAccessPolicyCommands::UserCommands::passwd(const QByteArray &oldPasswd,const QByteArray &newPasswd)
{
	return base->srvConn->execCommand("apm",QByteArrayList()<<"user"<<"passwd"<<oldPasswd<<newPasswd);
}

bool IotServerAccessPolicyCommands::UserCommands::chpass(const QByteArray &userName,const QByteArray &newPasswd)
{
	return base->srvConn->execCommand("apm",QByteArrayList()<<"user"<<"passwd"<<userName<<newPasswd);
}

bool IotServerAccessPolicyCommands::UserCommands::uid(const QByteArray &userName,IotServerApmIdType &uid)
{
	QByteArrayList retVal;
	if(!base->srvConn->execCommand("apm",QByteArrayList()<<"user"<<"uid"<<userName,retVal)||retVal.size()!=1)
		return false;
	bool ok=false;
	uid=retVal[0].toLong(&ok);
	return ok;
}

bool IotServerAccessPolicyCommands::UserCommands::name(IotServerApmIdType uid,QByteArray &userName)
{
	QByteArrayList retVal;
	if(!base->srvConn->execCommand("apm",QByteArrayList()<<"user"<<"name"<<QByteArray::number(uid),retVal)||
		retVal.size()!=1||retVal[0].isEmpty())
		return false;
	userName=retVal[0];
	return true;
}

IotServerAccessPolicyCommands::GroupCommands::GroupCommands(IotServerAccessPolicyCommands *bPtr)
{
	base=bPtr;
}

bool IotServerAccessPolicyCommands::GroupCommands::list(QList<IotServerUsersGroup> &list)
{
	list.clear();
	auto onCmData=[&list](const QByteArrayList &args)->bool
	{
		if(args.count()!=4)return false;
		bool ok=false;
		IotServerUsersGroup g;
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

bool IotServerAccessPolicyCommands::GroupCommands::create(const QByteArray &groupName,IotServerApmIdType &gid)
{
	QByteArrayList retVal;
	if(!base->srvConn->execCommand("apm",QByteArrayList()<<"grp"<<"create"<<groupName,retVal)||retVal.size()!=1)
		return false;
	bool ok=false;
	gid=retVal[0].toLong(&ok);
	return ok;
}

bool IotServerAccessPolicyCommands::GroupCommands::del(const QByteArray &groupName)
{
	return base->srvConn->execCommand("apm",QByteArrayList()<<"grp"<<"del"<<groupName);
}

bool IotServerAccessPolicyCommands::GroupCommands::listUsers(const QByteArray &groupName,QByteArrayList &userNames)
{
	QByteArrayList retVal;
	if(!base->srvConn->execCommand("apm",QByteArrayList()<<"grp"<<"list_users"<<groupName,retVal))
		return false;
	userNames=retVal;
	return true;
}

bool IotServerAccessPolicyCommands::GroupCommands::addUser(const QByteArray &groupName,const QByteArray &userName)
{
	return base->srvConn->execCommand("apm",QByteArrayList()<<"grp"<<"add_user"<<groupName<<userName);
}

bool IotServerAccessPolicyCommands::GroupCommands::delUser(const QByteArray &groupName,const QByteArray &userName)
{
	return base->srvConn->execCommand("apm",QByteArrayList()<<"grp"<<"del_user"<<groupName<<userName);
}

bool IotServerAccessPolicyCommands::GroupCommands::getModer(const QByteArray &groupName,QByteArray &userName)
{
	QByteArrayList retVal;
	if(!base->srvConn->execCommand("apm",QByteArrayList()<<"grp"<<"get_moder"<<groupName,retVal)||retVal.size()!=1)
		return false;
	userName=retVal[0];
	return true;
}

bool IotServerAccessPolicyCommands::GroupCommands::setModer(const QByteArray &groupName,const QByteArray &userName)
{
	return base->srvConn->execCommand("apm",QByteArrayList()<<"grp"<<"set_moder"<<groupName<<userName);
}

bool IotServerAccessPolicyCommands::GroupCommands::gid(const QByteArray &groupName,IotServerApmIdType &gid)
{
	QByteArrayList retVal;
	if(!base->srvConn->execCommand("apm",QByteArrayList()<<"grp"<<"uid"<<groupName,retVal)||retVal.size()!=1)
		return false;
	bool ok=false;
	gid=retVal[0].toLong(&ok);
	return ok;
}

bool IotServerAccessPolicyCommands::GroupCommands::name(IotServerApmIdType gid,QByteArray &groupName)
{
	QByteArrayList retVal;
	if(!base->srvConn->execCommand("apm",QByteArrayList()<<"grp"<<"name"<<QByteArray::number(gid),retVal)||
		retVal.size()!=1||retVal[0].isEmpty())
		return false;
	groupName=retVal[0];
	return true;
}

IotServerAccessPolicyCommands::DevCommands::DevCommands(IotServerAccessPolicyCommands *bPtr)
{
	base=bPtr;
}

bool IotServerAccessPolicyCommands::DevCommands::list(QList<QUuid> &list)
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

bool IotServerAccessPolicyCommands::DevCommands::listRules(const QUuid &devId,QList<IotServerDevicePolicyNote> &notes)
{
	QByteArrayList retVal;
	if(!base->srvConn->execCommand("apm",QByteArrayList()<<"dev"<<"list_rules"<<devId.toByteArray(),retVal)||
		retVal.size()%3!=0)
		return false;
	for(int i=0;i<retVal.size()/3;++i)
	{
		int bI=i*3;
		IotServerDevicePolicyNote n;
		if(retVal[bI]=="u")
			n.userPolicy=true;
		else if(retVal[bI]=="g")
			n.userPolicy=false;
		else return false;
		n.targetName=retVal[bI+1];
		if(n.targetName.isEmpty())return false;
		for(QChar c:retVal[bI+2])
		{
			if(c=='r')
				n.pol|=IotServerDevicePolicyFlag::READ_STORAGES;
			else if(c=='m')
				n.pol|=IotServerDevicePolicyFlag::SETUP_STORAGES;
			else if(c=='s')
				n.pol|=IotServerDevicePolicyFlag::READ_STATE;
			else if(c=='e')
				n.pol|=IotServerDevicePolicyFlag::EXECUTE_COMMANDS;
			else return false;
		}
		notes.append(n);
	}
	return true;
}

bool IotServerAccessPolicyCommands::DevCommands::setRule(const QUuid &devId,const IotServerDevicePolicyNote &rule)
{
	QByteArray polStr=rule.polToStr().toUtf8();
	return base->srvConn->execCommand("apm",QByteArrayList()<<"dev"<<"set_rule"<<devId.toByteArray()<<
		(rule.userPolicy?"u":"g")<<rule.targetName<<polStr);
}

bool IotServerAccessPolicyCommands::DevCommands::changeDevOwner(const QUuid &devId,const QByteArray &newOwnerName)
{
	if(newOwnerName.isEmpty())
		return base->srvConn->execCommand("apm",QByteArrayList()<<"dev"<<"chown"<<devId.toByteArray());
	else return base->srvConn->execCommand("apm",QByteArrayList()<<"dev"<<"chown"<<devId.toByteArray()<<newOwnerName);
}
