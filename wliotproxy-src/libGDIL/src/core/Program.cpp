#include "GIDL/core/Program.h"
#include <QMutexLocker>

Program::Program()
{
}

bool Program::extractSources()
{
	QMutexLocker l(&nextDataLock);
	for(int i=0;i<sourceBlocks.count();++i)
		if(!sourceBlocks[i]->extractData())
			return false;
	return true;
}

bool Program::prepareWorkData()
{
	QMutexLocker l(&nextDataLock);
	for(int i=0;i<sourceBlocks.count();++i)
		if(!sourceBlocks[i]->prepareWorkData())
			return false;
	return true;
}

bool Program::eval()
{
	for(int i=0;i<sourceBlocks.count();++i)
		sourceBlocks[i]->evalIfReady();
	return true;
}
