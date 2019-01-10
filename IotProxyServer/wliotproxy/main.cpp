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

int main(int argc,char *argv[])
{
	Q_INIT_RESOURCE(help);
	Q_INIT_RESOURCE(client);
	QCoreApplication app(argc,argv);
	IotClientCommandArgsParser parser(argc,argv);
	if(parser.getCommandStatus()==IotClientCommandArgsParser::DONE)return 0;
	else if(parser.getCommandStatus()==IotClientCommandArgsParser::ERROR)return 1;
	return app.exec();
}
