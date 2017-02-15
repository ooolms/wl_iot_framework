#ifndef ARPCIPARAMELEMENT_H
#define ARPCIPARAMELEMENT_H

#include "ARpcBase/ARpcControlsDefinition.h"
#include <QWidget>

class ARpcIParamElement
	:public QObject
{
	Q_OBJECT
public:
	explicit ARpcIParamElement(QObject *parent=0);
	virtual QString paramValue()=0;
	virtual QWidget* widget()=0;

public:
	static ARpcIParamElement* makeWidget(const ARpcControlParam &param);

signals:
	void activated();
};

#endif // ARPCIPARAMELEMENT_H
