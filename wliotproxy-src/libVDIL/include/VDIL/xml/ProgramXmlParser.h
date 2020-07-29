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
	class SubProgramBlock;
	class SubProgram;

	class ProgramXmlParser
	{
	private:
		struct LinkDef
		{
			quint32 fromBlockId;
			int fromOutputIndex;
			quint32 toBlockId;
			int toInputIndex;
		};

	public:
		static QByteArray toXml(BlocksXmlParserFactory *f,const Program *p);
		static Program* fromXml(BlocksXmlParserFactory *f,BlocksFactory *bf,const QByteArray &xml,bool tryFixErrors);

	private:
		static bool blockToXml(BlocksXmlParserFactory *f,BaseBlock *b,QDomElement &listElem);
		static bool blockFromXml(SubProgram *p,
			BlocksXmlParserFactory *f,BlocksFactory *bf,QDomElement &blockElem,bool tryFixErrors);
		static void subProgramToXml(BlocksXmlParserFactory *f,SubProgramBlock *b,QDomElement &blockElem);
		static bool subProgramFromXml(BlocksXmlParserFactory *f,BlocksFactory *bf,
			SubProgramBlock *b,QDomElement &blockElem,bool tryFixErrors);
		static bool renderLinks(SubProgram *p,SubProgramBlock *b,QDomElement linksElem,bool tryFixErrors);
	};
}

#endif // PROGRAMXMLPARSER_H
