#ifndef ARPCPARAMNULL_H
#define ARPCPARAMNULL_H

#include "ARpcUi_Private/ARpcIParamElement.h"

class ARpcParamNull
	:public ARpcIParamElement
{
	Q_OBJECT
public:
	explicit ARpcParamNull(QObject *parent=0);

private:
	QWidget *w;

public:
	virtual QString paramValue()override;
	virtual QWidget* widget()override;
	virtual void setValue(const QString &v)override;

private:
	QString value;
};

#endif // ARPCPARAMNULL_H
