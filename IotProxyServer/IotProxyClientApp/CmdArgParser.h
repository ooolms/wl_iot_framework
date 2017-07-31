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
