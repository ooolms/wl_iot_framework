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

#include "GDIL/core/BaseBlock.h"
#include "GDIL/core/Program.h"

using namespace WLIOT;
using namespace WLIOTGDIL;

BaseBlock::BaseBlock(quint32 bId)
{
	prg=0;
	mBlockId=bId;
}

BaseBlock::~BaseBlock()
{
	for(BlockInput *i:inputs)
		delete i;
	for(BlockOutput *o:outputs)
		delete o;
	if(evalTimer)
	{
		prg->rmEvalTimer(evalTimer.data());
		delete evalTimer.data();
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

int BaseBlock::inputsCount()
{
	return inputs.count();
}

BlockInput* BaseBlock::input(int index)
{
	return inputs.value(index);
}

int BaseBlock::outputsCount()
{
	return outputs.count();
}

BlockOutput* BaseBlock::output(int index)
{
	return outputs.value(index);
}

int BaseBlock::outputIndex(const BlockOutput *out)const
{
	return outputs.indexOf(const_cast<BlockOutput*>(out));
}

quint32 BaseBlock::blockId()const
{
	return mBlockId;
}

int BaseBlock::inputIndex(const BlockInput *in)const
{
	return inputs.indexOf(const_cast<BlockInput*>(in));
}

void BaseBlock::evalIfReady()
{
	bool hasEmptyInput=false;
	for(BlockInput *i:inputs)
	{
		if(!i->isReady())
		{
			hasEmptyInput=true;
			break;
		}
	}
	if(!hasEmptyInput)
	{
		eval();
		for(BlockInput *i:inputs)
			i->reset();
	}
}

void BaseBlock::evalOnTimer()
{
}

void BaseBlock::onInputTypeSelected(BlockInput *b)
{
	Q_UNUSED(b)
}

void BaseBlock::onConfigOptionChanged(const QByteArray &key)
{
	Q_UNUSED(key)
}

BlockInput* BaseBlock::mkInput(TypeConstraints suppTypes,DataUnit::Type currType,const QString &title,int index)
{
	BlockInput *in=new BlockInput(this,suppTypes,currType,title);
	if(index==-1)
		inputs.append(in);
	else inputs.insert(index,in);
	return in;
}

BlockOutput *BaseBlock::mkOutput(DataUnit::Type type,quint32 dim,const QString &title,int index)
{
	BlockOutput *out=new BlockOutput(this,type,dim,title);
	if(index==-1)
		outputs.append(out);
	else outputs.insert(index,out);
	return out;
}

void BaseBlock::rmInput(int index)
{
	if(index<0||index>=inputs.count())
		return;
	delete inputs.takeAt(index);
}

void BaseBlock::rmInput(BlockInput *in)
{
	rmInput(inputs.indexOf(in));
}

void BaseBlock::rmOutput(int index)
{
	if(index<0||index>=outputs.count())
		return;
	delete outputs.takeAt(index);
}

void BaseBlock::rmOutput(BlockOutput *out)
{
	rmOutput(outputs.indexOf(out));
}

void BaseBlock::evalOnTimerInMsec(quint32 msec)
{
	if(evalTimer)
	{
		prg->rmEvalTimer(evalTimer.data());
		delete evalTimer.data();
	}
	evalTimer=new QTimer;
	evalTimer->setSingleShot(true);
	QObject::connect(evalTimer.data(),&QTimer::timeout,[this](){onTimer();});
	evalTimer->start(msec);
	prg->addEvalTimer(evalTimer.data());
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

void BaseBlock::writeDebugMessage(const QString &msg)
{
	IEngineCallbacks *c=engineCallbacks();
	if(c)c->debugCallback(msg);
}

void BaseBlock::onTimer()
{
	prg->rmEvalTimer(evalTimer.data());
	delete evalTimer.data();
	evalOnTimer();
}
