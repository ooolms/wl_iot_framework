#ifndef TIMERBLOCKXMLPARSER_H
#define TIMERBLOCKXMLPARSER_H

#include "VDIL/xml/IBlockXmlParser.h"
#include "VDIL/core/TimerBlock.h"

namespace WLIOTVDIL
{
	class TimerBlockXmlParser
		:public IBlockXmlParser
	{
	public:
		virtual bool blockFromXml(BaseBlock *block,const QDomElement &blockElem,bool tryFixErrors)override;
		virtual void blockToXml(const BaseBlock *block,QDomElement &blockElem)override;
		static bool timerConfigFromXml(TimerBlock::TimerConfig &cfg,const QDomElement &elem);
		static void timerConfigToXml(const TimerBlock::TimerConfig &cfg,QDomElement &elem);
	};
}

#endif // TIMERBLOCKXMLPARSER_H