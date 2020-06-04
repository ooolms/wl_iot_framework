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

#include "wliot/client/VDILPrograms.h"

using namespace WLIOT;
using namespace WLIOTClient;

VDILPrograms::VDILPrograms(ServerConnection *conn,VDILProgramsCommands *cmds)
	:IVDILProgramsManager(conn)
{
	mCmds=cmds;
	mgr=new BaseProgramsManager(conn,cmds,this);
}


QByteArrayList WLIOTClient::VDILPrograms::ids()
{
	return mgr->ids();
}

bool WLIOTClient::VDILPrograms::get(const QByteArray &id,QByteArray &data)
{
	return mgr->get(id,data);
}

bool WLIOTClient::VDILPrograms::isWorking(const QByteArray &id)
{
	return mgr->isWorking(id);
}

QByteArray WLIOTClient::VDILPrograms::name(const QByteArray &id)
{
	return mgr->name(id);
}

bool WLIOTClient::VDILPrograms::create(const QByteArray &name,const QByteArray &data,QByteArray &id)
{
	return mgr->create(name,data,id);
}

bool WLIOTClient::VDILPrograms::update(const QByteArray &id,const QByteArray &data)
{
	return mgr->update(id,data);
}

bool WLIOTClient::VDILPrograms::remove(const QByteArray &id)
{
	return mgr->remove(id);
}

void WLIOTClient::VDILPrograms::start(const QByteArray &id)
{
	return mgr->start(id);
}

void WLIOTClient::VDILPrograms::stop(const QByteArray &id)
{
	return mgr->stop(id);
}

void WLIOTClient::VDILPrograms::restart(const QByteArray &id)
{
	return mgr->restart(id);
}

bool VDILPrograms::listConfigOptions(const QByteArray &id,QList<VDILConfigOption> &options)
{
	if(!mgr->ready()||!mgr->has(id))
		return false;
	return mCmds->listConfigOptions(id,options);
}

bool VDILPrograms::setConfigOption(const QByteArray &id,
	const WLIOTVDIL::ConfigOptionId &optionId,const WLIOTVDIL::DataUnit &data)
{
	if(!mgr->ready()||!mgr->has(id))
		return false;
	return mCmds->setConfigOption(id,optionId,data);
}

bool VDILPrograms::listTimers(const QByteArray &id,QList<VDILTimer> &timers)
{
	if(!mgr->ready()||!mgr->has(id))
		return false;
	return mCmds->listTimers(id,timers);
}

bool VDILPrograms::setTimer(const QByteArray &id,quint32 blockId,const WLIOTVDIL::TimerBlock::TimerConfig &cfg)
{
	if(!mgr->ready()||!mgr->has(id))
		return false;
	return mCmds->setTimer(id,blockId,cfg);
}

bool WLIOTClient::VDILPrograms::has(const QByteArray &id)
{
	return mgr->has(id);
}
