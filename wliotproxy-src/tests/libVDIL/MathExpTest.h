#ifndef MATHEXPTEST_H
#define MATHEXPTEST_H

#include <QtUnitTestSet.h>

class MathExpTest
	:public QtUnit::QtUnitTestSet
{
	Q_OBJECT
public:
	explicit MathExpTest(QObject *parent=nullptr);
	void testOperationsPriority();
	void testParseWithFunctions();
	void testExceptionHandling();
};

#endif // MATHEXPTEST_H
