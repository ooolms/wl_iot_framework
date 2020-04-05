#ifndef PROGRAMXMLPARSER_H
#define PROGRAMXMLPARSER_H

#include "GDIL/core/Program.h"

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
	static BaseBlock* blockFromXml(BlocksXmlParserFactory *f,BlocksFactory *bf,QDomElement &blockElem);
};

#endif // PROGRAMXMLPARSER_H
