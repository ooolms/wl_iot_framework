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

#ifndef BLOCKPORT_H
#define BLOCKPORT_H

#include "GDIL/core/DataUnit.h"

namespace WLIOTGDIL
{
	class BlockPort
	{
	public:
		//for program editing
		virtual DataUnit::Type type()const=0;
		virtual quint32 dim()const=0;
		const QString& title()const;
		void setTitle(const QString &t);

	protected:
		virtual ~BlockPort();

	private:
		BlockPort& operator=(const BlockPort &)=delete;

	protected:
		QString mTitle;
	};
}

#endif // BLOCKPORT_H
