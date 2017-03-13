#ifndef ARPCDBDRIVERFIXEDBLOCKS_H
#define ARPCDBDRIVERFIXEDBLOCKS_H

#include <QObject>
#include <QVector>

class QFile;

/**
 * @brief The ARpcDBDriverFixedBlocks class
 * База данных представляет собой файл, состоящий из блоков равной длины (типа строки).
 * Каждый блок поделен на записи тоже фиксированной длины (типа колонки).
 * Заголовок базы:
 * 4 байта - кол-во записей в блоке (quint32)
 * 4 байта * кол-во записей - размеры записей в блоке (каждый - quint32)
 * 20 байт - sha-1 хэш всей предыдущей части заголовка
 */
class ARpcDBDriverFixedBlocks
	:public QObject
{
	Q_OBJECT
public:
	explicit ARpcDBDriverFixedBlocks(QObject *parent=0);
	bool create(const QString &filePath,const QVector<int> &blockNotesSz);
	bool open(const QString &filePath);
	const QVector<quint32>& sizes();
	quint64 notesCount();

private:
	bool readHeader();
	bool writeHeader();

private:
	QVector<quint32> blockNotesSizes;//размеры записей блока
	quint32 blockSize;
	quint64 totalHeaderSize;
	QFile *file;
};

#endif // ARPCDBDRIVERFIXEDBLOCKS_H
