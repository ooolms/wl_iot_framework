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

#ifndef ARPCMESSAGEPARSER_H
#define ARPCMESSAGEPARSER_H

#include "ARpcBase/ARpcMessage.h"
#include "ARpcBase/ARpcConfig.h"
#include <QObject>

class ARpcMessageParser
	:public QObject
{
	Q_OBJECT
public:
	explicit ARpcMessageParser(QObject *parent=0);
	ARpcMessage parse(const QString &str)const;
	QString dump(const ARpcMessage &m)const;
};

#endif // ARPCMESSAGEPARSER_H
