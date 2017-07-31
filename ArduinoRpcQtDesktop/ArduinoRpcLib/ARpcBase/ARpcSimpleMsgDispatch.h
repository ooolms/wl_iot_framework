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

#ifndef ARPCSIMPLEMSGDISPATCH_H
#define ARPCSIMPLEMSGDISPATCH_H

#include "ARpcBase/ARpcDevice.h"
#include <QObject>

class ARpcSimpleMsgDispatch
	:public QObject
{
	Q_OBJECT
public:
	explicit ARpcSimpleMsgDispatch(ARpcDevice *dev,QObject *parent=0);

signals:
	void infoMsg(const QString &str);
	void measurementMsg(const QString &sensor,const QString &value);
	void commandStateChanged(const QString &command,int index,const QString &value);
	void additionalStateChanged(const QString &key,const QString &value);

private slots:
	void onRawMsg(const ARpcMessage &m);

private:
	ARpcDevice *device;
};

#endif // ARPCSIMPLEMSGDISPATCH_H
