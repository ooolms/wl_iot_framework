#ifndef ARPCCONTROLUI_H
#define ARPCCONTROLUI_H

#include <QWidget>
#include "ARpcBase/ARpcDevice.h"
#include "ARpcBase/ARpcControlsDefinition.h"

class ARpcControlUiGroup;

class ARpcControlUi
	:public QWidget
{
	Q_OBJECT
public:
	explicit ARpcControlUi(ARpcDevice *dev,const ARpcControlsGroup &controlsDef,QWidget *parent=0);

private slots:
	void onDeviceDestroyed();
	void onExecuteCommand(const QString &command,const QStringList &args,bool syncCall);

private:
	ARpcDevice *device;
	ARpcControlUiGroup *mainGroup;
};

#endif // ARPCCONTROLUI_H
