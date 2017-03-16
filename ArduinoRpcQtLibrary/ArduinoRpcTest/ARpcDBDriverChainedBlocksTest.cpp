#include "ARpcDBDriverChainedBlocksTest.h"
#include "ARpcLocalStorage/ARpcDBDriverChainedBlocks.h"

const QString fileName=QString("/tmp/tteesstt.bin.db");
const QByteArray block1=QByteArray("00000000000");
const QByteArray block2=QByteArray("11111");

ARpcDBDriverChainedBlocksTest::ARpcDBDriverChainedBlocksTest(QObject *parent)
	:QtUnitTestSet("ARpcDBDriverChainedBlocksTest",parent)
{
	addTest((TestFunction)&ARpcDBDriverChainedBlocksTest::testCreateAndOpen,"Test create database and open it");
	addTest((TestFunction)&ARpcDBDriverChainedBlocksTest::testReadWriteBlocks,"Test read/write blocks");
}

void ARpcDBDriverChainedBlocksTest::testCreateAndOpen()
{
	ARpcDBDriverChainedBlocks db;
	QFile(fileName).remove();
	VERIFY(db.create(fileName))
	db.close();
	ARpcDBDriverChainedBlocks db2;
	VERIFY(db2.open(fileName))
	VERIFY(db2.blocksCount()==0)
}

void ARpcDBDriverChainedBlocksTest::testReadWriteBlocks()
{
	ARpcDBDriverChainedBlocks db;
	QFile(fileName).remove();
	VERIFY(db.create(fileName))
	VERIFY(db.writeBlock(block1))
	VERIFY(db.writeBlock(block2.constData()))
	VERIFY(db.blocksCount()==2)
	db.close();
	ARpcDBDriverChainedBlocks db2;
	VERIFY(db2.open(fileName))
	COMPARE(db2.blocksCount(),2)
	QByteArray block11,block22;
	VERIFY(db2.readBlock(0,block11))
	block22.resize(block2.size());
	VERIFY(db2.readBlock(1,block22.data()))
	COMPARE(block1,block11)
	COMPARE(block2,block22)
	VERIFY(db2.writeBlock(block1))
}
