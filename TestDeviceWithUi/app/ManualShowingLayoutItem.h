#ifndef MANUALSHOWINGLAYOUTITEM_H
#define MANUALSHOWINGLAYOUTITEM_H

#include <QWidgetItem>

class BlockAllEventsFilter;

class ManualShowingLayoutItem
	:public QWidgetItem
{
public:
	ManualShowingLayoutItem(QWidget *wgt);
	virtual ~ManualShowingLayoutItem();
	void setVisible(bool vis);
	bool isVisible();
	void setForceInvalidate(bool f);

public:
	virtual Qt::Orientations expandingDirections()const;
	virtual QRect geometry()const;
	virtual bool isEmpty()const;
	virtual QSize maximumSize()const;
	virtual QSize minimumSize()const;
	virtual void setGeometry(const QRect &r);
	virtual QSize sizeHint()const;
	virtual QWidget* widget();

private:
	bool visible;
	bool invalidateAfterUpdate;
	QWidget *itemWidget;
	BlockAllEventsFilter *filter;
	QSize minSize,maxSize;
	QRect savedGeometry;
};

#endif // MANUALSHOWINGLAYOUTITEM_H
