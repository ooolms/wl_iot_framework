#ifndef ARPCCONTROLUIGROUP_H
#define ARPCCONTROLUIGROUP_H

#include "ARpcControlUiElement.h"
#include "ARpcBase/ARpcControlsDefinition.h"

class QGroupBox;

class ARpcControlUiGroup
	:public ARpcControlUiElement
{
	Q_OBJECT
public:
	explicit ARpcControlUiGroup(const ARpcControlsGroup &grp,QObject *parent=0);
	virtual QWidget *widget()override;

private:
	QGroupBox *w;
	QList<ARpcControlUiElement*> elements;
};

#endif // ARPCCONTROLUIGROUP_H
