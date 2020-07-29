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

#ifndef BLOCKINPUT_H
#define BLOCKINPUT_H

#include "VDIL/core/BlockPort.h"

namespace WLIOTVDIL
{
	class BlockOutput;
	class BaseBlock;

	//supported dim: 0 - any dim, >=1 - only specified dim
	class BlockInput
		:public BlockPort
	{
	public:
		//for program editing
		TypeConstraints supportedTypes()const;
		virtual TypeAndDim type()const override;
		BaseBlock* block();
		const BaseBlock* block()const;
		BlockOutput* linkedOutput();
		void replaceTypesAndDim(TypeConstraints suppTypes,TypeAndDim currType);
		void replaceTypesAndDim(const TypeAndDim &fixedType);

		//for work
		void reset();
		virtual bool setData(DataUnit u);
		bool isReady();
		const DataUnit& data();

	protected:
		explicit BlockInput(BaseBlock *b,TypeConstraints suppTypes,TypeAndDim currType,const QString &title);
		explicit BlockInput(BaseBlock *b,const TypeAndDim &fixedType,const QString &title);
		virtual ~BlockInput();
		virtual BlockInput& operator=(const BlockInput &b)=delete;

	protected:
		//for program editing
		TypeConstraints mSupportedTypes;
		TypeAndDim mCurrentType;
		BlockOutput *mLinkedOutput;
		BaseBlock *mBlock;

		//for work
		bool mDataIsSet;
		DataUnit mData;

		friend class BaseBlock;
		friend class BlockOutput;
	};
}

#endif // BLOCKINPUT_H
