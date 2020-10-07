#include "VDILProgramConfigDb.h"
#include "VDIL/xml/DataUnitXmlParser.h"
#include "VDIL/xml/TimerBlockXmlParser.h"
#include <QDomDocument>
#include <QDomElement>
#include <QFile>
#include <QFileInfo>

VDILProgramConfigDb::VDILProgramConfigDb(const QString &programPath,const QByteArray &programId)
	:BaseProgramConfigDb(programPath,programId)
{
	load();
}

void VDILProgramConfigDb::setConfigOption(const WLIOTVDIL::ConfigOptionId &id,const WLIOTVDIL::DataUnit &v)
{
	mConfigOptions[id]=v;
	storeDb();
}

void VDILProgramConfigDb::setTimerConfig(quint32 blockId,const WLIOTVDIL::TimerBlock::TimerConfig &cfg)
{
	if(!mTimers.contains(blockId))
		return;
	mTimers[blockId]=cfg;
	storeDb();
}

void VDILProgramConfigDb::cleanup(WLIOTVDIL::Program *p)
{
	if(!p)return;
	bool changed=false;
	for(auto i=mConfigOptions.begin();i!=mConfigOptions.end();++i)
	{
		if(!p->allConfigOptions().contains(i.key()))
		{
			i=mConfigOptions.erase(i);
			--i;
			changed=true;
		}
	}
	for(auto i=mTimers.begin();i!=mTimers.end();++i)
	{
		WLIOTVDIL::TimerBlock *b=p->timerBlocks().value(i.key());
		if(!b||!b->configurable())
		{
			i=mTimers.erase(i);
			--i;
			changed=true;
		}
	}
	if(changed)storeDb();
}

const QMap<WLIOTVDIL::ConfigOptionId,WLIOTVDIL::DataUnit>& VDILProgramConfigDb::configOptions()const
{
	return mConfigOptions;
}

const QMap<quint32,WLIOTVDIL::TimerBlock::TimerConfig>& VDILProgramConfigDb::timers()const
{
	return mTimers;
}

void VDILProgramConfigDb::loadOther(QDomElement &rootElem)
{
	QDomElement configOptionsElem=rootElem.firstChildElement("config_options");
	QDomElement timersElem=rootElem.firstChildElement("timers");
	if(configOptionsElem.isNull()||timersElem.isNull())return;
	for(int i=0;i<configOptionsElem.childNodes().count();++i)
	{
		QDomElement cfgElem=configOptionsElem.childNodes().at(i).toElement();
		if(cfgElem.isNull()||cfgElem.nodeName()!="option")continue;
		QDomElement valElem=cfgElem.firstChildElement("value").toElement();
		if(valElem.isNull())continue;
		quint32 blockId=cfgElem.attribute("block_id").toUInt();
		QByteArray key=cfgElem.attribute("key").toUtf8();
		WLIOTVDIL::DataUnit v;
		if(!WLIOTVDIL::DataUnitXmlParser::fromXml(v,valElem))continue;
		mConfigOptions[{blockId,key}]=v;
	}
	for(int i=0;i<timersElem.childNodes().count();++i)
	{
		QDomElement timerElem=timersElem.childNodes().at(i).toElement();
		if(timerElem.isNull()||timerElem.nodeName()!="timer")continue;
		QDomElement cfgElem=timerElem.firstChildElement("config").toElement();
		if(cfgElem.isNull())continue;
		quint32 blockId=timerElem.attribute("block_id").toUInt();
		WLIOTVDIL::TimerBlock::TimerConfig cfg;
		if(!WLIOTVDIL::TimerBlockXmlParser::timerConfigFromXml(cfg,cfgElem))continue;
		mTimers[blockId]=cfg;
	}
}

void VDILProgramConfigDb::storeOther(QDomElement &rootElem)
{
	QDomDocument doc=rootElem.ownerDocument();
	QDomElement configOptionsElem=doc.createElement("config_options");
	rootElem.appendChild(configOptionsElem);
	QDomElement timersElem=doc.createElement("timers");
	rootElem.appendChild(timersElem);
	for(auto i=mConfigOptions.begin();i!=mConfigOptions.end();++i)
	{
		QDomElement cfgElem=doc.createElement("option");
		configOptionsElem.appendChild(cfgElem);
		QDomElement valElem=doc.createElement("value");
		cfgElem.appendChild(valElem);
		cfgElem.setAttribute("block_id",i.key().blockId);
		cfgElem.setAttribute("key",QString::fromUtf8(i.key().key));
		WLIOTVDIL::DataUnitXmlParser::toXml(i.value(),valElem);
	}
	for(auto i=mTimers.begin();i!=mTimers.end();++i)
	{
		QDomElement timerElem=doc.createElement("timer");
		timersElem.appendChild(timerElem);
		QDomElement cfgElem=doc.createElement("config");
		timerElem.appendChild(cfgElem);
		timerElem.setAttribute("block_id",i.key());
		WLIOTVDIL::TimerBlockXmlParser::timerConfigToXml(i.value(),cfgElem);
	}
}
