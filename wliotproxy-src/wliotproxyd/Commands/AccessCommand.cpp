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

#include "AccessCommand.h"
#include "../IotProxyInstance.h"
#include "wliot/devices/CommandCall.h"
#include "StandardErrors.h"
#include "../IotProxyConfig.h"

AccessCommand::AccessCommand(QtIODeviceWrap *d,IotProxyCommandProcessor *p)
	:ICommand(d,p)
	,accessMgr(IotProxyConfig::accessManager)
{
}


bool AccessCommand::processCommand(CallContext &ctx)
{
	if(ctx.args.count()<1)
	{
		ctx.retVal.append(StandardErrors::invalidAgruments);
		return false;
	}
	QByteArray subCommand=ctx.args[0];
	ctx.args.removeAt(0);
	if(subCommand=="user")
		return processUserCommand(ctx);
	else if(subCommand=="grp")
		return processUserCommand(ctx);
	else if(subCommand=="dev")
		return processDevCommand(ctx);
	ctx.retVal.append("unknown command for access policy management");
	return false;
}

QByteArrayList AccessCommand::acceptedCommands()
{
	return QByteArrayList()<<"apm";
}

bool AccessCommand::processUserCommand(ICommand::CallContext &ctx)
{
	QByteArray subCommand=ctx.args[0];
	ctx.args.removeAt(0);
	if(subCommand=="create")
	{
		if(ctx.args.count()<2)
		{
			ctx.retVal.append(StandardErrors::invalidAgruments);
			return false;
		}
		QByteArray userName=ctx.args[0];
		QByteArray password=ctx.args[1];
		IdType uid;
		if(proc->uid()!=rootUid)return false;
		if(!accessMgr.createUser(userName,uid))
			return false;
		return accessMgr.userSetPass(uid,password);
	}
	else if(subCommand=="del")
	{
		ctx.retVal.append("unsupported now");
		return false;
	}
	else if(subCommand=="passwd")
	{
		if(ctx.args.count()<1)
		{
			ctx.retVal.append(StandardErrors::invalidAgruments);
			return false;
		}
		if(ctx.args.count()==1)
		{
			QByteArray password=ctx.args[0];
			return accessMgr.userSetPass(proc->uid(),password);
		}
		else//>=2
		{
			QByteArray userName=ctx.args[0];
			QByteArray password=ctx.args[1];
			IdType uid=accessMgr.userId(userName);
			if(uid==nullId||(proc->uid()!=rootUid&&proc->uid()!=uid))
			{
				ctx.retVal.append(StandardErrors::accessDenied);
				return false;
			}
			return accessMgr.userSetPass(uid,password);
		}

	}
	else if(subCommand=="list")
	{
		const QList<User> &list=accessMgr.allUsers();
		for(const User &u:list)
			writeCmdataMsg(ctx.callId,QByteArrayList()<<QByteArray::number(u.uid)<<u.userName);
		return true;
	}
	ctx.retVal.append("unknown command for access policy management");
	return false;
}

bool AccessCommand::processUserGroupCommand(ICommand::CallContext &ctx)
{
	QByteArray subCommand=ctx.args[0];
	ctx.args.removeAt(0);
	if(subCommand=="list")
	{
		const QList<UsersGroup> &list=accessMgr.allUsersGroups();
		auto idsToList=[](const QSet<IdType> &ids)->QByteArray
		{
			QByteArray retVal;
			for(IdType id:ids)
			{
				retVal.append(QByteArray::number(id));
				retVal.append(",");
			}
			if(!retVal.isEmpty())
				retVal.chop(1);
			return retVal;
		};
		for(const UsersGroup &g:list)
			writeCmdataMsg(ctx.callId,QByteArrayList()<<QByteArray::number(g.gid)<<g.groupName<<
				QByteArray::number(g.moderatorUid)<<idsToList(g.uids));
		return true;
	}
	else if(subCommand=="create")
	{
		if(ctx.args.count()<1)
		{
			ctx.retVal.append(StandardErrors::invalidAgruments);
			return false;
		}
		QByteArray groupName=ctx.args[0];
		IdType gid;
		if(!accessMgr.userCanCreateUsersGroup(proc->uid()))
		{
			ctx.retVal.append(StandardErrors::accessDenied);
			return false;
		}
		return accessMgr.createUsersGroup(groupName,proc->uid(),gid);
	}
	if(ctx.args.count()<1)
	{
		ctx.retVal.append(StandardErrors::invalidAgruments);
		return false;
	}
	QByteArray groupName=ctx.args[0];
	IdType gid=accessMgr.groupId(groupName);
	if(gid==nullId)
	{
		ctx.retVal.append(StandardErrors::invalidAgruments);
		return false;
	}
	else if(subCommand=="del")
	{
		if(proc->uid()!=rootUid||!accessMgr.userCanManageUsersGroup(proc->uid(),gid))
		{
			ctx.retVal.append(StandardErrors::accessDenied);
			return false;
		}
		return accessMgr.delUsersGroup(gid);
	}
	else if(subCommand=="add_user")
	{
		if(ctx.args.count()<2)
		{
			ctx.retVal.append(StandardErrors::invalidAgruments);
			return false;
		}
		QByteArray userName=ctx.args[1];
		IdType uid=accessMgr.userId(userName);
		if(uid==nullId||!accessMgr.userCanManageUsersInUsersGroup(uid,gid))
		{
			ctx.retVal.append(StandardErrors::invalidAgruments);
			return false;
		}
		return accessMgr.addUserToGroup(gid,uid);
	}
	else if(subCommand=="del_user")
	{
		if(ctx.args.count()<2)
		{
			ctx.retVal.append(StandardErrors::invalidAgruments);
			return false;
		}
		QByteArray userName=ctx.args[1];
		IdType uid=accessMgr.userId(userName);
		if(uid==nullId||!accessMgr.userCanManageUsersInUsersGroup(uid,gid))
		{
			ctx.retVal.append(StandardErrors::invalidAgruments);
			return false;
		}
		return accessMgr.delUserFromGroup(gid,uid);
	}
	else if(subCommand=="list_users")
	{
		QSet<IdType> uids=accessMgr.groupUsers(gid);
		QByteArrayList names;
		for(IdType uid:uids)
		{
			QByteArray name=accessMgr.userName(uid);
			if(name.isEmpty())
			{
				ctx.retVal.append(StandardErrors::someStrangeError);
				return false;
			}
			names.append(name);
		}
		ctx.retVal.append(names);
		return true;
	}
	else if(subCommand=="get_moder")
	{
		IdType uid=accessMgr.moderatorId(gid);
		QByteArray userName=accessMgr.userName(uid);
		if(userName.isEmpty())
		{
			ctx.retVal.append(StandardErrors::someStrangeError);
			return false;
		}
		ctx.retVal.append(userName);
		return true;
	}
	else if(subCommand=="set_moder")
	{
		if(ctx.args.count()<2)
		{
			ctx.retVal.append(StandardErrors::invalidAgruments);
			return false;
		}
		QByteArray userName=ctx.args[1];
		IdType uid=accessMgr.userId(userName);
		if(uid==nullId||!accessMgr.userCanManageUsersGroup(proc->uid(),gid)||
			!accessMgr.changeUsersGroupModerator(gid,uid))
		{
			ctx.retVal.append(StandardErrors::accessDenied);
			return false;
		}
		return true;
	}
	ctx.retVal.append("unknown command for access policy management");
	return false;
}

bool AccessCommand::processDevCommand(ICommand::CallContext &ctx)
{
	if(ctx.args.count()<3)
	{
		ctx.retVal.append(StandardErrors::invalidAgruments);
		return false;
	}
	QByteArray devIdOrName=ctx.args[0];
	bool grpPolicy;
	if(ctx.args[1]=="g"||ctx.args[1]=="grp")
		grpPolicy=true;
	else if(ctx.args[1]=="u"||ctx.args[1]=="user")
		grpPolicy=false;
	else
	{
		ctx.retVal.append(StandardErrors::invalidAgruments);
		return false;
	}
	QByteArray userOrGroupName=ctx.args[2];
	DevicePolicyActionFlags flags=DevicePolicyActionFlag::NO_RULE;
	if(ctx.args.count()>3)
	{
		QByteArray polStr=ctx.args[3];
		for(char c:polStr)
		{
			if(c=='r')
				flags|=DevicePolicyActionFlag::READ_STORAGES;
			else if(c=='m')
				flags|=DevicePolicyActionFlag::SETUP_STORAGES;
			else if(c=='s')
				flags|=DevicePolicyActionFlag::READ_STATE;
			else if(c=='e')
				flags|=DevicePolicyActionFlag::EXECUTE_COMMANDS;
			else
			{
				ctx.retVal.append(StandardErrors::invalidAgruments);
				return false;
			}
		}
	}
	QUuid devId(devIdOrName);
	if(devId.isNull())
	{
		RealDevice *dev=IotProxyInstance::inst().devices()->deviceByIdOrName(devIdOrName);
		if(!dev)
		{
			devId=IotProxyInstance::inst().storages()->findDeviceId(devIdOrName);
			if(devId.isNull())
			{
				ctx.retVal.append(QByteArray(StandardErrors::noDeviceFound).replace("%1",devIdOrName));
				return false;
			}
		}
		else devId=dev->id();
	}
	if(proc->uid()==nullId||(proc->uid()!=rootUid&&proc->uid()!=accessMgr.devOwner(devId)))
	{
		ctx.retVal.append(StandardErrors::accessDenied);
		return false;
	}
	IdType id;
	if(grpPolicy)
		id=accessMgr.groupId(userOrGroupName);
	else id=accessMgr.userId(userOrGroupName);
	if(id==nullId)
	{
		ctx.retVal.append(StandardErrors::invalidAgruments);
		return false;
	}
	if(grpPolicy)
		return accessMgr.setDevicePolicyForUsersGroup(devId,id,flags);
	else return accessMgr.setDevicePolicyForUser(devId,id,flags);
}
