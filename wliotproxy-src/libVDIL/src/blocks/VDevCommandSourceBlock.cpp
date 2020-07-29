/*******************************************
Copyright 2017 OOO "LMS"

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

	http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#include "VDIL/blocks/VDevCommandSourceBlock.h"
#include "VDIL/core/Program.h"
#include "VDIL/core/ProgramVirtualDevice.h"
#include "VDIL/core/CoreBlocksGroupFactory.h"
#include <stdlib.h>
#include <limits>

using namespace WLIOT;
using namespace WLIOTVDIL;

const QString VDevCommandSourceBlock::mBlockName=QString("vdev_command_source");

VDevCommandSourceBlock::VDevCommandSourceBlock(quint32 bId)
	:SourceBlock(bId)
{
	triggerOut=mkOutput(TypeAndDim(DataUnit::BOOL,1),"trigger out");
}

QString VDevCommandSourceBlock::groupName()const
{
	return CoreBlocksGroupFactory::mGroupName;
}

QString VDevCommandSourceBlock::blockName()const
{
	return mBlockName;
}

void VDevCommandSourceBlock::setCommand(const QByteArray &cmd)
{
	for(BlockOutput *o:argsOuts)
		rmOutput(o);
	argsOuts.clear();
	mCommand=cmd;
	if(mCommand.isEmpty())return;
	if(!prg)return;
	if(!prg->vdev()->commandsMap().contains(mCommand))return;
	const ControlsCommand &c=prg->vdev()->commandsMap()[mCommand];
	for(int i=0;i<c.params.count();++i)
		argsOuts.append(mkOutput(TypeAndDim(DataUnit::SINGLE,1),QString::fromUtf8(c.params[i].title)));
}

QByteArray VDevCommandSourceBlock::command()const
{
	return mCommand;
}

DataUnit VDevCommandSourceBlock::extractDataInternal()
{
	QVector<double> argsArr;
	if(!prg->vdev()->commandsMap().contains(mCommand))
		return DataUnit();
	const ControlsCommand &c=prg->vdev()->commandsMap()[mCommand];
	const auto &map=prg->vdev()->state().commandParams[mCommand];
	for(int i=0;i<c.params.count();++i)
		argsArr.append(map[(quint32)i].toDouble());
	return DataUnit(argsArr);
}

void VDevCommandSourceBlock::evalInternal(const DataUnit &data,bool triggerActivated)
{
	if(data.type()!=DataUnit::SINGLE)return;
	if(!prg->vdev()->commandsMap().contains(mCommand))
		return;
	if((quint32)argsOuts.count()!=data.dim())return;
	triggerOut->setData(DataUnit(triggerActivated));
	for(int i=0;i<argsOuts.count();++i)
		argsOuts[i]->setData(DataUnit(data.value()->valueToDouble((quint32)i)));
}
