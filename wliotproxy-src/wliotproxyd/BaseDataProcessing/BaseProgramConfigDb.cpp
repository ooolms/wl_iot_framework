#include "BaseProgramConfigDb.h"
#include <QDomDocument>
#include <QDomElement>
#include <QFile>
#include <QFileInfo>

BaseProgramConfigDb::BaseProgramConfigDb(const QString &programPath,const QByteArray &programId)
{
	mIsLoaded=false;
	mProgramId=programId;
	mProgramPath=programPath;
}

void BaseProgramConfigDb::setProgramName(const QByteArray &name)
{
	mProgramName=name;
	storeDb();
}

QString BaseProgramConfigDb::programPath()
{
	return mProgramPath;
}

QByteArray BaseProgramConfigDb::programName()
{
	return mProgramName;
}

QByteArray BaseProgramConfigDb::programId()
{
	return mProgramId;
}

void BaseProgramConfigDb::rmDb()
{
	QFile::remove(mProgramPath+".cfg");
}

bool BaseProgramConfigDb::isRunning()
{
	return mIsRunning;
}

void BaseProgramConfigDb::setRunning(bool r)
{
	if(mIsRunning==r)
		return;
	mIsRunning=r;
	storeDb();
}

void BaseProgramConfigDb::storeDb()
{
	if(!mIsLoaded)return;
	QDomDocument doc;
	QDomElement rootElem=doc.createElement("vdil_program_config");
	doc.appendChild(rootElem);
	rootElem.setAttribute("is_running",mIsRunning?"1":"0");
	rootElem.setAttribute("name",QString::fromUtf8(mProgramName));
	storeOther(rootElem);
	QFile file(mProgramPath+".cfg");
	if(!file.open(QIODevice::WriteOnly))return;
	file.write(doc.toByteArray());
	file.close();
}

void BaseProgramConfigDb::load()
{
	if(!QFile::exists(mProgramPath+".cfg"))
	{
		mIsLoaded=true;
		mProgramName=QFileInfo(mProgramPath).fileName().toUtf8();
		return;
	}
	QFile file(mProgramPath+".cfg");
	if(!file.open(QIODevice::ReadOnly))return;
	QByteArray data=file.readAll();
	file.close();
	mIsLoaded=true;
	QDomDocument doc;
	if(!doc.setContent(data))return;
	QDomElement rootElem=doc.firstChildElement("vdil_program_config");
	if(rootElem.isNull())return;
	mIsRunning=rootElem.attribute("is_running","0")!="0";
	mProgramName=rootElem.attribute("name").toUtf8();
	if(mProgramName.isEmpty())
		mProgramName=mProgramId;
	loadOther(rootElem);
}
