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

#include "GDIL/core/TimerBlock.h"
#include "GDIL/core/CoreBlocksGroupFactory.h"

using namespace WLIOT;
using namespace WLIOTGDIL;

const QString TimerBlock::mBlockName=QString("timer");

TimerBlock::TimerBlock(quint32 bId)
	:SourceBlock(bId)
{
	mConfig.startTime=QDateTime::currentDateTime();
	mRepeatInSec=0;
	mTimerTriggered=false;
	mConfigurable=false;
	mConfig.policy=SINGLE;
	mConfig.repeatInterval=0;
	mkOutput(DataUnit::BOOL,1,"signal");
}

void TimerBlock::setTimerTriggered()
{
	mTimerTriggered=true;
}

void TimerBlock::setConfig(const TimerConfig &cfg,bool configurable)
{
	if(!cfg.startTime.isValid())return;
	mConfig=cfg;
	if(mConfig.policy>=REGULAR_SEC&&mConfig.policy<=REGULAR_DAY)
	{
		if(mConfig.repeatInterval<=0)
			mConfig.repeatInterval=1;
		mRepeatInSec=mConfig.repeatInterval;
		if(mConfig.policy==REGULAR_MIN)
			mRepeatInSec*=60;
		else if(mConfig.policy==REGULAR_HOUR)
			mRepeatInSec*=3600;
		else if(mConfig.policy==REGULAR_DAY)
			mRepeatInSec*=3600*24;
	}
	else mRepeatInSec=0;
	mConfigurable=configurable;
}

TimerBlock::TimerConfig TimerBlock::config()const
{
	return mConfig;
}

qint64 TimerBlock::repeatInSec()const
{
	return mRepeatInSec;
}

QString TimerBlock::configString(const TimerConfig &cfg)
{
	QString s;
	TimerBlock::SchedulePolicy pol=cfg.policy;
	if(pol==TimerBlock::SINGLE)
		s="once: "+cfg.startTime.toString(Qt::ISODate);
	else
	{
		s="start: "+cfg.startTime.toString(Qt::ISODate)+"; repeat ";
		if(pol==TimerBlock::REGULAR_SEC)
			s+="each "+QString::number(cfg.repeatInterval)+" seconds";
		else if(pol==TimerBlock::REGULAR_MIN)
			s+="each "+QString::number(cfg.repeatInterval)+" minutes";
		else if(pol==TimerBlock::REGULAR_HOUR)
			s+="each "+QString::number(cfg.repeatInterval)+" hours";
		else if(pol==TimerBlock::REGULAR_DAY)
			s+="each "+QString::number(cfg.repeatInterval)+" days";
		else if(pol==TimerBlock::MONTH)
			s+="monthly";
		else //YEAR
			s+="yearly";
	}
	return s;
}

bool TimerBlock::configurable()const
{
	return mConfigurable;
}

QDateTime TimerBlock::nextTimeout(const QDateTime &currTime)const
{
	if(currTime<mConfig.startTime)
		return mConfig.startTime;
	if(mConfig.policy==SINGLE)
		return QDateTime();
	else if(mRepeatInSec!=0)
	{
		qint64 startTimeSec=mConfig.startTime.toMSecsSinceEpoch()/1000;
		qint64 secsFromStart=currTime.toMSecsSinceEpoch()/1000-startTimeSec;
		qint64 totalIntervals=secsFromStart/mRepeatInSec;
		return QDateTime::fromMSecsSinceEpoch((startTimeSec+(totalIntervals+1)*mRepeatInSec)*1000);
	}
	else if(mConfig.policy==MONTH)
	{
		QDate validDateInMonth=QDate(currTime.date().year(),currTime.date().month(),
			qMin(currTime.date().daysInMonth(),mConfig.startTime.date().day()));
		QDateTime nextTime=QDateTime(validDateInMonth,mConfig.startTime.time());
		if(nextTime>currTime)return nextTime;
		//switch to next month
		validDateInMonth=validDateInMonth.addMonths(1);
		validDateInMonth=QDate(validDateInMonth.year(),validDateInMonth.month(),
			qMin(validDateInMonth.daysInMonth(),mConfig.startTime.date().day()));
		return QDateTime(validDateInMonth,mConfig.startTime.time());
	}
	else//YEAR
	{
		QDate nextDate=QDate(currTime.date().year(),mConfig.startTime.date().month(),mConfig.startTime.date().day());
		if(!nextDate.isValid())//mStartTime is 29.02, curr year has 28 days if feb.
			nextDate=QDate(currTime.date().year(),mConfig.startTime.date().month(),mConfig.startTime.date().day()-1);
		QDateTime nextTime=QDateTime(nextDate,mConfig.startTime.time());
		if(nextTime>currTime)return nextTime;
		//switch to next year
		nextDate=QDate(currTime.date().year()+1,mConfig.startTime.date().month(),mConfig.startTime.date().day());
		if(!nextDate.isValid())//mStartTime is 29.02, curr year has 28 days if feb.
			nextDate=QDate(currTime.date().year()+1,mConfig.startTime.date().month(),mConfig.startTime.date().day()-1);
		return QDateTime(nextDate,mConfig.startTime.time());
	}
}

QString TimerBlock::groupName()const
{
	return CoreBlocksGroupFactory::mGroupName;
}

QString TimerBlock::blockName()const
{
	return mBlockName;
}

QByteArray TimerBlock::policyToStr(TimerBlock::SchedulePolicy pol)
{
	if(pol==SINGLE)
		return "single";
	else if(pol==REGULAR_SEC)
		return "sec";
	else if(pol==REGULAR_MIN)
		return "min";
	else if(pol==REGULAR_HOUR)
		return "hour";
	else if(pol==REGULAR_DAY)
		return "day";
	else if(pol==MONTH)
		return "month";
	else if(pol==YEAR)
		return "year";
	else return "";
}

TimerBlock::SchedulePolicy TimerBlock::policyFromStr(const QByteArray &str)
{
	if(str=="single")
		return SINGLE;
	else if(str=="sec")
		return REGULAR_SEC;
	else if(str=="min")
		return REGULAR_MIN;
	else if(str=="hour")
		return REGULAR_HOUR;
	else if(str=="day")
		return REGULAR_DAY;
	else if(str=="month")
		return MONTH;
	else if(str=="year")
		return YEAR;
	else return INVALID;
}

DataUnit TimerBlock::extractDataInternal()
{
	DataUnit u=DataUnit(mTimerTriggered);
	mTimerTriggered=false;
	return u;
}
