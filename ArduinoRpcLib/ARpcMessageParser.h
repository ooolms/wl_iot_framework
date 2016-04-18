#ifndef ARPCMESSAGEPARSER_H
#define ARPCMESSAGEPARSER_H

#include "ARpcMessage.h"
#include "ARpcConfig.h"
#include <QObject>

class ARpcMessageParser
	:public QObject
{
	Q_OBJECT
public:
	explicit ARpcMessageParser(const ARpcConfig &cfg,QObject *parent=0);
	ARpcMessage parse(const QString &str)const;
	QString dump(const ARpcMessage &m)const;

private:
	ARpcConfig config;
};

#endif // ARPCMESSAGEPARSER_H
