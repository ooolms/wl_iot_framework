#include "DeviceConfig.h"

QString cmdActToString(CommandReaction a)
{
	if(a==CommandReaction::CMD_ANSWER_OK)
		return "ok";
	else if(a==CommandReaction::CMD_ANSWER_ERR)
		return "err";
	else if(a==CommandReaction::CMD_WAIT_MANUAL_ANSWER)
		return "manual";
	else if(a==CommandReaction::DEV_RESET)
		return "reset";
	else if(a==CommandReaction::DEV_STUCK)
		return "stuck";
	else if(a==CommandReaction::CMD_JS_EXEC)
		return "js";
	else return "ignore";
}

CommandReaction cmdActFromString(const QString &s)
{
	if(s=="ok")
		return CommandReaction::CMD_ANSWER_OK;
	else if(s=="err")
		return CommandReaction::CMD_ANSWER_ERR;
	else if(s=="manual")
		return CommandReaction::CMD_WAIT_MANUAL_ANSWER;
	else if(s=="reset")
		return CommandReaction::DEV_RESET;
	else if(s=="stuck")
		return CommandReaction::DEV_STUCK;
	else if(s=="js")
		return CommandReaction::CMD_JS_EXEC;
	else return CommandReaction::IGNORE;
}
