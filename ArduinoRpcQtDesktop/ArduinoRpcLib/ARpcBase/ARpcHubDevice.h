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

#ifndef ARPCHUBDEVICE_H
#define ARPCHUBDEVICE_H

#include "ARpcRealDevice.h"

class ARpcHubDevice
	:public ARpcRealDevice
{
	Q_OBJECT
public:
	explicit ARpcHubDevice(const QUuid id,const QByteArray &name,ARpcRealDevice *parent);
	virtual bool writeMsg(const ARpcMessage &m)override;
	virtual bool isConnected()override;
	void setSelfConnected(bool c);
	void onRawMessage(const ARpcMessage &m);

private slots:
	void onParentDisconnected();

private:
	ARpcRealDevice *parentDevice;
	bool connected;
};

#endif // ARPCHUBDEVICE_H
