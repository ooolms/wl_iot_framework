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
	bool create(const QString &path,const QVector<quint32> &blockNotesSizes);

protected:
	virtual bool openInternal(const QString &path)override;

private:
	QVector<quint32> calculatedSizes;
	ARpcDBDriverFixedBlocks *db;
};

#endif // ARPCCONTINUOUSSTORAGE_H
