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

#ifndef JSSCRIPTS_H
#define JSSCRIPTS_H

#include "wliot/client/IJSScriptsManager.h"
#include "wliot/client/ServerConnection.h"
#include "wliot/client/JSScriptsCommands.h"
#include "wliot/client/BaseProgramsManager.h"

namespace WLIOTClient
{
	class JSScripts
		:public IJSScriptsManager
	{
		Q_OBJECT
	public:
		explicit JSScripts(ServerConnection *conn,JSScriptsCommands *cmds);
		virtual QByteArrayList ids()override;
		virtual bool get(const QByteArray &id,QByteArray &data)override;
		virtual bool isWorking(const QByteArray &id)override;
		virtual bool has(const QByteArray &id) override;
		virtual QByteArray name(const QByteArray &id)override;
		virtual bool create(const QByteArray &name,const QByteArray &data,QByteArray &id)override;
		virtual bool update(const QByteArray &id,const QByteArray &data) override;
		virtual bool rename(const QByteArray &id,const QByteArray &name)override;
		virtual bool remove(const QByteArray &id)override;
		virtual void start(const QByteArray &id)override;
		virtual void stop(const QByteArray &id)override;
		virtual void restart(const QByteArray &id)override;
		bool reloadPrograms();

	private:
		BaseProgramsManager *mgr;
	};
}

#endif // JSSCRIPTS_H
