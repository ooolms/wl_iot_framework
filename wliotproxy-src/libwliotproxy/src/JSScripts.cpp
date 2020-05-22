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

#include "wliot/client/JSScripts.h"

using namespace WLIOT;
using namespace WLIOTClient;

JSScripts::JSScripts(ServerConnection *conn,AllServerCommands *cmds)
	:IJSScriptsManager(conn)
{
	srvConn=conn;
	srvCmds=cmds;
	ready=false;
	connect(conn,&ServerConnection::connected,this,&JSScripts::onConnected);
	connect(conn,&ServerConnection::disconnected,this,&JSScripts::onDisconnected);
	if(conn->isConnected())
		onConnected();
}


QByteArrayList JSScripts::scripts()
{
	return scriptsMap.keys();
}

bool JSScripts::get(const QByteArray &scriptName,QByteArray &text)
{
	if(!ready||!scriptsMap.contains(scriptName))
		return false;
	return srvCmds->jsScriptsCommands()->get(scriptName,text);
}

bool JSScripts::isWorking(const QByteArray &scriptName)
{
	return scriptsMap.value(scriptName,false);
}

bool JSScripts::setText(const QByteArray &scriptName,const QByteArray &text)
{
	if(!ready)
		return false;
	if(!srvCmds->jsScriptsCommands()->upload(scriptName,text))
		return false;
	onConnected();
	return true;
}

bool JSScripts::remove(const QByteArray &scriptName)
{
	if(!ready)
		return false;
	if(!srvCmds->jsScriptsCommands()->remove(scriptName))
		return false;
	onConnected();
	return true;
}

void JSScripts::start(const QByteArray &scriptName)
{
	if(!ready)return;
	if(!srvCmds->jsScriptsCommands()->start(scriptName))
		return;
	scriptsMap[scriptName]=true;
}

void JSScripts::stop(const QByteArray &scriptName)
{
	if(!ready)return;
	if(!srvCmds->jsScriptsCommands()->stop(scriptName))
		return;
	scriptsMap[scriptName]=false;
}

void JSScripts::restart(const QByteArray &scriptName)
{
	if(!ready)return;
	if(!srvCmds->jsScriptsCommands()->restart(scriptName))
		return;
	scriptsMap[scriptName]=true;
}

bool JSScripts::reloadScripts()
{
	if(!ready)return false;
	onConnected();
	return ready;
}

void JSScripts::onConnected()
{
	scriptsMap.clear();
	QByteArrayList n;
	QList<bool> s;
	ready=srvCmds->jsScriptsCommands()->list(n,s);
	if(!ready)return;
	for(int i=0;i<n.count();++i)
		scriptsMap[n[i]]=s[i];
}

void JSScripts::onDisconnected()
{
	ready=false;
	scriptsMap.clear();
}
