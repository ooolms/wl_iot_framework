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

JSScripts::JSScripts(ServerConnection *conn,JSScriptsCommands *cmds)
	:IJSScriptsManager(conn)
{
	mgr=new BaseProgramsManager(conn,cmds,this);
}

QByteArrayList WLIOTClient::JSScripts::ids()
{
	return mgr->ids();
}

bool WLIOTClient::JSScripts::get(const QByteArray &id,QByteArray &data)
{
	return mgr->get(id,data);
}

bool WLIOTClient::JSScripts::isWorking(const QByteArray &id)
{
	return mgr->isWorking(id);
}

QByteArray WLIOTClient::JSScripts::name(const QByteArray &id)
{
	return mgr->name(id);
}

bool WLIOTClient::JSScripts::create(const QByteArray &name,const QByteArray &data,QByteArray &id)
{
	return mgr->create(name,data,id);
}

bool WLIOTClient::JSScripts::update(const QByteArray &id,const QByteArray &data)
{
	return mgr->update(id,data);
}

bool WLIOTClient::JSScripts::remove(const QByteArray &id)
{
	return mgr->remove(id);
}

void WLIOTClient::JSScripts::start(const QByteArray &id)
{
	return mgr->start(id);
}

void WLIOTClient::JSScripts::stop(const QByteArray &id)
{
	return mgr->stop(id);
}

void WLIOTClient::JSScripts::restart(const QByteArray &id)
{
	return mgr->restart(id);
}
