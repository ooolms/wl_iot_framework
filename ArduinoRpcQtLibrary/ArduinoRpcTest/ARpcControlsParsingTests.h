#ifndef ARPCCONTROLSPARSINGTESTS_H
#define ARPCCONTROLSPARSINGTESTS_H

#include "QtUnitTestSet.h"

class ARpcControlsParsingTests
	:public QtUnit::QtUnitTestSet
{
	Q_OBJECT
public:
	explicit ARpcControlsParsingTests(QObject *parent=0);
	void testParseJson();
	void testParseXml();
};

#endif // ARPCCONTROLSPARSINGTESTS_H
