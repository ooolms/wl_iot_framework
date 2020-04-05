#include "GDIL/xml/CommandBlockXmlParser.h"
#include "GDIL/blocks/CommandBlock.h"
#include "wliot/devices/StreamParser.h"

bool CommandBlockXmlParser::blockFromXml(BaseBlock *block,const QDomElement &blockElem)
{
	CommandBlock *b=(CommandBlock*)block;
	if(!blockElem.hasAttribute("dev_id")||!blockElem.hasAttribute("dev_name")||!blockElem.hasAttribute("cmd")||
		!blockElem.hasAttribute("args")||!blockElem.hasAttribute("inputs_count"))
		return false;
	QUuid devId=QUuid(blockElem.attribute("dev_id"));
	QByteArray devName=blockElem.attribute("dev_name").toUtf8();
	QByteArray cmd=blockElem.attribute("cmd").toUtf8();
	Message argsMsg;
	StreamParser::tryParse(WLIOTProtocolDefs::argDelim+blockElem.attribute("args").toUtf8()+
		WLIOTProtocolDefs::msgDelim,argsMsg);
	quint32 inputsCount=blockElem.attribute("inputs_count").toUInt();
	b->setParams(devId,devName,cmd,argsMsg.args,inputsCount);
	return true;
}

void CommandBlockXmlParser::blockToXml(const BaseBlock *block,QDomElement &blockElem)
{
	const CommandBlock *b=(const CommandBlock*)block;
	blockElem.setAttribute("dev_id",b->devId().toString());
	blockElem.setAttribute("dev_name",QString::fromUtf8(b->devName()));
	blockElem.setAttribute("cmd",QString::fromUtf8(b->cmd()));
	QByteArray args=Message::dump("",b->args()).mid(1);
	args.chop(1);
	blockElem.setAttribute("args",QString::fromUtf8(args));
	blockElem.setAttribute("inputs_count",b->inputsCount());
}
