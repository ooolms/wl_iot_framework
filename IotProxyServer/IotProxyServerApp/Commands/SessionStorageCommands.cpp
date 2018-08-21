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
#include "../IotProxyInstance.h"
#include "StandardErrors.h"
#include "ARpcLocalStorage/ARpcSessionStorage.h"
#include "ARpcBase/ARpcServerConfig.h"

SessionStorageCommands::SessionStorageCommands(ARpcOutsideDevice *d,IotProxyCommandProcessor *p)
	:ICommand(d,p)
{
}

bool SessionStorageCommands::processCommand(const ARpcMessage &m,QByteArrayList &retVal)
{
	if(m.title=="session_list")
		return listSessions(m,retVal);
	else if(m.title=="session_list_attrs")
		return listSessionAttrs(m,retVal);
	else if(m.title=="session_get_attr")
		return getSessionAttr(m,retVal);
	else if(m.title=="session_set_attr")
		return setSessionAttr(m,retVal);
	else if(m.title=="session_get_write_id")
		return getMainWriteSessionId(m,retVal);
	else if(m.title=="session_get_write_id")
		return getMainWriteSessionId(m,retVal);
	else if(m.title=="session_start")
		return sessionStart(m,retVal);
	else if(m.title=="session_stop")
		return sessionStop(m,retVal);
	else if(m.title=="session_continue")
		return sessionContinue(m,retVal);
	else if(m.title=="session_remove")
		return sessionRemove(m,retVal);
	retVal.append(StandardErrors::unknownCommand);
	return false;
}

QByteArrayList SessionStorageCommands::acceptedCommands()
{
	return QByteArrayList()<<"session_list"<<"session_list_attrs"<<"session_get_attr"<<"session_set_attr"<<
		"session_start"<<"session_stop"<<"session_get_write_id"<<"session_continue"<<"session_remove";
}

bool SessionStorageCommands::listSessions(const ARpcMessage &m,QByteArrayList &retVal)
{
	if(m.args.count()<2)
	{
		retVal.append(StandardErrors::invalidAgruments);
		return false;
	}
	ARpcSessionStorage *sStor=openStorage(m,retVal);
	if(!sStor)return false;
	QList<QUuid> ids;
	QByteArrayList titles;
	if(!sStor->listSessions(ids,titles))
	{
		retVal.append("can't list sessions");
		return false;
	}
	for(int i=0;i<ids.count();++i)
		clientDev->writeMsg(ARpcServerConfig::srvCmdDataMsg,QByteArrayList()<<ids[i].toByteArray()<<titles[i]);
	return true;
}

bool SessionStorageCommands::listSessionAttrs(const ARpcMessage &m,QByteArrayList &retVal)
{
	if(m.args.count()<3)
	{
		retVal.append(StandardErrors::invalidAgruments);
		return false;
	}
	QUuid sessionId=QUuid(m.args[2]);
	if(sessionId.isNull())
	{
		retVal.append(StandardErrors::sessionNotFound);
		return false;
	}
	ARpcSessionStorage *stor=openStorage(m,retVal);
	if(!stor)return false;
	bool closeLater=false;
	if(!openSession(stor,sessionId,retVal,closeLater))return false;
	QMap<QByteArray,QByteArray> map;
	if(!stor->listSessionAttributes(sessionId,map))
	{
		retVal.append("can't list session attributes");
		return false;
	}
	for(auto i=map.begin();i!=map.end();++i)
		clientDev->writeMsg(ARpcServerConfig::srvCmdDataMsg,QByteArrayList()<<i.key()<<i.value());
	if(closeLater)stor->closeSession(sessionId);
	return true;
}

bool SessionStorageCommands::getSessionAttr(const ARpcMessage &m,QByteArrayList &retVal)
{
	if(m.args.count()<4)
	{
		retVal.append(StandardErrors::invalidAgruments);
		return false;
	}
	QUuid sessionId=QUuid(m.args[2]);
	if(sessionId.isNull())
	{
		retVal.append(StandardErrors::sessionNotFound);
		return false;
	}
	QByteArray key=m.args[3];
	if(key.isEmpty())
	{
		retVal.append("empty attribute key");
		return false;
	}
	ARpcSessionStorage *stor=openStorage(m,retVal);
	if(!stor)return false;
	bool closeLater=false;
	if(!openSession(stor,sessionId,retVal,closeLater))return false;
	QByteArray val;
	if(!stor->getSessionAttribute(sessionId,key,val))
	{
		retVal.append("no attribute");
		return false;
	}
	retVal.append(val);
	if(closeLater)stor->closeSession(sessionId);
	return true;
}

bool SessionStorageCommands::setSessionAttr(const ARpcMessage &m,QByteArrayList &retVal)
{
	if(m.args.count()<4)
	{
		retVal.append(StandardErrors::invalidAgruments);
		return false;
	}
	QUuid sessionId=QUuid(m.args[2]);
	if(sessionId.isNull())
	{
		retVal.append(StandardErrors::sessionNotFound);
		return false;
	}
	QByteArray key=m.args[3];
	if(key.isEmpty())
	{
		retVal.append("empty attribute key");
		return false;
	}
	QByteArray val;
	bool set=false;
	if(m.args.count()>=5)
	{
		set=true;
		val=m.args[4];
	}
	ARpcSessionStorage *stor=openStorage(m,retVal);
	if(!stor)return false;
	bool closeLater=false;
	if(!openSession(stor,sessionId,retVal,closeLater))return false;
	if(!set)
	{
		if(!stor->removeSessionAttribute(sessionId,key))
		{
			retVal.append("can't remove session attribute");
			return false;
		}
	}
	else
	{
		if(!stor->setSessionAttribute(sessionId,key,val))
		{
			retVal.append("can't set session attribute");
			return false;
		}
	}
	return true;
}

bool SessionStorageCommands::sessionStart(const ARpcMessage &m,QByteArrayList &retVal)
{
	if(m.args.count()<3)
	{
		retVal.append(StandardErrors::invalidAgruments);
		return false;
	}
	QByteArray sessionTitle=m.args[2];
	if(sessionTitle.isEmpty())
	{
		retVal.append(StandardErrors::invalidAgruments);
		return false;
	}
	ARpcSessionStorage *stor=openStorage(m,retVal);
	if(!stor)return false;
	if(stor->getStoreMode()!=ARpcISensorStorage::MANUAL_SESSIONS)
	{
		retVal.append("only manual sessions can be started and stopped");
		return false;
	}
	if(stor->isMainWriteSessionOpened())
	{
		retVal.append("write session is already opened");
		return false;
	}
	QUuid sId;
	if(!stor->createSession(sessionTitle,sId)||!stor->openMainWriteSession(sId))
	{
		retVal.append("can't create session");
		return false;
	}
	retVal.append(sId.toByteArray());
	return true;
}


bool SessionStorageCommands::sessionStop(const ARpcMessage &m,QByteArrayList &retVal)
{
	if(m.args.count()<2)
	{
		retVal.append(StandardErrors::invalidAgruments);
		return false;
	}
	ARpcSessionStorage *stor=openStorage(m,retVal);
	if(!stor)return false;
	if(stor->getStoreMode()!=ARpcISensorStorage::MANUAL_SESSIONS)
	{
		retVal.append("only manual sessions can be started and stopped");
		return false;
	}
	if(!stor->isMainWriteSessionOpened())
	{
		retVal.append("write session is not opened");
		return false;
	}
	if(!stor->closeMainWriteSession())
	{
		retVal.append("can't close write session");
		return false;
	}
	return true;
}

bool SessionStorageCommands::sessionContinue(const ARpcMessage &m,QByteArrayList &retVal)
{
	if(m.args.count()<3)
	{
		retVal.append(StandardErrors::invalidAgruments);
		return false;
	}
	QUuid sessionId(m.args[2]);
	if(sessionId.isNull())
	{
		retVal.append(StandardErrors::invalidAgruments);
		return false;
	}
	ARpcSessionStorage *stor=openStorage(m,retVal);
	if(!stor)return false;
	if(stor->getStoreMode()!=ARpcISensorStorage::MANUAL_SESSIONS)
	{
		retVal.append("only manual sessions can be started and stopped");
		return false;
	}
	if(stor->isMainWriteSessionOpened())
	{
		retVal.append("write session is already opened");
		return false;
	}
	if(!stor->openMainWriteSession(sessionId))
	{
		retVal.append("can't open session");
		return false;
	}
	return true;
}

bool SessionStorageCommands::sessionRemove(const ARpcMessage &m, QByteArrayList &retVal)
{
	if(m.args.count()<3)
	{
		retVal.append(StandardErrors::invalidAgruments);
		return false;
	}
	QUuid sessionId=QUuid(m.args[2]);
	if(sessionId.isNull())
	{
		retVal.append(StandardErrors::invalidAgruments);
		return false;
	}
	ARpcSessionStorage *stor=openStorage(m,retVal);
	if(!stor)return false;
	if(stor->getStoreMode()!=ARpcISensorStorage::MANUAL_SESSIONS)
	{
		retVal.append("only manual sessions can be started and stopped");
		return false;
	}
	if(stor->getMainWriteSessionId()==sessionId)
	{
		retVal.append("can't remove write session");
		return false;
	}
	if(stor->isSessionOpened(sessionId))
		stor->closeSession(sessionId);
	if(!stor->removeSession(sessionId))
	{
		retVal.append("can't remove session");
		return false;
	}
	return true;
}

bool SessionStorageCommands::getMainWriteSessionId(const ARpcMessage &m,QByteArrayList &retVal)
{
	ARpcSessionStorage *stor=openStorage(m,retVal);
	if(!stor)return false;
	if(!stor->isMainWriteSessionOpened())
	{
		retVal.append("write session is not opened");
		return false;
	}
	QUuid sId=stor->getMainWriteSessionId();
	if(sId.isNull())
	{
		retVal.append("write session is not opened");
		return false;
	}
	retVal.append(sId.toByteArray());
	return true;
}

ARpcSessionStorage* SessionStorageCommands::openStorage(const ARpcMessage &m,QByteArrayList &retVal)
{
	if(m.args.count()<2)
	{
		retVal.append(StandardErrors::invalidAgruments);
		return 0;
	}
	QUuid devId;
	ARpcISensorStorage *stor=IotProxyInstance::inst().sensorsStorage()->findStorageForDevice(m.args[0],m.args[1],devId);
	if(!stor)
	{
		retVal.append(StandardErrors::storageNotFound);
		return 0;
	}
	if(stor->getStoreMode()!=ARpcISensorStorage::MANUAL_SESSIONS&&
		stor->getStoreMode()!=ARpcISensorStorage::AUTO_SESSIONS)
	{
		retVal.append("not a session storage");
		return 0;
	}
	ARpcSessionStorage *sStor=(ARpcSessionStorage*)stor;
	if(!sStor->isOpened()&&!sStor->open())
	{
		retVal.append(StandardErrors::storageNotFound);
		return 0;
	}
	return sStor;
}

bool SessionStorageCommands::openSession(ARpcSessionStorage *stor,
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
