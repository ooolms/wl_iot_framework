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

#ifndef COMMANDBLOCK_H
#define COMMANDBLOCK_H

#include "VDIL/core/BaseBlock.h"

namespace WLIOTVDIL
{
	class CommandBlock
		:public BaseBlock
	{
	public:
		enum OutputMode
		{
			NoOut,
			NotifyOnly,
			WaitForCommandAnswer
		};

	public:
		explicit CommandBlock(quint32 bId=0);
		void setParams(const QUuid &deviceId,const QByteArray &cmd,
			const QByteArrayList &args,quint32 argInputsCount,bool enableConditionInput,
			OutputMode outMode);
		virtual QString groupName()const override;
		virtual QString blockName()const override;
		const QUuid& deviceId()const;
		const QByteArray& cmd()const;
		const QByteArrayList& args()const;
		quint32 argInputsCount()const;
		bool enableConditionInput()const;
		OutputMode outMode()const;
		static QString outModeToStr(OutputMode m);
		static OutputMode outModeFromStr(const QString &s);

	protected:
		virtual void eval()override;
		virtual QList<QUuid> usedDevices()const override;

	public:
		static const QString mBlockName;

	private:
		QUuid mDevId;
		QByteArray mCmd;
		QByteArrayList mArgs;
		OutputMode mOutMode;
		bool mEnableConditionInput;
		quint32 mArgInCount;
		BlockInput *condInput;
		BlockOutput *out;
		QList<BlockInput*> argsInputs;
	};
}

#endif // COMMANDBLOCK_H
