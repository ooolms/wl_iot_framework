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

#ifndef SUBPROGRAM_H
#define SUBPROGRAM_H

#include "VDIL/core/SourceBlock.h"

namespace WLIOTVDIL
{
	class ProgramEvalTimers;

	class SubProgramInternalInputBlock
		:public BaseBlock
	{
	public:
		explicit SubProgramInternalInputBlock(quint32 bId=0);
		virtual QString groupName()const override;
		virtual QString blockName()const override;
		void setSubProgramOutput(const QString &output,const TypeAndDim &type);
		const QString& subProgramOutput()const;

	protected:
		virtual void eval()override;

	public:
		static const QString mBlockName;

	private:
		QString mSubProgramOutput;
		DataUnit mData;
		BlockInput *in;
	};

	class SubProgramInternalOutputBlock
		:public BaseBlock
	{
	public:
		explicit SubProgramInternalOutputBlock(quint32 bId);
		virtual QString groupName()const override;
		virtual QString blockName()const override;
		void setSubProgramInput(const QString &input,const TypeAndDim &type);
		const QString& subProgramInput()const;

		//runtime
		void setData(const DataUnit &u);

	protected:
		virtual void eval()override;

	public:
		static const QString mBlockName;

	private:
		QString mSubProgramInput;
		DataUnit mData;
		BlockOutput *out;
	};

	class SubProgram
	{
	public:
		explicit SubProgram();
		virtual ~SubProgram();
		const QMap<quint32,BaseBlock*>& selfBlocks()const;
		bool addBlock(BaseBlock *b);
		void rmBlock(quint32 bId);
		void setInputs(const QMap<QString,TypeAndDim> &inputs);
		void setOutputs(const QMap<QString,TypeAndDim> &outputs);
		const QMap<QString,TypeAndDim>& inputs()const;
		const QMap<QString,TypeAndDim>& outputs()const;

	protected:
		void evalSubProgram();

		//runtime
		void setInputData(const QString &input,const DataUnit &d);
		const QMap<QString,DataUnit>& outputsData();
		void prepareSubProgramToStart();
		void cleanupSubProgramAfterStop();

	private:
		ProgramEvalTimers* evalTimers();

	protected:
		friend class Program;
		friend class BaseBlock;
		friend class SubProgramBlock;
		friend class SubProgramInternalInputBlock;

		Program *prg;
		QMap<quint32,BaseBlock*> mSelfBlocks;
		QMap<quint32,SourceBlock*> mSelfSourceBlocks;
		QMap<QString,TypeAndDim> mInputs;
		QMap<QString,TypeAndDim> mOutputs;
		QMap<QString,QList<SubProgramInternalOutputBlock*>> mInternalOutputBlocks;
		ProgramEvalTimers *mEvalTimers;

		//runtime
		QMap<QString,DataUnit> mInputsData;
		QMap<QString,DataUnit> mOutputsData;
	};
}

#endif // SUBPROGRAM_H
