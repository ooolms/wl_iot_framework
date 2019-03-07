#ifndef QTUNITMAIN
#define QTUNITMAIN

#include "QtUnitTestCollection.h"

class QCoreApplication;
class QApplication;

namespace QtUnit
{
	/**
	 * Cmd line argumentss:
	 *	-console - run in console
	 *	--dCol=<collection_name> - don't run tests collection
	 *	--dSet=<collection_name> - don't run tests set
	 *	--xml=<file_path> - write xml dump
	 *	--author=<author> - worker who runs tests (will be in dump)
	 *	--help - show this help
	*/
	class QtUnitMain
	{
	public:
		QtUnitMain(int argc,char **argv);
		int main();

	protected:
		virtual QtUnitTestCollection* makeCollection()=0;
		virtual QString project()=0;
		virtual bool createQAppInstForConsoleApps();//вместо QCoreApplication

	private:
		int argc;
		char **argv;
		bool helpMode;
		bool consoleMode;
		QStringList disabledColls,disabledSets;
		QString xmlDumpFile;
		QString author;
	};
}

#endif // QTUNITMAIN

