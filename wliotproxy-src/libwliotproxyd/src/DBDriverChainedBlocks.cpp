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

#include "DBDriverChainedBlocks.h"

static const QByteArray fixedHeader=QByteArray("ARpcDBCB");
static const int fixedHeaderSize=8;

using namespace WLIOT;

DBDriverChainedBlocks::DBDriverChainedBlocks()
{
	opened=false;
}

bool DBDriverChainedBlocks::create(const QString &filePath)
{
	if(opened)return false;
	file.setFileName(filePath);
	if(file.exists())return false;
	if(!file.open(QIODevice::ReadWrite))return false;
	if(file.write(fixedHeader.data(),fixedHeaderSize)!=fixedHeaderSize)return false;
	if(!file.flush())return false;
	totalOffset=fixedHeaderSize;
	opened=true;
	return true;
}

bool DBDriverChainedBlocks::open(const QString &filePath)
{
	if(opened)return false;
	file.setFileName(filePath);
	if(!file.open(QIODevice::ReadWrite))return false;
	if(!readHeaderAndBlockOffsets())return false;
	opened=true;
	return true;
}

void DBDriverChainedBlocks::close()
{
	if(!opened)return;
	file.close();
	opened=false;
	blockOffsets.clear();
	blockSizes.clear();
	totalOffset=0;
}

bool DBDriverChainedBlocks::isOpened()const
{
	return opened;
}

quint32 DBDriverChainedBlocks::blocksCount()
{
	return blockSizes.size();
}

quint32 DBDriverChainedBlocks::blockSize(quint32 blockIndex)
{
	return blockSizes[blockIndex];
}

bool DBDriverChainedBlocks::writeBlock(const void *data,quint32 size)
{
	if(!opened)return false;
	if(!file.seek(totalOffset))return false;
	if(file.write((const char *)&size,sizeof(quint32))!=sizeof(quint32))return false;
	if(file.write((const char*)data,size)!=size)return false;
	totalOffset+=sizeof(quint32);
	blockSizes.push_back(size);
	blockOffsets.push_back(totalOffset);
	totalOffset+=size;
	return true;
}

bool DBDriverChainedBlocks::writeBlock(const QByteArray &data)
{
	return writeBlock(data.constData(),data.size());
}

bool DBDriverChainedBlocks::readBlock(quint32 blockIndex,void *data)
{
	if(blockIndex>=blockSizes.size())return false;
	if(!file.seek(blockOffsets[blockIndex]))return false;
	return file.read((char*)data,blockSizes[blockIndex])==blockSizes[blockIndex];
}

bool DBDriverChainedBlocks::readBlock(quint32 blockIndex,QByteArray &data)
{
	if(blockIndex>=blockSizes.size())return false;
	if(!file.seek(blockOffsets[blockIndex]))return false;
	data.resize(blockSizes[blockIndex]);
	return file.read(data.data(),blockSizes[blockIndex])==blockSizes[blockIndex];
}

bool DBDriverChainedBlocks::readHeaderAndBlockOffsets()
{
	if(!file.seek(0))return false;
	QByteArray fHdr=file.read(fixedHeaderSize);
	if(fHdr!=fixedHeader)return false;
	totalOffset=fixedHeaderSize;
	while(!file.atEnd())
	{
		quint32 bs=0;
		if(file.read((char*)&bs,sizeof(quint32))!=sizeof(quint32))return false;
		totalOffset+=sizeof(quint32);
		blockOffsets.push_back(totalOffset);
		blockSizes.push_back(bs);
		totalOffset+=bs;
		if(!file.seek(totalOffset))return false;
	}
	return true;
}
