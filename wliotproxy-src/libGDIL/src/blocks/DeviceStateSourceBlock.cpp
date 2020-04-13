#include "GDIL/blocks/DeviceStateSourceBlock.h"
#include "GDIL/core/CoreBlocksGroupFactory.h"
#include "GDIL/core/Program.h"

const QString DeviceStateSourceBlock::mBlockName=QString("device_state_source");

QString DeviceStateSourceBlock::groupName()const
{
	return CoreBlocksGroupFactory::mGroupName;
}

QString DeviceStateSourceBlock::blockName()const
{
	return mBlockName;
}

DataUnit DeviceStateSourceBlock::extractDataInternal()
{
	if(!prg||!prg->helper())
		return DataUnit(DataUnit::INVALID,1);
	RealDevice *d=prg->helper()->devById(mDevId);
	if(!d)
		return DataUnit(DataUnit::INVALID,1);
	DeviceState s;
	d->getState(s);
	QByteArray val;
	if(mCmdState)
		val=s.commandParams.value(mStateKey).value(mCmdStateIndex);
	else val=s.additionalAttributes.value(mStateKey);
	if(val.isEmpty())
		return DataUnit(DataUnit::INVALID,1);
	if(mBoolOut)
	{
		if(val=="0")
			return DataUnit(false);
		else if(val=="1")
			return DataUnit(true);
		else return DataUnit(DataUnit::INVALID,1);
	}
	else
	{
		bool ok=false;
		qint64 valS64=val.toLongLong(&ok);
		if(ok)
			return DataUnit(valS64);
		double valF64=val.toDouble(&ok);
		if(ok)
			return DataUnit(valF64);
		return DataUnit(DataUnit::INVALID,1);
	}
}
