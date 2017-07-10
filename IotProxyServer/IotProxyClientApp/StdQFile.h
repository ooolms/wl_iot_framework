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

private:
	QFile *mStdin,*mStderr,*mStdout;
};

#endif // STDQFILE_H
