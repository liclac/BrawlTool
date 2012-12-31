#include "InvisibilityFilter.h"

InvisibilityFilter::InvisibilityFilter(QObject *parent) :
	QObject(parent)
{
	
}

bool InvisibilityFilter::eventFilter(QObject *, QEvent *ev)
{
	return (ev->type() == QEvent::Paint);
}

void InvisibilityFilter::setWidgetVisible(QWidget *widget, bool visible)
{
	if(visible) widget->removeEventFilter(this);
	else widget->installEventFilter(this);
	
	foreach(QObject *child, widget->children())
	{
		if(visible) child->removeEventFilter(this);
		else child->installEventFilter(this);
	}
	
	widget->update();
}
