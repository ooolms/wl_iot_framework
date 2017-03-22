#ifndef ARPCDBDRIVERHELPERS_H
#define ARPCDBDRIVERHELPERS_H

#include "ARpcDBDriverFixedBlocks.h"
#include "ARpcDBDriverChainedBlocks.h"
#include "ARpcBase/ARpcISensorValue.h"

class ARpcDBDriverHelpers
{
public:
	static bool writeSensorValueToFixedBlocksDB(
		ARpcDBDriverFixedBlocks *db,const ARpcISensorValue *val,bool replaceLocalTimeWithGlobalTime);
	static bool writeSensorValueToChainedBlocksDB(
		ARpcDBDriverChainedBlocks *db,const ARpcISensorValue *val,bool replaceLocalTimeWithGlobalTime);
};

#endif // ARPCDBDRIVERHELPERS_H
