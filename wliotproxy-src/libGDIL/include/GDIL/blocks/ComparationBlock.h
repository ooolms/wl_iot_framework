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

#ifndef COMPARATIONBLOCK_H
#define COMPARATIONBLOCK_H

#include "GDIL/core/BaseBlock.h"

namespace WLIOTGDIL
{
	class ComparationBlock
		:public BaseBlock
	{
	public:
		enum Operation
		{
			EQ,
			NEQ,
			GT,
			LT,
			GTEQ,
			LTEQ,
			DISTANCE
		};

		enum OutMode
		{
			SINGLE_BOOL,
			SPLITTED_BOOL,
			SPLITTED_INPUT
		};

	public:
		explicit ComparationBlock(quint32 bId=0);
		void setParams(OutMode outMode,bool externalV2Input,quint32 dimIndex,Operation operation);
		void setDistValue(const DataUnit &val);
		void setV2Value(const DataUnit &val);
		virtual QString groupName()const override;
		virtual QString blockName()const override;
		OutMode outMode()const;
		const DataUnit& distValue()const;
		quint32 dimIndex()const;
		Operation operation()const;
		bool externalV2Input()const;
		const DataUnit& v2Value()const;

	protected:
		virtual void eval()override;
		virtual void onInputTypeSelected(BlockInput *b)override;

	public:
		static const QString mBlockName;

	private:
		BlockInput *in1,*in2;
		BlockOutput *out1,*out2;
		OutMode mOutMode;
		bool mExternalV2Input;
		DataUnit mV2Value;
		DataUnit mDistValue;
		quint32 mDimIndex;
		Operation mOp;
	};
}

#endif // COMPARATIONBLOCK_H
