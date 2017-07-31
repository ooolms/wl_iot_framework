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

#ifndef ARPCCONTROLUIELEMENT_H
#define ARPCCONTROLUIELEMENT_H

#include "ARpcBase/ARpcMessage.h"
#include <QObject>

class ARpcControlUiElement
	:public QObject
{
	Q_OBJECT
public:
	explicit ARpcControlUiElement(QObject *parent=0);
	virtual QWidget* widget()=0;
	virtual bool isGroup()const{return false;}
	virtual bool isCommand()const{return false;}

signals:
	void executeCommand(const QString &cmd,const QStringList &args,bool syncCall);
};

#endif // ARPCCONTROLUIELEMENT_H
