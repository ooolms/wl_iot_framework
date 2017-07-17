#ifndef STDQFILE_H
#define STDQFILE_H

#include <QFile>

class StdQFile
{
private:
	StdQFile();
	~StdQFile();

public:
	static StdQFile& inst();
	QFile* stdout();
	QFile* stdin();
	QFile* stderr();
	QDebug stdoutDebug();
	QDebug stderrDebug();

private:
	QFile *mStdin,*mStderr,*mStdout;
};

#endif // STDQFILE_H
