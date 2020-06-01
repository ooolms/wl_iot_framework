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

#ifndef STATICSOURCEBLOCK_H
#define STATICSOURCEBLOCK_H

#include "GDIL/core/SourceBlock.h"

namespace WLIOTGDIL
{
	class StaticSourceBlock
		:public SourceBlock
	{
	public:
		explicit StaticSourceBlock(quint32 bId=0);
		virtual QString groupName()const override;
		virtual QString blockName()const override;
		void setParams(const DataUnit &u,bool configurable);
		const DataUnit& value()const;
		bool configurable()const;

	protected:
		virtual DataUnit extractDataInternal()override;
		virtual void onConfigOptionChanged(const QByteArray &key)override;

	public:
		static const QString mBlockName;

	private:
		DataUnit mValue;
		bool mConfigurable;
	};
}

#endif // STATICSOURCEBLOCK_H
