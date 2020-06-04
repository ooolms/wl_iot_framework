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

#ifndef BLOCKOUTPUT_H
#define BLOCKOUTPUT_H

#include "VDIL/core/BlockPort.h"
#include <QList>

//CRIT make "relink" operation to other output block,
//that move all links from this block to other one (with type checks, unmet inputs must be unlinked)

namespace WLIOTVDIL
{
	class BlockInput;
	class BaseBlock;

	class BlockOutput
		:public BlockPort
	{
	public:
		//for program editing
		virtual DataUnit::Type type()const override;
		virtual quint32 dim()const override;
		bool linkTo(BlockInput *input);
		void unlinkFrom(BlockInput *input);
		int linksCount()const;
		const BlockInput* link(int index)const;
		BaseBlock* block();
		void replaceTypeAndDim(DataUnit::Type newType,quint32 newDim);

		//for work
		void setData(DataUnit u);

	private:
		explicit BlockOutput(BaseBlock *b,DataUnit::Type t,quint32 dim,const QString &title);
		virtual ~BlockOutput();
		virtual BlockOutput& operator=(const BlockOutput &b)=delete;

	private:
		//for program editing
		QList<BlockInput*> linkedInputs;
		DataUnit::Type mType;
		quint32 mDim;
		BaseBlock *mBlock;

		friend class BaseBlock;
	};
}

#endif // BLOCKOUTPUT_H
