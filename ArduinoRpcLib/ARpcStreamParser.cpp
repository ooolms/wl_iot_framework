#include "ARpcStreamParser.h"

ARpcStreamParser::ARpcStreamParser(QObject *parent)
	:QObject(parent)
{
}

void ARpcStreamParser::pushData(const QString &data)
{
	buffer.append(data);
	int index=buffer.indexOf('\n');
	while(index!=-1)
	{
		QString msg=buffer.mid(0,index);
		ARpcMessage m;
		m.parse(msg);
		emit processMessage(m);
		buffer.remove(0,index+1);
		index=buffer.indexOf('\n');
	}
}

void ARpcStreamParser::reset()
{
	buffer.clear();
}
