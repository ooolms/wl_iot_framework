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

#ifndef VDEVCOMMANDSOURCEBLOCK_H
#define VDEVCOMMANDSOURCEBLOCK_H

#include "VDIL/core/SourceBlock.h"

namespace WLIOTVDIL
{
	class VDevCommandSourceBlock
		:public SourceBlock
	{
	public:
		explicit VDevCommandSourceBlock(quint32 bId=0);
		virtual QString groupName()const override;
		virtual QString blockName()const override;
		void setCommand(const QByteArray &cmd);
		QByteArray command()const;

	protected:
		virtual DataUnit extractDataInternal()override;
		virtual void evalInternal(const DataUnit &data,bool triggerActivated)override;

	public:
		static const QString mBlockName;

	private:
		BlockOutput *triggerOut;
		QList<BlockOutput*> argsOuts;
		QByteArray mCommand;
	};
}

#endif // VDEVCOMMANDSOURCEBLOCK_H
