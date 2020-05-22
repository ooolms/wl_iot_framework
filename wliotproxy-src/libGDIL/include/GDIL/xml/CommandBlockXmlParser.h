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

#ifndef COMMANDBLOCKXMLPARSER_H
#define COMMANDBLOCKXMLPARSER_H

#include "GDIL/xml/IBlockXmlParser.h"

namespace WLIOTGDIL
{
	class CommandBlockXmlParser
		:public IBlockXmlParser
	{
	public:
		virtual bool blockFromXml(BaseBlock *block,const QDomElement &blockElem)override;
		virtual void blockToXml(const BaseBlock *block,QDomElement &blockElem)override;
	};
}

#endif // COMMANDBLOCKXMLPARSER_H
