#ifndef ARPCPARAMSLIDER_H
#define ARPCPARAMSLIDER_H

#include "ARpcUi_Private/ARpcIParamElement.h"

class QSlider;

class ARpcParamSlider
	:public ARpcIParamElement
{
	Q_OBJECT
public:
	explicit ARpcParamSlider(const ARpcControlParam &p,QObject *parent=0);

public:
	virtual QString paramValue()override;
	virtual QWidget* widget()override;
	virtual void setValue(const QString &v)override;

private:
	QWidget *w;
	QSlider *edit;
};

#endif // ARPCPARAMSLIDER_H
