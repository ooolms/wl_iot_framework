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

	class SubProgramBlock
		:public BaseBlock
	{
	public:
		explicit SubProgramBlock(quint32 blockId=0);
		virtual ~SubProgramBlock();
		virtual QString groupName()const override;
		virtual QString blockName()const override;
		SubProgram* subProgram();
		const SubProgram* subProgram()const;
		bool setupInputsOutputs(const QStringList &savedInputsOrder,const QStringList &savedOutputsOrder);
		void updateInputsOutputs();
		QStringList inputsOrder();
		QStringList outputsOrder();

	protected:
		virtual void onProgramIsSet()override;
		virtual void eval()override;

	private:
		int inputIndex(const QString &input);
		int outputIndex(const QString &output);

	public:
		static const QString mBlockName;

	private:
		SubProgram *sprg;
		QList<BlockInput*> mArgInputs;
		QList<BlockOutput*> mArgOutputs;
	};
}

#endif // SUBPROGRAMBLOCK_H
