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

#include "VDIL/blocks/VDevSensorSendBlock.h"
#include "VDIL/core/Program.h"
#include "VDIL/core/ProgramVirtualDevice.h"
#include "VDIL/core/CoreBlocksGroupFactory.h"

using namespace WLIOT;
using namespace WLIOTVDIL;

const QString VDevSensorSendBlock::mBlockName=QString("vdev_sensor");

VDevSensorSendBlock::VDevSensorSendBlock(quint32 bId)
	:BaseBlock(bId)
{
	in=0;
}

QString VDevSensorSendBlock::groupName()const
{
	return CoreBlocksGroupFactory::mGroupName;
}

QString VDevSensorSendBlock::blockName()const
{
	return mBlockName;
}

void VDevSensorSendBlock::setSensorName(const QByteArray &sensName)
{
	if(in)
	{
		rmInput(in);
		in=0;
	}
	mSensorName=sensName;
	if(!prg)return;
	SensorDef sens=prg->vdev()->sensorByName(mSensorName);
	if(!sens.type.isValid())return;
	DataUnit::Type t;
	if(sens.type.packType==SensorDef::PACKET)
		t=DataUnit::ARRAY;
	else t=DataUnit::SINGLE;
	quint32 dim=sens.type.dim;
	in=mkInput(TypeConstraints(t,dim),t,"in");
}

QByteArray VDevSensorSendBlock::sensorName()const
{
	return mSensorName;
}

void VDevSensorSendBlock::updateInput()
{
	if(!in)return;
	SensorDef sens=prg->vdev()->sensorByName(mSensorName);
	if(!sens.type.isValid())return;
	DataUnit::Type t;
	if(sens.type.packType==SensorDef::PACKET)
		t=DataUnit::ARRAY;
	else t=DataUnit::SINGLE;
	quint32 dim=sens.type.dim;
	if(in->type()!=t||in->dim()!=dim)
	{
		rmInput(in);
		in=mkInput(TypeConstraints(t,dim),t,"in");
	}
}

void VDevSensorSendBlock::eval()
{
	SensorDef sens=prg->vdev()->sensorByName(mSensorName);
	if(!sens.type.isValid())return;
	DataUnit u=in->data();
	if(sens.type!=u.value()->type())return;
	QByteArray binData=u.value()->dumpToBinary();
	if(engineCallbacks())
		engineCallbacks()->sendVDevMeasurementB(mSensorName,binData);
}
