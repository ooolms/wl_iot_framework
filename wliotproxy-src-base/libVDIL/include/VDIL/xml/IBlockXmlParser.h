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

#ifndef IBLOCKXMLPARSER_H
#define IBLOCKXMLPARSER_H

#include <QDomElement>
#include "VDIL/core/BaseBlock.h"

namespace WLIOTVDIL
{
	class IBlockXmlParser
	{
	public:
		virtual ~IBlockXmlParser(){}
		virtual bool blockFromXml(BaseBlock *block,const QDomElement &blockElem,bool tryFixErrors)=0;

		/**
		 * @brief blockToXml
		 * common reserved attributes (don't use to store block's information):
		 * name, group, id, title, position_x, position_y
		 * @param block
		 * @param blockElem
		 */
		virtual void blockToXml(const BaseBlock *block,QDomElement &blockElem)=0;
	};
}

#endif // IBLOCKXMLPARSER_H
