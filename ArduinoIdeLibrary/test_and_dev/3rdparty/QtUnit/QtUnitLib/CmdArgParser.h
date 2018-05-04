#ifndef CMDARGPARSER_H
#define CMDARGPARSER_H

#include <QByteArray>
#include <QList>
#include <QStringList>
#include <QMap>

struct CmdVar
{
	CmdVar(){}
	CmdVar(const QString &n,const QString &v)
	{
		name=n;
		values.append(v);
	}
	CmdVar(const QString &n,const QStringList &vs)
	{
		name=n;
		values=vs;
	}

public:
	QString name;
	QStringList values;
};

/**
  Разбор командной строки по следующим условиям:
  1. Строки, начинающиеся с дефиса, считаются ключами (keys), могут содежать больше одной буквы
		-str -c -f -qweqwe_asdasd
  2. Строки, начинающиеся с двух дефисов, считаются парой строк ключ-значение, разделенных символом "=".
	Допускается несколько одинаковых ключей, значения будут помещены в список
		--arg1= --arg2=1 --arg2=qweqwe1 --arg3=zxc=asd --arg1
		Результат - (arg1=>("",""),arg2=>("1","qweqwe1"),arg3=>("zxc=asd"))
*/
class CmdArgParser
{
public:
	CmdArgParser(const QStringList &arguments);
	CmdArgParser(int argc,char *argv[]);
	QStringList getKeys();//starts with -
	QStringList getArgs();
	const QMap<QString,QStringList>& getVars();//starts with --
	bool hasKey(const QString &key);
	QStringList getVar(const QString &name);
	bool hasVar(const QString &name);

private:
	void parseCmdLine(const QStringList &arguments);
	void clearAll();

private:
	QStringList keys;
	QMap<QString,QStringList> vars;
	QStringList args;
};

#endif // CMDARGPARSER_H
