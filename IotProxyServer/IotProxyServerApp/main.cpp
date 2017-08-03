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

#include "IotProxyInstance.h"
#include <QCoreApplication>
#include <QDebug>

using namespace std;

int main(int argc,char **argv)
{
	QCoreApplication app(argc,argv);
	IotProxyInstance::inst().setup(argc,argv);
	if(IotProxyInstance::inst().terminated)return 0;
	int retVal=app.exec();
	IotProxyInstance::inst().terminate();
	return retVal;
}
