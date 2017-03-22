#ifndef ARPCCONTINUOUSSTORAGE_H
#define ARPCCONTINUOUSSTORAGE_H

#include "ARpcLocalStorage/ARpcISensorStorage.h"
#include "ARpcLocalStorage/ARpcDBDriverFixedBlocks.h"
#include "ARpcLocalStorage/ARpcDBDriverChainedBlocks.h"

class ARpcContinuousStorage
	:public ARpcISensorStorage
{
public:
	explicit ARpcContinuousStorage(QObject *parent=0);
	virtual StoreMode getStoreMode()const override;
	virtual bool writeSensorValue(const ARpcISensorValue *val)override;
	bool createAsFixedBlocksDb(const QVector<quint32> &blockNotesSizes);//TODO переделать поприличнее!!
	bool createAsChainedBlocksDb();
	bool isFixesBlocksDb()const;
	bool isChainedBlocksDb()const;

protected:
	virtual bool openInternal()override;
	virtual void closeInternal()override;

private:
	bool writeSensorValueFB(const ARpcISensorValue *val);
	bool writeSensorValueCB(const ARpcISensorValue *val);

private:
	ARpcDBDriverFixedBlocks *fbDb;
	ARpcDBDriverChainedBlocks *cbDb;
	enum{FIXED_BLOCKS,CHAINED_BLOCKS}dbType;
	bool opened;
};

#endif // ARPCCONTINUOUSSTORAGE_H
