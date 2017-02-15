#ifndef ARPCPARAMSELECT_H
#define ARPCPARAMSELECT_H

#include "ARpcIParamElement.h"

class QComboBox;

class ARpcParamSelect
	:public ARpcIParamElement
{
	Q_OBJECT
public:
	explicit ARpcParamSelect(const ARpcControlParam &p,QObject *parent=0);

public:
	virtual QString paramValue()override;
	virtual QWidget* widget()override;

private:
	QWidget *w;
	QComboBox *edit;
};

#endif // ARPCPARAMSELECT_H
