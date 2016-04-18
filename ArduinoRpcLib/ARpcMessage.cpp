#include "ARpcMessage.h"

ARpcMessage::ARpcMessage()
{
}

ARpcMessage::ARpcMessage(const QString &t,const QStringList &a)
{
	title=t;
	args=a;
}
