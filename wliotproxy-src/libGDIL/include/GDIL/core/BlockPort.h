#ifndef BLOCKPORT_H
#define BLOCKPORT_H

#include "GDIL/core/DataUnit.h"

class BlockPort
{
public:
	virtual ~BlockPort();

	//for program editing
	virtual DataUnit::Type type()const=0;
	virtual quint32 dim()const=0;
	const QString& title()const;

protected:
	QString mTitle;
};

#endif // BLOCKPORT_H
