#ifndef MESSAGETESTS_H
#define MESSAGETESTS_H

#include "QtUnitTestSet.h"

class MessageTests
	:public QtUnit::QtUnitTestSet
{
	Q_OBJECT
public:
	explicit MessageTests(QObject *parent=nullptr);
	void testDump();
	void testEscape();
};

#endif // MESSAGETESTS_H
