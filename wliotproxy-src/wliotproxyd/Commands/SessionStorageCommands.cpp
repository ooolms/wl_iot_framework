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

#include "SessionStorageCommands.h"
#include "../ServerInstance.h"
#include "StandardErrors.h"
#include "wliot/storages/SessionStorage.h"
#include "wliot/WLIOTServerProtocolDefs.h"

SessionStorageCommands::SessionStorageCommands(CommandProcessor *p)
	:ICommand(p)
{
}

bool SessionStorageCommands::processCommand(CallContext &ctx)
{
	if(ctx.cmd=="session_list")
		return listSessions(ctx);
	else if(ctx.cmd=="session_list_attrs")
		return listSessionAttrs(ctx);
	else if(ctx.cmd=="session_get_attr")
		return getSessionAttr(ctx);
	else if(ctx.cmd=="session_set_attr")
		return setSessionAttr(ctx);
	else if(ctx.cmd=="session_get_write_id")
		return getMainWriteSessionId(ctx);
	else if(ctx.cmd=="session_get_write_id")
		return getMainWriteSessionId(ctx);
	else if(ctx.cmd=="session_create")
		return sessionCreate(ctx);
	else if(ctx.cmd=="session_start")
		return sessionStart(ctx);
	else if(ctx.cmd=="session_stop")
		return sessionStop(ctx);
	else if(ctx.cmd=="session_continue")
		return sessionContinue(ctx);
	else if(ctx.cmd=="session_remove")
		return sessionRemove(ctx);
	ctx.retVal.append(StandardErrors::unknownCommand);
	return false;
}

QByteArrayList SessionStorageCommands::acceptedCommands()
{
	return QByteArrayList()<<"session_list"<<"session_list_attrs"<<"session_get_attr"<<"session_set_attr"<<
		"session_start"<<"session_stop"<<"session_get_write_id"<<"session_continue"<<"session_remove"<<"session_create";
}

bool SessionStorageCommands::listSessions(CallContext &ctx)
{
	if(ctx.args.count()<2)
	{
		ctx.retVal.append(StandardErrors::invalidAgruments);
		return false;
	}
	SessionStorage *sStor=openStorage(ctx);
	if(!sStor)return false;
	QList<QUuid> ids;
	QByteArrayList titles;
	if(!sStor->listSessions(ids,titles))
	{
		ctx.retVal.append("can't list sessions");
		return false;
	}
	for(int i=0;i<ids.count();++i)
		writeCmdataMsg(ctx.callId,QByteArrayList()<<ids[i].toByteArray()<<titles[i]);
	return true;
}

bool SessionStorageCommands::listSessionAttrs(CallContext &ctx)
{
	if(ctx.args.count()<3)
	{
		ctx.retVal.append(StandardErrors::invalidAgruments);
		return false;
	}
	QUuid sessionId=QUuid(ctx.args[2]);
	if(sessionId.isNull())
	{
		ctx.retVal.append(StandardErrors::sessionNotFound);
		return false;
	}
	SessionStorage *stor=openStorage(ctx);
	if(!stor)return false;
	bool closeLater=false;
	if(!openSession(stor,sessionId,ctx.retVal,closeLater))return false;
	QMap<QByteArray,QByteArray> map;
	if(!stor->listSessionAttributes(sessionId,map))
	{
		ctx.retVal.append("can't list session attributes");
		return false;
	}
	for(auto i=map.begin();i!=map.end();++i)
		writeCmdataMsg(ctx.callId,QByteArrayList()<<i.key()<<i.value());
	if(closeLater)stor->closeSession(sessionId);
	return true;
}

bool SessionStorageCommands::getSessionAttr(CallContext &ctx)
{
	if(ctx.args.count()<4)
	{
		ctx.retVal.append(StandardErrors::invalidAgruments);
		return false;
	}
	QUuid sessionId=QUuid(ctx.args[2]);
	if(sessionId.isNull())
	{
		ctx.retVal.append(StandardErrors::sessionNotFound);
		return false;
	}
	QByteArray key=ctx.args[3];
	if(key.isEmpty())
	{
		ctx.retVal.append("empty attribute key");
		return false;
	}
	SessionStorage *stor=openStorage(ctx);
	if(!stor)return false;
	bool closeLater=false;
	if(!openSession(stor,sessionId,ctx.retVal,closeLater))return false;
	QByteArray val;
	if(!stor->getSessionAttribute(sessionId,key,val))
	{
		ctx.retVal.append("no attribute");
		return false;
	}
	ctx.retVal.append(val);
	if(closeLater)stor->closeSession(sessionId);
	return true;
}

bool SessionStorageCommands::setSessionAttr(CallContext &ctx)
{
	if(ctx.args.count()<4)
	{
		ctx.retVal.append(StandardErrors::invalidAgruments);
		return false;
	}
	QUuid sessionId=QUuid(ctx.args[2]);
	if(sessionId.isNull())
	{
		ctx.retVal.append(StandardErrors::sessionNotFound);
		return false;
	}
	QByteArray key=ctx.args[3];
	if(key.isEmpty())
	{
		ctx.retVal.append("empty attribute key");
		return false;
	}
	QByteArray val;
	bool set=false;
	if(ctx.args.count()>=5)
	{
		set=true;
		val=ctx.args[4];
	}
	SessionStorage *stor=openStorage(ctx);
	if(!stor)return false;
	bool closeLater=false;
	if(!openSession(stor,sessionId,ctx.retVal,closeLater))return false;
	if(!set)
	{
		if(!stor->removeSessionAttribute(sessionId,key))
		{
			ctx.retVal.append("can't remove session attribute");
			return false;
		}
	}
	else
	{
		if(!stor->setSessionAttribute(sessionId,key,val))
		{
			ctx.retVal.append("can't set session attribute");
			return false;
		}
	}
	return true;
}

bool SessionStorageCommands::sessionCreate(CallContext &ctx)
{
	if(ctx.args.count()<3)
	{
		ctx.retVal.append(StandardErrors::invalidAgruments);
		return false;
	}
	QByteArray sessionTitle=ctx.args[2];
	if(sessionTitle.isEmpty())
	{
		ctx.retVal.append(StandardErrors::invalidAgruments);
		return false;
	}
	SessionStorage *stor=openStorage(ctx);
	if(!stor)return false;
	if(stor->storeMode()!=ISensorStorage::MANUAL_SESSIONS)
	{
		ctx.retVal.append("only manual sessions can be created manually");
		return false;
	}
	QUuid sId;
	if(!stor->createSession(sessionTitle,sId))
	{
		ctx.retVal.append("can't create session");
		return false;
	}
	ctx.retVal.append(sId.toByteArray());
	return true;
}

bool SessionStorageCommands::sessionStart(CallContext &ctx)
{
	if(ctx.args.count()<3)
	{
		ctx.retVal.append(StandardErrors::invalidAgruments);
		return false;
	}
	QUuid sessionId=QUuid(ctx.args[2]);
	if(sessionId.isNull())
	{
		ctx.retVal.append(StandardErrors::invalidAgruments);
		return false;
	}
	SessionStorage *stor=openStorage(ctx);
	if(!stor)return false;
	if(stor->storeMode()!=ISensorStorage::MANUAL_SESSIONS)
	{
		ctx.retVal.append("only manual sessions can be started and stopped");
		return false;
	}
	if(stor->isMainWriteSessionOpened())
	{
		ctx.retVal.append("write session is already opened");
		return false;
	}
	if(!stor->openMainWriteSession(sessionId))
	{
		ctx.retVal.append("can't open session");
		return false;
	}
	return true;
}


bool SessionStorageCommands::sessionStop(CallContext &ctx)
{
	if(ctx.args.count()<2)
	{
		ctx.retVal.append(StandardErrors::invalidAgruments);
		return false;
	}
	SessionStorage *stor=openStorage(ctx);
	if(!stor)return false;
	if(stor->storeMode()!=ISensorStorage::MANUAL_SESSIONS)
	{
		ctx.retVal.append("only manual sessions can be started and stopped");
		return false;
	}
	if(!stor->isMainWriteSessionOpened())
	{
		ctx.retVal.append("write session is not opened");
		return false;
	}
	if(!stor->closeMainWriteSession())
	{
		ctx.retVal.append("can't close write session");
		return false;
	}
	return true;
}

bool SessionStorageCommands::sessionContinue(CallContext &ctx)
{
	if(ctx.args.count()<3)
	{
		ctx.retVal.append(StandardErrors::invalidAgruments);
		return false;
	}
	QUuid sessionId(ctx.args[2]);
	if(sessionId.isNull())
	{
		ctx.retVal.append(StandardErrors::invalidAgruments);
		return false;
	}
	SessionStorage *stor=openStorage(ctx);
	if(!stor)return false;
	if(stor->storeMode()!=ISensorStorage::MANUAL_SESSIONS)
	{
		ctx.retVal.append("only manual sessions can be started and stopped");
		return false;
	}
	if(stor->isMainWriteSessionOpened())
	{
		ctx.retVal.append("write session is already opened");
		return false;
	}
	if(!stor->openMainWriteSession(sessionId))
	{
		ctx.retVal.append("can't open session");
		return false;
	}
	return true;
}

bool SessionStorageCommands::sessionRemove(CallContext &ctx)
{
	if(ctx.args.count()<3)
	{
		ctx.retVal.append(StandardErrors::invalidAgruments);
		return false;
	}
	QUuid sessionId=QUuid(ctx.args[2]);
	if(sessionId.isNull())
	{
		ctx.retVal.append(StandardErrors::invalidAgruments);
		return false;
	}
	SessionStorage *stor=openStorage(ctx);
	if(!stor)return false;
	if(stor->storeMode()!=ISensorStorage::MANUAL_SESSIONS)
	{
		ctx.retVal.append("only manual sessions can be started and stopped");
		return false;
	}
	if(stor->getMainWriteSessionId()==sessionId)
	{
		ctx.retVal.append("can't remove write session");
		return false;
	}
	if(stor->isSessionOpened(sessionId))
		stor->closeSession(sessionId);
	if(!stor->removeSession(sessionId))
	{
		ctx.retVal.append("can't remove session");
		return false;
	}
	return true;
}

bool SessionStorageCommands::getMainWriteSessionId(CallContext &ctx)
{
	if(ctx.args.count()<2)
	{
		ctx.retVal.append(StandardErrors::invalidAgruments);
		return false;
	}
	SessionStorage *stor=openStorage(ctx);
	if(!stor)return false;
	if(!stor->isMainWriteSessionOpened())
	{
		ctx.retVal.append("write session is not opened");
		return false;
	}
	QUuid sId=stor->getMainWriteSessionId();
	if(sId.isNull())
	{
		ctx.retVal.append("write session is not opened");
		return false;
	}
	ctx.retVal.append(sId.toByteArray());
	return true;
}

SessionStorage* SessionStorageCommands::openStorage(CallContext &ctx)
{
	if(ctx.args.count()<2)
	{
		ctx.retVal.append(StandardErrors::invalidAgruments);
		return 0;
	}
	QUuid devId;
	ISensorStorage *stor=ServerInstance::inst().storages()->findStorageForDevice(
		ctx.args[0],ctx.args[1],devId);
	if(!stor)
	{
		ctx.retVal.append(StandardErrors::storageNotFound);
		return 0;
	}
	if(stor->storeMode()!=ISensorStorage::MANUAL_SESSIONS&&
		stor->storeMode()!=ISensorStorage::AUTO_SESSIONS)
	{
		ctx.retVal.append("not a session storage");
		return 0;
	}
	SessionStorage *sStor=reinterpret_cast<SessionStorage*>(stor);
	if(!sStor->isOpened()&&!sStor->open())
	{
		ctx.retVal.append(StandardErrors::storageNotFound);
		return 0;
	}
	return sStor;
}

bool SessionStorageCommands::openSession(SessionStorage *stor,
	const QUuid &sessionId,QByteArrayList &retVal,bool &closeLater)
{
	if(!stor->isSessionOpened(sessionId))
	{
		if(!stor->openSession(sessionId))
		{
			retVal.append(StandardErrors::sessionNotFound);
			return false;
		}
		closeLater=true;
	}
	return true;
}
