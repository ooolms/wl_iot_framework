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
