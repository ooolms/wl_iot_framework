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

#include "IotServerGDILPrograms.h"

IotServerGDILPrograms::IotServerGDILPrograms(IotServerConnection *conn,IotServerCommands *cmds)
	:IotServerIGDILProgramsManager(conn)
{
	srvConn=conn;
	srvCmds=cmds;
	ready=false;
	connect(conn,&IotServerConnection::connected,this,&IotServerGDILPrograms::onConnected);
	connect(conn,&IotServerConnection::disconnected,this,&IotServerGDILPrograms::onDisconnected);
	if(conn->isConnected())
		onConnected();
}


QByteArrayList IotServerGDILPrograms::programs()
{
	return programsMap.keys();
}

bool IotServerGDILPrograms::get(const QByteArray &programName,QByteArray &text)
{
	if(!ready||!programsMap.contains(programName))
		return false;
	return srvCmds->gdilPrograms()->get(programName,text);
}

bool IotServerGDILPrograms::isWorking(const QByteArray &programName)
{
	return programsMap.value(programName,false);
}

bool IotServerGDILPrograms::setProgramXml(const QByteArray &programName,const QByteArray &xml)
{
	if(!ready)
		return false;
	if(!srvCmds->gdilPrograms()->upload(programName,xml))
		return false;
	onConnected();
	return true;
}

bool IotServerGDILPrograms::remove(const QByteArray &programName)
{
	if(!ready)
		return false;
	if(!srvCmds->gdilPrograms()->remove(programName))
		return false;
	onConnected();
	return true;
}

void IotServerGDILPrograms::start(const QByteArray &programName)
{
	if(!ready)return;
	if(!srvCmds->gdilPrograms()->start(programName))
		return;
	programsMap[programName]=true;
}

void IotServerGDILPrograms::stop(const QByteArray &programName)
{
	if(!ready)return;
	if(!srvCmds->gdilPrograms()->stop(programName))
		return;
	programsMap[programName]=false;
}

void IotServerGDILPrograms::restart(const QByteArray &programName)
{
	if(!ready)return;
	if(!srvCmds->gdilPrograms()->restart(programName))
		return;
	programsMap[programName]=true;
}

bool IotServerGDILPrograms::listConfigOptions(const QByteArray &programName,QList<IotServerGDILConfigOption> &options)
{
	if(!ready||!programsMap.contains(programName))
		return false;
	return srvCmds->gdilPrograms()->listConfigOptions(programName,options);
}

bool IotServerGDILPrograms::setConfigOption(const QByteArray &programName,const ConfigOptionId &id,const DataUnit &data)
{
	if(!ready||!programsMap.contains(programName))
		return false;
	return srvCmds->gdilPrograms()->setConfigOption(programName,id,data);
}

bool IotServerGDILPrograms::listTimers(const QByteArray &programName,QList<IotServerGDILTimer> &timers)
{
	if(!ready||!programsMap.contains(programName))
		return false;
	return srvCmds->gdilPrograms()->listTimers(programName,timers);
}

bool IotServerGDILPrograms::setTimer(const QByteArray &programName,quint32 blockId,const TimerBlock::TimerConfig &cfg)
{
	if(!ready||!programsMap.contains(programName))
		return false;
	return srvCmds->gdilPrograms()->setTimer(programName,blockId,cfg);
}

bool IotServerGDILPrograms::reloadPrograms()
{
	if(!ready)return false;
	onConnected();
	return ready;
}

void IotServerGDILPrograms::onConnected()
{
	programsMap.clear();
	QByteArrayList n;
	QList<bool> s;
	ready=srvCmds->gdilPrograms()->list(n,s);
	if(!ready)return;
	for(int i=0;i<n.count();++i)
		programsMap[n[i]]=s[i];
}

void IotServerGDILPrograms::onDisconnected()
{
	ready=false;
	programsMap.clear();
}
