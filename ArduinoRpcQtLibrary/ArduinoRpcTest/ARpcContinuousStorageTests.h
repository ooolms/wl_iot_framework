#ifndef ARPCCONTINUOUSSTORAGETESTS_H
#define ARPCCONTINUOUSSTORAGETESTS_H

#include "QtUnitTestSet.h"

//TODO !!! packets
class ARpcContinuousStorageTests
	:public QtUnit::QtUnitTestSet
{
	Q_OBJECT
public:
	explicit ARpcContinuousStorageTests(QObject *parent=0);
	void testStorageSingleDontTouchTime();
	void testStorageSingleAddGT();
	void testStorageSingleLTDontTouchTime();
	void testStorageSingleLTAddGT();
	void testStorageSingleLTDropTime();
	void testStorageSingleGTDontTouchTime();
	void testStorageSingleGTDropTime();
	void testStoragePacketDontTouchTimeFixedBlocks();
	void testStoragePacketGTDontTouchTimeFixedBlocks();
	void testStoragePacketGTDontTouchTimeChainedBlocks();
	virtual bool init();
	virtual void cleanup();

signals:

public slots:
};

#endif // ARPCCONTINUOUSSTORAGETESTS_H
