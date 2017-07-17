#include "ARpcDBDriverChainedBlocks.h"

static const QByteArray fixedHeader=QByteArray("ARpcDBCB");
static const int fixedHeaderSize=8;

ARpcDBDriverChainedBlocks::ARpcDBDriverChainedBlocks(QObject *parent)
	:QObject(parent)
{
	opened=false;
}

bool ARpcDBDriverChainedBlocks::create(const QString &filePath)
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

bool ARpcDBDriverChainedBlocks::open(const QString &filePath)
{
	if(opened)return false;
	file.setFileName(filePath);
	if(!file.open(QIODevice::ReadWrite))return false;
	if(!readHeaderAndBlockOffsets())return false;
	opened=true;
	return true;
}

void ARpcDBDriverChainedBlocks::close()
{
	if(!opened)return;
	file.close();
	opened=false;
	blockOffsets.clear();
	blockSizes.clear();
	totalOffset=0;
}

bool ARpcDBDriverChainedBlocks::isOpened()const
{
	return opened;
}

quint32 ARpcDBDriverChainedBlocks::blocksCount()
{
	return blockSizes.size();
}

quint32 ARpcDBDriverChainedBlocks::blockSize(quint32 blockIndex)
{
	return blockSizes[blockIndex];
}

bool ARpcDBDriverChainedBlocks::writeBlock(const void *data,quint32 size)
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

bool ARpcDBDriverChainedBlocks::writeBlock(const QByteArray &data)
{
	return writeBlock(data.constData(),data.size());
}

bool ARpcDBDriverChainedBlocks::readBlock(quint32 blockIndex,void *data)
{
	if(blockIndex>=blockSizes.size())return false;
	if(!file.seek(blockOffsets[blockIndex]))return false;
	return file.read((char*)data,blockSizes[blockIndex])==blockSizes[blockIndex];
}

bool ARpcDBDriverChainedBlocks::readBlock(quint32 blockIndex,QByteArray &data)
{
	if(blockIndex>=blockSizes.size())return false;
	if(!file.seek(blockOffsets[blockIndex]))return false;
	data.resize(blockSizes[blockIndex]);
	return file.read(data.data(),blockSizes[blockIndex])==blockSizes[blockIndex];
}

bool ARpcDBDriverChainedBlocks::readHeaderAndBlockOffsets()
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
