#ifndef ARPCCONTROLUICOMMAND_H
#define ARPCCONTROLUICOMMAND_H

#include "ARpcControlUiElement.h"
#include "ARpcIParamElement.h"
#include "ARpcBase/ARpcControlsDefinition.h"
#include <QObject>

class QGroupBox;

class ARpcControlUiCommand
	:public ARpcControlUiElement
{
	Q_OBJECT
public:
	explicit ARpcControlUiCommand(const ARpcControlsCommand &cmd,QObject *parent=0);
	virtual QWidget* widget()override;

private slots:
	void onElementActivated();
	void onSendCommand();

private:
	QGroupBox *w;
	QList<ARpcIParamElement*> elements;
	QString command;
	bool sendCommandOnElementActivation;
	bool syncCall;
};

#endif // ARPCCONTROLUIELEMENT_H
