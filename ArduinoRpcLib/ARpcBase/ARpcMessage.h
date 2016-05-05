#ifndef ARPCMESSAGE_H
#define ARPCMESSAGE_H

#include <QString>
#include <QStringList>

class ARpcMessage
{
public:
	ARpcMessage();
	explicit ARpcMessage(const QString &t);
	ARpcMessage(const QString &t,const QStringList &a);

public:
	QString title;
	QStringList args;
};

#endif // ARPCMESSAGE_H
