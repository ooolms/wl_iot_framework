#include "ARpcDBDriverGTimeIndex.h"

static const QVector<quint32> sizes=QVector<quint32>()<<sizeof(qint64)<<sizeof(quint64);

ARpcDBDriverGTimeIndex::ARpcDBDriverGTimeIndex(QObject *parent)
	:QObject(parent)
{
	opened=false;
	dbDriver=new ARpcDBDriverFixedBlocks(this);
}

ARpcDBDriverGTimeIndex::~ARpcDBDriverGTimeIndex()
{
	close();
}

bool ARpcDBDriverGTimeIndex::create(const QString &filePath)
{
	return dbDriver->create(filePath,sizes);
}

bool ARpcDBDriverGTimeIndex::open(const QString &filePath)
{
	if(!dbDriver->open(filePath))return false;
	loadIndex();
	return true;
}

bool ARpcDBDriverGTimeIndex::isOpened()
{
	return dbDriver->isOpened();
}

void ARpcDBDriverGTimeIndex::close()
{
	if(dbDriver->isOpened())
	{
		dbDriver->close();
		times.clear();
		dbIndexes.clear();
	}
}

bool ARpcDBDriverGTimeIndex::append(qint64 tm,quint64 dbInd)
{
	if(!dbDriver->addBlock())return false;
	if(!dbDriver->writeNote(0,&tm))return false;
	if(!dbDriver->writeNote(1,&dbInd))return false;
	times.append(tm);
	dbIndexes.append(dbInd);
	return true;
}

quint64 ARpcDBDriverGTimeIndex::findIndex(qint64 ts)
{
	if(dbIndexes.size()<=1)return dbIndexes[0];
	quint64 sIndex=0,eIndex=dbIndexes.size()-1;
	while(true)
	{
		if(eIndex-sIndex==1)
		{
			if(times[eIndex]==ts)return eIndex;
			else return sIndex;
		}
		quint64 mIndex=sIndex+((eIndex-sIndex)>>1);
		if(times[mIndex]==ts)return mIndex;
		else if(times[mIndex]<ts)sIndex=mIndex;
		else eIndex=mIndex;
	}
}

void ARpcDBDriverGTimeIndex::loadIndex()
{
	times.clear();
	dbIndexes.clear();
	qint64 t=0;
	quint64 v=0;
	for(quint64 i=0;i<dbDriver->blocksCount();++i)
	{
		dbDriver->readNote(i,0,&t);
		dbDriver->readNote(i,1,&v);
		times.append(t);
		dbIndexes.append(v);
	}
}
