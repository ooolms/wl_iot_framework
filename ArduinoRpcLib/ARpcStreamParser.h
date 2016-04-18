#ifndef ARPCSTREAMPARSER_H
#define ARPCSTREAMPARSER_H

#include "ARpcMessage.h"
#include "ARpcConfig.h"
#include "ARpcMessageParser.h"
#include <QObject>

class ARpcStreamParser
	:public QObject
{
	Q_OBJECT
public:
	explicit ARpcStreamParser(const ARpcConfig &cfg,ARpcMessageParser *mParser,QObject *parent=0);
	void pushData(const QString &data);
	void reset();

signals:
	void processMessage(const ARpcMessage &m);

private:
	QString buffer;
	ARpcConfig config;
	ARpcMessageParser *msgParser;
};

#endif // ARPCSTREAMPARSER_H
