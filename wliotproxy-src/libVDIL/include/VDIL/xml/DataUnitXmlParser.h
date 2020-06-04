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

#ifndef DATAUNITXMLPARSER_H
#define DATAUNITXMLPARSER_H

#include "VDIL/core/DataUnit.h"
#include <QDomElement>

namespace WLIOTVDIL
{
	class DataUnitXmlParser
	{
	public:
		static void toXml(const DataUnit &u,QDomElement &elem);
		static bool fromXml(DataUnit &u,const QDomElement &elem);
	};
}

#endif // DATAUNITXMLPARSER_H
