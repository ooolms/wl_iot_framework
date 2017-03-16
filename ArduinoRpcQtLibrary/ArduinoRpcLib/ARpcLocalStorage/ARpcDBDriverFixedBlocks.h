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
 * 8 байт - строка "ARpcDBFB"
 * 4 байта - кол-во записей в блоке (quint32)
 * 4 байта * кол-во записей - размеры записей в блоке (каждый - quint32)
 * 20 байт - sha-1 хэш всей предыдущей части заголовка
 * Максимально допустимое число блоков - 2^64
 * Максимальная длина блока - 2^32
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
	quint32 blockSize();
	int notesCount();

	//Функции чтения/записи не принимают параметр размера данных, т.к. все размеры фиксированы.
	// !!! Не проверяется размер даже для QByteArray, помни про переполнение, в QByteArray не больше 2 млрд байт
	bool readBlock(quint64 blockIndex,QByteArray &data);//все индексы с 0
	bool readBlock(quint64 blockIndex,char *data);
	bool readNote(quint64 blockIndex,int noteIndex,QByteArray &data);
	bool readNote(quint64 blockIndex,int noteIndex,char *data);
	bool writeBlock(const QByteArray &data);
	bool writeBlock(const char *data);
	bool addBlock();//add zero-filled block
	bool writeNote(int noteIndex,const QByteArray &data);//in last block
	bool writeNote(int noteIndex,const char *data);//in last block

private:
	bool readHeader();
	bool writeHeader();
	void calcHeaderSizeAndOffsets();

private:
	QVector<quint32> blockNotesSizes;//размеры записей блока
	QVector<quint64> blockNotesOffsets;//отступы записей блока от начала блока
	quint32 wholeBlockSize;//размер всего блока
	quint32 totalHeaderSize;//размер всего заголовка (с хэшем)
	quint64 totalBlocksCount;//кол-во записей в базе
	QFile file;
	bool opened;
};

#endif // ARPCDBDRIVERFIXEDBLOCKS_H
