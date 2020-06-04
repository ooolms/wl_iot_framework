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

#ifndef PROGRAMXMLPARSER_H
#define PROGRAMXMLPARSER_H

#include "VDIL/core/Program.h"

namespace WLIOTVDIL
{
	class BlocksXmlParserFactory;
	class BlocksFactory;

	class ProgramXmlParser
	{
	public:
		static QByteArray toXml(BlocksXmlParserFactory *f,const Program *p);
		static Program* fromXml(BlocksXmlParserFactory *f,BlocksFactory *bf,const QByteArray &xml);

	private:
		static bool blockToXml(BlocksXmlParserFactory *f,BaseBlock *b,QDomElement &listElem);
		static void linksToXml(BaseBlock *b,QDomElement &linksElem);
		static bool blockFromXml(Program *p,BlocksXmlParserFactory *f,BlocksFactory *bf,QDomElement &blockElem);
	};
}

#endif // PROGRAMXMLPARSER_H
