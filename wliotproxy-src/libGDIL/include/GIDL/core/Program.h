#ifndef PROGRAM_H
#define PROGRAM_H

#include "wliot/storages/StorageId.h"
#include <QUuid>
#include <QList>

class Program
{
public:
	Program();

private:
	QList<QUuid> deviceTriggers;
	QList<StorageId> sensorTriggers;
};

#endif // PROGRAM_H
