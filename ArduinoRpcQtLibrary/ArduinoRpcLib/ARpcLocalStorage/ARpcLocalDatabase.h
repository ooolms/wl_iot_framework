#ifndef ARPCLOCALDATABASE_H
#define ARPCLOCALDATABASE_H

#include <QObject>

class ARpcLocalDatabase
	:public QObject
{
	Q_OBJECT
public:
	explicit ARpcLocalDatabase(QObject *parent=0);
};

#endif // ARPCLOCALDATABASE_H
