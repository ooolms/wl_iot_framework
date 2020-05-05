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
#include "GDIL/core/CoreBlocksGroupFactory.h"
#include <QMutexLocker>
#include <QCoreApplication>

Program::Program()
{
	hlp=0;
	mCb=0;
	maxBlockId=0;
}

Program::~Program()
{
	for(auto i=mAllBlocks.begin();i!=mAllBlocks.end();++i)
		delete i.value();
}

void Program::setHelper(IEngineHelper *h)
{
	hlp=h;
}

QString Program::findDevName(const QUuid &id)const
{
	return mKnownDevNames.value(id);
}

IEngineHelper* Program::helper() const
{
	return hlp;
}

void Program::setEngineCallbacks(IEngineCallbacks *c)
{
	mCb=c;
}

void Program::addEvalTimer(QTimer *t)
{
	evalTimers.insert(t);
}

void Program::rmEvalTimer(QTimer *t)
{
	evalTimers.remove(t);
}

void Program::updateExtTimersList()
{
	mExtTimerConfigs.clear();
	for(TimerBlock *b:mTimerBlocks)
		mExtTimerConfigs[b->blockId()]=b->config();
}

void Program::updateDevNames()
{
	QSet<QUuid> usedDevs;
	for(BaseBlock *b:mAllBlocks)
		usedDevs.unite(b->usedDevices().toSet());
	QSet<QUuid> knownDevs=mKnownDevNames.keys().toSet();
	QSet<QUuid> rmDevs=QSet<QUuid>(knownDevs).subtract(usedDevs);
	for(auto i=rmDevs.begin();i!=rmDevs.end();++i)
		mKnownDevNames.remove(*i);
	if(!hlp)return;
	usedDevs.subtract(knownDevs);
	for(auto i=usedDevs.begin();i!=usedDevs.end();++i)
	{
		QString name=hlp->findDevName(*i);
		if(!name.isEmpty())
			mKnownDevNames[*i]=name;
	}
}

IEngineCallbacks* Program::engineCallbacks()const
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
	while(!evalTimers.isEmpty())
		qApp->processEvents();
	return true;
}

bool Program::addBlock(BaseBlock *b)
{
	if(b->mBlockId!=0)
	{
		if(mAllBlocks.contains(b->mBlockId))
			return false;
		b->prg=this;
		maxBlockId=qMax(maxBlockId,b->mBlockId);
	}
	else b->mBlockId=++maxBlockId;
	mAllBlocks[b->mBlockId]=b;
	updateDevNames();
	if(b->groupName()==CoreBlocksGroupFactory::mGroupName&&b->blockName()==TimerBlock::mBlockName)
		mTimerBlocks[b->mBlockId]=(TimerBlock*)b;
	if(b->isSourceBlock())
	{
		mSourceBlocks[b->mBlockId]=(SourceBlock*)b;
		b->prg=this;
	}
	return true;
}

void Program::rmBlock(quint32 bId)
{
	mSourceBlocks.remove(bId);
	mTimerBlocks.remove(bId);
	if(mAllBlocks.contains(bId))
	{
		BaseBlock *b=mAllBlocks.take(bId);
		if(b->groupName()==CoreBlocksGroupFactory::mGroupName&&b->blockName()==StorageSourceBlock::mBlockName)
		{
			StorageSourceBlock *sb=(StorageSourceBlock*)b;
			mStorageTriggers.removeOne(sb->storageId());
			delete b;
			updateDevNames();
		}
	}
}

BaseBlock* Program::blockById(quint32 bId)
{
	return mAllBlocks.value(bId);
}

const QMap<quint32,SourceBlock*>& Program::sourceBlocks()const
{
	return mSourceBlocks;
}

const QMap<quint32,TimerBlock*>& Program::timerBlocks()const
{
	return mTimerBlocks;
}

const QMap<quint32,BaseBlock*>& Program::allBlocks()const
{
	return mAllBlocks;
}

void Program::setStorageTriggers(const QList<StorageId> &list)
{
	mStorageTriggers=list;
}

const QList<QUuid>& Program::deviceTriggers()const
{
	return mDeviceTriggers;
}

const QList<StorageId> &Program::storageTriggers()const
{
	return mStorageTriggers;
}

QList<StorageId> Program::allUsedStorages()const
{
	QList<StorageId> ids;
	for(auto i=mSourceBlocks.constBegin();i!=mSourceBlocks.constEnd();++i)
	{
		const SourceBlock *b=i.value();
		if(b->groupName()==CoreBlocksGroupFactory::mGroupName&&b->blockName()==StorageSourceBlock::mBlockName)
		{
			const StorageSourceBlock *sb=(StorageSourceBlock*)b;
			if(!sb->storageId().deviceId.isNull()&&!sb->storageId().sensorName.isEmpty())
				ids.append(sb->storageId());
		}
	}
	return ids;

}

QList<ConfigOptionId> Program::allConfigOptions()const
{
	return mAllConfigOptions;
}

TypeConstraints Program::configOptionConstraints(ConfigOptionId id)const
{
	BaseBlock *b=mAllBlocks.value(id.blockId);
	if(!b)return TypeConstraints();
	return b->configOptionConstraints(id.key);
}

DataUnit Program::configOptionValue(ConfigOptionId id) const
{
	BaseBlock *b=mAllBlocks.value(id.blockId);
	if(!b)return DataUnit();
	return b->configOptionValue(id.key);
}

bool Program::setConfigOptionValue(ConfigOptionId id,const DataUnit &val)
{
	BaseBlock *b=mAllBlocks.value(id.blockId);
	if(!b)return false;
	return b->setConfigOption(id.key,val);
}

void Program::calcConfigOptions()
{
	mAllConfigOptions.clear();
	for(auto i=mAllBlocks.begin();i!=mAllBlocks.end();++i)
	{
		quint32 bId=i.value()->blockId();
		for(auto &j:i.value()->configOptions())
			mAllConfigOptions.append({bId,j});
	}
}

bool ConfigOptionId::operator<(const ConfigOptionId &t)const
{
	if(blockId==t.blockId)
		return key<t.key;
	return blockId<t.blockId;
}
