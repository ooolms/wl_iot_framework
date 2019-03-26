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

#ifndef HUBDEVICE_H
#define HUBDEVICE_H

#include "wliot/devices/RealDevice.h"

class HubDevice
	:public RealDevice
{
	Q_OBJECT
public:
	explicit HubDevice(const QUuid id,const QByteArray &name,RealDevice *parent);
	virtual bool writeMsgToDevice(const Message &m)override;
	void setSelfConnected(bool c);
	virtual QByteArray deviceType(){return "hub";}

signals:
	void internalSyncFailed();

protected:
	virtual void syncFailed()override;

private slots:
	void onParentConnected();
	void onParentDisconnected();

private:
	RealDevice *parentDevice;
	bool mSelfConnected;
};

#endif // HUBDEVICE_H