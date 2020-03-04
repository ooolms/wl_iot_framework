#include "GIDL/core/DataUnit.h"

DataUnit::DataUnit(DataUnit::Type t)
{
	mType=t;
}

DataUnit::Type DataUnit::type()const
{
	return mType;
}

DataUnit::DataUnit(const DataUnit &t)
{
	mType=t.mType;
	mValue=t.mValue->mkCopy();
}
