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

#ifndef VERYBIGARRAY
#define VERYBIGARRAY

#include <QtGlobal>
#include <QVector>

//хранит данные блоками по 1 Мб
template <typename T>
class VeryBigArray
{
	static const quint32 blockSize=1<<20;
	static const quint32 sizeShift=20;

public:
	VeryBigArray()
	{
		totalSize=0;
	}

	VeryBigArray(const VeryBigArray &t)=default;

	VeryBigArray(VeryBigArray &&t)
	{
		totalSize=t.totalSize;
		t.totalSize=0;
		realData.swap(t.realData);
	}

	VeryBigArray& operator=(const VeryBigArray &t)=default;

	VeryBigArray& operator=(VeryBigArray &&t)
	{
		if(this==&t)return *this;
		totalSize=t.totalSize;
		t.totalSize=0;
		realData.swap(t.realData);
		return *this;
	}

	void append(const T &t)
	{
		quint32 upIndex=totalSize>>sizeShift;
		quint32 downIndex=(totalSize-(upIndex<<sizeShift));
		if(downIndex==0)
		{
			QVector<T> vv;
			vv.resize(blockSize);
			realData.append(vv);
		}
		realData[upIndex][downIndex]=t;
		++totalSize;
	}

	const T& at(quint64 index)const
	{
		quint32 upIndex=index>>sizeShift;
		quint32 downIndex=(index-(upIndex<<sizeShift));
		return realData[upIndex][downIndex];
	}

	T& at(quint64 index)
	{
		quint32 upIndex=index>>sizeShift;
		quint32 downIndex=(index-(upIndex<<sizeShift));
		return realData[upIndex][downIndex];
	}

	const T& operator[](quint64 index)const
	{
		quint32 upIndex=index>>sizeShift;
		quint32 downIndex=(index-(upIndex<<sizeShift));
		return realData[upIndex][downIndex];
	}

	T& operator[](quint64 index)
	{
		quint32 upIndex=index>>sizeShift;
		quint32 downIndex=(index-(upIndex<<sizeShift));
		return realData[upIndex][downIndex];
	}

	void clear()
	{
		totalSize=0;
		realData.clear();
	}

	quint64 size()const{return totalSize;}

	void resize(quint64 sz)
	{
		if(sz==totalSize)return;
		quint32 blocksCount=sz>>sizeShift;
		if(sz%(1ULL<<sizeShift)!=0)
			++blocksCount;
		if(sz<totalSize)
		{
			if((quint32)realData.size()!=blocksCount)
				realData.resize(blocksCount);
		}
		else
		{
			QVector<T> vv;
			vv.resize(blockSize);
			while((quint32)realData.size()<blocksCount)
				realData.append(vv);
		}
		totalSize=sz;
	}

private:
	quint64 totalSize;
	QVector<QVector<T>> realData;
};

#endif // VERYBIGARRAY
