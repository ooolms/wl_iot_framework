#ifndef DATAUNITXMLPARSER_H
#define DATAUNITXMLPARSER_H

#include "GDIL/core/DataUnit.h"
#include <QDomElement>

class DataUnitXmlParser
{
public:
	static void toXml(const DataUnit &u,QDomElement &elem);
	static bool fromXml(DataUnit &u,const QDomElement &elem);
};

#endif // DATAUNITXMLPARSER_H
