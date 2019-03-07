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

#ifndef IMESSAGEHANDLER_H
#define IMESSAGEHANDLER_H

#include <QObject>
#include "wliot/devices/Message.h"

class IMessageHandler
	:public QObject
{
public:
	explicit IMessageHandler(QObject *parent=0):QObject(parent){}
	virtual ~IMessageHandler(){}
	virtual void processMessage(const Message &m)=0;
};

#endif // IMESSAGEHANDLER_H
