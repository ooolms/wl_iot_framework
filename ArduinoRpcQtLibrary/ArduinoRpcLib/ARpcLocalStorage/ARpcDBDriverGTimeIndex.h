#ifndef ARPCDBDRIVERGTIMEINDEX_H
#define ARPCDBDRIVERGTIMEINDEX_H

#include <QObject>
#include "ARpcDBDriverFixedBlocks.h"

class ARpcDBDriverGTimeIndex
	:public QObject
{
	Q_OBJECT
public:
	explicit ARpcDBDriverGTimeIndex(QObject *parent=0);
	bool create(const QString &filePath);
	bool open(const QString &filePath);
	bool isOpened();
	void close();

private:
	void loadIndex();

private:
	ARpcDBDriverFixedBlocks *dbDriver;
	QVector<qint64> times;
	QVector<quint64> dbIndexes;
	bool opened;
};

#endif // ARPCDBDRIVERGTIMEINDEX_H
