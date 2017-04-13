#ifndef REMOVEDIRRECUSIVE_H
#define REMOVEDIRRECUSIVE_H

#include <QDir>

class RemoveDirRecusive
{
public:
	static bool rmDirRec(QDir dir);
	static bool rmDirContentsRec(QDir dir);
};

#endif // REMOVEDIRRECUSIVE_H
