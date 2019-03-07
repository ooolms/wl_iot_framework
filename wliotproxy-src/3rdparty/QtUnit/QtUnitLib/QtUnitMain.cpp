#include "QtUnitMain.h"
#include "QtUnitWidget.h"
#include "QtUnitConsoleRunner.h"
#include "CmdArgParser.h"
#include "QtUnitXmlDump.h"
#include <QApplication>
#include <QFile>

using namespace QtUnit;
static const char *helpStr="Cmd line arguments:\n"
	"\t-console - run in console\n"
	"\t--dCol=<collection_name> - don't run tests collection\n"
	"\t--dSet=<collection_name> - don't run tests set\n"
	"\t--xml=<file_path> - write xml dump\n"
	"\t--author=<author> - worker who runs tests (will be in dump)\n"
	"\t--help - show this help\n";

QtUnit::QtUnitMain::QtUnitMain(int argc,char **argv)
{
	this->argc=argc;
	this->argv=argv;
	CmdArgParser parser(argc,argv);
	helpMode=parser.hasVar("help");
	if(helpMode)return;
	consoleMode=parser.getKeys().contains("console");
	if(parser.hasVar("dCol"))disabledColls=parser.getVar("dCol");
	if(parser.hasVar("dSet"))disabledSets=parser.getVar("dSet");
	if(!parser.getVar("xml").isEmpty())xmlDumpFile=parser.getVar("xml")[0];
	if(!parser.getVar("author").isEmpty())author=parser.getVar("author")[0];
}

int QtUnitMain::main()
{
	if(helpMode)
	{
		printf("%s",helpStr);
		fflush(stdout);
		return 0;
	}
	int retVal=0;
	QtUnitTestCollection *coll;
	if(consoleMode)
	{
		if(createQAppInstForConsoleApps())
		{
			QApplication app(argc,argv);
			coll=makeCollection();
			QtUnitConsoleRunner consRunner(disabledColls,disabledSets,coll);
			consRunner.runTests();
		}
		else
		{
			QCoreApplication app(argc,argv);
			coll=makeCollection();
			QtUnitConsoleRunner consRunner(disabledColls,disabledSets,coll);
			consRunner.runTests();
		}
	}
	else
	{
		QApplication app(argc,argv);
		coll=makeCollection();
		QtUnitWidget wgt(disabledColls,disabledSets,coll);
		wgt.showMaximized();
		app.exec();
	}
	if(coll->hasFails())retVal=1;
	if(!xmlDumpFile.isEmpty())
	{
		QtUnitXmlDump dump;
		dump.dump(coll,xmlDumpFile,project(),author);
	}
	delete coll;
	return retVal;
}

bool QtUnitMain::createQAppInstForConsoleApps()
{
	return false;
}
