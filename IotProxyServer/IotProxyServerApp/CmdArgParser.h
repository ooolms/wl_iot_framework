#ifndef CMDARGPARSER_H
#define CMDARGPARSER_H

#include <QByteArray>
#include <QList>
#include <QMap>
#include <QStringList>

class CmdArgParser
{
public:
	CmdArgParser(const QStringList &arguments);
	CmdArgParser(int argc,char *argv[]);
	QStringList getKeys()const;//starts with -
	QStringList getArgs()const;
	bool isKeySet(const QString &k)const;
	QStringList getVar(const QString &name)const;
	QString getVarSingle(const QString &name)const;
	const QMap<QString,QStringList>& getVars()const;//starts with --

private:
	void parseCmdLine(const QStringList &arguments);
	void clearAll();

private:
	QStringList keys;
	QMap<QString,QStringList> vars;
	QStringList args;
};

#endif // CMDARGPARSER_H
