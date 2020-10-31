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

#include "DBDriverGTimeIndex.h"

static const QVector<quint32> sizes=QVector<quint32>()<<sizeof(qint64)<<sizeof(quint64);

using namespace WLIOT;

DBDriverGTimeIndex::DBDriverGTimeIndex()
{
	opened=false;
	dbDriver=new DBDriverFixedBlocks;
}

DBDriverGTimeIndex::~DBDriverGTimeIndex()
{
	close();
	delete dbDriver;
}

bool DBDriverGTimeIndex::create(const QString &filePath)
{
	return dbDriver->create(filePath,sizes);
}

bool DBDriverGTimeIndex::open(const QString &filePath)
{
	if(!dbDriver->open(filePath))return false;
	loadIndex();
	return true;
}

bool DBDriverGTimeIndex::isOpened()
{
	return dbDriver->isOpened();
}

void DBDriverGTimeIndex::close()
{
	if(dbDriver->isOpened())
	{
		dbDriver->close();
		times.clear();
		dbIndexes.clear();
	}
}

bool DBDriverGTimeIndex::append(qint64 tm,quint64 dbInd)
{
	if(!dbDriver->addBlock())return false;
	if(!dbDriver->writeNote(0,&tm))return false;
	if(!dbDriver->writeNote(1,&dbInd))return false;
	times.append(tm);
	dbIndexes.append(dbInd);
	return true;
}

quint64 DBDriverGTimeIndex::findIndex(qint64 ts)
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

void DBDriverGTimeIndex::loadIndex()
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
