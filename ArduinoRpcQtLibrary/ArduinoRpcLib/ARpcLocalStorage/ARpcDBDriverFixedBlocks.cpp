#include "ARpcDBDriverFixedBlocks.h"
#include <QFileInfo>
#include <QCryptographicHash>

ARpcDBDriverFixedBlocks::ARpcDBDriverFixedBlocks(QObject *parent)
	:QObject(parent)
{
	blockSize=totalHeaderSize=0;
	opened=false;
}

bool ARpcDBDriverFixedBlocks::create(const QString &filePath,const QVector<quint32> &blockNotesSz)
{
	if(opened)return false;
	file.setFileName(filePath);
	if(file.exists())return false;
	if(!file.open(QIODevice::WriteOnly))return false;
	blockNotesSizes=blockNotesSz;
	blockSize=0;
	for(auto sz:blockNotesSizes)
		blockSize+=sz;
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
	if(blockIndex>=totalBlocksCount)return false;
	if(!file.seek(totalHeaderSize+blockIndex*blockSize))return false;
	data=file.read(blockSize);
	return data.size()==(int)blockSize;
}

bool ARpcDBDriverFixedBlocks::readNote(quint64 blockIndex,quint32 noteIndex,QByteArray &data)
{
	if(blockIndex>=totalBlocksCount)return false;
	if(noteIndex>=(quint32)blockNotesSizes.size())return false;
	if(!file.seek(totalHeaderSize+blockIndex*blockSize+blockNotesOffsets[noteIndex]))return false;
	data=file.read(blockNotesSizes[noteIndex]);
	return (quint32)data.size()==blockNotesSizes[noteIndex];
}

bool ARpcDBDriverFixedBlocks::readHeader()
{
	if(!file.seek(0))return false;
	QCryptographicHash hash(QCryptographicHash::Sha1);
	quint32 bCount=0;
	if(file.read((char*)&bCount,4)!=4)return false;
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
