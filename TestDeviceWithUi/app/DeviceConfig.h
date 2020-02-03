#ifndef DEVICECONFIG_H
#define DEVICECONFIG_H

#include <QMap>
#include <functional>
#include "DeviceState.h"

enum class CommandReaction
{
	CMD_ANSWER_OK,
	CMD_ANSWER_ERR,
	CMD_WAIT_MANUAL_ANSWER,
	CMD_JS_EXEC,
	DEV_STUCK,
	DEV_RESET,
	IGNORE
};

struct CommandReactionConfig
{
	CommandReaction act;
	QByteArrayList retVal;
	DeviceStateMap stateChangeBeforeAnswer;
	DeviceStateMap stateChangeAfterAnswer;
	QString jsScript;
};

QString cmdActToString(CommandReaction a);
CommandReaction cmdActFromString(const QString &s);

#endif // DEVICERCONFIG_H
