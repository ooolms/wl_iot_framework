#ifndef PROGRAM_H
#define PROGRAM_H

#include "wliot/storages/StorageId.h"
#include "GIDL/core/BaseBlock.h"
#include "GIDL/core/SourceBlock.h"
#include <QUuid>
#include <QList>

class Program
{
public:
	Program();
	bool eval();

private:
	QList<QUuid> deviceTriggers;
	QList<StorageId> sensorTriggers;
	QList<SourceBlock*> sourceBlocks;
	QList<BaseBlock*> processingBlocks;
};

#endif // PROGRAM_H
