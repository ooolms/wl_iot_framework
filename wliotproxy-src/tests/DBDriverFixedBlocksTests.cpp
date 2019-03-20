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

#include "DBDriverFixedBlocksTests.h"
#include "DBDriverFixedBlocks.h"
#include <QDebug>

const QByteArray row1col1=QByteArray("000000");
const QByteArray row1col2=QByteArray("111");
const QByteArray row1col3=QByteArray("22222222");
const QByteArray row2col1=QByteArray("333333");
const QByteArray row2col2=QByteArray("444");
const QByteArray row2col3=QByteArray("55555555");

const QString fileName=QString("/tmp/tteesstt.bin.db");
const QVector<quint32> sizes=QVector<quint32>()<<6<<3<<8;

DBDriverFixedBlocksTests::DBDriverFixedBlocksTests(QObject *parent)
	:QtUnitTestSet("DBDriverFixedBlocksTest",parent)
{
	addTest((TestFunction)&DBDriverFixedBlocksTests::testCreateAndOpen,"Test create database and open it");
	addTest((TestFunction)&DBDriverFixedBlocksTests::testReadWriteWholeBlock,"Test read/write whole block");
	addTest((TestFunction)&DBDriverFixedBlocksTests::testReadWriteNote,"Test read/write note by note");
}

void DBDriverFixedBlocksTests::testCreateAndOpen()
{
	DBDriverFixedBlocks db;
	QFile(fileName).remove();
	VERIFY(db.create(fileName,sizes));
	db.close();
	DBDriverFixedBlocks db2;
	VERIFY(db2.open(fileName));
	COMPARE(db2.noteSizes(),sizes);
	VERIFY(db2.blocksCount()==0);
}

void DBDriverFixedBlocksTests::testReadWriteWholeBlock()
{
	DBDriverFixedBlocks db;
	QFile(fileName).remove();
	VERIFY(db.create(fileName,sizes));
	QByteArray block1=row1col1+row1col2+row1col3;
	QByteArray block2=row2col1+row2col2+row2col3;
	VERIFY(db.writeBlock(block1));
	VERIFY(db.writeBlock(block2.constData()));
	VERIFY(db.blocksCount()==2);
	db.close();
	DBDriverFixedBlocks db2;
	VERIFY(db2.open(fileName));
	COMPARE(db2.noteSizes(),sizes);
	COMPARE(db2.blocksCount(),2);
	QByteArray block11,block22;
	VERIFY(db2.readBlock(0,block11));
	block22.resize(block2.size());
	VERIFY(db2.readBlock(1,block22.data()));
	COMPARE(block1,block11);
	COMPARE(block2,block22);
	VERIFY(db2.writeBlock(block1));
}

void DBDriverFixedBlocksTests::testReadWriteNote()
{
	DBDriverFixedBlocks db;
	QFile(fileName).remove();
	VERIFY(db.create(fileName,sizes));
	VERIFY(db.addBlock());
	VERIFY(db.writeNote(0,row1col1));
	VERIFY(db.writeNote(1,row1col2));
	VERIFY(db.writeNote(2,row1col3));
	VERIFY(db.addBlock());
	VERIFY(db.writeNote(0,row2col1));
	VERIFY(db.writeNote(1,row2col2));
	VERIFY(db.writeNote(2,row2col3));
	VERIFY(db.blocksCount()==2);
	db.close();
	DBDriverFixedBlocks db2;
	VERIFY(db2.open(fileName));
	COMPARE(db2.noteSizes(),sizes);
	COMPARE(db2.blocksCount(),2);
	QByteArray block1=row1col1+row1col2+row1col3;
	QByteArray block2=row2col1+row2col2+row2col3;
	QByteArray block11,block22;
	VERIFY(db2.readBlock(0,block11));
	block22.resize(block2.size());
	VERIFY(db2.readBlock(1,block22.data()));
	COMPARE(block1,block11);
	COMPARE(block2,block22);
	QByteArray note;
	VERIFY(db2.readNote(0,0,note));
	COMPARE(note,row1col1);
	VERIFY(db2.readNote(0,1,note));
	COMPARE(note,row1col2);
	VERIFY(db2.readNote(0,2,note));
	COMPARE(note,row1col3);
	VERIFY(db2.readNote(1,0,note));
	COMPARE(note,row2col1);
	VERIFY(db2.readNote(1,1,note));
	COMPARE(note,row2col2);
	VERIFY(db2.readNote(1,2,note));
	COMPARE(note,row2col3);
}
