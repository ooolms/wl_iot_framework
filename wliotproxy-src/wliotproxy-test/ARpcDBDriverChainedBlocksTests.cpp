/*******************************************
Copyright 2017 OOO "LMS"

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

	http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#include "ARpcDBDriverChainedBlocksTests.h"
#include "ARpcStorages/ARpcDBDriverChainedBlocks.h"

const QString fileName=QString("/tmp/tteesstt.bin.db");
const QByteArray block1=QByteArray("00000000000");
const QByteArray block2=QByteArray("11111");

ARpcDBDriverChainedBlocksTests::ARpcDBDriverChainedBlocksTests(QObject *parent)
	:QtUnitTestSet("ARpcDBDriverChainedBlocksTest",parent)
{
	addTest((TestFunction)&ARpcDBDriverChainedBlocksTests::testCreateAndOpen,"Test create database and open it");
	addTest((TestFunction)&ARpcDBDriverChainedBlocksTests::testReadWriteBlocks,"Test read/write blocks");
}

void ARpcDBDriverChainedBlocksTests::testCreateAndOpen()
{
	ARpcDBDriverChainedBlocks db;
	QFile(fileName).remove();
	VERIFY(db.create(fileName));
	db.close();
	ARpcDBDriverChainedBlocks db2;
	VERIFY(db2.open(fileName));
	VERIFY(db2.blocksCount()==0);
}

void ARpcDBDriverChainedBlocksTests::testReadWriteBlocks()
{
	ARpcDBDriverChainedBlocks db;
	QFile(fileName).remove();
	VERIFY(db.create(fileName));
	VERIFY(db.writeBlock(block1));
	VERIFY(db.writeBlock(block2.constData()));
	VERIFY(db.blocksCount()==2);
	db.close();
	ARpcDBDriverChainedBlocks db2;
	VERIFY(db2.open(fileName));
	COMPARE(db2.blocksCount(),2);
	QByteArray block11,block22;
	VERIFY(db2.readBlock(0,block11));
	block22.resize(block2.size());
	VERIFY(db2.readBlock(1,block22.data()));
	COMPARE(block1,block11);
	COMPARE(block2,block22);
	VERIFY(db2.writeBlock(block1));
}
