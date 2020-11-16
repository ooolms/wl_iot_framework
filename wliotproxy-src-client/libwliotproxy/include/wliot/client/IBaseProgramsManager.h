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

#ifndef IBASEPROGRAMSMANAGER_H
#define IBASEPROGRAMSMANAGER_H

#include <QByteArrayList>

namespace WLIOTClient
{
	/**
	 * @brief Интерфейс для управления запускаемыми сервером программами обработки данных
	 */
	class IBaseProgramsManager
	{
	public:
		virtual ~IBaseProgramsManager(){}
		virtual QByteArrayList ids()=0;
		virtual bool get(const QByteArray &id,QByteArray &data)=0;
		virtual bool has(const QByteArray &id)=0;
		virtual bool isWorking(const QByteArray &id)=0;
		virtual QByteArray name(const QByteArray &id)=0;
		virtual bool create(const QByteArray &name,const QByteArray &data,QByteArray &id)=0;
		virtual bool update(const QByteArray &id,const QByteArray &data)=0;
		virtual bool remove(const QByteArray &id)=0;
		virtual bool rename(const QByteArray &id,const QByteArray &name)=0;
		virtual void start(const QByteArray &id)=0;
		virtual void stop(const QByteArray &id)=0;
		virtual void restart(const QByteArray &id)=0;
	};
}


#endif // IBASEPROGRAMSMANAGER_H
