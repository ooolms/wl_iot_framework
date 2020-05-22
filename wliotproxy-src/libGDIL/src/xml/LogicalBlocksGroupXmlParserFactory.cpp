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

#include "GDIL/xml/LogicalBlocksGroupXmlParserFactory.h"
#include "GDIL/core/LogicalBlocksGroupFactory.h"
#include "GDIL/blocks/LogicalBlocks.h"
#include "GDIL/xml/DefaultBlockXmlParser.h"

using namespace WLIOT;
using namespace WLIOTGDIL;

LogicalBlocksGroupXmlParserFactory::LogicalBlocksGroupXmlParserFactory()
{
	addParser(NotBoolBlock::mBlockName,new DefaultBlockXmlParser);
	addParser(AndBoolBlock::mBlockName,new DefaultBlockXmlParser);
	addParser(OrBoolBlock::mBlockName,new DefaultBlockXmlParser);
	addParser(XorBoolBlock::mBlockName,new DefaultBlockXmlParser);
	addParser(AndNotBoolBlock::mBlockName,new DefaultBlockXmlParser);
	addParser(OrNotBoolBlock::mBlockName,new DefaultBlockXmlParser);
	addParser(XorNotBoolBlock::mBlockName,new DefaultBlockXmlParser);
}

QString LogicalBlocksGroupXmlParserFactory::groupName()const
{
	return LogicalBlocksGroupFactory::mGroupName;
}
