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
#include "VDIL/blocks/StorageSourceBlock.h"
#include "VDIL/core/CoreBlocksGroupFactory.h"
#include "VDIL/core/ProgramEvalTimers.h"
#include "VDIL/core/ProgramRuntimeVars.h"
#include "VDIL/core/ProgramVirtualDevice.h"
#include "VDIL/core/ProgramDevicesBridge.h"
#include "VDIL/core/ProgramStoragesBridge.h"
#include "VDIL/core/SubProgramBlock.h"
#include <QMutexLocker>
#include <QCoreApplication>

using namespace WLIOT;
using namespace WLIOTVDIL;

Program::Program()
{
	hlp=0;
	mCb=0;
	maxBlockId=0;
	prg=this;
	mRuntimeVars=new ProgramRuntimeVars(this);
	mVDev=new ProgramVirtualDevice(this);
	mDevBridge=new ProgramDevicesBridge(this);
	mStorBridge=new ProgramStoragesBridge(this);
}

Program::~Program()
{
	for(auto i=mAllBlocks.begin();i!=mAllBlocks.end();++i)
		delete i.value();
	delete mVDev;
	delete mRuntimeVars;
	delete mDevBridge;
	delete mStorBridge;
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
	if(b->groupName()==CoreBlocksGroupFactory::mGroupName)
	{
		if(b->blockName()==TimerBlock::mBlockName)
			mTimerBlocks[b->mBlockId]=(TimerBlock*)b;
		else if(b->blockName()==SubProgramBlock::mBlockName)
			mSubProgramBlocks[b->blockId()]=(SubProgramBlock*)b;
	}
	if(b->isSourceBlock())
		mSourceBlocks[b->mBlockId]=(SourceBlock*)b;
	return true;
}

void Program::rmBlockFromSubProgram(quint32 bId)
{
	BaseBlock *b=mAllBlocks.value(bId);
	if(!b)return;
	if(b->groupName()==CoreBlocksGroupFactory::mGroupName)
	{
		if(b->blockName()==TimerBlock::mBlockName)
			mTimerBlocks.remove(bId);
		else if(b->blockName()==SubProgramBlock::mBlockName)
			mSubProgramBlocks[b->blockId()]=(SubProgramBlock*)b;
	}
	if(b->isSourceBlock())
		mSourceBlocks.remove(bId);
	mAllBlocks.remove(bId);
	delete b;
}

bool Program::extractSources()
{
	QMutexLocker l(&nextDataLock);
	for(auto i=mSourceBlocks.begin();i!=mSourceBlocks.end();++i)
		if(!i.value()->extractNextData())
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
	mVDev->cleanupAfterStop();
	mDevBridge->cleanupAfterStop();
	mStorBridge->cleanupAfterStop();
}

bool Program::addBlock(BaseBlock *b)
{
	if(!addBlockFromSubProgram(b))return false;
	b->ownerSubPrg=this;
	mSelfBlocks[b->blockId()]=b;
	if(b->isSourceBlock())
		mSelfSourceBlocks[b->blockId()]=(SourceBlock*)b;
	b->onProgramIsSet();
	return true;
}

void Program::rmBlock(quint32 bId)
{
	SubProgram::rmBlock(bId);
	mSelfSourceBlocks.remove(bId);
	rmBlockFromSubProgram(bId);
}

const QMap<quint32,SourceBlock*>& Program::sourceBlocks()const
{
	return mSourceBlocks;
}

const QMap<quint32,TimerBlock*>& Program::timerBlocks()const
{
	return mTimerBlocks;
}

const QMap<quint32,SubProgramBlock*>& Program::subProgramBlocks()const
{
	return mSubProgramBlocks;
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

ProgramVirtualDevice* Program::vdev()
{
	return mVDev;
}

const ProgramVirtualDevice* Program::vdev()const
{
	return mVDev;
}

ProgramDevicesBridge *Program::devBr()
{
	return mDevBridge;
}

const ProgramDevicesBridge *Program::devBr() const
{
	return mDevBridge;
}

ProgramStoragesBridge *Program::storBr()
{
	return mStorBridge;
}

const ProgramStoragesBridge *Program::storBr() const
{
	return mStorBridge;
}

void Program::prepareToStart()
{
	mVDev->prepareToStart();
	mDevBridge->prepareToStart();
	mStorBridge->prepareToStart();
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
