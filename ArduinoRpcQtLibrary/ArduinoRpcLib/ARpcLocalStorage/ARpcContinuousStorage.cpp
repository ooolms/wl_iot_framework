#include "ARpcContinuousStorage.h"

ARpcContinuousStorage::ARpcContinuousStorage(QObject *parent)
	:ARpcISensorStorage(parent)
{
}

ARpcISensorStorage::StoreMode ARpcContinuousStorage::getStoreMode()const
{
	return CONTINUOUS;
}
