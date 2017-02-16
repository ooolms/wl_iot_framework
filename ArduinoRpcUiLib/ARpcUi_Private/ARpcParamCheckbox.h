#ifndef ARPCPARAMCHECKBOX_H
#define ARPCPARAMCHECKBOX_H

#include "ARpcUi_Private/ARpcIParamElement.h"

class QCheckBox;

class ARpcParamCheckbox
	:public ARpcIParamElement
{
	Q_OBJECT
public:
	explicit ARpcParamCheckbox(const ARpcControlParam &p,QObject *parent=0);
	virtual QString paramValue()override;
	virtual QWidget* widget()override;

private:
	QCheckBox *check;
	QString onValue;
	QString offValue;
};

#endif // ARPCPARAMCHECKBOX_H
