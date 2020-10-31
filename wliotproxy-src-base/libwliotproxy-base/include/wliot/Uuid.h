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

#ifndef UUID_H
#define UUID_H

#include <QUuid>

//CRIT ??? заменить QUuid на WLIOT::Uuid для избежания лишнего преобразования чисел в строку

namespace WLIOT
{
	/**
	 * @brief The Uuid class
	 * Работает почти как QUuid, но строковое представление хранит в памяти сразу, а не генерирует по необходимости
	 */
	class Uuid
	{
	public:
		Uuid()=default;
		Uuid(const QUuid &id);
		Uuid(const QByteArray &idStr);
		Uuid(const QString &idStr);
		const QUuid& uid()const;
		const QByteArray toByteArray()const;
		const QString toString()const;
		Uuid& operator=(const Uuid &t);
		bool operator==(const Uuid &t)const;
		bool operator<(const Uuid &t)const;

	private:
		QUuid mId;
		QByteArray mIdStr;
	};
}

#endif // UUID_H
