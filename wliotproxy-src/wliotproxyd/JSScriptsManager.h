#ifndef JSSCRIPTSMANAGER_H
#define JSSCRIPTSMANAGER_H

#include <QObject>
#include "JSDataProcessing/JSThread.h"
#include "AccessManagement/AccessPolicyTypes.h"

class JSScriptsManager
	:public QObject
{
	Q_OBJECT
public:
	explicit JSScriptsManager(QObject *parent=nullptr);
	virtual ~JSScriptsManager();
	QStringList scripts(IdType uid);
	bool startStopScript(IdType uid,const QString &scriptName,bool start);
	bool addScript(IdType uid,QString scriptName,const QByteArray &text);
	bool getScript(IdType uid,const QString &scriptName,QByteArray &text);
	bool removeScript(IdType uid,const QString &scriptName);
	bool updateScript(IdType uid,const QString &scriptName,const QByteArray &text);

private:
	QMap<IdType,QMap<QString,JSThread*>> scriptsMap;
};

#endif // JSSCRIPTSMANAGER_H
