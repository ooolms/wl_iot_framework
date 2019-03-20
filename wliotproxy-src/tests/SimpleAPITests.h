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

#ifndef SIMPLEAPITESTS_H
#define SIMPLEAPITESTS_H

#include "QtUnitTestSet.h"
#include "FakeDevice.h"

//need TestSketch2 on Ardunio on /dev/ttyACM0
class SimpleAPITests
	:public QtUnit::QtUnitTestSet
{
	Q_OBJECT
public:
	explicit SimpleAPITests(QObject *parent=0);
	virtual bool init();
	virtual void cleanup();
	virtual bool testInit();
	void testOk();
	void testErr();
	void testLongCommand();
	void testSimpleMsgDispatch();
	void testDevResetWhenCall();

private:
	FakeDevice *device;
};

#endif // SIMPLEAPITESTS_H
