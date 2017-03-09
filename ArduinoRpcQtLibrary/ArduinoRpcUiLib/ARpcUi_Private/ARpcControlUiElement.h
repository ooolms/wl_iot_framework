#ifndef ARPCCONTROLUIELEMENT_H
#define ARPCCONTROLUIELEMENT_H

#include "ARpcBase/ARpcMessage.h"
#include <QObject>

class ARpcControlUiElement
	:public QObject
{
	Q_OBJECT
public:
	explicit ARpcControlUiElement(QObject *parent=0);
	virtual QWidget* widget()=0;
	virtual bool isGroup()const{return false;}
	virtual bool isCommand()const{return false;}

signals:
	void executeCommand(const QString &cmd,const QStringList &args,bool syncCall);
};

#endif // ARPCCONTROLUIELEMENT_H
