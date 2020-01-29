#ifndef DEVICEREACTIONCONFIG_H
#define DEVICEREACTIONCONFIG_H

#include <QObject>
#include <QMap>
#include <functional>
#include "DeviceState.h"

class Device;

class DeviceReactionConfig
	:public QObject
{
	Q_OBJECT
public:
	enum Reaction
	{
		CMD_ANSWER_OK,
		CMD_ANSWER_ERR,
		CMD_WAIT_MANUAL_ANSWER,
		DEV_STUCK,
		DEV_RESET,
		IGNORE
	};
	struct CommandCfg
	{
		Reaction act;
		QByteArrayList retVal;
		DeviceStateMap stateChangeBeforeAnswer;
		DeviceStateMap stateChangeAfterAnswer;
	};

public:
	explicit DeviceReactionConfig(QObject *parent=nullptr);

public:
	QMap<QByteArray,CommandCfg> commandReactions;
};

#endif // DEVICEREACTIONCONFIG_H
