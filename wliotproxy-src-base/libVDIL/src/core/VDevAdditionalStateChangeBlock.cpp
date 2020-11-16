#include "VDIL/core/VDevAdditionalStateChangeBlock.h"
#include "VDIL/core/Program.h"
#include "VDIL/core/ProgramVirtualDevice.h"

using namespace  WLIOTVDIL;

const QString VDevAdditionalStateChangeBlock::mBlockName="vdev_state_change";

VDevAdditionalStateChangeBlock::VDevAdditionalStateChangeBlock(quint32 bId)
	:BaseBlock(bId)
{
	in=0;
}

QString VDevAdditionalStateChangeBlock::groupName()const
{
	return Program::reservedCoreGroupName;
}

QString VDevAdditionalStateChangeBlock::blockName()const
{
	return mBlockName;
}

void VDevAdditionalStateChangeBlock::setStateParam(const QByteArray &paramName)
{
	mParamName=paramName;
	if(!prg)return;
	if(in&&!prg->vdev()->startupState().additionalAttributes.contains(mParamName))
	{
		rmInput(in);
		in=0;
	}
	else if(!in&&prg->vdev()->startupState().additionalAttributes.contains(mParamName))
		in=mkInput(TypeConstraints(DataUnit::SINGLE|DataUnit::BOOL,1),DataUnit::SINGLE,"in");
}

QByteArray VDevAdditionalStateChangeBlock::stateParam()const
{
	return mParamName;
}

void VDevAdditionalStateChangeBlock::eval()
{
	if(!in)return;
	QByteArray data=in->data().value()->valueToString(0);
	if(engineCallbacks())
		engineCallbacks()->vdevAdditionalStateChanged(mParamName,data);
}
