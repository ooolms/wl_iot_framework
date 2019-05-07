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

#include "AlterozoomAuthCommand.h"
#include "../ServerInstance.h"
#include "StandardErrors.h"
#include "../ExternServices/AlterozoomAuthentificationStorage.h"
#include "../ExternServices/AlterozoomApi.h"
#include "wliot/WLIOTServerProtocolDefs.h"
#include <QCoreApplication>

AlterozoomAuthCommand::AlterozoomAuthCommand(CommandProcessor *p)
	:ICommand(p)
{
}

bool AlterozoomAuthCommand::processCommand(CallContext &ctx)
{
	if(ctx.args.count()<3)
	{
		ctx.retVal.append(StandardErrors::invalidAgruments);
		return false;
	}
	AlterozoomApi api;
	bool done=false,ok=false;
	QObject::connect(&api,&AlterozoomApi::authenticationComplete,[this,&done,&ok,&api](
		bool mOk,const QByteArray &host,const QByteArray &email)
	{
		ok=mOk;
		done=true;
		if(!ok)return;
		auto val=api.authCred(host,email);
		AlterozoomAuthentificationStorage::setAuth(host,email,val.userId,val.token);
		AlterozoomAuthentificationStorage::storeConfig();
	});
	api.authenticate(ctx.args[0],ctx.args[1],ctx.args[2]);
	while(!done)
		qApp->processEvents(QEventLoop::WaitForMoreEvents);
	return ok;
}

QByteArrayList AlterozoomAuthCommand::acceptedCommands()
{
	return QByteArrayList()<<"alterozoom_auth";
}
