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

#include "VDIL/xml/BlocksXmlParserFactory.h"

using namespace WLIOT;
using namespace WLIOTVDIL;

BlocksXmlParserFactory::BlocksXmlParserFactory()
{
}

BlocksXmlParserFactory::~BlocksXmlParserFactory()
{
	for(auto v:mGroups)
		delete v;
	mGroups.clear();
}

IBlocksGroupXmlParserFactory* BlocksXmlParserFactory::groupFactory(const QString &groupName)
{
	return mGroups.value(groupName);
}

IBlockXmlParser* BlocksXmlParserFactory::blockXmlParser(const QString &groupName,const QString &blockName)
{
	IBlocksGroupXmlParserFactory* f=mGroups.value(groupName);
	if(!f)return 0;
	return f->blockXmlParser(blockName);
}

bool BlocksXmlParserFactory::registerGroupFactory(IBlocksGroupXmlParserFactory *f)
{
	if(mGroups.contains(f->groupName()))return false;
	mGroups[f->groupName()]=f;
	return true;
}
