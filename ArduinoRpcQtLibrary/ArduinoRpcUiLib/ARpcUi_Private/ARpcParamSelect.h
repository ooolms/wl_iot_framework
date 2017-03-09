#ifndef ARPCPARAMSELECT_H
#define ARPCPARAMSELECT_H

#include "ARpcUi_Private/ARpcIParamElement.h"

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
	virtual void setValue(const QString &v)override;

private:
	QWidget *w;
	QComboBox *edit;
};

#endif // ARPCPARAMSELECT_H
