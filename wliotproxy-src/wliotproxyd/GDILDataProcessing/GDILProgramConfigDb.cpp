#include "GDILProgramConfigDb.h"
#include "GDIL/xml/DataUnitXmlParser.h"
#include "GDIL/xml/TimerBlockXmlParser.h"
#include <QDomDocument>
#include <QDomElement>
#include <QFile>

GDILProgramConfigDb::GDILProgramConfigDb(const QString &programPath)
{
	mDbPath=programPath+".cfg";
	QFile file(mDbPath);
	if(!file.open(QIODevice::ReadOnly))return;
	QByteArray data=file.readAll();
	file.close();
	QDomDocument doc;
	if(!doc.setContent(data))return;
	QDomElement rootElem=doc.firstChildElement("gdil_program_config");
	if(rootElem.isNull())return;
	mIsRunning=rootElem.attribute("is_running","0")!="0";
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
		DataUnit v;
		if(!DataUnitXmlParser::fromXml(v,valElem))continue;
		configOptions[{blockId,key}]=v;
	}
	for(int i=0;i<timersElem.childNodes().count();++i)
	{
		QDomElement timerElem=timersElem.childNodes().at(i).toElement();
		if(timerElem.isNull()||timerElem.nodeName()!="timer")continue;
		QDomElement cfgElem=timerElem.firstChildElement("config").toElement();
		if(cfgElem.isNull())continue;
		quint32 blockId=timerElem.attribute("block_id").toUInt();
		TimerBlock::TimerConfig cfg;
		if(!TimerBlockXmlParser::timerConfigFromXml(cfg,cfgElem))continue;
		timers[blockId]=cfg;
	}
}

void GDILProgramConfigDb::load(Program *p)
{
	for(auto i=configOptions.begin();i!=configOptions.end();++i)
		p->setConfigOptionValue(i.key(),i.value());
	for(auto i=timers.begin();i!=timers.end();++i)
	{
		TimerBlock *b=p->timerBlocks().value(i.key());
		if(b&&b->configurable())
			b->setConfig(i.value(),true);
	}
}

void GDILProgramConfigDb::cleanup(Program *p)
{
	bool changed=false;
	for(auto i=configOptions.begin();i!=configOptions.end();++i)
	{
		if(!p->allConfigOptions().contains(i.key()))
		{
			i=configOptions.erase(i);
			--i;
			changed=true;
		}
	}
	for(auto i=timers.begin();i!=timers.end();++i)
	{
		TimerBlock *b=p->timerBlocks().value(i.key());
		if(!b||!b->configurable())
		{
			i=timers.erase(i);
			--i;
			changed=true;
		}
	}
	if(changed)storeDb();
}

void GDILProgramConfigDb::setConfigOption(const ConfigOptionId &id,const DataUnit &v)
{
	configOptions[id]=v;
	storeDb();
}

void GDILProgramConfigDb::setTimerConfig(quint32 blockId,const TimerBlock::TimerConfig &cfg)
{
	if(!timers.contains(blockId))
		return;
	timers[blockId]=cfg;
	storeDb();
}

QString GDILProgramConfigDb::dbPath()
{
	return mDbPath;
}

bool GDILProgramConfigDb::isRunning()
{
	return mIsRunning;
}

void GDILProgramConfigDb::setRunning(bool r)
{
	if(mIsRunning==r)
		return;
	mIsRunning=r;
	storeDb();
}

void GDILProgramConfigDb::storeDb()
{
	QDomDocument doc;
	QDomElement rootElem=doc.createElement("gdil_program_config");
	doc.appendChild(rootElem);
	rootElem.setAttribute("is_running",mIsRunning?"1":"0");
	QDomElement configOptionsElem=doc.createElement("config_options");
	rootElem.appendChild(configOptionsElem);
	QDomElement timersElem=doc.createElement("timers");
	rootElem.appendChild(timersElem);
	for(auto i=configOptions.begin();i!=configOptions.end();++i)
	{
		QDomElement cfgElem=doc.createElement("option");
		configOptionsElem.appendChild(cfgElem);
		QDomElement valElem=doc.createElement("value");
		cfgElem.appendChild(valElem);
		cfgElem.setAttribute("block_id",i.key().blockId);
		cfgElem.setAttribute("key",QString::fromUtf8(i.key().key));
		DataUnitXmlParser::toXml(i.value(),valElem);
	}
	for(auto i=timers.begin();i!=timers.end();++i)
	{
		QDomElement timerElem=doc.createElement("timer");
		timersElem.appendChild(timerElem);
		QDomElement cfgElem=doc.createElement("config");
		timerElem.appendChild(cfgElem);
		timerElem.setAttribute("block_id",i.key());
		TimerBlockXmlParser::timerConfigToXml(i.value(),cfgElem);
	}
	QFile file(mDbPath);
	if(!file.open(QIODevice::WriteOnly))return;
	file.write(doc.toByteArray());
	file.close();
}
