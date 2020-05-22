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
#include "wliot/client/AllServerCommands.h"

namespace WLIOTClient
{
	class JSScripts
		:public IJSScriptsManager
	{
		Q_OBJECT
	public:
		explicit JSScripts(ServerConnection *conn,AllServerCommands *cmds);
		virtual QByteArrayList scripts()override;
		virtual bool get(const QByteArray &scriptName,QByteArray &text)override;
		virtual bool isWorking(const QByteArray &scriptName)override;
		virtual bool setText(const QByteArray &scriptName,const QByteArray &text)override;
		virtual bool remove(const QByteArray &scriptName)override;
		virtual void start(const QByteArray &scriptName)override;
		virtual void stop(const QByteArray &scriptName)override;
		virtual void restart(const QByteArray &scriptName)override;
		bool reloadScripts();

	private slots:
		void onConnected();
		void onDisconnected();

	private:
		ServerConnection *srvConn;
		AllServerCommands *srvCmds;
		QMap<QByteArray,bool> scriptsMap;
		bool ready;
	};
}

#endif // JSSCRIPTS_H
