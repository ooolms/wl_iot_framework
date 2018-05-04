#include "CmdArgParser.h"

CmdArgParser::CmdArgParser(const QStringList &arguments)
{
	parseCmdLine(arguments);
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

QStringList CmdArgParser::getArgs()
{
	return args;
}

QStringList CmdArgParser::getKeys()
{
	return keys;
}

const QMap<QString,QStringList>& CmdArgParser::getVars()
{
	return vars;
}

bool CmdArgParser::hasKey(const QString &key)
{
	return keys.contains(key);
}

QStringList CmdArgParser::getVar(const QString &name)
{
	if(vars.contains(name))return vars[name];
	return QStringList();
}

bool CmdArgParser::hasVar(const QString &name)
{
	return vars.contains(name);
}

void CmdArgParser::parseCmdLine(const QStringList &arguments)
{
	clearAll();
	for(QString arg:arguments)
	{
		if(arg.startsWith("--"))
		{
			arg.remove(0,2);
			int pos=arg.indexOf("=");
			QString name,value;
			if(pos!=-1)
			{
				name=arg.left(pos);
				value=arg.remove(0,pos+1);
			}
			else name=arg;
			vars[name].append(value);
		}
		else if(arg.startsWith("-"))
		{
			arg.remove(0,1);
			keys.append(arg);
		}
		else args.append(arg);
	}
}
