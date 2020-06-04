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

#ifndef DEBUGBLOCK_H
#define DEBUGBLOCK_H

#include "VDIL/core/BaseBlock.h"

namespace WLIOTVDIL
{
	class DebugBlock
		:public BaseBlock
	{
	public:
		explicit DebugBlock(quint32 bId=0);
		void setParams(const QString &debugString,quint32 inCount);
		virtual QString groupName()const override;
		virtual QString blockName()const override;
		QString debugString()const;
		quint32 inCount()const;

	protected:
		virtual void eval()override;

	public:
		static const QString mBlockName;

	private:
		QString mDebugString;
		quint32 mInCount;
	};
}

#endif // DEBUGBLOCK_H
