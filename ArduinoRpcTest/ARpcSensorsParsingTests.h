#ifndef ARPCSENSORSPARSINGTESTS_H
#define ARPCSENSORSPARSINGTESTS_H

#include "QtUnitTestSet.h"

class ARpcSensorsParsingTests
	:public QtUnit::QtUnitTestSet
{
	Q_OBJECT
public:
	explicit ARpcSensorsParsingTests(QObject *parent=0);
	void testParseJson();
	void testParseXml();
};

#endif // ARPCSENSORSPARSINGTESTS_H
