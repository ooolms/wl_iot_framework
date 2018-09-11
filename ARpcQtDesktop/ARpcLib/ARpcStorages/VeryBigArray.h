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
#include <type_traits>

//хранит данные блоками по 1 Мб
//не хранить двойные, тройные и т.д. указатели!
template <typename T>
class VeryBigArray
{
	static const quint32 blockSize=1<<10;
	static const quint32 sizeShift=10;

public:
	VeryBigArray()
	{
		totalSize=0;
	}

	VeryBigArray(const VeryBigArray &t)
	{
		totalSize=t.totalSize;
		copyData(t.mData,0);
	}

	VeryBigArray(VeryBigArray &&t)
	{
		totalSize=t.totalSize;
		t.totalSize=0;
		mData.swap(t.mData);
	}

	~VeryBigArray()
	{
		clear();
	}

	VeryBigArray& operator=(const VeryBigArray &t)
	{
		clear();
		totalSize=t.totalSize;
		copyData(t.mData,0);
	}

	VeryBigArray& operator=(VeryBigArray &&t)
	{
		if(this==&t)return *this;
		totalSize=t.totalSize;
		mData.swap(t.mData);
		t.clear();
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
			mData.append(vv);
		}
		mData[upIndex][downIndex]=t;
		++totalSize;
	}

	const T& at(quint64 index)const
	{
		quint32 upIndex=index>>sizeShift;
		quint32 downIndex=(index-(upIndex<<sizeShift));
		return mData[upIndex][downIndex];
	}

	T& at(quint64 index)
	{
		quint32 upIndex=index>>sizeShift;
		quint32 downIndex=(index-(upIndex<<sizeShift));
		return mData[upIndex][downIndex];
	}

	const T& operator[](quint64 index)const
	{
		quint32 upIndex=index>>sizeShift;
		quint32 downIndex=(index-(upIndex<<sizeShift));
		return mData[upIndex][downIndex];
	}

	T& operator[](quint64 index)
	{
		quint32 upIndex=index>>sizeShift;
		quint32 downIndex=(index-(upIndex<<sizeShift));
		return mData[upIndex][downIndex];
	}

	void clear()
	{
		T *s=0;
		cleanData(s);
		totalSize=0;
		mData.clear();
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
			if((quint32)mData.size()!=blocksCount)
				mData.resize(blocksCount);
		}
		else
		{
			QVector<T> vv;
			vv.resize(blockSize);
			while((quint32)mData.size()<blocksCount)
				mData.append(vv);
		}
		totalSize=sz;
	}

private:
	template<typename U=T>
	void cleanData(typename std::enable_if<!std::is_pointer<U>::value,U>::type *)
	{
	}

	template<typename U=T>
	void cleanData(typename std::enable_if<std::is_pointer<U>::value,U>::type *)
	{
		quint32 upIndex;
		quint32 downIndex;
		for(quint64 i=0;i<totalSize;++i)
		{
			upIndex=i>>sizeShift;
			downIndex=(i-(upIndex<<sizeShift));
			delete mData[upIndex][downIndex];
		}
	}

	template<typename U=T>
	void copyData(const QVector<QVector<T>> &v,typename std::enable_if<!std::is_pointer<U>::value,U>::type *)
	{
		mData.resize(v.size());
		for(int i=0;i<mData.size();++i)
			mData[i].resize(blockSize);
		quint32 upIndex;
		quint32 downIndex;
		for(quint64 i=0;i<totalSize;++i)
		{
			upIndex=i>>sizeShift;
			downIndex=(i-(upIndex<<sizeShift));
			mData[upIndex][downIndex]=v[upIndex][downIndex];
		}
	}

	template<typename U=T>
	void copyData(const QVector<QVector<T>> &v,typename std::enable_if<std::is_pointer<U>::value,U>::type *)
	{
		mData.resize(v.size());
		for(int i=0;i<mData.size();++i)
			mData[i].resize(blockSize);
		quint32 upIndex;
		quint32 downIndex;
		for(quint64 i=0;i<totalSize;++i)
		{
			upIndex=i>>sizeShift;
			downIndex=(i-(upIndex<<sizeShift));
			T t=new typename std::remove_pointer<T>::type();
			*t=v[upIndex][downIndex];
			mData[upIndex][downIndex]=t;
		}
	}

private:
	quint64 totalSize;
	QVector<QVector<T>> mData;
};

#endif // VERYBIGARRAY
