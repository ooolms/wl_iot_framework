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

#ifndef IOTKITAGENTCOMMANDSOURCE_H
#define IOTKITAGENTCOMMANDSOURCE_H

#include "IExternCommandSource.h"
#include <QUdpSocket>

class IotkitAgentCommandSource
	:public IExternCommandSource
{
	Q_OBJECT
public:
	explicit IotkitAgentCommandSource(QObject *parent=0);

private slots:
	void onDataFromCommandSocket();

private:
	void parseCommand(const QByteArray &data);

private:
	QUdpSocket commandReadSock;
};

#endif // IOTKITAGENTCOMMANDSOURCE_H
