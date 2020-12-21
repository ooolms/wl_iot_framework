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

#ifndef SERIALNOTIFICATOR_H
#define SERIALNOTIFICATOR_H

#include <QObject>
#include <QFileSystemWatcher>

namespace WLIOT
{
	class SerialNotificator
		:public QObject
	{
		Q_OBJECT
	private:
		explicit SerialNotificator();
		virtual ~SerialNotificator();
		SerialNotificator(const SerialNotificator&);
		SerialNotificator& operator=(const SerialNotificator&);

	public:
		static SerialNotificator& inst();

	signals:
		void checkSerialPorts();

	private:
		QFileSystemWatcher w;
	};
}

#endif // SERIALNOTIFICATOR_H
