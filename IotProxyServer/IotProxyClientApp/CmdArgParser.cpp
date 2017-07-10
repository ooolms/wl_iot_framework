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

const QStringList& CmdArgParser::getArgs()const
{
	return args;
}

bool CmdArgParser::isKeySet(const QString &k)const
{
	return keys.contains(k);
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
	for(QString arg:arguments)
	{
		if(arg.startsWith("--"))
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
