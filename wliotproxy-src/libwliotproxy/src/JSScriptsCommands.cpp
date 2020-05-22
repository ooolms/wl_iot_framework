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

#include "wliot/client/JSScriptsCommands.h"

using namespace WLIOT;
using namespace WLIOTClient;

JSScriptsCommands::JSScriptsCommands(ServerConnection *conn)
	:QObject(conn)
{
	srvConn=conn;
}

bool JSScriptsCommands::list(QByteArrayList &scripts,QList<bool> &states)
{
	QByteArrayList retVal;
	if(!srvConn->execCommand("js_list",QByteArrayList(),retVal)||(retVal.count()%2)!=0)
		return false;
	scripts.clear();
	states.clear();
	for(int i=0;i<retVal.count()>>1;++i)
	{
		scripts.append(retVal[i<<1]);
		states.append(retVal[(i<<1)+1]=="1");
	}
	return true;
}

bool JSScriptsCommands::get(const QByteArray &scriptName, QByteArray &text)
{
	QByteArrayList retVal;
	if(!srvConn->execCommand("js_get",QByteArrayList()<<scriptName,retVal)||retVal.count()<1)
		return false;
	text.append(retVal[0]);
	return true;
}

bool JSScriptsCommands::remove(const QByteArray &scriptName)
{
	return srvConn->execCommand("js_remove",QByteArrayList()<<scriptName);
}

bool JSScriptsCommands::upload(const QByteArray &scriptName,const QByteArray &text)
{
	return srvConn->execCommand("js_upload",QByteArrayList()<<scriptName<<text);
}

bool JSScriptsCommands::start(const QByteArray &scriptName)
{
	return srvConn->execCommand("js_start",QByteArrayList()<<scriptName);
}

bool JSScriptsCommands::stop(const QByteArray &scriptName)
{
	return srvConn->execCommand("js_stop",QByteArrayList()<<scriptName);
}

bool JSScriptsCommands::restart(const QByteArray &scriptName)
{
	return srvConn->execCommand("js_restart",QByteArrayList()<<scriptName);
}
