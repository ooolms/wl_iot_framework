#ifndef QTUNITXMLDUMP_H
#define QTUNITXMLDUMP_H

#include "QtUnitTestCollection.h"
#include <QObject>

class QDomDocument;
class QDomElement;

namespace QtUnit
{
	class QtUnitXmlDump
		:public QObject
	{
		Q_OBJECT
	public:
		explicit QtUnitXmlDump(QObject *parent=0);
		void dump(QtUnitTestCollection *coll,const QString &fileName,
			const QString &project=QString(),const QString &author=QString());

	private:
		void dumpCollectionRecursive(QtUnitTestCollection *coll,QDomDocument &doc,QDomElement &parent);
	};
}

#endif // QTUNITXMLDUMP_H
