#include "debug.h"
#include "simpleindicator.h"
#include <iostream.h>
#include <qbrush.h>
#include <qpainter.h>

SimpleIndicatorWidget::SimpleIndicatorWidget (
	QColor on, QColor off,
	QWidget *parent, const char *name, WFlags f) :
	QLabel(parent, name, f), on_color(on), off_color(off)
	{
	if(::debuglevel & DEBUGLEVEL_CONSTRUCTOR)
		cerr << "SimpleIndicatorWidget constructor called." << endl;
	setFrameStyle(QFrame::Panel | QFrame::Sunken);
	setAlignment(AlignCenter);
	setAutoMask(false);
	setAutoResize(false);
	state = false;
}


void SimpleIndicatorWidget::setState(bool b) {
	bool oldstate = state;
	state = b;
	if(oldstate != state)
		emit stateChanged(state);
	update();
}

QSize SimpleIndicatorWidget::sizeHint() const {
	return QSize(15, 15);
}

QSizePolicy SimpleIndicatorWidget::sizePolicy() const {
	return QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
}

void SimpleIndicatorWidget::paintEvent(QPaintEvent *) {
	QPainter p(this);
	p.setPen(state ? on_color : off_color);	// set circle color
	p.setBrush(QBrush(state ? on_color : off_color));	// fill the circle w/ pen color
	const QRect &rr=rect();
	int x=rr.left(), y=rr.top();
	int w=rr.width(), h=rr.height();
	QRect r(x+w/4, y+h/4, w/2, h/2);
	p.drawEllipse(r);
}
