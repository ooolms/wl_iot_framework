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

#ifndef IJSSCRIPTSMANAGER_H
#define IJSSCRIPTSMANAGER_H

#include <QObject>
#include <QByteArray>

namespace WLIOTClient
{
	class IJSScriptsManager
		:public QObject
	{
		Q_OBJECT
	public:
		explicit IJSScriptsManager(QObject *parent=nullptr);
		virtual QByteArrayList scripts()=0;
		virtual bool get(const QByteArray &scriptName,QByteArray &text)=0;
		virtual bool isWorking(const QByteArray &scriptName)=0;
		virtual bool setText(const QByteArray &scriptName,const QByteArray &text)=0;
		virtual bool remove(const QByteArray &scriptName)=0;
		virtual void start(const QByteArray &scriptName)=0;
		virtual void stop(const QByteArray &scriptName)=0;
		virtual void restart(const QByteArray &scriptName)=0;

	signals:
		void stateChanged(const QByteArray &scriptName,bool isWorking);
	};
}

#endif // IJSSCRIPTSMANAGER_H
