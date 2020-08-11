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

#include "VDIL/core/BaseBlock.h"
#include "VDIL/core/Program.h"
#include "VDIL/core/ProgramEvalTimers.h"

using namespace WLIOT;
using namespace WLIOTVDIL;

BaseBlock::BaseBlock(quint32 bId)
{
	prg=0;
	mBlockId=bId;
}

BaseBlock::~BaseBlock()
{
	for(BlockInput *i:mInputs)
		delete i;
	for(BlockOutput *o:mOutputs)
		delete o;
	if(prg&&evalTimer)
	{
		prg->evalTimers()->rmEvalTimer(evalTimer.data());
		delete evalTimer.data();
	}
	if(prg&&!mConfigOptions.isEmpty())
	{
		mConfigOptions.clear();
		prg->calcConfigOptions();
	}
}

bool BaseBlock::isSourceBlock()const
{
	return false;
}

QByteArrayList BaseBlock::configOptions()const
{
	return mConfigOptions.keys();
}

bool BaseBlock::setConfigOption(const QByteArray &key,const DataUnit &value)
{
	if(!mConfigOptions.contains(key))return false;
	DataUnit &u=mConfigOptions[key];
	if(u.type()!=value.type())return false;
	u=value;
	onConfigOptionChanged(key);
	return true;
}

DataUnit BaseBlock::configOptionValue(const QByteArray &key)
{
	return mConfigOptions.value(key);
}

Program* BaseBlock::program()
{
	return prg;
}

const Program* BaseBlock::program()const
{
	return prg;
}

SubProgram* BaseBlock::ownerSubProgram()
{
	return ownerSubPrg;
}

const SubProgram* BaseBlock::ownerSubProgram()const
{
	return ownerSubPrg;
}

int BaseBlock::inputsCount()const
{
	return mInputs.count();
}

BlockInput* BaseBlock::input(int index)
{
	return mInputs.value(index);
}

int BaseBlock::outputsCount()const
{
	return mOutputs.count();
}

BlockOutput* BaseBlock::output(int index)
{
	return mOutputs.value(index);
}

int BaseBlock::outputIndex(const BlockOutput *out)const
{
	return mOutputs.indexOf(const_cast<BlockOutput*>(out));
}

quint32 BaseBlock::blockId()const
{
	return mBlockId;
}

int BaseBlock::inputIndex(const BlockInput *in)const
{
	return mInputs.indexOf(const_cast<BlockInput*>(in));
}

void BaseBlock::evalIfReady()
{
	bool hasEmptyInput=false;
	for(BlockInput *i:mInputs)
	{
		if(!i->isReady())
		{
			hasEmptyInput=true;
			break;
		}
	}
	if(!hasEmptyInput)
		eval();
}

void BaseBlock::cleanupAfterEval()
{
	cleanupInputs();
	cleanupAfterEvalInternal();
}

void BaseBlock::cleanupAfterEvalInternal()
{
}

void BaseBlock::cleanupInputs()
{
	for(BlockInput *i:mInputs)
		i->reset();
}

void BaseBlock::evalOnTimer()
{
}

void BaseBlock::onInputTypeSelected(BlockInput *b)
{
	Q_UNUSED(b)
}

void BaseBlock::onProgramIsSet()
{
}

void BaseBlock::onConfigOptionChanged(const QByteArray &key)
{
	Q_UNUSED(key)
}

BlockInput* BaseBlock::mkInput(TypeConstraints suppTypes,const TypeAndDim &currType,const QString &title,int index)
{
	BlockInput *in=new BlockInput(this,suppTypes,currType,title);
	if(index==-1)
		mInputs.append(in);
	else mInputs.insert(index,in);
	return in;
}

BlockInput *BaseBlock::mkInput(TypeConstraints suppTypes,DataUnit::Type currType,const QString &title,int index)
{
	quint32 dim=1;
	if(suppTypes.dim!=0)
		dim=suppTypes.dim;
	BlockInput *in=new BlockInput(this,suppTypes,TypeAndDim(currType,dim),title);
	if(index==-1)
		mInputs.append(in);
	else mInputs.insert(index,in);
	return in;
}

BlockInput *BaseBlock::mkInput(const TypeAndDim &fixedType,const QString &title,int index)
{
	BlockInput *in=new BlockInput(this,fixedType,title);
	if(index==-1)
		mInputs.append(in);
	else mInputs.insert(index,in);
	return in;
}

BlockOutput *BaseBlock::mkOutput(const TypeAndDim &type,const QString &title,int index)
{
	BlockOutput *out=new BlockOutput(this,type,title);
	if(index==-1)
		mOutputs.append(out);
	else mOutputs.insert(index,out);
	return out;
}

void BaseBlock::rmInput(int index)
{
	if(index<0||index>=mInputs.count())
		return;
	delete mInputs.takeAt(index);
}

void BaseBlock::rmInput(BlockInput *in)
{
	rmInput(mInputs.indexOf(in));
}

void BaseBlock::rmOutput(int index)
{
	if(index<0||index>=mOutputs.count())
		return;
	delete mOutputs.takeAt(index);
}

void BaseBlock::rmOutput(BlockOutput *out)
{
	rmOutput(mOutputs.indexOf(out));
}

void BaseBlock::evalOnTimerInMsec(quint32 msec)
{
	if(evalTimer)
	{
		ownerSubPrg->evalTimers()->rmEvalTimer(evalTimer.data());
		delete evalTimer.data();
	}
	evalTimer=new QTimer;
	evalTimer->setSingleShot(true);
	QObject::connect(evalTimer.data(),&QTimer::timeout,[this](){onTimer();});
	evalTimer->start(msec);
	ownerSubPrg->evalTimers()->addEvalTimer(evalTimer.data());
}

void BaseBlock::addConfigOption(const QByteArray &key,const DataUnit &defaultValue)
{
	mConfigOptions[key]=defaultValue;
	if(prg)
		prg->calcConfigOptions();
}

void BaseBlock::rmConfigOption(const QByteArray &key)
{
	mConfigOptions.remove(key);
	if(prg)
		prg->calcConfigOptions();
}

IEngineHelper* BaseBlock::helper()const
{
	if(!prg)return 0;
	return prg->helper();
}

IEngineCallbacks* BaseBlock::engineCallbacks()const
{
	if(!prg)return 0;
	return prg->engineCallbacks();
}

QList<QUuid> BaseBlock::usedDevices()const
{
	return QList<QUuid>();
}

void BaseBlock::onTimer()
{
	prg->evalTimers()->rmEvalTimer(evalTimer.data());
	delete evalTimer.data();
	evalOnTimer();
}
