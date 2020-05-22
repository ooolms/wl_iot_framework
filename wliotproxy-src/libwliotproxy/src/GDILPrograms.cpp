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

GDILPrograms::GDILPrograms(ServerConnection *conn,AllServerCommands *cmds)
	:IGDILProgramsManager(conn)
{
	srvConn=conn;
	srvCmds=cmds;
	ready=false;
	connect(conn,&ServerConnection::connected,this,&GDILPrograms::onConnected);
	connect(conn,&ServerConnection::disconnected,this,&GDILPrograms::onDisconnected);
	if(conn->isConnected())
		onConnected();
}


QByteArrayList GDILPrograms::programs()
{
	return programsMap.keys();
}

bool GDILPrograms::get(const QByteArray &programName,QByteArray &text)
{
	if(!ready||!programsMap.contains(programName))
		return false;
	return srvCmds->gdilPrograms()->get(programName,text);
}

bool GDILPrograms::isWorking(const QByteArray &programName)
{
	return programsMap.value(programName,false);
}

bool GDILPrograms::setProgramXml(const QByteArray &programName,const QByteArray &xml)
{
	if(!ready)
		return false;
	if(!srvCmds->gdilPrograms()->upload(programName,xml))
		return false;
	onConnected();
	return true;
}

bool GDILPrograms::remove(const QByteArray &programName)
{
	if(!ready)
		return false;
	if(!srvCmds->gdilPrograms()->remove(programName))
		return false;
	onConnected();
	return true;
}

void GDILPrograms::start(const QByteArray &programName)
{
	if(!ready)return;
	if(!srvCmds->gdilPrograms()->start(programName))
		return;
	programsMap[programName]=true;
}

void GDILPrograms::stop(const QByteArray &programName)
{
	if(!ready)return;
	if(!srvCmds->gdilPrograms()->stop(programName))
		return;
	programsMap[programName]=false;
}

void GDILPrograms::restart(const QByteArray &programName)
{
	if(!ready)return;
	if(!srvCmds->gdilPrograms()->restart(programName))
		return;
	programsMap[programName]=true;
}

bool GDILPrograms::listConfigOptions(const QByteArray &programName,QList<GDILConfigOption> &options)
{
	if(!ready||!programsMap.contains(programName))
		return false;
	return srvCmds->gdilPrograms()->listConfigOptions(programName,options);
}

bool GDILPrograms::setConfigOption(const QByteArray &programName,
	const WLIOTGDIL::ConfigOptionId &id,const WLIOTGDIL::DataUnit &data)
{
	if(!ready||!programsMap.contains(programName))
		return false;
	return srvCmds->gdilPrograms()->setConfigOption(programName,id,data);
}

bool GDILPrograms::listTimers(const QByteArray &programName,QList<GDILTimer> &timers)
{
	if(!ready||!programsMap.contains(programName))
		return false;
	return srvCmds->gdilPrograms()->listTimers(programName,timers);
}

bool GDILPrograms::setTimer(const QByteArray &programName,quint32 blockId,const WLIOTGDIL::TimerBlock::TimerConfig &cfg)
{
	if(!ready||!programsMap.contains(programName))
		return false;
	return srvCmds->gdilPrograms()->setTimer(programName,blockId,cfg);
}

bool GDILPrograms::reloadPrograms()
{
	if(!ready)return false;
	onConnected();
	return ready;
}

void GDILPrograms::onConnected()
{
	programsMap.clear();
	QByteArrayList n;
	QList<bool> s;
	ready=srvCmds->gdilPrograms()->list(n,s);
	if(!ready)return;
	for(int i=0;i<n.count();++i)
		programsMap[n[i]]=s[i];
}

void GDILPrograms::onDisconnected()
{
	ready=false;
	programsMap.clear();
}
