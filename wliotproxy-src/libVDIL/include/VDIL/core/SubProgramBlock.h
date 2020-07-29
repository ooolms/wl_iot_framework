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

#ifndef SUBPROGRAMBLOCK_H
#define SUBPROGRAMBLOCK_H

#include "VDIL/core/BaseBlock.h"

namespace WLIOTVDIL
{
	class SubProgram;
	class SubProgramBlock;

	class SubProgramInternalOutputsFakeBlock
		:public BaseBlock
	{
	public:
		explicit SubProgramInternalOutputsFakeBlock();
		virtual QString groupName()const override;
		virtual QString blockName()const override;
		using BaseBlock::mkOutput;
		using BaseBlock::rmOutput;
		void updateOutput(int index,const TypeAndDim &type,const QString &title);

	protected:
		virtual void eval()override;

	public:
		static const QString mBlockName;
	};

	class SubProgramInternalInputsFakeBlock
		:public BaseBlock
	{
	public:
		explicit SubProgramInternalInputsFakeBlock();
		virtual QString groupName()const override;
		virtual QString blockName()const override;
		using BaseBlock::mkInput;
		using BaseBlock::rmInput;
		void updateInput(int index,const TypeAndDim &t,const QString &tt);

	protected:
		virtual void eval()override;

	public:
		static const QString mBlockName;
	};

	class SubProgramBlock
		:public BaseBlock
	{
	public:
		explicit SubProgramBlock(quint32 blockId=0);
		virtual ~SubProgramBlock();
		virtual QString groupName()const override;
		virtual QString blockName()const override;
		SubProgram* subProgram();
		void setParams(const QList<TypeAndDim> &inputTypes,const QStringList &inputTitles,
			const QList<TypeAndDim> &outputTypes,const QStringList &outputTitles);
		QList<TypeAndDim> inputTypes()const;
		QStringList inputTitles()const;
		QList<TypeAndDim> outputTypes()const;
		QStringList outputTitles()const;
		SubProgramInternalOutputsFakeBlock* internalOutputsBlock();
		const SubProgramInternalOutputsFakeBlock* internalOutputsBlock()const;
		SubProgramInternalInputsFakeBlock* internalInputsBlock();
		const SubProgramInternalInputsFakeBlock* internalInputsBlock()const;

		//inputs/outputs
		//внешним входам соответствуют внутренние выходы и наоборот
		int argInputsCount()const;
		BlockInput* argInput(int index);
		const BlockInput* argInput(int index)const;
		int argOutputsCount()const;
		BlockOutput* argOutput(int index);
		const BlockOutput* argOutput(int index)const;

	protected:
		virtual void onProgramIsSet()override;
		virtual void eval()override;
		virtual void cleanupAfterEvalInternal() override;

	public:
		static const QString mBlockName;

	private:
		SubProgram *sprg;
		QList<BlockInput*> mArgInputs;
		QList<BlockOutput*> mArgOutputs;
		SubProgramInternalOutputsFakeBlock *mInternalOutputsBlock;
		SubProgramInternalInputsFakeBlock *mInternalInputsBlock;
	};
}

#endif // SUBPROGRAMBLOCK_H
