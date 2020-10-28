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

#include "VDIL/xml/TimerBlockXmlParser.h"
#include "VDIL/core/TimerBlock.h"

using namespace WLIOT;
using namespace WLIOTVDIL;

static QString polToStr(TimerBlock::SchedulePolicy pol)
{
	if(pol==TimerBlock::REGULAR_SEC)
		return QString::fromUtf8("sec");
	else if(pol==TimerBlock::REGULAR_MIN)
		return QString::fromUtf8("min");
	else if(pol==TimerBlock::REGULAR_HOUR)
		return QString::fromUtf8("hour");
	else if(pol==TimerBlock::REGULAR_DAY)
		return QString::fromUtf8("day");
	else if(pol==TimerBlock::MONTH)
		return QString::fromUtf8("month");
	else if(pol==TimerBlock::YEAR)
		return QString::fromUtf8("year");
	else //SINGLE
		return QString::fromUtf8("single");
}

static TimerBlock::SchedulePolicy polFromStr(const QString &s)
{
	if(s=="sec")
		return TimerBlock::REGULAR_SEC;
	else if(s=="min")
		return TimerBlock::REGULAR_MIN;
	else if(s=="hour")
		return TimerBlock::REGULAR_HOUR;
	else if(s=="day")
		return TimerBlock::REGULAR_DAY;
	else if(s=="month")
		return TimerBlock::MONTH;
	else if(s=="year")
		return TimerBlock::YEAR;
	else return TimerBlock::SINGLE;
}

bool TimerBlockXmlParser::blockFromXml(BaseBlock *block,const QDomElement &blockElem,bool tryFixErrors)
{
	TimerBlock *b=(TimerBlock*)block;
	TimerBlock::TimerConfig cfg;
	if(!blockElem.hasAttribute("configurable"))
		if(!tryFixErrors)return false;
	if(!timerConfigFromXml(cfg,blockElem))
	{
		if(!tryFixErrors)return false;
		cfg.startTime=QDateTime::currentDateTime();
		cfg.policy=TimerBlock::SINGLE;
	}
	b->setConfig(cfg,blockElem.attribute("configurable")=="1");
	return true;
}

void TimerBlockXmlParser::blockToXml(const BaseBlock *block,QDomElement &blockElem)
{
	TimerBlock::TimerConfig cfg=((const TimerBlock*)block)->config();
	timerConfigToXml(cfg,blockElem);
	blockElem.setAttribute("configurable",QString::fromUtf8(((const TimerBlock*)block)->configurable()?"1":"0"));
}

bool TimerBlockXmlParser::timerConfigFromXml(TimerBlock::TimerConfig &cfg,const QDomElement &elem)
{
	if(!elem.hasAttribute("policy")||!elem.hasAttribute("repeat_interval")||!elem.hasAttribute("start"))
		return false;
	cfg.policy=polFromStr(elem.attribute("policy"));
	cfg.startTime=QDateTime::fromString(elem.attribute("start"),Qt::ISODate);
	cfg.repeatInterval=elem.attribute("repeat_interval").toLongLong();
	return true;
}

void TimerBlockXmlParser::timerConfigToXml(const TimerBlock::TimerConfig &cfg,QDomElement &elem)
{
	elem.setAttribute("policy",polToStr(cfg.policy));
	elem.setAttribute("repeat_interval",cfg.repeatInterval);
	elem.setAttribute("start",cfg.startTime.toString(Qt::ISODate));
}
