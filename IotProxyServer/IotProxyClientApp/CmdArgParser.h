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
	const QStringList& getKeys()const;//starts with -
	const QStringList& getArgs()const;
	bool hasKey(const QString &k)const;
	bool hasVar(const QString &k)const;
	QStringList getVar(const QString &name)const;
	QString getVarSingle(const QString &name)const;
	const QMap<QString,QStringList>& getVars()const;//starts with --
	QStringList& getKeysToChange();
	QStringList& getArgsToChange();
	QMap<QString,QStringList>& getVarsToChange();

private:
	void parseCmdLine(const QStringList &arguments);
	void clearAll();

private:
	QStringList keys;
	QMap<QString,QStringList> vars;
	QStringList args;
};

#endif // CMDARGPARSER_H
