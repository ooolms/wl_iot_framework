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

	class SubProgram
	{
	public:
		explicit SubProgram();
		virtual ~SubProgram();
		const QMap<quint32,BaseBlock*>& selfBlocks()const;
		virtual bool addBlock(BaseBlock *b);
		virtual void rmBlock(quint32 bId);
		void evalSubProgram();

	private:
		ProgramEvalTimers* evalTimers();

	protected:
		friend class BaseBlock;
		friend class SubProgramBlock;
		Program *prg;
		QMap<quint32,BaseBlock*> mSelfBlocks;
		QMap<quint32,SourceBlock*> mSelfSourceBlocks;
		ProgramEvalTimers *mEvalTimers;
	};
}

#endif // SUBPROGRAM_H
