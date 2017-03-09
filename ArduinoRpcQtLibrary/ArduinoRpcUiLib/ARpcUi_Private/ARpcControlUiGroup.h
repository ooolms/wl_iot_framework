#ifndef ARPCCONTROLUIGROUP_H
#define ARPCCONTROLUIGROUP_H

#include "ARpcUi_Private/ARpcControlUiElement.h"
#include "ARpcBase/ARpcControlsDefinition.h"
#include "ARpcBase/ARpcDeviceState.h"

class QGroupBox;

class ARpcControlUiGroup
	:public ARpcControlUiElement
{
	Q_OBJECT
public:
	explicit ARpcControlUiGroup(const ARpcControlsGroup &grp,QObject *parent=0);
	virtual QWidget *widget()override;
	void updateState(const ARpcDeviceState &state);
	virtual bool isGroup()const override{return true;}

private:
	QGroupBox *w;
	QList<ARpcControlUiElement*> elements;
};

#endif // ARPCCONTROLUIGROUP_H
