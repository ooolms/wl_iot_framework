/*******************************************
Copyright 2017 OOO "LMS"

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

	http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

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
