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

#ifndef BASEPROGRAMSMANAGER_H
#define BASEPROGRAMSMANAGER_H

#include "wliot/client/BaseProgramsControlCommands.h"
#include "wliot/client/IBaseProgramsManager.h"
#include "wliot/client/ServerConnection.h"

namespace WLIOTClient
{
	class BaseProgramsManager
		:public QObject
		,public IBaseProgramsManager
	{
		Q_OBJECT
	public:
		explicit BaseProgramsManager(ServerConnection *srvConn,BaseProgramsControlCommands *cmds,QObject *parent=0);
		virtual QByteArrayList ids()override;
		virtual bool get(const QByteArray &id, QByteArray &data)override;
		virtual bool has(const QByteArray &id)override;
		virtual bool isWorking(const QByteArray &id)override;
		virtual QByteArray name(const QByteArray &id)override;
		virtual bool create(const QByteArray &name,const QByteArray &data,QByteArray &id)override;
		virtual bool update(const QByteArray &id,const QByteArray &data)override;
		virtual bool remove(const QByteArray &id)override;
		virtual bool rename(const QByteArray &id,const QByteArray &name)override;
		virtual void start(const QByteArray &id)override;
		virtual void stop(const QByteArray &id)override;
		virtual void restart(const QByteArray &id)override;
		bool reloadPrograms();
		bool ready();

	private slots:
		void onConnected();
		void onDisconnected();

	private:
		bool mReady;
		ServerConnection *mSrvConn;
		BaseProgramsControlCommands *mCmds;
		QMap<QByteArray,QByteArray> namesMap;
		QMap<QByteArray,bool> workingMap;
	};
}

#endif // BASEPROGRAMSMANAGER_H
