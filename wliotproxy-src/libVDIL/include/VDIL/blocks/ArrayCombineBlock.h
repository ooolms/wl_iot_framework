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

#ifndef ARRAYCOMBINEBLOCK_H
#define ARRAYCOMBINEBLOCK_H

#include "VDIL/core/BaseBlock.h"

namespace WLIOTVDIL
{
	class ArrayCombineBlock
		:public BaseBlock
	{
	public:
		explicit ArrayCombineBlock(quint32 bId=0);
		virtual QString groupName()const override;
		virtual QString blockName()const override;
		void setParams(quint32 inputsCount,quint32 dim);
		quint32 dim()const;

	protected:
		virtual void eval()override;

	public:
		static const QString mBlockName;

	private:
		QList<BlockInput*> mArrayInputs;
		BlockOutput *out;
		quint32 mDim;
	};
}

#endif // ARRAYCOMBINEBLOCK_H
