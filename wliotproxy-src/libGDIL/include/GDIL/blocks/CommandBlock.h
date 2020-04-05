#ifndef COMMANDBLOCK_H
#define COMMANDBLOCK_H

#include "GDIL/core/BaseBlock.h"

class CommandBlock
	:public BaseBlock
{
public:
	explicit CommandBlock(quint32 bId=0);
	void setParams(const QUuid &devId,const QByteArray &devName,const QByteArray &cmd,
		const QByteArrayList &args,quint32 inCount);
	virtual QUuid typeId()const override;
	const QUuid& devId()const;
	const QByteArray& devName()const;
	const QByteArray& cmd()const;
	const QByteArrayList& args()const;
	quint32 inputsCount()const;

protected:
	virtual void eval()override;

public:
	static const QUuid mTypeId;

private:
	QUuid mDevId;
	QByteArray mDevName;
	QByteArray mCmd;
	QByteArrayList mArgs;
	quint32 mInputsCount;
};

#endif // COMMANDBLOCK_H
