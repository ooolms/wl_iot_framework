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

#ifndef ITRIGGER_H
#define ITRIGGER_H

#include <QObject>

namespace WLIOTVDIL
{
	class ITrigger
		:public QObject
	{
		Q_OBJECT
	public:
		explicit ITrigger(QObject *parent=nullptr);
		virtual ~ITrigger(){}
		void connectToActivate(QObject *o,const char *sig);

	signals:
		void activate();
	};
}

#endif // ITRIGGER_H
