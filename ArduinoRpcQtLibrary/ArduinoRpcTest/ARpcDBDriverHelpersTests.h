#ifndef ARPCDBDRIVERHELPERSTESTS_H
#define ARPCDBDRIVERHELPERSTESTS_H

#include "QtUnitTestSet.h"

class ARpcDBDriverHelpersTests
	:public QtUnit::QtUnitTestSet
{
	Q_OBJECT
public:
	explicit ARpcDBDriverHelpersTests(QObject *parent=0);
	void testDetectIfHasTime();
	void testGetTime();
};

#endif // ARPCDBDRIVERHELPERSTESTS_H
