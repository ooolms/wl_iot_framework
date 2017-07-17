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
