#include "ARpcDBDriverFixedBlocks.h"
#include <QFileInfo>
#include <QCryptographicHash>

ARpcDBDriverFixedBlocks::ARpcDBDriverFixedBlocks(QObject *parent)
	:QObject(parent)
{
	blockSize=totalHeaderSize=0;
	opened=false;
}

ARpcDBDriverFixedBlocks::~ARpcDBDriverFixedBlocks()
{
	if(opened)close();
}

bool ARpcDBDriverFixedBlocks::create(const QString &filePath,const QVector<quint32> &blockNotesSz)
{
	if(opened)return false;
	if(blockNotesSz.isEmpty())return false;
	file.setFileName(filePath);
	if(file.exists())return false;
	if(!file.open(QIODevice::WriteOnly))return false;
	blockNotesSizes=blockNotesSz;
	blockSize=0;
	totalBlocksCount=0;
	for(auto sz:blockNotesSizes)
		blockSize+=sz;
	totalHeaderSize=4+blockNotesSizes.size()*4+20;
	calcOffsets();
	if(!writeHeader())return false;
	opened=true;
	return true;
}

bool ARpcDBDriverFixedBlocks::open(const QString &filePath)
{
	if(opened)return false;
	file.setFileName(filePath);
	if(!file.open(QIODevice::ReadOnly))return false;
	if(!readHeader())return false;
	opened=true;
	return true;
}

void ARpcDBDriverFixedBlocks::close()
{
	if(!opened)return;
	file.close();
	opened=false;
	blockNotesSizes.clear();
	blockSize=totalHeaderSize=totalBlocksCount=0;
}

bool ARpcDBDriverFixedBlocks::isOpened()const
{
	return opened;
}

const QVector<quint32>& ARpcDBDriverFixedBlocks::sizes()
{
	return blockNotesSizes;
}

quint64 ARpcDBDriverFixedBlocks::blocksCount()
{
	return totalBlocksCount;
}

quint32 ARpcDBDriverFixedBlocks::notesCount()
{
	return blockNotesSizes.count();
}

bool ARpcDBDriverFixedBlocks::readBlock(quint64 blockIndex,QByteArray &data)
{
	if(!opened)return false;
	if(blockIndex>=totalBlocksCount)return false;
	if(!file.seek(totalHeaderSize+blockIndex*blockSize))return false;
	data=file.read(blockSize);
	return data.size()==(int)blockSize;
}

bool ARpcDBDriverFixedBlocks::readBlock(quint64 blockIndex,char *data)
{
	if(!opened)return false;
	if(blockIndex>=totalBlocksCount)return false;
	if(!file.seek(totalHeaderSize+blockIndex*blockSize))return false;
	return file.read(data,blockSize)==blockSize;
}

bool ARpcDBDriverFixedBlocks::readNote(quint64 blockIndex,quint32 noteIndex,QByteArray &data)
{
	if(!opened)return false;
	if(blockIndex>=totalBlocksCount)return false;
	if(noteIndex>=(quint32)blockNotesSizes.size())return false;
	if(!file.seek(totalHeaderSize+blockIndex*blockSize+blockNotesOffsets[noteIndex]))return false;
	data=file.read(blockNotesSizes[noteIndex]);
	return (quint32)data.size()==blockNotesSizes[noteIndex];
}

bool ARpcDBDriverFixedBlocks::readNote(quint64 blockIndex, quint32 noteIndex, char *data)
{
	if(!opened)return false;
	if(blockIndex>=totalBlocksCount)return false;
	if(noteIndex>=(quint32)blockNotesSizes.size())return false;
	if(!file.seek(totalHeaderSize+blockIndex*blockSize+blockNotesOffsets[noteIndex]))return false;
	return file.read(data,blockNotesSizes[noteIndex])==blockNotesSizes[noteIndex];
}

bool ARpcDBDriverFixedBlocks::addBlock(const QByteArray &data)
{
	if(!opened)return false;
	if((quint32)data.size()!=blockSize)return false;
	if(!file.seek(totalHeaderSize+totalBlocksCount*blockSize))return false;
	if(file.write(data)!=blockSize)return false;
	++totalBlocksCount;
	return true;
}

bool ARpcDBDriverFixedBlocks::addBlock(const char *data)
{
	if(!opened)return false;
	if(!file.seek(totalHeaderSize+totalBlocksCount*blockSize))return false;
	if(file.write(data,blockSize)!=blockSize)return false;
	++totalBlocksCount;
	return true;
}

bool ARpcDBDriverFixedBlocks::addBlock()
{
	if(!opened)return false;
	if(!file.seek(totalHeaderSize+totalBlocksCount*blockSize))return false;
	QByteArray data;
	data.resize(blockSize);
	data.fill(0);
	if(file.write(data)!=blockSize)return false;
	++totalBlocksCount;
	return true;
}

bool ARpcDBDriverFixedBlocks::writeNote(quint32 noteIndex,const QByteArray &data)
{
	if(!opened)return false;
	if(totalBlocksCount==0)return false;
	if(noteIndex>=(quint32)blockNotesSizes.size())return false;
	if((quint32)data.size()!=blockNotesSizes[noteIndex])return false;
	if(!file.seek(totalHeaderSize+(totalBlocksCount-1)*blockSize+blockNotesOffsets[noteIndex]))return false;
	if(file.write(data)!=blockNotesSizes[noteIndex])return false;
	return true;
}

bool ARpcDBDriverFixedBlocks::writeNote(quint32 noteIndex,const char *data)
{
	if(!opened)return false;
	if(totalBlocksCount==0)return false;
	if(noteIndex>=(quint32)blockNotesSizes.size())return false;
	if(!file.seek(totalHeaderSize+(totalBlocksCount-1)*blockSize+blockNotesOffsets[noteIndex]))return false;
	if(file.write(data,blockNotesSizes[noteIndex])!=blockNotesSizes[noteIndex])return false;
	return true;
}

bool ARpcDBDriverFixedBlocks::addMultiBlocks(const QByteArray &data)
{
	if(!opened)return false;
	if(data.size()==0||data.size()%blockSize!=0)return false;
	quint64 bCount=data.size()/blockSize;
	if(!file.seek(totalHeaderSize+totalBlocksCount*blockSize))return false;
	if(file.write(data)!=data.size())return false;
	totalBlocksCount+=bCount;
	return true;
}

bool ARpcDBDriverFixedBlocks::addMultiBlocks(const char *data,quint32 bCount)
{
	if(!opened)return false;
	if(bCount==0)return false;
	if(!file.seek(totalHeaderSize+totalBlocksCount*blockSize))return false;
	quint64 totalSize=bCount*blockSize;
	if((quint64)file.write(data,totalSize)!=totalSize)return false;
	totalBlocksCount+=bCount;
	return true;
}

bool ARpcDBDriverFixedBlocks::readHeader()
{
	if(!file.seek(0))return false;
	QCryptographicHash hash(QCryptographicHash::Sha1);
	quint32 bCount=0;
	if(file.read((char*)&bCount,4)!=4)return false;
	if(bCount==0)return false;
	hash.addData((const char*)&bCount,4);
	blockNotesSizes.resize(bCount);
	blockSize=0;
	for(quint32 i=0;i<bCount;++i)
	{
		quint32 sz=0;
		if(file.read((char*)&sz,4)!=4)return false;
		hash.addData((const char*)&sz,4);
		blockNotesSizes[i]=sz;
		blockSize+=sz;
	}
	calcOffsets();
	totalHeaderSize=4+bCount*4+20;
	QByteArray hashFromFile=file.read(20);
	if(hashFromFile.size()!=20)return false;
	if(hashFromFile!=hash.result())return false;
	if((file.size()-(totalHeaderSize))%blockSize!=0)return false;//corrupted data
	totalBlocksCount=(file.size()-(totalHeaderSize))/blockSize;
	return true;
}

bool ARpcDBDriverFixedBlocks::writeHeader()
{
	if(file.pos()!=0)return false;
	if(file.size()!=0)return false;
	QCryptographicHash hash(QCryptographicHash::Sha1);
	quint32 bCount=blockNotesSizes.size();
	if(file.write((const char*)&bCount,4)!=4)return false;
	hash.addData((const char*)&bCount,4);
	for(quint32 i=0;i<bCount;++i)
	{
		if(file.write((const char*)&blockNotesSizes[i],4)!=4)return false;
		hash.addData((const char*)&blockNotesSizes[i],4);
	}
	QByteArray hashResult=hash.result();
	if(hashResult.size()!=20)return false;//o_O not sha-1?
	file.write(hashResult);
	return true;
}

void ARpcDBDriverFixedBlocks::calcOffsets()
{
	blockNotesOffsets.resize(blockNotesSizes.size());
	if(!blockNotesOffsets.isEmpty())
		blockNotesOffsets[0]=0;
	for(int i=1;i<blockNotesSizes.size();++i)
		blockNotesOffsets[i]=blockNotesOffsets[i-1]+blockNotesSizes[i-1];
}
