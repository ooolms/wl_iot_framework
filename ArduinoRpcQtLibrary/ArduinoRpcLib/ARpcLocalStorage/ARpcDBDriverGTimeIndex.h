#ifndef ARPCDBDRIVERGTIMEINDEX_H
#define ARPCDBDRIVERGTIMEINDEX_H

#include <QObject>
#include "ARpcLocalStorage/ARpcDBDriverFixedBlocks.h"
#include "ARpcLocalStorage/VeryBigArray.h"

class ARpcDBDriverGTimeIndex
	:public QObject
{
	Q_OBJECT
public:
	explicit ARpcDBDriverGTimeIndex(QObject *parent=0);
	virtual ~ARpcDBDriverGTimeIndex();
	bool create(const QString &filePath);
	bool open(const QString &filePath);
	bool isOpened();
	void close();
	bool append(qint64 tm,quint64 dbInd);
	quint64 findIndex(qint64 ts);

private:
	void loadIndex();

private:
	ARpcDBDriverFixedBlocks *dbDriver;
	VeryBigArray<qint64> times;
	VeryBigArray<quint64> dbIndexes;
	bool opened;
};

#endif // ARPCDBDRIVERGTIMEINDEX_H
