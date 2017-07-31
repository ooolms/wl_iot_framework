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

#ifndef IEXTERNCOMMANDSOURCE_H
#define IEXTERNCOMMANDSOURCE_H

#include <QStringList>
#include <QObject>

class IExternCommandSource
	:public QObject
{
public:
	explicit IExternCommandSource(QObject *parent=0);
	virtual ~IExternCommandSource(){}

protected:
	bool execCommand(const QString &devIdOrName,const QString &command,
		const QStringList &arguments,QStringList &retVal);
};

#endif // IEXTERNCOMMANDSOURCE_H
