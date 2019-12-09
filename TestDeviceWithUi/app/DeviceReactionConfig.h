#ifndef DEVICEREACTIONCONFIG_H
#define DEVICEREACTIONCONFIG_H

#include <QObject>
#include <QMap>

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
		IGNORE
	};
	struct CommandCfg
	{
		Reaction act;
		QByteArrayList retVal;
	};

public:
	explicit DeviceReactionConfig(QObject *parent=nullptr);

signals:
	void commandForManualReaction(const char *cmd,const char **args,unsigned long argsCount,CommandCfg &reaction);

public:
	QMap<QByteArray,CommandCfg> commandReactions;
};

#endif // DEVICEREACTIONCONFIG_H
