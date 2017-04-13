#ifndef ARPCDBDRIVERFIXEDBLOCKSTESTS_H
#define ARPCDBDRIVERFIXEDBLOCKSTESTS_H

#include "QtUnitTestSet.h"

//CRIT test for big sizes
class ARpcDBDriverFixedBlocksTests
	:public QtUnit::QtUnitTestSet
{
	Q_OBJECT
public:
	explicit ARpcDBDriverFixedBlocksTests(QObject *parent=0);
	void testCreateAndOpen();
	void testReadWriteWholeBlock();
	void testReadWriteNote();
};

#endif // ARPCDBDRIVERFIXEDBLOCKSTESTS_H
