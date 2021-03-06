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

#include "DBDriverFixedBlocks.h"
#include <QFileInfo>
#include <QCryptographicHash>

static const quint32 hashSize=20;//SHA-1
static const QByteArray fixedHeader=QByteArray("ARpcDBFB");
static const int fixedHeaderSize=8;

using namespace WLIOT;

DBDriverFixedBlocks::DBDriverFixedBlocks()
{
	wholeBlockSize=totalHeaderSize=0;
	totalBlocksCount=0;
	opened=false;
}

DBDriverFixedBlocks::~DBDriverFixedBlocks()
{
	if(opened)close();
}

bool DBDriverFixedBlocks::create(const QString &filePath,const QVector<quint32> &blockNotesSz)
{
	if(opened)return false;
	if(blockNotesSz.isEmpty())return false;
	file.setFileName(filePath);
	if(file.exists())return false;
	if(!file.open(QIODevice::ReadWrite))return false;
	blockNotesSizes=blockNotesSz;
	wholeBlockSize=0;
	totalBlocksCount=0;
	for(auto sz:blockNotesSizes)
		wholeBlockSize+=sz;
	calcHeaderSizeAndOffsets();
	if(!writeHeader())return false;
	opened=true;
	return true;
}

bool DBDriverFixedBlocks::open(const QString &filePath)
{
	if(opened)return false;
	file.setFileName(filePath);
	if(!file.open(QIODevice::ReadWrite))return false;
	if(!readHeader())return false;
	opened=true;
	return true;
}

void DBDriverFixedBlocks::close()
{
	if(!opened)return;
	file.close();
	opened=false;
	blockNotesSizes.clear();
	blockNotesOffsets.clear();
	wholeBlockSize=totalHeaderSize=0;
	totalBlocksCount=0;
}

bool DBDriverFixedBlocks::isOpened()const
{
	return opened;
}

const QVector<quint32> &DBDriverFixedBlocks::noteSizes()
{
	return blockNotesSizes;
}

quint64 DBDriverFixedBlocks::blocksCount()
{
	return totalBlocksCount;
}

quint32 DBDriverFixedBlocks::blockSize()
{
	return wholeBlockSize;
}

int DBDriverFixedBlocks::noteCount()
{
	return blockNotesSizes.size();
}

bool DBDriverFixedBlocks::readBlock(quint64 blockIndex,QByteArray &data)
{
	if(!opened)return false;
	if(blockIndex>=totalBlocksCount)return false;
	if(!file.seek(totalHeaderSize+blockIndex*wholeBlockSize))return false;
	data.resize(wholeBlockSize);
	return file.read(data.data(),wholeBlockSize)==wholeBlockSize;
}

bool DBDriverFixedBlocks::readBlock(quint64 blockIndex,void *data)
{
	if(!opened)return false;
	if(blockIndex>=totalBlocksCount)return false;
	if(!file.seek(totalHeaderSize+blockIndex*wholeBlockSize))return false;
	return file.read((char*)data,wholeBlockSize)==wholeBlockSize;
}

bool DBDriverFixedBlocks::readNote(quint64 blockIndex,quint32 noteIndex,QByteArray &data)
{
	if(!opened)return false;
	if(blockIndex>=totalBlocksCount)return false;
	if(noteIndex>=(quint32)blockNotesSizes.size())return false;
	if(!file.seek(totalHeaderSize+blockIndex*wholeBlockSize+blockNotesOffsets[noteIndex]))return false;
	data.resize(blockNotesSizes[noteIndex]);
	return file.read(data.data(),blockNotesSizes[noteIndex])==blockNotesSizes[noteIndex];
}

bool DBDriverFixedBlocks::readNote(quint64 blockIndex,quint32 noteIndex,void *data)
{
	if(!opened)return false;
	if(blockIndex>=totalBlocksCount)return false;
	if(noteIndex>=(quint32)blockNotesSizes.size())return false;
	if(!file.seek(totalHeaderSize+blockIndex*wholeBlockSize+blockNotesOffsets[noteIndex]))return false;
	return file.read((char*)data,blockNotesSizes[noteIndex])==blockNotesSizes[noteIndex];
}

bool DBDriverFixedBlocks::writeBlock(const QByteArray &data)
{
	if((quint32)data.size()!=wholeBlockSize)return false;
	return writeBlock(data.constData());
}

bool DBDriverFixedBlocks::writeBlock(const void *data)
{
	if(!opened)return false;
	if(!file.seek(totalHeaderSize+totalBlocksCount*wholeBlockSize))return false;
	if(file.write((const char*)data,wholeBlockSize)!=wholeBlockSize)return false;
	++totalBlocksCount;
	return true;
}

bool DBDriverFixedBlocks::updateBlock(quint64 blockIndex,const QByteArray &data)
{
	if((quint32)data.size()!=wholeBlockSize)return false;
	return updateBlock(blockIndex,data.constData());
}

bool DBDriverFixedBlocks::updateBlock(quint64 blockIndex,const void *data)
{
	if(!opened)return false;
	if(blockIndex>=totalBlocksCount)return false;
	if(!file.seek(totalHeaderSize+blockIndex*wholeBlockSize))return false;
	if(file.write((const char*)data,wholeBlockSize)!=wholeBlockSize)return false;
	++totalBlocksCount;
	return true;
}

bool DBDriverFixedBlocks::addBlock()
{
	if(!opened)return false;
	if(!file.seek(totalHeaderSize+totalBlocksCount*wholeBlockSize))return false;
	char *data=new (std::nothrow) char[wholeBlockSize];
	if(!data)return false;
	memset(data,0,wholeBlockSize);
	if(file.write(data,wholeBlockSize)!=wholeBlockSize)
	{
		delete[] data;
		return false;
	}
	++totalBlocksCount;
	delete[] data;
	return true;
}

bool DBDriverFixedBlocks::addManyBlocks(quint64 count)
{
	if(!opened)return false;
	if(!file.resize(totalHeaderSize+(totalBlocksCount+count)*wholeBlockSize))return false;
	totalBlocksCount+=count;
	return true;
}

bool DBDriverFixedBlocks::addManyBlocks(quint64 count,const QByteArray &data)
{
	if(!opened)return false;
	if(!file.seek(totalHeaderSize+totalBlocksCount*wholeBlockSize))return false;
	if((quint32)data.size()!=wholeBlockSize)return false;
	const char *dataStr=data.data();
	for(quint64 i=0;i<count;++i)
	{
		if(file.write(dataStr,wholeBlockSize)!=wholeBlockSize)return false;
		++totalBlocksCount;
	}
	return true;
}

bool DBDriverFixedBlocks::writeNote(quint32 noteIndex,const QByteArray &data)
{
	if(!opened)return false;
	if(totalBlocksCount==0)return false;
	if(noteIndex>=(quint32)blockNotesSizes.size())return false;
	if(blockNotesSizes[noteIndex]!=(quint32)data.size())return false;
	if(!file.seek(totalHeaderSize+(totalBlocksCount-1)*wholeBlockSize+blockNotesOffsets[noteIndex]))return false;
	if(file.write((const char*)data.constData(),blockNotesSizes[noteIndex])!=blockNotesSizes[noteIndex])return false;
	return true;
}

bool DBDriverFixedBlocks::writeNote(quint32 noteIndex,const void *data)
{
	if(!opened)return false;
	if(totalBlocksCount==0)return false;
	if(noteIndex>=(quint32)blockNotesSizes.size())return false;
	if(!file.seek(totalHeaderSize+(totalBlocksCount-1)*wholeBlockSize+blockNotesOffsets[noteIndex]))return false;
	if(file.write((const char*)data,blockNotesSizes[noteIndex])!=blockNotesSizes[noteIndex])return false;
	return true;
}

bool DBDriverFixedBlocks::readHeader()
{
	if(!file.seek(0))return false;
	QByteArray fHdr=file.read(fixedHeaderSize);
	if(fHdr!=fixedHeader)return false;
	QCryptographicHash hash(QCryptographicHash::Sha1);
	hash.addData(fHdr);
	quint32 notesCount=0;
	if(file.read((char*)&notesCount,sizeof(notesCount))!=sizeof(notesCount))return false;
	if(notesCount==0)return false;
	hash.addData((const char*)&notesCount,sizeof(notesCount));
	blockNotesSizes.resize(notesCount);
	wholeBlockSize=0;
	for(quint32 i=0;i<notesCount;++i)
	{
		quint32 sz=0;
		if(file.read((char*)&sz,sizeof(quint32))!=sizeof(quint32))return false;
		hash.addData((const char*)&sz,sizeof(quint32));
		blockNotesSizes[i]=sz;
		wholeBlockSize+=sz;
	}
	calcHeaderSizeAndOffsets();
	QByteArray hashFromFile=file.read(hashSize);
	if(hashFromFile.size()!=hashSize)return false;
	if(hashFromFile!=hash.result())return false;
	if((file.size()-totalHeaderSize)%wholeBlockSize!=0)return false;//corrupted data
	totalBlocksCount=(file.size()-(totalHeaderSize))/wholeBlockSize;
	return true;
}

bool DBDriverFixedBlocks::writeHeader()
{
	if(file.pos()!=0)return false;
	if(file.size()!=0)return false;
	if(file.write(fixedHeader.constData(),fixedHeaderSize)!=fixedHeaderSize)return false;
	QCryptographicHash hash(QCryptographicHash::Sha1);
	hash.addData(fixedHeader);
	quint32 bCount=blockNotesSizes.size();
	if(file.write((const char*)&bCount,sizeof(quint32))!=sizeof(quint32))return false;
	hash.addData((const char*)&bCount,sizeof(quint32));
	for(quint32 i=0;i<bCount;++i)
	{
		if(file.write((const char*)&blockNotesSizes[i],sizeof(quint32))!=sizeof(quint32))return false;
		hash.addData((const char*)&blockNotesSizes[i],sizeof(quint32));
	}
	QByteArray hashResult=hash.result();
	if(hashResult.size()!=20)return false;//o_O not sha-1?
	if(file.write(hashResult)!=hashResult.size())return false;
	return file.flush();
}

void DBDriverFixedBlocks::calcHeaderSizeAndOffsets()
{
	totalHeaderSize=sizeof(quint32)+blockNotesSizes.size()*sizeof(quint32)+hashSize+fixedHeaderSize;
	blockNotesOffsets.resize(blockNotesSizes.size());
	if(!blockNotesOffsets.isEmpty())
		blockNotesOffsets[0]=0;
	for(int i=1;i<blockNotesSizes.size();++i)
		blockNotesOffsets[i]=blockNotesOffsets[i-1]+blockNotesSizes[i-1];
}
