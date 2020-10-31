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

#include "RemoveDirRecusive.h"
#include <QFileInfo>

bool RemoveDirRecusive::rmDirRec(QDir dir)
{
	if(!dir.exists())return false;
	QStringList entries=dir.entryList(QDir::Files|QDir::Hidden|QDir::System);
	for(int i=0;i<entries.count();++i)if(!dir.remove(entries[i]))return false;
	entries=dir.entryList(QDir::Dirs|QDir::NoDotAndDotDot);
	for(int i=0;i<entries.count();++i)
	{
		QDir chDir=dir;
		if(!chDir.cd(entries[i]))return false;
		if(!rmDirRec(chDir))return false;
	}
	return dir.rmdir(dir.absolutePath());
}

bool RemoveDirRecusive::rmDirContentsRec(QDir dir)
{
	if(!dir.exists())return false;
	QStringList entries=dir.entryList(QDir::Files|QDir::Hidden|QDir::System);
	for(int i=0;i<entries.count();++i)if(!dir.remove(entries[i]))return false;
	entries=dir.entryList(QDir::Dirs|QDir::NoDotAndDotDot);
	for(int i=0;i<entries.count();++i)
	{
		QDir chDir=dir;
		if(!chDir.cd(entries[i]))return false;
		if(!rmDirRec(chDir))return false;
	}
	return true;
}
