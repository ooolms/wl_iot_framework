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

#ifndef FAKEDEVICE_H
#define FAKEDEVICE_H

#include "ARpcBase/ARpcDevice.h"

class FakeDevice
	:public ARpcDevice
{
	Q_OBJECT
public:
	explicit FakeDevice(QObject *parent=0);
	void msgFromDevice(const ARpcMessage &m);

public:
	virtual bool writeMsg(const ARpcMessage &m)override;
	virtual bool isConnected()override;

private:
	QUuid devId;
};

#endif // FAKEDEVICE_H
