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
	void append(const T &t)
	{
		quint32 upIndex=totalSize>>sizeShift;
		quint32 downIndex=(totalSize-((totalSize>>sizeShift)<<sizeShift));
		if(downIndex==0)realData.push_back(std::vector<T>());
		realData[upIndex].push_back(t);
		++totalSize;
	}

	const T& at(quint64 index)
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

	quint64 size(){return totalSize;}

private:
	quint64 totalSize;
	std::vector<std::vector<T>> realData;
};

#endif // VERYBIGARRAY

