#include "GDIL/xml/TimerBlockXmlParser.h"
#include "GDIL/core/TimerBlock.h"

static QString polToStr(TimerBlock::SchedulePolicy pol)
{
	if(pol==TimerBlock::REGULAR_SEC)
		return "sec";
	else if(pol==TimerBlock::REGULAR_MIN)
		return "min";
	else if(pol==TimerBlock::REGULAR_HOUR)
		return "hour";
	else if(pol==TimerBlock::REGULAR_DAY)
		return "day";
	else if(pol==TimerBlock::MONTH)
		return "month";
	else if(pol==TimerBlock::YEAR)
		return "year";
	else //SINGLE
		return "single";
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

bool TimerBlockXmlParser::blockFromXml(BaseBlock *block,const QDomElement &blockElem)
{
	TimerBlock *b=(TimerBlock*)block;
	TimerBlock::TimerConfig cfg;
	if(!blockElem.hasAttribute("policy")||!blockElem.hasAttribute("repeat_interval")||!blockElem.hasAttribute("start")||
		!blockElem.hasAttribute("configurable"))
		return false;
	cfg.policy=polFromStr(blockElem.attribute("policy"));
	cfg.startTime=QDateTime::fromString(blockElem.attribute("start"),Qt::ISODate);
	cfg.repeatInterval=blockElem.attribute("repeat_interval").toLongLong();
	b->setConfig(cfg,blockElem.attribute("configurable")=="1");
	return true;
}

void TimerBlockXmlParser::blockToXml(const BaseBlock *block,QDomElement &blockElem)
{
	TimerBlock::TimerConfig cfg=((const TimerBlock*)block)->config();
	blockElem.setAttribute("policy",polToStr(cfg.policy));
	blockElem.setAttribute("repeat_interval",cfg.repeatInterval);
	blockElem.setAttribute("start",cfg.startTime.toString(Qt::ISODate));
	blockElem.setAttribute("configurable",((const TimerBlock*)block)->configurable()?"1":"0");
}
