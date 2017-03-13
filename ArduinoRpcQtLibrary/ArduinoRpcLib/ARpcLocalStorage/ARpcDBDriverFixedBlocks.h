#ifndef ARPCDBDRIVERFIXEDBLOCKS_H
#define ARPCDBDRIVERFIXEDBLOCKS_H

#include <QObject>
#include <QVector>
#include <QFile>

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
	bool create(const QString &filePath,const QVector<quint32> &blockNotesSz);
	bool open(const QString &filePath);
	void close();
	bool isOpened()const;
	const QVector<quint32>& sizes();
	quint64 blocksCount();
	quint32 notesCount();
	bool readBlock(quint64 blockIndex,QByteArray &data);//все индексы с 0
	bool readNote(quint64 blockIndex,quint32 noteIndex,QByteArray &data);

private:
	bool readHeader();
	bool writeHeader();
	void calcOffsets();

private:
	QVector<quint32> blockNotesSizes;//размеры записей блока
	QVector<quint32> blockNotesOffsets;//отступы записей блока от начала блока
	quint32 blockSize;//размер всего блока
	quint64 totalHeaderSize;//размер всего заголовка (с хэшем)
	quint64 totalBlocksCount;//кол-во записей в базе
	QFile file;
	bool opened;
};

#endif // ARPCDBDRIVERFIXEDBLOCKS_H
