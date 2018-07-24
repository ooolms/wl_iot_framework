#ifndef ARPCPARAMRADIO_H
#define ARPCPARAMRADIO_H

#include "ARpcIParamElement.h"

class QGroupBox;
class QRadioButton;

class ARpcParamRadio
	:public ARpcIParamElement
{
	Q_OBJECT
public:
	explicit ARpcParamRadio(const ARpcControlParam &p,QObject *parent=nullptr);
	virtual QByteArray paramValue()override;
	virtual QWidget *widget()override;
	virtual void setValue(const QByteArray &v)override;

private:
	QGroupBox *w;
	QList<QRadioButton*> btns;
};

#endif // ARPCPARAMRADIO_H
