#ifndef ARPCCONTINUOUSSTORAGE_H
#define ARPCCONTINUOUSSTORAGE_H

#include "ARpcLocalStorage/ARpcISensorStorage.h"
#include "ARpcLocalStorage/ARpcDBDriverFixedBlocks.h"

class ARpcContinuousStorage
	:public ARpcISensorStorage
{
public:
	explicit ARpcContinuousStorage(QObject *parent=0);
	virtual StoreMode getStoreMode()const;

protected:
	virtual bool createInternal(const QString &path)override;
	virtual bool openInternal(const QString &path)override;

private:
	ARpcDBDriverFixedBlocks *db;
};

#endif // ARPCCONTINUOUSSTORAGE_H
