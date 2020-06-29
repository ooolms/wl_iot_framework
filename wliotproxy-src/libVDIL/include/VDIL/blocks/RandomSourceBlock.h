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

#ifndef RANDOMSOURCEBLOCK_H
#define RANDOMSOURCEBLOCK_H

#include "VDIL/core/SourceBlock.h"

namespace WLIOTVDIL
{
	class RandomSourceBlock
		:public SourceBlock
	{
	public:
		explicit RandomSourceBlock(quint32 bId=0);
		void setParams(const QList<QPair<qint32,qint32>> &bounds);
		virtual QString groupName()const override;
		virtual QString blockName()const override;
		const QList<QPair<qint32,qint32>>& bounds()const;

	protected:
		virtual DataUnit extractDataInternal()override;

	public:
		static const QString mBlockName;

	private:
		BlockOutput *out;
		QList<QPair<qint32,qint32>> mBounds;//min,max
	};
}

#endif // RANDOMSOURCEBLOCK_H
