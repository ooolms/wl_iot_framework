#ifndef QTUNITCONSOLERUNNER_H
#define QTUNITCONSOLERUNNER_H

#include "QtUnitTestCollection.h"
#include <QObject>
#include <QFile>
#include <QDebug>

//TODO with levels - auto indent on entering collection and set from stdline args (-indent)
namespace QtUnit
{
	class QtUnitConsoleRunner
		:public QObject
	{
		Q_OBJECT
	public:
		explicit QtUnitConsoleRunner(const QStringList &disabledColls,const QStringList &disabledSets,
			QtUnitTestCollection *coll,QObject *parent=0);
		void runTests();
		bool hasFails();

	private slots:
		void onCollectionBegin();
		void onCollectionEnd();
		void onCollectionSkipped();
		void onSetBegin();
		void onSetEnd();
		void onSetSkipped();
		void onTestBegin(int testId);
		void onTestEnd(int testId,bool result,const QString &message,const QString &log);

	private:
		void setupCollection(QtUnitTestCollection *col);
		void setupSet(QtUnitTestSet *set);

	private:
		QtUnitTestCollection *collection;
		int nextId;
		QStringList dColls,dSets;
		QFile stdoutFile;
		QDebug stdoutDbg;
	};
}

#endif // QTUNITCONSOLERUNNER_H
