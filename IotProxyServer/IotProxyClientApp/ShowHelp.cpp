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

#include "ShowHelp.h"
#include "StdQFile.h"
#include <QFile>
#include <QDebug>

void ShowHelp::showHelp(const QString &section,const QString &title)
{
	QString fileName;
	if(title.isEmpty())
		fileName=":/help/main.txt";
	else
	{
		if(section.isEmpty())
			fileName=":/help/help."+title+".txt";
		else fileName=":/help/"+section+"/help."+title+".txt";
	}
	QFile file(fileName);
	if(!file.open(QIODevice::ReadOnly))
	{
		StdQFile::inst().stderrDebug()<<"No help for "<<section<<":"<<title<<"\n";
		return;
	}
	StdQFile::inst().stdoutDebug()<<QString::fromUtf8(file.readAll());
	file.close();
}
