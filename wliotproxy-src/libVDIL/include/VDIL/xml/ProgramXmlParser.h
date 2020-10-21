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
	class Engine;
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
			QList<QPointF> linePoints;
		};

	public:
		static QByteArray toXml(Engine *e,const Program *p);
		static Program* fromXml(Engine *e,const QByteArray &xml,bool tryFixErrors);

	private:
		static bool blockToXml(Engine *e,BaseBlock *b,QDomElement &listElem);
		static bool blockFromXml(SubProgram *p,Engine *e,QDomElement &blockElem,bool tryFixErrors);
		static void subProgramToXml(Engine *e,SubProgramBlock *b,QDomElement &blockElem);
		static bool subProgramFromXml(Engine *e,SubProgramBlock *b,QDomElement &blockElem,bool tryFixErrors);
		static bool renderLinks(SubProgram *p,QDomElement linksElem,bool tryFixErrors);
		static QString storeLinePoints(const QList<QPointF> &pts);
		static QList<QPointF> parseLinePoints(const QString &s);
	};
}

#endif // PROGRAMXMLPARSER_H
