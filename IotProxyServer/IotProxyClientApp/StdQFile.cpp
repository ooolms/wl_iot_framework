#include "StdQFile.h"
#include <unistd.h>
#include <QDebug>

StdQFile::StdQFile()
{
	mStdout=new QFile;
	mStdout->open(STDOUT_FILENO,QIODevice::WriteOnly);
	mStdin=new QFile;
	mStdin->open(STDIN_FILENO,QIODevice::ReadOnly);
	mStderr=new QFile;
	mStderr->open(STDERR_FILENO,QIODevice::WriteOnly);
}

StdQFile::~StdQFile()
{
	delete mStdout;
	delete mStdin;
	delete mStderr;
}

StdQFile& StdQFile::inst()
{
	static StdQFile instVar;
	return instVar;
}

QFile* StdQFile::stdout()
{
	return mStdout;
}

QFile* StdQFile::stdin()
{
	return mStdin;
}

QFile* StdQFile::stderr()
{
	return mStderr;
}

QDebug StdQFile::stdoutDebug()
{
	return QDebug(mStdout).noquote().nospace();
}

QDebug StdQFile::stderrDebug()
{
	return QDebug(mStderr).noquote().nospace();
}
