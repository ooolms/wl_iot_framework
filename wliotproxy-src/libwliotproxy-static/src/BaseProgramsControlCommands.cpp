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

#include "wliot/client/BaseProgramsControlCommands.h"

using namespace WLIOT;
using namespace WLIOTClient;

BaseProgramsControlCommands::BaseProgramsControlCommands(const QByteArray &cmdPrefix,ServerConnection *conn)
	:QObject(conn)
{
	srvConn=conn;
	mCmdPrefix=cmdPrefix;
}

bool BaseProgramsControlCommands::list(QByteArrayList &ids,QByteArrayList &names,QList<bool> &states)
{
	QByteArrayList retVal;
	CmDataCallback cb=[&ids,&names,&states](const QByteArrayList &args)->bool
	{
		if(args.count()!=3)return false;
		ids.append(args[0]);
		names.append(args[1]);
		states.append(args[2]=="1");
		return true;
	};
	ids.clear();
	names.clear();
	states.clear();
	return srvConn->execCommand(mCmdPrefix+"_list",QByteArrayList(),retVal,cb);
}

bool BaseProgramsControlCommands::get(const QByteArray &id,QByteArray &text)
{
	QByteArrayList retVal;
	if(!srvConn->execCommand(mCmdPrefix+"_get",QByteArrayList()<<id,retVal)||retVal.count()!=1)
		return false;
	text.append(retVal[0]);
	return true;
}

bool BaseProgramsControlCommands::remove(const QByteArray &id)
{
	return srvConn->execCommand(mCmdPrefix+"_remove",QByteArrayList()<<id);
}

bool BaseProgramsControlCommands::rename(const QByteArray &id, const QByteArray &name)
{
	return srvConn->execCommand(mCmdPrefix+"_rename",QByteArrayList()<<id<<name);
}

bool BaseProgramsControlCommands::create(const QByteArray &name,const QByteArray &data,QByteArray &id)
{
	QByteArrayList retVal;
	if(!srvConn->execCommand(mCmdPrefix+"_create",QByteArrayList()<<name<<data,retVal)||retVal.count()!=1)
		return false;
	id=retVal[0];
	return true;
}

bool BaseProgramsControlCommands::update(const QByteArray &id,const QByteArray &data)
{
	return srvConn->execCommand(mCmdPrefix+"_update",QByteArrayList()<<id<<data);
}

bool BaseProgramsControlCommands::start(const QByteArray &id)
{
	return srvConn->execCommand(mCmdPrefix+"_start",QByteArrayList()<<id);
}

bool BaseProgramsControlCommands::stop(const QByteArray &id)
{
	return srvConn->execCommand(mCmdPrefix+"_stop",QByteArrayList()<<id);
}

bool BaseProgramsControlCommands::restart(const QByteArray &id)
{
	return srvConn->execCommand(mCmdPrefix+"_restart",QByteArrayList()<<id);
}
