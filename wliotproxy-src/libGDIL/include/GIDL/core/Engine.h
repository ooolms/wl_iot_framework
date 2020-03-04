#ifndef ENGINE_H
#define ENGINE_H

#include <QObject>
#include <QThread>
#include "GIDL/core/Program.h"

class Engine
	:public QObject
{
	Q_OBJECT
public:
	explicit Engine(QObject *parent=nullptr);
	virtual ~Engine();
	void start(Program *p=nullptr);//TODO start with QByteArray param with containing program (xml?)
	void stop();

private:
	QThread *trd;
	Program *prg;
};

#endif // ENGINE_H
