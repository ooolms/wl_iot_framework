#ifndef ARPCLASTNVALUESSTORAGETESTS_H
#define ARPCLASTNVALUESSTORAGETESTS_H

#include "QtUnitTestSet.h"

class ARpcLastNValuesStorageTests
	:public QtUnit::QtUnitTestSet
{
	Q_OBJECT
public:
	explicit ARpcLastNValuesStorageTests(QObject *parent=0);
	void testStorageSingleDontTouchTime();
	void testStorageSingleLTDontTouchTime();
	void testStorageSingleGTDropTime();
	virtual bool init();
	virtual void cleanup();
};

#endif // ARPCLASTNVALUESSTORAGETESTS_H
