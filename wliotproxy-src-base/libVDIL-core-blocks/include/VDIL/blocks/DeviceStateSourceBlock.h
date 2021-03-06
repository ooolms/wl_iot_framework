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

#ifndef DEVICESTATESOURCEBLOCK_H
#define DEVICESTATESOURCEBLOCK_H

#include "VDIL/core/SourceBlock.h"

namespace WLIOTVDIL
{
	class DeviceStateSourceBlock
		:public SourceBlock
	{
	public:
		explicit DeviceStateSourceBlock(quint32 bId=0);
		virtual QString groupName()const override;
		virtual QString blockName()const override;
		void setParams(const QUuid &devId,const QByteArray &stateKey,bool cmdState,bool boolOut,quint32 cmdStateIndex);
		QUuid deviceId()const;
		QByteArray stateKey()const;
		bool commandState()const;
		bool boolOut()const;
		quint32 commandStateIndex()const;
		void setValFromTriggerSlot(const QByteArray &v);

	protected:
		virtual DataUnit extractDataInternal()override;
		virtual QList<QUuid> usedDevices() const override;

	public:
		static const QString mBlockName;

	private:
		QUuid mDevId;
		QByteArray mStateKey;
		QByteArray valFromTriggerSlot;
		bool mCmdState;
		bool mBoolOut;
		quint32 mCmdStateIndex;
	};
}

#endif // DEVICESTATESOURCEBLOCK_H
