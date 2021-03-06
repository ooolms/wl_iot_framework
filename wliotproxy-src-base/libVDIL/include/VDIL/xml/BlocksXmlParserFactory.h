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

#ifndef BLOCKSXMLPARSERFACTORY_H
#define BLOCKSXMLPARSERFACTORY_H

#include "VDIL/xml/IBlocksGroupXmlParserFactory.h"

namespace WLIOTVDIL
{
	class BlocksXmlParserFactory
	{
	public:
		IBlocksGroupXmlParserFactory* groupFactory(const QString &groupName);
		IBlockXmlParser* blockXmlParser(const QString &groupName,const QString &blockName);
		bool registerGroupFactory(IBlocksGroupXmlParserFactory *f);

	private:
		BlocksXmlParserFactory();
		~BlocksXmlParserFactory();

	private:
		friend class Engine;
		QMap<QString,IBlocksGroupXmlParserFactory*> mGroups;
	};
}

#endif // BLOCKSXMLPARSERFACTORY_H
