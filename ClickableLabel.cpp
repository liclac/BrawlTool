#include "ClickableLabel.h"
#include <QDebug>
#include <QResizeEvent>

ClickableLabel::ClickableLabel(QWidget *parent) :
	QLabel(parent)
{
	
}

void ClickableLabel::setGray(bool grayedOut)
{
	if(grayedOut)
	{
		QGraphicsColorizeEffect *grayEffect = new QGraphicsColorizeEffect(this);
		grayEffect->setColor(QColor(0,0,0));
		this->setGraphicsEffect(grayEffect);
		m_gray = false;
	}
	else
	{
		this->setGraphicsEffect(0);
		m_gray = false;
	}
}

bool ClickableLabel::isGray()
{
	return m_gray;
}

void ClickableLabel::mousePressEvent(QMouseEvent *ev)
{
	QLabel::mousePressEvent(ev);
	emit clicked();
}

void ClickableLabel::resizeEvent(QResizeEvent *ev)
{
	QLabel::resizeEvent(ev);
	if(this->pixmap())
		QLabel::setPixmap(this->pixmap()->scaled(ev->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
}
