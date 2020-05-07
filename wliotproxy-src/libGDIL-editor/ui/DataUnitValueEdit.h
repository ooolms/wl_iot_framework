#ifndef DATAUNITVALUEEDIT_H
#define DATAUNITVALUEEDIT_H

#include <QScrollArea>

class QLineEdit;
class QLabel;

class DataUnitValueEdit
	:public QScrollArea
{
	Q_OBJECT
public:
	explicit DataUnitValueEdit(QWidget *parent=nullptr);
	void setDim(quint32 dim);
	bool canBeS64()const;
	QVector<double> f64Values(bool *ok=0);
	QVector<qint64> s64Values(bool *ok=0);
	void setValues(const QVector<double> &vals);
	void setValues(const QVector<qint64> &vals);
	virtual QSize sizeHint()const override;
	virtual QSize minimumSizeHint()const override;

private:
	QWidget *contentWidget;
	QList<QLineEdit*> edits;
	QList<QLabel*> commas;
};

#endif // DATAUNITVALUEEDIT_H
