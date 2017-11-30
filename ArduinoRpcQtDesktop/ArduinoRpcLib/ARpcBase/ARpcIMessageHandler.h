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

#ifndef ARPCIMESSAGEHANDLER_H
#define ARPCIMESSAGEHANDLER_H

#include <QObject>
#include "ARpcBase/ARpcMessage.h"

class ARpcIMessageHandler
	:public QObject
{
public:
	explicit ARpcIMessageHandler(QObject *parent=0):QObject(parent){}
	virtual ~ARpcIMessageHandler(){}
	virtual void processMessage(const ARpcMessage &m)=0;
};

#endif // ARPCIMESSAGEHANDLER_H
