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

#ifndef TIMERBLOCK_H
#define TIMERBLOCK_H

#include "GDIL/core/SourceBlock.h"
#include <QDateTime>

class TimerBlock
	:public SourceBlock
{
public:
	enum SchedulePolicy
	{
		INVALID,
		SINGLE,
		REGULAR_SEC,
		REGULAR_MIN,
		REGULAR_HOUR,
		REGULAR_DAY,
		MONTH,
		YEAR
	};
	struct TimerConfig
	{
		QDateTime startTime;
		SchedulePolicy policy;
		qint64 repeatInterval;
		//repeatInterval - in corresponding units for REGULAR_SEC, REGULAR_MIN, REGULAR_HOUR and REGULAR_DAY
	};

public:
	explicit TimerBlock(quint32 bId);
	void setTimerTriggered();
	void setConfig(const TimerConfig &cfg,bool configurable);
	TimerConfig config()const;
	bool configurable()const;
	qint64 repeatInSec()const;

	QDateTime nextTimeout(const QDateTime &currTime)const;
	virtual QString groupName()const override;
	virtual QString blockName()const override;

	static QByteArray policyToStr(SchedulePolicy pol);
	static SchedulePolicy policyFromStr(const QByteArray &str);

protected:
	virtual DataUnit extractDataInternal()override;

public:
	static const QString mBlockName;

private:
	bool mTimerTriggered;
	bool mConfigurable;
	qint64 mRepeatInSec;
	TimerConfig mConfig;
};

#endif // TIMERBLOCK_H
