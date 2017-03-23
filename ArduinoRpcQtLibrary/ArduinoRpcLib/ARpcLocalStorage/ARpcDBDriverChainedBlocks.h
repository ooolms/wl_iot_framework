#ifndef ARPCDBDRIVERCHAINEDBLOCKS_H
#define ARPCDBDRIVERCHAINEDBLOCKS_H

#include <QObject>
#include "ARpcLocalStorage/VeryBigArray.h"
#include <QFile>

/**
 * @brief The ARpcDBDriverChainedBlocks class
 * База данных представляет собой файл, состоящий из цепочки блоков.
 * Каждый блок имеет заголовок 4 байт (quint32), содержащий размер блока
 * Размер блока не больше 2^32 байта, кол-во блоков не более 2^32
 * Заголовок базы:
 * 8 байт - строка "ARpcDBCB"
 */
class ARpcDBDriverChainedBlocks
	:public QObject
{
	Q_OBJECT
public:
	explicit ARpcDBDriverChainedBlocks(QObject *parent=0);
	bool create(const QString &filePath);
	bool open(const QString &filePath);
	void close();
	bool isOpened()const;
	quint32 blocksCount();
	quint32 blockSize(quint32 blockIndex);
	bool writeBlock(const void *data,quint32 size);
	bool writeBlock(const QByteArray &data);
	bool readBlock(quint32 blockIndex,void *data);
	bool readBlock(quint32 blockIndex,QByteArray &data);

private:
	bool readHeaderAndBlockOffsets();
	bool writeHeader();

private:
//	VeryBigArray<quint64> blockOffsets;//отступы от начала файла, размер заголовка прибавлять не надо
//	VeryBigArray<quint32> blockSizes;
	std::vector<quint64> blockOffsets;
	std::vector<quint32> blockSizes;
	quint64 totalOffset;
	QFile file;
	bool opened;
};

#endif // ARPCDBDRIVERCHAINEDBLOCKS_H
