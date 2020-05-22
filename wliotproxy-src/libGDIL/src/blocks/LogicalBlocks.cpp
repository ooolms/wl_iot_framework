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

#include "GDIL/blocks/LogicalBlocks.h"
#include "GDIL/core/CoreBlocksGroupFactory.h"

using namespace WLIOT;
using namespace WLIOTGDIL;

const QString NotBoolBlock::mBlockName=QString("not");
const QString AndBoolBlock::mBlockName=QString("and");
const QString OrBoolBlock::mBlockName=QString("or");
const QString XorBoolBlock::mBlockName=QString("xor");
const QString AndNotBoolBlock::mBlockName=QString("and_not");
const QString OrNotBoolBlock::mBlockName=QString("or_not");
const QString XorNotBoolBlock::mBlockName=QString("xor_not");

NotBoolBlock::NotBoolBlock(quint32 bId)
	:BaseBlock(bId)
{
	in=mkInput(TypeConstraints(DataUnit::BOOL,1),DataUnit::BOOL,"in");
	out=mkOutput(DataUnit::BOOL,1,"out");
}

QString NotBoolBlock::groupName()const
{
	return CoreBlocksGroupFactory::mGroupName;
}

QString NotBoolBlock::blockName()const
{
	return mBlockName;
}

void NotBoolBlock::eval()
{
	const SensorValueU8 *vIn=(const SensorValueU8*)in->data().value();
	bool bv=vIn->getT(0)==1;
	QScopedPointer<SensorValueU8> vOut((SensorValueU8*)vIn->mkCopy());
	vOut.data()->setT(0,(!bv)?1:0);
	out->setData(DataUnit(vOut.data()));
}

Base2InputsBoolBlock::Base2InputsBoolBlock(quint32 bId)
	:BaseBlock(bId)
{
	in1=mkInput(TypeConstraints(DataUnit::BOOL,1),DataUnit::BOOL,"in 1");
	in2=mkInput(TypeConstraints(DataUnit::BOOL,1),DataUnit::BOOL,"in 2");
	out=mkOutput(DataUnit::BOOL,1,"out");
}


void Base2InputsBoolBlock::eval()
{
	const SensorValueU8 *v1=(const SensorValueU8*)in1->data().value();
	const SensorValueU8 *v2=(const SensorValueU8*)in2->data().value();
	QScopedPointer<SensorValueU8> v((SensorValueU8*)v2->mkCopy());
	v.data()->setT(0,calc(v1->getT(0),v2->getT(0)));
	out->setData(DataUnit(v.data()));
}

AndBoolBlock::AndBoolBlock(quint32 bId)
	:Base2InputsBoolBlock(bId)
{
}

QString AndBoolBlock::groupName()const
{
	return CoreBlocksGroupFactory::mGroupName;
}

QString AndBoolBlock::blockName()const
{
	return mBlockName;
}


quint8 AndBoolBlock::calc(quint8 v1,quint8 v2)
{
	return (v1&&v2)?1:0;
}

OrBoolBlock::OrBoolBlock(quint32 bId)
	:Base2InputsBoolBlock(bId)
{
}

QString OrBoolBlock::groupName()const
{
	return CoreBlocksGroupFactory::mGroupName;
}

QString OrBoolBlock::blockName()const
{
	return mBlockName;
}

quint8 OrBoolBlock::calc(quint8 v1,quint8 v2)
{
	return (v1||v2)?1:0;
}

XorBoolBlock::XorBoolBlock(quint32 bId)
	:Base2InputsBoolBlock(bId)
{
}

QString XorBoolBlock::groupName()const
{
	return CoreBlocksGroupFactory::mGroupName;
}

QString XorBoolBlock::blockName()const
{
	return mBlockName;
}

quint8 XorBoolBlock::calc(quint8 v1,quint8 v2)
{
	return (v1^v2)?1:0;
}

AndNotBoolBlock::AndNotBoolBlock(quint32 bId)
	:Base2InputsBoolBlock(bId)
{
}

QString AndNotBoolBlock::groupName()const
{
	return CoreBlocksGroupFactory::mGroupName;
}

QString AndNotBoolBlock::blockName()const
{
	return mBlockName;
}

quint8 AndNotBoolBlock::calc(quint8 v1,quint8 v2)
{
	return (v1&&v2)?0:1;
}

OrNotBoolBlock::OrNotBoolBlock(quint32 bId)
	:Base2InputsBoolBlock(bId)
{
}

QString OrNotBoolBlock::groupName()const
{
	return CoreBlocksGroupFactory::mGroupName;
}

QString OrNotBoolBlock::blockName()const
{
	return mBlockName;
}

quint8 OrNotBoolBlock::calc(quint8 v1,quint8 v2)
{
	return (v1||v2)?0:1;
}

XorNotBoolBlock::XorNotBoolBlock(quint32 bId)
	:Base2InputsBoolBlock(bId)
{
}

QString XorNotBoolBlock::groupName()const
{
	return CoreBlocksGroupFactory::mGroupName;
}

QString XorNotBoolBlock::blockName()const
{
	return mBlockName;
}

quint8 XorNotBoolBlock::calc(quint8 v1,quint8 v2)
{
	return (v1^v2)?0:1;
}
