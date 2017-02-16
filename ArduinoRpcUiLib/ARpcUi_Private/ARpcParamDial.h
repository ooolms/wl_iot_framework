#ifndef ARPCPARAMDIAL_H
#define ARPCPARAMDIAL_H

#include "ARpcUi_Private/ARpcIParamElement.h"

class QDial;

class ARpcParamDial
	:public ARpcIParamElement
{
	Q_OBJECT
public:
	explicit ARpcParamDial(const ARpcControlParam &p,QObject *parent=0);

public:
	virtual QString paramValue()override;
	virtual QWidget* widget()override;

private:
	QWidget *w;
	QDial *edit;
};

#endif // ARPCPARAMDIAL_H
