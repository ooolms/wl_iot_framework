#ifndef ARPCMESSAGEPARSER_H
#define ARPCMESSAGEPARSER_H

#include "ARpcBase/ARpcMessage.h"
#include "ARpcBase/ARpcConfig.h"
#include <QObject>

class ARpcMessageParser
	:public QObject
{
	Q_OBJECT
public:
	explicit ARpcMessageParser(QObject *parent=0);
	ARpcMessage parse(const QString &str)const;
	QString dump(const ARpcMessage &m)const;
};

#endif // ARPCMESSAGEPARSER_H
