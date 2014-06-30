#ifndef _SIMPLEINDICATOR_H
#define _SIMPLEINDICATOR_H

#include <qlabel.h>
#include <qpixmap.h>
#include <qcolor.h>

class SimpleIndicatorWidget : public QLabel {
	Q_OBJECT
	public:
		SimpleIndicatorWidget(QColor on_color, QColor off_color,
			QWidget *parent=0, const char *name=0, WFlags f=0);

		bool getState() {
			return state;
		}
	signals:
		void stateChanged(bool b);
		void clicked(QMouseEvent *e);

	public slots:
		void setState(bool b);
		void toggle() {
			setState(!getState());
		}

	protected:
		virtual void mousePressEvent(QMouseEvent *e) {
			emit clicked(e);
		}
		virtual void paintEvent(QPaintEvent *e);
		virtual QSizePolicy sizePolicy() const;
		virtual QSize sizeHint() const;

	private:
		QColor on_color, off_color;
		bool state;
};

#endif	// _SIMPLEINDICATOR_H
