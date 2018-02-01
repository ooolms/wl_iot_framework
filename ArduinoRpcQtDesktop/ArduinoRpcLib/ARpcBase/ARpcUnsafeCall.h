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

#ifndef ARPCUNSAFECALL_H
#define ARPCUNSAFECALL_H

#include "ARpcBase/ARpcConfig.h"
#include "ARpcBase/ARpcMessage.h"
#include <QObject>

class ARpcRealDevice;

class ARpcUnsafeCall
	:public QObject
{
	Q_OBJECT
public:
	explicit ARpcUnsafeCall(QObject *parent=0);
	bool call(ARpcRealDevice *dev,const QByteArray &func,const QByteArrayList &args,QByteArrayList &retVal);
	bool call(ARpcRealDevice *dev,const QByteArray &func,QByteArrayList &retVal);
	void abort();

signals:
//	void unhandledMsg(const ARpcMessage &m);
	void abortInternal();
};

#endif // ARPCUNSAFECALL_H
