#ifndef _CHARDIALOG_H
#define _CHARDIALOG_H

#include <qdialog.h>
#include <qpushbutton.h>
#include <qstring.h>

class CharButton : public QPushButton {
	Q_OBJECT
	public:
		CharButton(QChar _c, QWidget *parent, const char *name=0) :
			QPushButton(parent, name), c(_c) {
			setText(c);	// set button label
			connect(this, SIGNAL(clicked()), SLOT(buttonClicked()));
		}

	signals:
		void buttonClicked(QChar);

	private slots:
		void buttonClicked() {
			emit buttonClicked(c[0]);
		}

	private:
		const QString c;
};


class CharDialog : public QDialog {
	Q_OBJECT
	public:
		CharDialog(QWidget *parent=0, const char *name=0, bool modal=false, WFlags f=0);

	signals:
		void charSelected(char c);	// character was selected

	private slots:
		void buttonClicked(QChar c);
};

#endif	// _CHARDIALOG_H

