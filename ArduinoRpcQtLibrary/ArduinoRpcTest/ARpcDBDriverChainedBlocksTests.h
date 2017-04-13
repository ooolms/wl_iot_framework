#ifndef ARPCDBDRIVERCHAINEDBLOCKSTESTS_H
#define ARPCDBDRIVERCHAINEDBLOCKSTESTS_H

#include "QtUnitTestSet.h"

class ARpcDBDriverChainedBlocksTests
	:public QtUnit::QtUnitTestSet
{
	Q_OBJECT
public:
	explicit ARpcDBDriverChainedBlocksTests(QObject *parent=0);
	void testCreateAndOpen();
	void testReadWriteBlocks();
};

#endif // ARPCDBDRIVERCHAINEDBLOCKSTESTS_H
