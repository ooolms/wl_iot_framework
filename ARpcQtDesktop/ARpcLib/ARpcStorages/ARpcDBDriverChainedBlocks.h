/*******************************************
Copyright 2017 OOO "LMS"

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

	http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#ifndef ARPCDBDRIVERCHAINEDBLOCKS_H
#define ARPCDBDRIVERCHAINEDBLOCKS_H

#include <QObject>
#include "ARpcStorages/VeryBigArray.h"
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
