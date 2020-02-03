#include "ManualShowingLayoutItem.h"
#include <QWidget>
#include <QApplication>
#include <QDebug>

class BlockAllEventsFilter
	:public QObject
{
public:
	BlockAllEventsFilter(bool en,QWidget *filtered,QObject *parent=0)
		:QObject(parent)
	{
		target=filtered;
		enabled=en;
	}

	void enable(bool en)
	{
		enabled=en;
	}

protected:
	virtual bool eventFilter(QObject *o,QEvent *ev)
	{
		if(o==target)
		{
			switch(ev->type())
			{
			case QEvent::Close:
			case QEvent::CloseSoftwareInputPanel:
			case QEvent::ContentsRectChange:
			case QEvent::ContextMenu:
			case QEvent::CursorChange:
			case QEvent::DragEnter:
			case QEvent::DragLeave:
			case QEvent::DragMove:
			case QEvent::Drop:
			case QEvent::Enter:
			case QEvent::EnterWhatsThisMode:
			case QEvent::FocusAboutToChange:
			case QEvent::FocusIn:
			case QEvent::FocusOut:
			case QEvent::GrabKeyboard:
			case QEvent::GrabMouse:
			case QEvent::Hide:
			case QEvent::HideToParent:
			case QEvent::HoverEnter:
			case QEvent::HoverLeave:
			case QEvent::HoverMove:
			case QEvent::IconDrag:
			case QEvent::IconTextChange:
			case QEvent::InputMethod:
			case QEvent::KeyPress:
			case QEvent::KeyRelease:
			case QEvent::Leave:
			case QEvent::LeaveWhatsThisMode:
			case QEvent::NonClientAreaMouseButtonDblClick:
			case QEvent::NonClientAreaMouseButtonPress:
			case QEvent::NonClientAreaMouseButtonRelease:
			case QEvent::NonClientAreaMouseMove:
//			case QEvent::ModifiedChange:
			case QEvent::MouseButtonDblClick:
			case QEvent::MouseButtonPress:
			case QEvent::MouseButtonRelease:
			case QEvent::MouseMove:
			case QEvent::MouseTrackingChange:
			case QEvent::Move:
			case QEvent::Paint:
			case QEvent::PlatformPanel:
//			case QEvent::Polish:
//			case QEvent::PolishRequest:
			case QEvent::QueryWhatsThis:
			case QEvent::RequestSoftwareInputPanel:
			case QEvent::Resize:
			case QEvent::Shortcut:
			case QEvent::ShortcutOverride:
			case QEvent::Show:
			case QEvent::ShowToParent:
			case QEvent::StatusTip:
			case QEvent::TabletMove:
			case QEvent::TabletPress:
			case QEvent::TabletRelease:
			case QEvent::ToolTip:
			case QEvent::UngrabKeyboard:
			case QEvent::UngrabMouse:
			case QEvent::UpdateLater:
			case QEvent::UpdateRequest:
			case QEvent::WhatsThis:
			case QEvent::WhatsThisClicked:
			case QEvent::Wheel:
			case QEvent::WindowActivate:
			case QEvent::WindowBlocked:
			case QEvent::WindowDeactivate:
			case QEvent::WindowIconChange:
			case QEvent::WindowStateChange:
			case QEvent::WindowTitleChange:
			case QEvent::WindowUnblocked:
			case QEvent::ZOrderChange:
			case QEvent::TouchBegin:
			case QEvent::TouchUpdate:
			case QEvent::TouchEnd:
			case QEvent::Gesture:
			case QEvent::GestureOverride:
				return enabled;
			default:
				break;
			}
		}
		return QObject::eventFilter(o,ev);
	}

private:
	bool enabled;
	QWidget *target;
};

ManualShowingLayoutItem::ManualShowingLayoutItem(QWidget *wgt)
	:QWidgetItem(wgt)
{
	itemWidget=wgt;
	invalidateAfterUpdate=true;
	minSize=itemWidget->minimumSize();
	maxSize=itemWidget->maximumSize();
	savedGeometry=itemWidget->geometry();
	visible=false;
	itemWidget->setFixedSize(0,0);
	itemWidget->setGeometry(0,0,0,0);
	filter=new BlockAllEventsFilter(true,itemWidget);
	itemWidget->installEventFilter(filter);
	setVisible(false);
}

ManualShowingLayoutItem::~ManualShowingLayoutItem()
{
	delete filter;
}

bool ManualShowingLayoutItem::isVisible()
{
	return visible;
}

void ManualShowingLayoutItem::setForceInvalidate(bool f)
{
	invalidateAfterUpdate=f;
}

void ManualShowingLayoutItem::setVisible(bool vis)
{
	if(visible==vis)return;
	visible=vis;
	if(!visible)
	{
		minSize=itemWidget->minimumSize();
		maxSize=itemWidget->maximumSize();
		savedGeometry=itemWidget->geometry();
		itemWidget->setFixedSize(0,0);
		setGeometry(QRect(0,0,0,0));
		itemWidget->hide();
		itemWidget->close();
//		qApp->processEvents();
		filter->enable(true);
		itemWidget->setEnabled(false);
	}
	else
	{
		filter->enable(false);
		itemWidget->setEnabled(true);
		itemWidget->setMinimumSize(minSize);
		itemWidget->setMaximumSize(maxSize);
		itemWidget->setGeometry(savedGeometry);
		QWidget *parentWidget=itemWidget->parentWidget();
		if(parentWidget&&!parentWidget->isHidden())itemWidget->show();
	}
	if(invalidateAfterUpdate)invalidate();
}

Qt::Orientations ManualShowingLayoutItem::expandingDirections()const
{
	return Qt::Vertical|Qt::Horizontal;
}

QRect ManualShowingLayoutItem::geometry()const
{
	if(visible)return QWidgetItem::geometry();
	return QRect(0,0,0,0);
}

bool ManualShowingLayoutItem::isEmpty()const
{
	return !visible&&QWidgetItem::isEmpty();
}

QSize ManualShowingLayoutItem::minimumSize()const
{
	if(visible)return QWidgetItem::minimumSize();
	return QSize(0,0);
}

QSize ManualShowingLayoutItem::maximumSize()const
{
	if(visible)return QWidgetItem::maximumSize();
	return QSize(0,0);
}

void ManualShowingLayoutItem::setGeometry(const QRect &r)
{
	if(visible)QWidgetItem::setGeometry(r);
	else itemWidget->close();
}

QSize ManualShowingLayoutItem::sizeHint()const
{
	if(visible)return QWidgetItem::sizeHint();
	return QSize(0,0);
}

QWidget* ManualShowingLayoutItem::widget()
{
//	if(!visible)return 0;
	return itemWidget;
}
