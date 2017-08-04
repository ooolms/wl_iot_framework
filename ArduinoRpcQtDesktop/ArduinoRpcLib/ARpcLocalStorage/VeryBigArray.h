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
#include <vector>

//хранит данные блоками по 1 Мб
template <typename T>
class VeryBigArray
{
	static const quint32 blockSize=1<<20;
	static const int sizeShift=20;

public:
	VeryBigArray()
	{
		totalSize=0;
	}

	void append(const T &t)
	{
		quint32 upIndex=totalSize>>sizeShift;
		quint32 downIndex=(totalSize-((totalSize>>sizeShift)<<sizeShift));
		if(downIndex==0)
		{
			std::vector<T> vv;
			vv.resize(blockSize);
			realData.push_back(vv);
		}
		realData[upIndex][downIndex]=t;
		++totalSize;
	}

	const T& at(quint64 index)const
	{
		quint32 upIndex=index>>sizeShift;
		quint32 downIndex=(index-((index>>sizeShift)<<sizeShift));
		return realData[upIndex][downIndex];
	}

	T& at(quint64 index)
	{
		quint32 upIndex=index>>sizeShift;
		quint32 downIndex=(index-((index>>sizeShift)<<sizeShift));
		return realData[upIndex][downIndex];
	}

	const T& operator[](quint64 index)const
	{
		quint32 upIndex=index>>sizeShift;
		quint32 downIndex=(index-((index>>sizeShift)<<sizeShift));
		return realData[upIndex][downIndex];
	}

	T& operator[](quint64 index)
	{
		quint32 upIndex=index>>sizeShift;
		quint32 downIndex=(index-((index>>sizeShift)<<sizeShift));
		return realData[upIndex][downIndex];
	}

	void clear()
	{
		totalSize=0;
		realData.clear();
	}

	quint64 size()const{return totalSize;}

private:
	quint64 totalSize;
	std::vector<std::vector<T>> realData;
};

#endif // VERYBIGARRAY

