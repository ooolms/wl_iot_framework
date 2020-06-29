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

#include "VDIL/blocks/DateTimeSourceBlock.h"
#include "VDIL/core/CoreBlocksGroupFactory.h"

using namespace WLIOTVDIL;

const QString DateTimeSourceBlock::mBlockName=QString("datetime");

DateTimeSourceBlock::DateTimeSourceBlock(quint32 id)
	:SourceBlock(id)
{
	yearOut=monthOut=dayOfWeekOut=dayOfMonthOut=hourOut=secondOut=minuteOut=unixTimeOut=dateTimeOut=0;
}

QString DateTimeSourceBlock::groupName()const
{
	return CoreBlocksGroupFactory::mGroupName;
}

QString DateTimeSourceBlock::blockName()const
{
	return mBlockName;
}

void DateTimeSourceBlock::setDateOutputs(const DateTimeOutputs &o)
{
	mDateOutputs=o;

#define CHECK_OUT_PORT(portFlag,portVar,portTitle)\
	if((mDateOutputs&portFlag)&&!portVar)portVar=mkOutput(DataUnit::SINGLE,1,portTitle);\
	else if(!(mDateOutputs&portFlag)&&portVar){rmOutput(portVar);portVar=0;}

	CHECK_OUT_PORT(UNIX_TIME,unixTimeOut,"unix time")
	CHECK_OUT_PORT(SECONDS,secondOut,"seconds")
	CHECK_OUT_PORT(MINUTES,minuteOut,"minutes")
	CHECK_OUT_PORT(HOURS,hourOut,"hours")
	CHECK_OUT_PORT(DAY_OF_MONTH,dayOfMonthOut,"day of month")
	CHECK_OUT_PORT(DAY_OF_WEEK,dayOfWeekOut,"day of week")
	CHECK_OUT_PORT(MONTH,monthOut,"month")
	CHECK_OUT_PORT(YEAR,yearOut,"year")

	if((mDateOutputs&DATETIME)&&!dateTimeOut)
		dateTimeOut=mkOutput(DataUnit::DATETIME,1,"date+time");
	else if(!(mDateOutputs&DATETIME)&&dateTimeOut)
	{
		rmOutput(dateTimeOut);
		dateTimeOut=0;
	}
}

DateTimeSourceBlock::DateTimeOutputs DateTimeSourceBlock::dateOutputs()const
{
	return mDateOutputs;
}

QString DateTimeSourceBlock::dateOutputsToStr(DateTimeOutputs o)
{
	QStringList s;
	if(o&SECONDS)
		s+="sec";
	if(o&MINUTES)
		s+="min";
	if(o&HOURS)
		s+="hour";
	if(o&DAY_OF_MONTH)
		s+="day";
	if(o&DAY_OF_WEEK)
		s+="dayw";
	if(o&MONTH)
		s+="mon";
	if(o&YEAR)
		s+="year";
	if(o&UNIX_TIME)
		s+="unix";
	if(o&DATETIME)
		s+="datetime";
	return s.join(',');
}

DateTimeSourceBlock::DateTimeOutputs DateTimeSourceBlock::dateOutputsFromStr(const QString &s)
{
	DateTimeOutputs o=NO_OUTPUT;
	for(QString &i:s.split(','))
	{
		if(i=="sec")
			o|=SECONDS;
		else if(i=="min")
			o|=MINUTES;
		else if(i=="hour")
			o|=HOURS;
		else if(i=="day")
			o|=DAY_OF_MONTH;
		else if(i=="dayw")
			o|=DAY_OF_WEEK;
		else if(i=="mon")
			o|=MONTH;
		else if(i=="year")
			o|=YEAR;
		else if(i=="unix")
			o|=UNIX_TIME;
		else if(i=="datetime")
			o|=DATETIME;
	}
	return o;
}

DataUnit DateTimeSourceBlock::extractDataInternal()
{
	QDateTime dt=QDateTime::currentDateTime();
	return DataUnit(dt);

}

void DateTimeSourceBlock::evalInternal(const DataUnit &data,bool)
{
	QDateTime dt=data.dateTimeValue();
	if(mDateOutputs&SECONDS)
		secondOut->setData(DataUnit((qint64)dt.time().second()));
	if(mDateOutputs&MINUTES)
		minuteOut->setData(DataUnit((qint64)dt.time().minute()));
	if(mDateOutputs&HOURS)
		hourOut->setData(DataUnit((qint64)dt.time().hour()));
	if(mDateOutputs&DAY_OF_MONTH)
		dayOfMonthOut->setData(DataUnit((qint64)dt.date().day()));
	if(mDateOutputs&DAY_OF_WEEK)
		dayOfWeekOut->setData(DataUnit((qint64)dt.date().dayOfWeek()));
	if(mDateOutputs&MONTH)
		monthOut->setData(DataUnit((qint64)dt.date().month()));
	if(mDateOutputs&YEAR)
		yearOut->setData(DataUnit((qint64)dt.date().year()));
	if(mDateOutputs&UNIX_TIME)
		unixTimeOut->setData(DataUnit(dt.toMSecsSinceEpoch()/1000));
	if(mDateOutputs&DATETIME)
		dateTimeOut->setData(DataUnit(dt));
}
