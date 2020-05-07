#ifndef DATAUNITEDIT_H
#define DATAUNITEDIT_H

#include <QWidget>
#include "GDIL/core/DataUnit.h"

namespace Ui
{
	class DataUnitEdit;
}
class DataUnitValueEdit;

class DataUnitEdit
	:public QWidget
{
	Q_OBJECT
public:
	explicit DataUnitEdit(const TypeConstraints &c,QWidget *parent=0);
	~DataUnitEdit();
	void setValue(const DataUnit &v);
	DataUnit value()const;

private slots:
	void onTypeBtnClicked();
	void onArrAddRowClicked();
	void onArrDelRowClicked();

private:
	QString valToStr(const SensorValue *v,quint32 packIndex);

private:
	TypeConstraints mConstr;
	Ui::DataUnitEdit *ui;
	DataUnitValueEdit *singleValueEdit;
};

#endif // DATAUNITEDIT_H
