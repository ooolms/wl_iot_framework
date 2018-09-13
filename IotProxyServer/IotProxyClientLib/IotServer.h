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

#ifndef IOTSERVER_H
#define IOTSERVER_H

#include <QObject>
#include "IotServerConnection.h"
#include "IotServerCommands.h"

class IotServer
	:public QObject
{
	Q_OBJECT
public:
	explicit IotServer(QObject *parent=nullptr);
	IotServerConnection* connection();
	IotServerCommands* commands();

private:
	IotServerConnection *conn;
	IotServerCommands *mCommands;
};

#endif // IOTSERVER_H
