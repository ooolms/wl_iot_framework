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

#include <QCoreApplication>
#include "IotClientCommandArgsParser.h"

QtMessageHandler defaultMessageHandler;
bool showDebug=false;

void myMessageOutput(QtMsgType type,const QMessageLogContext &context,const QString &msg)
{
#if QT_VERSION >= QT_VERSION_CHECK(5,5,0)
	if((type==QtDebugMsg||type==QtInfoMsg)&&!showDebug)
#else
	if(type==QtDebugMsg&&!showDebug)
#endif
		return;
	defaultMessageHandler(type,context,msg);
}

int main(int argc,char *argv[])
{
	Q_INIT_RESOURCE(help);
	Q_INIT_RESOURCE(client);
	QCoreApplication app(argc,argv);
	showDebug=app.arguments().contains("-v");
	defaultMessageHandler=qInstallMessageHandler(&myMessageOutput);
	IotClientCommandArgsParser parser(argc,argv);
	if(parser.getCommandStatus()==IotClientCommandArgsParser::DONE)return 0;
	else if(parser.getCommandStatus()==IotClientCommandArgsParser::CONNECTION_ERROR)return 1;
	else if(parser.getCommandStatus()==IotClientCommandArgsParser::AUTHENTICATE_ERROR)return 2;
	else if(parser.getCommandStatus()==IotClientCommandArgsParser::ARGS_PARSING_ERROR)return 3;
	else if(parser.getCommandStatus()==IotClientCommandArgsParser::COMMAND_ERROR)return 4;
	return app.exec();
}
