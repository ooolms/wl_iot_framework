#include "GIDL/core/Program.h"

Program::Program()
{
}

bool Program::eval()
{
	for(int i=0;i<sourceBlocks.count();++i)
		if(!sourceBlocks[i]->prepareData())
			return false;
	for(int i=0;i<sourceBlocks.count();++i)
		sourceBlocks[i]->evalIfReady();
	return true;
}
