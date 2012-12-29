#ifndef CLICKABLELABEL_H
#define CLICKABLELABEL_H

#include <QLabel>
#include <QGraphicsColorizeEffect>

class ClickableLabel : public QLabel
{
	Q_OBJECT
	Q_PROPERTY(bool gray READ isGray WRITE setGray)
	
public:
	explicit ClickableLabel(QWidget *parent = 0);
	void setGray(bool gray);
	bool isGray();
	
signals:
	void clicked();
	
public slots:
	void mousePressEvent(QMouseEvent *ev);
	void resizeEvent(QResizeEvent *ev);
	
private:
	bool m_gray;
};

#endif // CLICKABLELABEL_H
