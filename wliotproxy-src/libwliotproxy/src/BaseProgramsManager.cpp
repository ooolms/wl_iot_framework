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

#include "wliot/client/BaseProgramsManager.h"

using namespace WLIOTClient;

BaseProgramsManager::BaseProgramsManager(ServerConnection *srvConn,BaseProgramsControlCommands *cmds,QObject *parent)
	:QObject(parent)
{
	mSrvConn=srvConn;
	mCmds=cmds;
	mReady=false;
	connect(mSrvConn,&ServerConnection::connected,this,&BaseProgramsManager::onConnected);
	connect(mSrvConn,&ServerConnection::disconnected,this,&BaseProgramsManager::onDisconnected);
	if(mSrvConn->isConnected())
		onConnected();
}

QByteArrayList BaseProgramsManager::ids()
{
	return namesMap.keys();
}

bool BaseProgramsManager::get(const QByteArray &id,QByteArray &data)
{
	if(!mReady||!namesMap.contains(id))
		return false;
	return mCmds->get(id,data);
}

bool BaseProgramsManager::has(const QByteArray &id)
{
	return mReady&&namesMap.contains(id);
}

bool BaseProgramsManager::isWorking(const QByteArray &id)
{
	return mReady&&workingMap.value(id,false);
}

QByteArray BaseProgramsManager::name(const QByteArray &id)
{
	return mReady?namesMap.value(id):QByteArray();
}

bool BaseProgramsManager::create(const QByteArray &name,const QByteArray &data,QByteArray &id)
{
	if(!mReady)return false;
	if(!mCmds->create(name,data,id))return false;
	namesMap[id]=name;
	workingMap[id]=true;
	return true;
}

bool BaseProgramsManager::update(const QByteArray &id,const QByteArray &data)
{
	if(!mReady||!namesMap.contains(id))
		return false;
	return mCmds->update(id,data);
}

bool BaseProgramsManager::remove(const QByteArray &id)
{
	if(!mReady||!namesMap.contains(id))
		return false;
	if(!mCmds->remove(id))return false;
	namesMap.remove(id);
	workingMap.remove(id);
	return true;
}

bool BaseProgramsManager::rename(const QByteArray &id,const QByteArray &name)
{
	if(!mReady||!namesMap.contains(id))return false;
	if(!mCmds->rename(id,name))return false;
	namesMap[id]=name;
	return true;
}

void BaseProgramsManager::start(const QByteArray &id)
{
	if(!mReady||!namesMap.contains(id))return;
	if(!mCmds->start(id))return;
	reloadPrograms();
}

void BaseProgramsManager::stop(const QByteArray &id)
{
	if(!mReady||!namesMap.contains(id))return;
	if(!mCmds->stop(id))return;
	reloadPrograms();
}

void BaseProgramsManager::restart(const QByteArray &id)
{
	if(!mReady||!namesMap.contains(id))return;
	if(!mCmds->restart(id))return;
	reloadPrograms();
}

bool BaseProgramsManager::reloadPrograms()
{
	if(!mReady)return false;
	onConnected();
	return mReady;
}

bool BaseProgramsManager::ready()
{
	return mReady;
}

void BaseProgramsManager::onConnected()
{
	namesMap.clear();
	workingMap.clear();
	QByteArrayList ids,names;
	QList<bool> working;
	mReady=mCmds->list(ids,names,working);
	if(!mReady)return;
	if(ids.count()!=names.count()||ids.count()!=working.count())
	{
		mReady=false;
		return;
	}
	for(int i=0;i<ids.count();++i)
	{
		namesMap[ids[i]]=names[i];
		workingMap[ids[i]]=working[i];
	}
}

void BaseProgramsManager::onDisconnected()
{
	mReady=false;
	namesMap.clear();
	workingMap.clear();
}
