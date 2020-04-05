#include "GDIL/core/Program.h"
#include <QMutexLocker>

Program::Program()
{
	hlp=0;
	cmdCb=0;
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

void Program::setCommandCallback(ICommandCallback *c)
{
	cmdCb=c;
}

ICommandCallback* Program::commandCallback()
{
	return cmdCb;
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

bool Program::addSourceBlock(SourceBlock *b)
{
	if(b->mBlockId!=0)
	{
		if(mSourceBlocks.contains(b->mBlockId)||mProcessingBlocks.contains(b->mBlockId))
			return false;
		maxBlockId=qMax(maxBlockId,b->mBlockId);
	}
	else b->mBlockId=++maxBlockId;
	mSourceBlocks[b->mBlockId]=b;
	return true;
}

bool Program::addProcessingBlock(BaseBlock *b)
{
	if(b->mBlockId!=0)
	{
		if(mSourceBlocks.contains(b->mBlockId)||mProcessingBlocks.contains(b->mBlockId))
			return false;
		maxBlockId=qMax(maxBlockId,b->mBlockId);
	}
	else b->mBlockId=++maxBlockId;
	mProcessingBlocks[b->mBlockId]=b;
	return true;
}

void Program::rmBlock(quint32 bId)
{
	if(mSourceBlocks.contains(bId))
		delete mSourceBlocks.take(bId);
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
