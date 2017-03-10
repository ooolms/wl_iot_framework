#include "ARpcContinuousStorage.h"

ARpcContinuousStorage::ARpcContinuousStorage()
{
}

ARpcISensorStorage::StoreMode ARpcContinuousStorage::getStoreMode()const
{
	return CONTINUOUS;
}
