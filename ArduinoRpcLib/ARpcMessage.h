#ifndef ARPCMESSAGE_H
#define ARPCMESSAGE_H

#include <QString>
#include <QStringList>

class ARpcMessage
{
public:
	ARpcMessage();
	void parse(const QString &str);
	QString dump();

public:
	QString title;
	QStringList args;
};

#endif // ARPCMESSAGE_H
