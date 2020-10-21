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

#ifndef VDEVSENSORSENDBLOCK_H
#define VDEVSENSORSENDBLOCK_H

#include "VDIL/core/BaseBlock.h"

namespace WLIOTVDIL
{
	class VDevSensorSendBlock
		:public BaseBlock
	{
	public:
		explicit VDevSensorSendBlock(quint32 bId=0);
		virtual QString groupName()const override;
		virtual QString blockName()const override;
		void setSensorName(const QByteArray &sensName);
		QByteArray sensorName()const;
		void updateInput();

	protected:
		virtual void eval()override;

	public:
		static const QString mBlockName;

	private:
		BlockInput *in;
		QByteArray mSensorName;
	};
}

#endif // VDEVSENSORSENDBLOCK_H
