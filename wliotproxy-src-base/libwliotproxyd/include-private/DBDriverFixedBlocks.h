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

#ifndef DBDRIVERFIXEDBLOCKS_H
#define DBDRIVERFIXEDBLOCKS_H

#include <QObject>
#include <QVector>
#include <QFile>

namespace WLIOT
{
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
	 * Все функции в класса, принимающие void*, не принимают размер, т.к. размеры уже известны из конфигурации базы.
	 * Функции, которые принимают QByteArray&, проверяют размер.
	 */
	class DBDriverFixedBlocks
	{
	public:
		DBDriverFixedBlocks();
		~DBDriverFixedBlocks();
		bool create(const QString &filePath,const QVector<quint32> &blockNotesSz);
		bool open(const QString &filePath);
		void close();
		bool isOpened()const;
		const QVector<quint32>& noteSizes();
		quint64 blocksCount();
		quint32 blockSize();
		int noteCount();

		//Функции чтения/записи не принимают параметр размера данных, т.к. все размеры фиксированы.
		// !!! Не проверяется размер даже для QByteArray, помни про переполнение, в QByteArray не больше 2 млрд байт
		bool readBlock(quint64 blockIndex,QByteArray &data);//все индексы с 0
		bool readBlock(quint64 blockIndex,void *data);//data is size of blockSize
		bool readNote(quint64 blockIndex,quint32 noteIndex,QByteArray &data);
		bool readNote(quint64 blockIndex,quint32 noteIndex,void *data);
		bool writeBlock(const QByteArray &data);
		bool writeBlock(const void *data);
		bool updateBlock(quint64 blockIndex,const QByteArray &data);
		bool updateBlock(quint64 blockIndex,const void *data);
		bool addBlock();//add zero-filled block
		bool addManyBlocks(quint64 count);
		bool addManyBlocks(quint64 count,const QByteArray &data);
		bool writeNote(quint32 noteIndex,const QByteArray &data);//in last block
		bool writeNote(quint32 noteIndex,const void *data);//in last block

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
}

#endif // DBDRIVERFIXEDBLOCKS_H
