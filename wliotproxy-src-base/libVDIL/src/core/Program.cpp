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

#include "VDIL/core/Program.h"
#include "VDIL/core/ProgramEvalTimers.h"
#include "VDIL/core/ProgramRuntimeVars.h"
#include "VDIL/core/ProgramVirtualDevice.h"
#include "VDIL/core/IProgramRuntime.h"
#include "VDIL/core/SubProgramBlock.h"
#include <QMutexLocker>
#include <QCoreApplication>

using namespace WLIOT;
using namespace WLIOTVDIL;

const QString Program::reservedCoreGroupName=QString("core");

Program::Program()
{
	hlp=0;
	mCb=0;
	maxBlockId=0;
	prg=this;
	mRuntimeVars=new ProgramRuntimeVars(this);
	mVDev=new ProgramVirtualDeviceRuntimeInstance(this);
	mRuntimes.append(mRuntimeVars);
	mRuntimes.append(mVDev);
}

Program::~Program()
{
	for(auto i=mAllBlocks.begin();i!=mAllBlocks.end();++i)
		delete i.value();
	for(auto r:mRuntimes)
		delete r;
}

void Program::setHelper(IEngineHelper *h)
{
	hlp=h;
}

IEngineHelper* Program::helper()const
{
	return hlp;
}

void Program::setEngineCallbacks(IEngineCallbacks *c)
{
	mCb=c;
}

IEngineCallbacks* Program::engineCallbacks()const
{
	return mCb;
}

bool Program::addBlockFromSubProgram(BaseBlock *b)
{
	if(b->mBlockId!=0)
	{
		if(mAllBlocks.contains(b->mBlockId))
			return false;
		maxBlockId=qMax(maxBlockId,b->mBlockId);
	}
	else b->mBlockId=++maxBlockId;
	b->prg=this;
	mAllBlocks[b->mBlockId]=b;
	if(b->groupName()==reservedCoreGroupName)
	{
		if(b->blockName()==TimerBlock::mBlockName)
			mTimerBlocks[b->mBlockId]=(TimerBlock*)b;
		else if(b->blockName()==SubProgramBlock::mBlockName)
			mSubProgramBlocks[b->blockId()]=(SubProgramBlock*)b;
	}
	if(b->isSourceBlock())
		mAllSourceBlocks[b->mBlockId]=(SourceBlock*)b;
	return true;
}

void Program::rmBlockFromSubProgram(quint32 bId)
{
	BaseBlock *b=mAllBlocks.value(bId);
	if(!b)return;
	if(b->groupName()==reservedCoreGroupName)
	{
		if(b->blockName()==TimerBlock::mBlockName)
			mTimerBlocks.remove(bId);
		else if(b->blockName()==SubProgramBlock::mBlockName)
			mSubProgramBlocks[b->blockId()]=(SubProgramBlock*)b;
	}
	if(b->isSourceBlock())
		mAllSourceBlocks.remove(bId);
	mAllBlocks.remove(bId);
	delete b;
}

bool Program::extractSources()
{
	QMutexLocker l(&nextDataLock);
	for(auto i=mAllSourceBlocks.begin();i!=mAllSourceBlocks.end();++i)
		if(!i.value()->extractNextData())
			return false;
	return true;
}

bool Program::prepareWorkData()
{
	QMutexLocker l(&nextDataLock);
	for(auto i=mAllSourceBlocks.begin();i!=mAllSourceBlocks.end();++i)
		if(!i.value()->prepareWorkData())
			return false;
	return true;
}

bool Program::eval()
{
	for(auto i=mSelfSourceBlocks.begin();i!=mSelfSourceBlocks.end();++i)
		i.value()->evalIfReady();
	mEvalTimers->waitForTimers();
	return true;
}

void Program::cleanupAfterEval()
{
	for(BaseBlock *b:mAllBlocks)
		b->cleanupAfterEval();
}

void Program::cleanupAfterStop()
{
	for(auto r:mRuntimes)
		r->cleanupAfterStop();
	cleanupSubProgramAfterStop();
	for(SubProgramBlock *b:mSubProgramBlocks)
		b->subProgram()->cleanupSubProgramAfterStop();
}

const QMap<quint32,SourceBlock*>& Program::sourceBlocks()const
{
	return mAllSourceBlocks;
}

const QMap<quint32,TimerBlock*>& Program::timerBlocks()const
{
	return mTimerBlocks;
}

const QMap<quint32,SubProgramBlock*>& Program::subProgramBlocks()const
{
	return mSubProgramBlocks;
}

QMap<quint32,BaseBlock*> Program::selectBlocks(const QString &groupName,const QString &blockName)const
{
	QMap<quint32,BaseBlock*> retVal;
	for(auto b:mAllBlocks)
		if(b->groupName()==groupName&&b->blockName()==blockName)
			retVal[b->blockId()]=b;
	return retVal;
}

const QMap<quint32,BaseBlock*>& Program::allBlocks()const
{
	return mAllBlocks;
}

//QList<StorageId> Program::allUsedStorages()const
//{
//	QList<StorageId> ids;
//	for(auto i=mSourceBlocks.constBegin();i!=mSourceBlocks.constEnd();++i)
//	{
//		const SourceBlock *b=i.value();
//		if(b->groupName()==CoreBlocksGroupFactory::mGroupName&&b->blockName()==StorageSourceBlock::mBlockName)
//		{
//			const StorageSourceBlock *sb=(StorageSourceBlock*)b;
//			if(!sb->storageId().deviceId.isNull()&&!sb->storageId().sensorName.isEmpty())
//				ids.append(sb->storageId());
//		}
//	}
//	return ids;

//}

const QList<ConfigOptionId>& Program::allConfigOptions()const
{
	return mAllConfigOptions;
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

ProgramRuntimeVars* Program::runtimeVars()
{
	return mRuntimeVars;
}

const ProgramRuntimeVars* Program::runtimeVars()const
{
	return mRuntimeVars;
}

ProgramVirtualDeviceRuntimeInstance* Program::vdev()
{
	return mVDev;
}

const ProgramVirtualDeviceRuntimeInstance* Program::vdev()const
{
	return mVDev;
}

void Program::addRuntime(IProgramRuntimeInstance *r)
{
	if(r)mRuntimes.append(r);
}

const QList<IProgramRuntimeInstance*> Program::runtimes()
{
	return mRuntimes;
}

void Program::prepareToStart()
{
	for(auto r:mRuntimes)
		r->prepareToStart();
	prepareSubProgramToStart();
	for(SubProgramBlock *b:mSubProgramBlocks)
		b->subProgram()->prepareSubProgramToStart();
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

bool ConfigOptionId::operator==(const ConfigOptionId &t)const
{
	return blockId==t.blockId&&key==t.key;
}
