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

#ifndef ARPCSYNCCALL_H
#define ARPCSYNCCALL_H

#include "ARpcBase/ARpcConfig.h"
#include "ARpcBase/ARpcMessage.h"
#include <QObject>

class ARpcRealDevice;

class ARpcSyncCall
	:public QObject
{
	Q_OBJECT
public:
	explicit ARpcSyncCall(QObject *parent=0);
	bool call(ARpcRealDevice *dev,const QString &func,const QStringList &args,QStringList &retVal);
	bool call(ARpcRealDevice *dev,const QString &func,QStringList &retVal);
	void abort();

signals:
//	void unhandledMsg(const ARpcMessage &m);
	void abortInternal();//for internal use
};

#endif // ARPCSYNCCALL_H
