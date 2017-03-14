#ifndef ARPCDBDRIVERFIXEDBLOCKS_H
#define ARPCDBDRIVERFIXEDBLOCKS_H

#include <QObject>
#include <QVector>
#include <QFile>

/**
 * @brief The ARpcDBDriverFixedBlocks class
 * База данных представляет собой файл, состоящий из блоков равной длины (типа строки).
 * Каждый блок поделен на записи тоже фиксированной длины (типа колонки).
 * Можно использовать для хранения потоковых данных.
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
	virtual ~ARpcDBDriverFixedBlocks();
	bool create(const QString &filePath,const QVector<quint32> &blockNotesSz);
	bool open(const QString &filePath);
	void close();
	bool isOpened()const;
	const QVector<quint32>& sizes();
	quint64 blocksCount();
	quint32 notesCount();
	bool readBlock(quint64 blockIndex,QByteArray &data);//все индексы с 0
	bool readBlock(quint64 blockIndex,char *data);
	bool readNote(quint64 blockIndex,quint32 noteIndex,QByteArray &data);
	bool readNote(quint64 blockIndex,quint32 noteIndex,char *data);
	bool addBlock(const QByteArray &data);
	bool addBlock(const char *data);
	bool addBlock();//add zero-filled block
	bool writeNote(quint32 noteIndex,const QByteArray &data);//in last block
	bool writeNote(quint32 noteIndex,const char *data);//in last block
	bool addMultiBlocks(const QByteArray &data);
	bool addMultiBlocks(const char *data,quint32 bCount);

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
