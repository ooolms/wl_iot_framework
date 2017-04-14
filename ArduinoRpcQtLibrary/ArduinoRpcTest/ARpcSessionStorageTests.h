#ifndef ARPCSESSIONSTORAGETESTS_H
#define ARPCSESSIONSTORAGETESTS_H

#include "QtUnitTestSet.h"

class ARpcSessionStorageTests
	:public QtUnit::QtUnitTestSet
{
	Q_OBJECT
public:
	explicit ARpcSessionStorageTests(QObject *parent=0);
	void testStorageSingleDontTouchTime();
	void testStorageSingleLTDontTouchTime();
	virtual bool init();
	virtual void cleanup();

signals:

public slots:
};

#endif // ARPCSESSIONSTORAGETESTS_H
