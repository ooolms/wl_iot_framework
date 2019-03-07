#ifndef QTUNITTESTCOLLECTION_H
#define QTUNITTESTCOLLECTION_H

#include "QtUnitTestSet.h"
#include <QStringList>

namespace QtUnit
{
	class QtUnitTestCollection
		:public QObject
	{
		Q_OBJECT
	public:
		QtUnitTestCollection(QObject *parent=0);
		QtUnitTestCollection(const QString &name,QObject *parent=0);
		virtual ~QtUnitTestCollection();

	public:
		void addTestCollection(QtUnitTestCollection *collection);
		void addTestSet(QtUnitTestSet *set);
		void runTests(const QStringList &disabledColls,const QStringList &disabledSets);
		const QList<QtUnitTestCollection*>& collections();
		const QList<QtUnitTestSet*>& sets();
		bool hasFails();
		bool processed();
		void skipCollection();

	signals:
		void collectionBegin();
		void collectionEnd();
		void collectionSkipped();

	public:
		QString collectionName;

	private:
		QList<QtUnitTestCollection*> childCollections;
		QList<QtUnitTestSet*> childSets;
		bool wasProcessed;
	};
}

#endif // QTUNITTESTCOLLECTION_H
