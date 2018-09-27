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

#ifndef ARPCOUTSIDEDEVICEWRAP_H
#define ARPCOUTSIDEDEVICEWRAP_H

#include "ARpcBase/ARpcOutsideDevice.h"
#include "ARpcBase/ARpcRealDevice.h"
#include <QUuid>

class ARpcOutsideDeviceWrap
	:public ARpcRealDevice
{
	Q_OBJECT
public:
	explicit ARpcOutsideDeviceWrap(const QByteArray &devIdOrName,ARpcOutsideDevice *srvSocketDev,QObject *parent=0);
	void queryDeviceId();
	bool listControls(ARpcControlsGroup &controls);
	virtual QByteArray deviceType(){return "wrap";}

public:
	virtual bool writeMsg(const ARpcMessage &m)override;
	virtual bool isConnected()override;

private slots:
	void onRawMessage(const ARpcMessage &m);

private:
	bool execSrvCommand(const QByteArray &cmd,const QByteArrayList &args,QByteArrayList &retVal);

private:
	QByteArray deviceIdOrName;
	QUuid deviceId;
	QMap<QByteArray,QByteArray> callIdsMap;//callIdToServer -> callIdFromUi
	ARpcOutsideDevice *srvSock;
};

#endif // ARPCOUTSIDEDEVICEWRAP_H
