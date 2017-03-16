#ifndef ARPCDBDRIVERFIXEDBLOCKSTEST_H
#define ARPCDBDRIVERFIXEDBLOCKSTEST_H

#include "QtUnitTestSet.h"

//CRIT test for big sizes
class ARpcDBDriverFixedBlocksTest
	:public QtUnit::QtUnitTestSet
{
	Q_OBJECT
public:
	explicit ARpcDBDriverFixedBlocksTest(QObject *parent=0);
	void testCreateAndOpen();
	void testReadWriteWholeBlock();
	void testReadWriteNote();
};

#endif // ARPCDBDRIVERFIXEDBLOCKSTEST_H
