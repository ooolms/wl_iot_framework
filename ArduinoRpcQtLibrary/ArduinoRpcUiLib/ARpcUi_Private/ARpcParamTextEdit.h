#ifndef ARPCPARAMTEXTEDIT_H
#define ARPCPARAMTEXTEDIT_H

#include "ARpcUi_Private/ARpcIParamElement.h"

class QLineEdit;

class ARpcParamTextEdit
	:public ARpcIParamElement
{
	Q_OBJECT
public:
	explicit ARpcParamTextEdit(const ARpcControlParam &p,QObject *parent=0);

public:
	virtual QString paramValue()override;
	virtual QWidget* widget()override;
	virtual void setValue(const QString &v)override;

private:
	QWidget *w;
	QLineEdit *edit;
};

#endif // ARPCPARAMTEXTEDIT_H
