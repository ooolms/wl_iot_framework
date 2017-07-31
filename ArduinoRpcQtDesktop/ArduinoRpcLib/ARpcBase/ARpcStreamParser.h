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

#ifndef ARPCSTREAMPARSER_H
#define ARPCSTREAMPARSER_H

#include "ARpcBase/ARpcMessage.h"
#include "ARpcBase/ARpcConfig.h"
#include "ARpcBase/ARpcMessageParser.h"
#include <QObject>

class ARpcStreamParser
	:public QObject
{
	Q_OBJECT
public:
	explicit ARpcStreamParser(QObject *parent=0);
	void pushData(const QString &data);
	void reset();

signals:
	void processMessage(const ARpcMessage &m);

private:
	QString buffer;
	ARpcMessageParser *msgParser;
};

#endif // ARPCSTREAMPARSER_H
