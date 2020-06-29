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

#ifndef DATETIMESOURCEBLOCK_H
#define DATETIMESOURCEBLOCK_H

#include "VDIL/core/SourceBlock.h"

namespace WLIOTVDIL
{
	class DateTimeSourceBlock
		:public SourceBlock
	{
	public:
		enum DateTimeOutput
		{
			NO_OUTPUT=0,
			SECONDS=0x1,
			MINUTES=0x2,
			HOURS=0x4,
			DAY_OF_MONTH=0x8,
			DAY_OF_WEEK=0x10,
			MONTH=0x20,
			YEAR=0x40,
			UNIX_TIME=0x80,
			DATETIME=0x100,
			ALL=0xffff
		};

		Q_DECLARE_FLAGS(DateTimeOutputs,DateTimeOutput)

	public:
		explicit DateTimeSourceBlock(quint32 id=0);
		virtual QString groupName()const override;
		virtual QString blockName()const override;
		void setDateOutputs(const DateTimeOutputs &o);
		DateTimeOutputs dateOutputs()const;
		static QString dateOutputsToStr(DateTimeOutputs o);
		static DateTimeOutputs dateOutputsFromStr(const QString &s);

	protected:
		virtual DataUnit extractDataInternal()override;
		virtual void evalInternal(const DataUnit &data)override;

	public:
		static const QString mBlockName;

	private:
		BlockOutput *yearOut,*monthOut,*dayOfMonthOut,*dayOfWeekOut,
			*hourOut,*minuteOut,*secondOut,*unixTimeOut,*dateTimeOut;
		DateTimeOutputs mDateOutputs;
	};

	Q_DECLARE_OPERATORS_FOR_FLAGS(DateTimeSourceBlock::DateTimeOutputs)
}

#endif // DATETIMESOURCEBLOCK_H
