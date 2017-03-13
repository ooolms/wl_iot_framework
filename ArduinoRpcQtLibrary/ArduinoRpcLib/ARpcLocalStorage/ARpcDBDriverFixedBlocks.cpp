#include "ARpcDBDriverFixedBlocks.h"
#include <QFile>
#include <QFileInfo>
#include <QCryptographicHash>

ARpcDBDriverFixedBlocks::ARpcDBDriverFixedBlocks(QObject *parent)
	:QObject(parent)
{
	file=0;
	blockSize=totalHeaderSize=0;
}

const QVector<quint32>& ARpcDBDriverFixedBlocks::sizes()
{
	return blockNotesSizes;
}

bool ARpcDBDriverFixedBlocks::readHeader()
{
	if(!file->seek(0))return false;
	QCryptographicHash hash(QCryptographicHash::Sha1);
	quint32 bCount=0;
	if(file->read((char*)&bCount,4)!=4)return false;
	hash.addData((const char*)&bCount,4);
	blockNotesSizes.resize(bCount);
	blockSize=0;
	for(quint32 i=0;i<bCount;++i)
	{
		quint32 sz=0;
		if(file->read((char*)&sz,4)!=4)return false;
		hash.addData((const char*)&sz,4);
		blockNotesSizes[i]=sz;
		blockSize+=sz;
	}
	totalHeaderSize=4+bCount*4+20;
	QByteArray hashFromFile=file->read(20);
	if(hashFromFile.size()!=20)return false;
	if(hashFromFile!=hash.result())return false;
	if((file->size()-(totalHeaderSize))%blockSize!=0)return false;//corrupted data
	return true;
}

bool ARpcDBDriverFixedBlocks::writeHeader()
{
	if(file->pos()!=0)return false;
	if(file->size()!=0)return false;
	QCryptographicHash hash(QCryptographicHash::Sha1);
	quint32 bCount=blockNotesSizes.size();
	if(file->write((const char*)&bCount,4)!=4)return false;
	hash.addData((const char*)&bCount,4);
	for(quint32 i=0;i<bCount;++i)
	{
		if(file->write((const char*)&blockNotesSizes[i],4)!=4)return false;
		hash.addData((const char*)&blockNotesSizes[i],4);
	}
	QByteArray hashResult=hash.result();
	if(hashResult.size()!=20)return false;//o_O not sha-1?
	file->write(hashResult);
	return true;
}
