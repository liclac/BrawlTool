#ifndef INVISIBILITYFILTER_H
#define INVISIBILITYFILTER_H

#include <QtCore>
#include <QtGui>

class InvisibilityFilter : public QObject
{
	Q_OBJECT
public:
	explicit InvisibilityFilter(QObject *parent = 0);
	bool eventFilter(QObject *, QEvent *);
	
	void setWidgetVisible(QWidget *widget, bool visible);
};

#endif // INVISIBILITYFILTER_H
