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
	if(!blockElem.hasAttribute("configurable"))
		return false;
	if(!timerConfigFromXml(cfg,blockElem))
		return false;
	b->setConfig(cfg,blockElem.attribute("configurable")=="1");
	return true;
}

void TimerBlockXmlParser::blockToXml(const BaseBlock *block,QDomElement &blockElem)
{
	TimerBlock::TimerConfig cfg=((const TimerBlock*)block)->config();
	timerConfigToXml(cfg,blockElem);
	blockElem.setAttribute("configurable",((const TimerBlock*)block)->configurable()?"1":"0");
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
