#include "ARpcDBDriverFixedBlocksTest.h"
#include "ARpcLocalStorage/ARpcDBDriverFixedBlocks.h"
#include <QDebug>

const QByteArray row1col1=QByteArray("000000");
const QByteArray row1col2=QByteArray("111");
const QByteArray row1col3=QByteArray("22222222");
const QByteArray row2col1=QByteArray("333333");
const QByteArray row2col2=QByteArray("444");
const QByteArray row2col3=QByteArray("55555555");

const QString fileName=QString("/tmp/tteesstt.bin.db");
const QVector<quint32> sizes=QVector<quint32>()<<6<<3<<8;

ARpcDBDriverFixedBlocksTest::ARpcDBDriverFixedBlocksTest(QObject *parent)
	:QtUnitTestSet("ARpcDBDriverFixedBlocksTest",parent)
{
	addTest((TestFunction)&ARpcDBDriverFixedBlocksTest::testCreateAndOpen,"Test create database and open it");
	addTest((TestFunction)&ARpcDBDriverFixedBlocksTest::testReadWriteWholeBlock,"Test read/write whole block");
	addTest((TestFunction)&ARpcDBDriverFixedBlocksTest::testReadWriteNote,"Test read/write note by note");
}

void ARpcDBDriverFixedBlocksTest::testCreateAndOpen()
{
	ARpcDBDriverFixedBlocks db;
	QFile(fileName).remove();
	VERIFY(db.create(fileName,sizes))
	db.close();
	ARpcDBDriverFixedBlocks db2;
	VERIFY(db2.open(fileName))
	COMPARE(db2.sizes(),sizes)
	VERIFY(db2.blocksCount()==0)
}

void ARpcDBDriverFixedBlocksTest::testReadWriteWholeBlock()
{
	ARpcDBDriverFixedBlocks db;
	QFile(fileName).remove();
	VERIFY(db.create(fileName,sizes))
	QByteArray block1=row1col1+row1col2+row1col3;
	QByteArray block2=row2col1+row2col2+row2col3;
	VERIFY(db.writeBlock(block1))
	VERIFY(db.writeBlock(block2.constData()))
	VERIFY(db.blocksCount()==2)
	db.close();
	ARpcDBDriverFixedBlocks db2;
	VERIFY(db2.open(fileName))
	COMPARE(db2.sizes(),sizes)
	COMPARE(db2.blocksCount(),2)
	QByteArray block11,block22;
	VERIFY(db2.readBlock(0,block11))
	block22.resize(block2.size());
	VERIFY(db2.readBlock(1,block22.data()))
	COMPARE(block1,block11)
	COMPARE(block2,block22)
	VERIFY(db2.writeBlock(block1))
}

void ARpcDBDriverFixedBlocksTest::testReadWriteNote()
{
	ARpcDBDriverFixedBlocks db;
	QFile(fileName).remove();
	VERIFY(db.create(fileName,sizes))
	VERIFY(db.addBlock())
	VERIFY(db.writeNote(0,row1col1))
	VERIFY(db.writeNote(1,row1col2))
	VERIFY(db.writeNote(2,row1col3))
	VERIFY(db.addBlock())
	VERIFY(db.writeNote(0,row2col1))
	VERIFY(db.writeNote(1,row2col2))
	VERIFY(db.writeNote(2,row2col3))
	VERIFY(db.blocksCount()==2)
	db.close();
	ARpcDBDriverFixedBlocks db2;
	VERIFY(db2.open(fileName))
	COMPARE(db2.sizes(),sizes)
	COMPARE(db2.blocksCount(),2)
	QByteArray block1=row1col1+row1col2+row1col3;
	QByteArray block2=row2col1+row2col2+row2col3;
	QByteArray block11,block22;
	VERIFY(db2.readBlock(0,block11))
	block22.resize(block2.size());
	VERIFY(db2.readBlock(1,block22.data()))
	COMPARE(block1,block11)
	COMPARE(block2,block22)
	QByteArray note;
	VERIFY(db2.readNote(0,0,note))
	COMPARE(note,row1col1)
	VERIFY(db2.readNote(0,1,note))
	COMPARE(note,row1col2)
	VERIFY(db2.readNote(0,2,note))
	COMPARE(note,row1col3)
	VERIFY(db2.readNote(1,0,note))
	COMPARE(note,row2col1)
	VERIFY(db2.readNote(1,1,note))
	COMPARE(note,row2col2)
	VERIFY(db2.readNote(1,2,note))
	COMPARE(note,row2col3)
}
