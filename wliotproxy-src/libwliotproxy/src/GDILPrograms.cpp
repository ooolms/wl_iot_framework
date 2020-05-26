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

#include "wliot/client/GDILPrograms.h"

using namespace WLIOT;
using namespace WLIOTClient;

GDILPrograms::GDILPrograms(ServerConnection *conn,GDILProgramsCommands *cmds)
	:IGDILProgramsManager(conn)
{
	mCmds=cmds;
	mgr=new BaseProgramsManager(conn,cmds,this);
}


QByteArrayList WLIOTClient::GDILPrograms::ids()
{
	return mgr->ids();
}

bool WLIOTClient::GDILPrograms::get(const QByteArray &id,QByteArray &data)
{
	return mgr->get(id,data);
}

bool WLIOTClient::GDILPrograms::isWorking(const QByteArray &id)
{
	return mgr->isWorking(id);
}

QByteArray WLIOTClient::GDILPrograms::name(const QByteArray &id)
{
	return mgr->name(id);
}

bool WLIOTClient::GDILPrograms::create(const QByteArray &name,const QByteArray &data,QByteArray &id)
{
	return mgr->create(name,data,id);
}

bool WLIOTClient::GDILPrograms::update(const QByteArray &id,const QByteArray &data)
{
	return mgr->update(id,data);
}

bool WLIOTClient::GDILPrograms::remove(const QByteArray &id)
{
	return mgr->remove(id);
}

void WLIOTClient::GDILPrograms::start(const QByteArray &id)
{
	return mgr->start(id);
}

void WLIOTClient::GDILPrograms::stop(const QByteArray &id)
{
	return mgr->stop(id);
}

void WLIOTClient::GDILPrograms::restart(const QByteArray &id)
{
	return mgr->restart(id);
}

bool GDILPrograms::listConfigOptions(const QByteArray &id,QList<GDILConfigOption> &options)
{
	if(!mgr->ready()||!mgr->has(id))
		return false;
	return mCmds->listConfigOptions(id,options);
}

bool GDILPrograms::setConfigOption(const QByteArray &id,
	const WLIOTGDIL::ConfigOptionId &optionId,const WLIOTGDIL::DataUnit &data)
{
	if(!mgr->ready()||!mgr->has(id))
		return false;
	return mCmds->setConfigOption(id,optionId,data);
}

bool GDILPrograms::listTimers(const QByteArray &id,QList<GDILTimer> &timers)
{
	if(!mgr->ready()||!mgr->has(id))
		return false;
	return mCmds->listTimers(id,timers);
}

bool GDILPrograms::setTimer(const QByteArray &id,quint32 blockId,const WLIOTGDIL::TimerBlock::TimerConfig &cfg)
{
	if(!mgr->ready()||!mgr->has(id))
		return false;
	return mCmds->setTimer(id,blockId,cfg);
}

bool WLIOTClient::GDILPrograms::has(const QByteArray &id)
{
	return mgr->has(id);
}
