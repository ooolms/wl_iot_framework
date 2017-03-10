#ifndef ARPCCONTINUOUSSTORAGE_H
#define ARPCCONTINUOUSSTORAGE_H

#include "ARpcLocalStorage/ARpcISensorStorage.h"

class ARpcContinuousStorage
	:public ARpcISensorStorage
{
public:
	ARpcContinuousStorage();
	virtual StoreMode getStoreMode()const;

protected:
	virtual bool createInternal(const QString &path);
	virtual bool openInternal(const QString &path);
};

#endif // ARPCCONTINUOUSSTORAGE_H
