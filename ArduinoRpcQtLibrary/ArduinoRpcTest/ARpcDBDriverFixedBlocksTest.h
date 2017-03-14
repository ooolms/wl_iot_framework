#ifndef ARPCDBDRIVERFIXEDBLOCKSTEST_H
#define ARPCDBDRIVERFIXEDBLOCKSTEST_H

#include "QtUnitTestSet.h"

class ARpcDBDriverFixedBlocksTest
	:public QtUnit::QtUnitTestSet
{
	Q_OBJECT
public:
	explicit ARpcDBDriverFixedBlocksTest(QObject *parent=0);
	void testCreateAndOpen();
	void testReadWriteWholeBlock();
	void testReadWriteMultipleBlocksBlocks();
	void testReadWriteNote();
};

#endif // ARPCDBDRIVERFIXEDBLOCKSTEST_H
