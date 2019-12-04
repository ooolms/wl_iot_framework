#ifndef COMMANDSREACTIONCONFIG_H
#define COMMANDSREACTIONCONFIG_H

#include <QObject>

class CommandsReactionConfig
	:public QObject
{
	Q_OBJECT
public:
	enum Reaction
	{
		AUTO_OK,
		AUTO_ERR,
		STUCK,
		IGNORE,
		WAIT_MANUAL_ANSWER
	};

public:
	explicit CommandsReactionConfig(QObject *parent=nullptr);
	void setCommandReaction(const QByteArray &cmd,Reaction r,const QByteArrayList &autoAnswerArgs=QByteArrayList());

signals:
	void commandForManualReaction(const char *cmd,const char **args,unsigned long argsCount);
};

#endif // COMMANDSREACTIONCONFIG_H
