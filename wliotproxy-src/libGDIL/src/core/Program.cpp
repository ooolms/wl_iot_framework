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

#include "GDIL/core/Program.h"
#include "GDIL/blocks/StorageSourceBlock.h"
#include <QMutexLocker>

Program::Program()
{
	hlp=0;
	mCb=0;
	maxBlockId=0;
}

Program::~Program()
{
	for(auto i=mSourceBlocks.begin();i!=mSourceBlocks.end();++i)
		delete i.value();
	for(auto i=mProcessingBlocks.begin();i!=mProcessingBlocks.end();++i)
		delete i.value();
}

void Program::setHelper(IEngineHelper *h)
{
	hlp=h;
}

IEngineHelper* Program::helper()
{
	return hlp;
}

void Program::setEngineCallbacks(IEngineCallbacks *c)
{
	mCb=c;
}

IEngineCallbacks* Program::engineCallbacks()
{
	return mCb;
}

bool Program::extractSources()
{
	QMutexLocker l(&nextDataLock);
	for(auto i=mSourceBlocks.begin();i!=mSourceBlocks.end();++i)
		if(!i.value()->extractData())
			return false;
	return true;
}

bool Program::prepareWorkData()
{
	QMutexLocker l(&nextDataLock);
	for(auto i=mSourceBlocks.begin();i!=mSourceBlocks.end();++i)
		if(!i.value()->prepareWorkData())
			return false;
	return true;
}

bool Program::eval()
{
	for(auto i=mSourceBlocks.begin();i!=mSourceBlocks.end();++i)
		i.value()->evalIfReady();
	return true;
}

bool Program::addBlock(BaseBlock *b)
{
	if(b->mBlockId!=0)
	{
		if(mSourceBlocks.contains(b->mBlockId)||mProcessingBlocks.contains(b->mBlockId))
			return false;
		b->prg=this;
		maxBlockId=qMax(maxBlockId,b->mBlockId);
	}
	else b->mBlockId=++maxBlockId;
	if(b->isSourceBlock())
	{
		mSourceBlocks[b->mBlockId]=(SourceBlock*)b;
		b->prg=this;
		calcTriggers();
	}
	else mProcessingBlocks[b->mBlockId]=b;
	return true;
}

void Program::rmBlock(quint32 bId)
{
	if(mSourceBlocks.contains(bId))
	{
		delete mSourceBlocks.take(bId);
		calcTriggers();
	}
	else if(mProcessingBlocks.contains(bId))
		delete mProcessingBlocks.take(bId);
}

BaseBlock* Program::blockById(quint32 bId)
{
	if(mSourceBlocks.contains(bId))
		return mSourceBlocks.value(bId);
	return mProcessingBlocks.value(bId);
}

const QMap<quint32,SourceBlock*>& Program::sourceBlocks()const
{
	return mSourceBlocks;
}

const QMap<quint32,BaseBlock*>& Program::processingBlocks()const
{
	return mProcessingBlocks;
}

const QList<QUuid>& Program::deviceTriggers()
{
	return mDeviceTriggers;
}

const QList<StorageId>& Program::storageTriggers()
{
	return mStorageTriggers;
}

void Program::calcTriggers()
{
	mDeviceTriggers.clear();
	mStorageTriggers.clear();
	for(auto i=mSourceBlocks.begin();i!=mSourceBlocks.end();++i)
	{
		SourceBlock *b=i.value();
		if(typeid(*b)==typeid(StorageSourceBlock))
		{
			StorageSourceBlock *sb=(StorageSourceBlock*)b;
			if(!sb->storageId().deviceId.isNull()&&!sb->storageId().sensorName.isEmpty())
				mStorageTriggers.append(sb->storageId());
		}
	}
}
