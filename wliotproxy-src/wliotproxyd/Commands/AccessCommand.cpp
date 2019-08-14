/************************* ******************
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
#include "../ServerInstance.h"
#include "wliot/devices/CommandCall.h"
#include "StandardErrors.h"
#include "../MainServerConfig.h"

AccessCommand::AccessCommand(CommandProcessor *p)
	:ICommand(p)
	,accessMgr(MainServerConfig::accessManager)
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
		return processUserGroupCommand(ctx);
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
	if(ctx.args.count()<1)
	{
		ctx.retVal.append(StandardErrors::invalidAgruments);
		return false;
	}
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
		ctx.retVal.append(QByteArray::number(uid));
		return accessMgr.userSetPass(uid,password);
	}
	else if(subCommand=="del")
	{
		ctx.retVal.append("unsupported now");
		return false;
	}
	else if(subCommand=="chpass")
	{
		if(ctx.args.count()<2)
		{
			ctx.retVal.append(StandardErrors::invalidAgruments);
			return false;
		}
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
	else if(subCommand=="passwd")
	{
		if(ctx.args.count()<2)
		{
			ctx.retVal.append(StandardErrors::invalidAgruments);
			return false;
		}
		QByteArray oldPassword=ctx.args[0];
		QByteArray newPassword=ctx.args[1];
		if(accessMgr.authentificateUser(accessMgr.userName(proc->uid()),oldPassword)==nullId)
		{
			ctx.retVal.append(StandardErrors::accessDenied);
			return false;
		}
		return accessMgr.userSetPass(proc->uid(),newPassword);
	}
	else if(subCommand=="list")
	{
		const QMap<IdType,User> &list=accessMgr.allUsers();
		for(const User &u:list)
			writeCmdataMsg(ctx.callId,QByteArrayList()<<QByteArray::number(u.uid)<<u.userName);
		return true;
	}
	else if(subCommand=="uid")
	{
		if(ctx.args.count()<1)
		{
			ctx.retVal.append(StandardErrors::invalidAgruments);
			return false;
		}
		QByteArray name=ctx.args[0];
		IdType uid=accessMgr.userId(name);
		if(uid==nullId)
		{
			ctx.retVal.append("user not found");
			return false;
		}
		ctx.retVal.append(QByteArray::number(uid));
		return true;
	}
	else if(subCommand=="name")
	{
		if(ctx.args.count()<1)
		{
			ctx.retVal.append(StandardErrors::invalidAgruments);
			return false;
		}
		bool ok=false;
		IdType uid=ctx.args[0].toLong(&ok);
		if(!ok)
		{
			ctx.retVal.append(StandardErrors::invalidAgruments);
			return false;
		}
		QByteArray name=accessMgr.userName(uid);
		if(name.isEmpty())
		{
			ctx.retVal.append("user not found");
			return false;
		}
		ctx.retVal.append(name);
		return true;
	}
	else
	{
		ctx.retVal.append("unknown command for access policy management");
		return false;
	}
}

bool AccessCommand::processUserGroupCommand(ICommand::CallContext &ctx)
{
	if(ctx.args.count()<1)
	{
		ctx.retVal.append(StandardErrors::invalidAgruments);
		return false;
	}
	QByteArray subCommand=ctx.args[0];
	ctx.args.removeAt(0);
	if(subCommand=="list")
	{
		const QMap<IdType,UsersGroup> &list=accessMgr.allUsersGroups();
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
		if(!accessMgr.createUsersGroup(groupName,proc->uid(),gid))
		{
			ctx.retVal.append(StandardErrors::someStrangeError);
			return false;
		}
		ctx.retVal.append(QByteArray::number(gid));
		return true;
	}
	if(ctx.args.count()<1)
	{
		ctx.retVal.append(StandardErrors::invalidAgruments);
		return false;
	}
	QByteArray groupName=ctx.args[0];
	IdType gid=accessMgr.usersGroupId(groupName);
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
		if(uid==nullId||!accessMgr.userCanManageUsersInUsersGroup(proc->uid(),gid))
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
		if(uid==nullId||!accessMgr.userCanManageUsersInUsersGroup(proc->uid(),gid))
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
	else if(subCommand=="gid")
	{
		if(ctx.args.count()<1)
		{
			ctx.retVal.append(StandardErrors::invalidAgruments);
			return false;
		}
		QByteArray name=ctx.args[0];
		IdType gid=accessMgr.usersGroupId(name);
		if(gid==nullId)
		{
			ctx.retVal.append("group not found");
			return false;
		}
		ctx.retVal.append(QByteArray::number(gid));
		return true;
	}
	else if(subCommand=="name")
	{
		if(ctx.args.count()<1)
		{
			ctx.retVal.append(StandardErrors::invalidAgruments);
			return false;
		}
		bool ok=false;
		IdType gid=ctx.args[0].toLong(&ok);
		if(!ok)
		{
			ctx.retVal.append(StandardErrors::invalidAgruments);
			return false;
		}
		QByteArray name=accessMgr.usersGroupName(gid);
		if(name.isEmpty())
		{
			ctx.retVal.append("group not found");
			return false;
		}
		ctx.retVal.append(name);
		return true;
	}
	else
	{
		ctx.retVal.append("unknown command for access policy management");
		return false;
	}
}

bool AccessCommand::processDevCommand(ICommand::CallContext &ctx)
{
	QByteArray subCommand=ctx.args[0];
	ctx.args.removeAt(0);
	if(subCommand=="list")
	{
		QSet<QUuid> ids=accessMgr.allUserDevices(proc->uid());
		for(auto &id:ids)
			ctx.retVal.append(id.toByteArray());
		return true;
	}
	else if(subCommand=="list_rules")
	{
		if(ctx.args.count()<1)
		{
			ctx.retVal.append(StandardErrors::invalidAgruments);
			return false;
		}
		QUuid devId=QUuid(ctx.args[0]);
		if(devId.isNull())
		{
			ctx.retVal.append(StandardErrors::invalidAgruments);
			return false;
		}
		if(proc->uid()!=rootUid&&proc->uid()!=accessMgr.devOwner(devId))
		{
			ctx.retVal.append(StandardErrors::accessDenied);
			return false;
		}
		QMap<IdType,DevicePolicyActionFlags> rules;
		accessMgr.listDeviceUserPolicies(devId,rules);
		for(auto i=rules.begin();i!=rules.end();++i)
			writeCmdataMsg(ctx.callId,QByteArrayList()<<"u"<<accessMgr.userName(i.key())<<
				accessMgr.polToStr(i.value()));
		accessMgr.listDeviceUserGroupPolicies(devId,rules);
		for(auto i=rules.begin();i!=rules.end();++i)
			writeCmdataMsg(ctx.callId,QByteArrayList()<<"g"<<accessMgr.usersGroupName(i.key())<<
				accessMgr.polToStr(i.value()));
		return true;
	}
	else if(subCommand=="set_rule")
	{
		if(ctx.args.count()<3)
		{
			ctx.retVal.append(StandardErrors::invalidAgruments);
			return false;
		}
		QByteArray devIdOrName=ctx.args[0];
		bool grpPolicy;
		if(ctx.args[1]=="g")
			grpPolicy=true;
		else if(ctx.args[1]=="u")
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
		QUuid devId=ServerInstance::inst().findDevId(devIdOrName);
		if(devId.isNull())
		{
			ctx.retVal.append(QByteArray(StandardErrors::noDeviceFound).replace("%1",devIdOrName));
			return false;
		}
		if(proc->uid()==nullId||(proc->uid()!=rootUid&&proc->uid()!=accessMgr.devOwner(devId)))
		{
			ctx.retVal.append(StandardErrors::accessDenied);
			return false;
		}
		IdType id;
		if(grpPolicy)
			id=accessMgr.usersGroupId(userOrGroupName);
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
	else if(subCommand=="chown")
	{
		if(ctx.args.count()<1)
		{
			ctx.retVal.append(StandardErrors::invalidAgruments);
			return false;
		}
		QByteArray devIdOrName=ctx.args[0];
		QUuid devId=ServerInstance::inst().findDevId(devIdOrName);
		if(devId.isNull())
		{
			ctx.retVal.append(QByteArray(StandardErrors::noDeviceFound).replace("%1",devIdOrName));
			return false;
		}
		IdType uid=proc->uid();
		IdType newOwnerId=proc->uid();
		if(ctx.args.count()>1)
		{
			QByteArray userName=ctx.args[1];
			newOwnerId=MainServerConfig::accessManager.userId(userName);
			if(newOwnerId==nullId)
			{
				ctx.retVal.append(QByteArray(StandardErrors::noUserFound).replace("%1",userName));
				return false;
			}
		}
		if(MainServerConfig::accessManager.userCanChangeDeviceOwner(devId,uid))
		{
			MainServerConfig::accessManager.setDevOwner(devId,newOwnerId);
			return true;
		}
		ctx.retVal.append(StandardErrors::accessDenied);
		return false;
	}
	else
	{
		ctx.retVal.append("unknown command for access policy management");
		return false;
	}
}
