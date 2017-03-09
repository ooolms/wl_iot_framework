#ifndef ARPCCONTROLUICOMMAND_H
#define ARPCCONTROLUICOMMAND_H

#include "ARpcUi_Private/ARpcControlUiElement.h"
#include "ARpcUi_Private/ARpcIParamElement.h"
#include "ARpcBase/ARpcControlsDefinition.h"
#include <QObject>

class QGroupBox;

class ARpcControlUiCommand
	:public ARpcControlUiElement
{
	Q_OBJECT
public:
	explicit ARpcControlUiCommand(const ARpcCommandControl &cmd,QObject *parent=0);
	virtual QWidget* widget()override;
	void updateState(const QMap<int,QString> &values);
	virtual bool isCommand()const override{return true;}
	QString getCommand()const;

private slots:
	void onElementActivated();
	void onSendCommand();

private:
	QWidget *w;
	QList<ARpcIParamElement*> elements;
	QString command;
	bool sendCommandOnElementActivation;
	bool syncCall;
};

#endif // ARPCCONTROLUIELEMENT_H
