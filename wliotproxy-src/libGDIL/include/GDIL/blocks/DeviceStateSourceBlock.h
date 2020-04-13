#ifndef DEVICESTATESOURCEBLOCK_H
#define DEVICESTATESOURCEBLOCK_H

#include "GDIL/core/SourceBlock.h"

class DeviceStateSourceBlock
	:public SourceBlock
{
public:
	virtual QString groupName()const override;
	virtual QString blockName()const override;

protected:
	virtual DataUnit extractDataInternal()override;

public:
	static const QString mBlockName;

private:
	QUuid mDevId;
	QString mDevName;
	QByteArray mStateKey;
	bool mCmdState;
	bool mBoolOut;
	quint32 mCmdStateIndex;
};

#endif // DEVICESTATESOURCEBLOCK_H
