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

#include "CmdArgParser.h"

CmdArgParser::CmdArgParser(const QStringList &arguments)
{
	parseCmdLine(arguments.mid(1));
}

CmdArgParser::CmdArgParser(int argc,char *argv[])
{
	QStringList arguments;
	for(int i=1;i<argc;++i)
		arguments.append(QString::fromUtf8(argv[i]));
	parseCmdLine(arguments);
}

void CmdArgParser::clearAll()
{
	args.clear();
	keys.clear();
	vars.clear();
}

const QStringList& CmdArgParser::getArgs()const
{
	return args;
}

bool CmdArgParser::hasKey(const QString &k)const
{
	return keys.contains(k);
}

bool CmdArgParser::hasVar(const QString &k)const
{
	return vars.contains(k);
}

QStringList CmdArgParser::getVar(const QString &name)const
{
	if(!vars.contains(name))return QStringList();
	return vars[name];
}

QString CmdArgParser::getVarSingle(const QString &name)const
{
	if(!vars.contains(name))return QString();
	const QStringList &l=vars[name];
	if(l.isEmpty())return QString();
	return l[0];
}

const QStringList& CmdArgParser::getKeys()const
{
	return keys;
}

const QMap<QString,QStringList>& CmdArgParser::getVars()const
{
	return vars;
}

QStringList& CmdArgParser::getKeysToChange()
{
	return keys;
}

QStringList& CmdArgParser::getArgsToChange()
{
	return args;
}

QMap<QString,QStringList>& CmdArgParser::getVarsToChange()
{
	return vars;
}

void CmdArgParser::parseCmdLine(const QStringList &arguments)
{
	clearAll();
	bool onlyArgsLeft=false;
	for(QString arg:arguments)
	{
		if(onlyArgsLeft)
			args.append(arg);
		else if(arg=="--")
			onlyArgsLeft=true;
		else if(arg.startsWith("--"))
		{
			arg.remove(0,2);
			int pos=arg.indexOf("=");
			if(pos!=-1)
			{
				QString name=arg.left(pos);
				QString value=arg.remove(0,pos+1);
				if(!name.isEmpty())vars[name].append(value);
			}
			else if(!arg.isEmpty())vars[arg].append(QString());
		}
		else if(arg.startsWith("-"))
		{
			arg.remove(0,1);
			if(!arg.isEmpty())keys.append(arg);
		}
		else args.append(arg);
	}
}
