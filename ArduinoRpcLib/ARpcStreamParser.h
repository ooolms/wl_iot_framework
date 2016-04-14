#ifndef ARPCSTREAMPARSER_H
#define ARPCSTREAMPARSER_H

#include "ARpcMessage.h"
#include <QObject>

class ARpcStreamParser
	:public QObject
{
	Q_OBJECT
public:
	explicit ARpcStreamParser(QObject *parent=0);
	void pushData(const QString &data);
	void reset();

signals:
	void processMessage(const ARpcMessage &m);

private:
	QString buffer;
};

#endif // ARPCSTREAMPARSER_H
