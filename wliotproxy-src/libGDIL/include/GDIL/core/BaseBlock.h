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

#ifndef BASEBLOCK_H
#define BASEBLOCK_H

#include "GDIL/core/BlockInput.h"
#include "GDIL/core/BlockOutput.h"
#include "GDIL/core/IEngineHelper.h"
#include <QList>
#include <QPoint>

class Program;

class BaseBlock
{
public:
	explicit BaseBlock(quint32 bId);
	virtual ~BaseBlock();
	virtual QString groupName()const=0;
	virtual QString blockName()const=0;
	virtual bool isSourceBlock()const;

	//for program
	int inputsCount();
	BlockInput* input(int index);
	int inputIndex(const BlockInput *in)const;
	int outputsCount();
	BlockOutput* output(int index);
	int outputIndex(const BlockOutput *out)const;
	quint32 blockId()const;

	//for work
	void evalIfReady();

protected:
	virtual void eval()=0;
	virtual void onInputTypeSelected(BlockInput *b);
	BlockInput* mkInput(DataUnit::Types suppTypes,DataUnit::Type currType,quint32 supportedDim,const QString &title);
	BlockOutput* mkOutput(DataUnit::Type type,quint32 dim,const QString &title);
	void rmInput(int index);
	void rmInput(BlockInput *in);
	void rmOutput(int index);
	void rmOutput(BlockOutput *out);

public:
	//editing attributes
	QString title;
	QString hint;
	QPointF position;

protected:
	Program *prg;

private:
	QList<BlockInput*> inputs;
	QList<BlockOutput*> outputs;

private:
	quint32 mBlockId;
	friend class BlockOutput;
	friend class BlockInput;
	friend class Program;
};

#endif // BASEBLOCK_H
