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
	QString getVarSingle(const QString &name)const;

private:
	void parseCmdLine(const QStringList &arguments);
	void clearAll();

public:
	QStringList keys;
	QMap<QString,QStringList> vars;
	QStringList args;
};

#endif // CMDARGPARSER_H
