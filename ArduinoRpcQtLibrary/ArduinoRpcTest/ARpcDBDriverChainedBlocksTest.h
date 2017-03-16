#ifndef ARPCDBDRIVERCHAINEDBLOCKSTEST_H
#define ARPCDBDRIVERCHAINEDBLOCKSTEST_H

#include "QtUnitTestSet.h"

class ARpcDBDriverChainedBlocksTest
	:public QtUnit::QtUnitTestSet
{
	Q_OBJECT
public:
	explicit ARpcDBDriverChainedBlocksTest(QObject *parent=0);
	void testCreateAndOpen();
	void testReadWriteBlocks();
};

#endif // ARPCDBDRIVERCHAINEDBLOCKSTEST_H
