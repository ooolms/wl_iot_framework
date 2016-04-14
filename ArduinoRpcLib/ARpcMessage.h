#ifndef ARPCMESSAGE_H
#define ARPCMESSAGE_H

#include <QString>
#include <QStringList>

class ARpcMessage
{
public:
	ARpcMessage();
	ARpcMessage(const QString &t,const QStringList &a);
	void parse(const QString &str);
	QString dump()const;

public:
	QString title;
	QStringList args;
};

#endif // ARPCMESSAGE_H
