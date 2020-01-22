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

#ifndef VIRTUALDEVICE_H
#define VIRTUALDEVICE_H

#include "wliot/devices/RealDevice.h"

class VirtualDeviceBackend;

/**
 * @brief The VirtualDevice class
 * Виртуальное устройство. Идентификатор и имя задаются заранее,
 * сообщения к устройству обрабатываются вовне (сигнал messageToDevice),
 * сообщения от устройства так же генерируются извне (функция writeMsgFromDevice).
 * Устройство всегда подключено и идентифицировано.
 */
class VirtualDevice
	:public RealDevice
{
	Q_OBJECT

public:
	explicit VirtualDevice(const QUuid &id,const QByteArray &name,const QUuid &typeId=QUuid(),QObject *parent=nullptr);
	void* clientPtr();
	void setClientPtr(void* p);
	void setConnected(bool c);
	void emulateMessageFromDevice(const Message &m);

signals://device actions, to process send some messages to client and wait for answer
	void messageToDevice(const Message &m);

private:
	void setBackend(IHighLevelDeviceBackend *b);
	IHighLevelDeviceBackend* takeBackend();

private:
	VirtualDeviceBackend *virtualBackend;
	void *clientPointer;//указатель на какой-то объект, характеризующий клиента, зарегистрировавшего устройство
	//если не 0, виртуальное устройство занято
};

#endif // VIRTUALDEVICE_H
