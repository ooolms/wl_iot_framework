#ifndef ICOMMANDCALLBACK_H
#define ICOMMANDCALLBACK_H

#include <QUuid>
#include <QByteArrayList>

class ICommandCallback
{
public:
	virtual ~ICommandCallback(){}
	virtual void commandCallback(const QUuid &devId,const QByteArray &cmd,const QByteArrayList &args)=0;
};

#endif // ICOMMANDCALLBACK_H
