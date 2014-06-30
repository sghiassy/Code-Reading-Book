#include <ctype.h>
#include <iostream>
#include "chardialog.h"

CharDialog::CharDialog(QWidget *parent, const char *name, bool modal, WFlags f) :
	QDialog(parent, name, modal, f) {
	setFont(QFont("Terminal", 10));
	int n=0;	// number of printable chars
	int w = CharButton('.', this).sizeHint().width();
	int h = CharButton('.', this).sizeHint().height();
	for(unsigned char i=0; i!=255; i++) {
		if(isprint(i) || i > 127) {	// printable character found
			QChar ch(i, 0);
			CharButton *b = new CharButton(ch, this);	// make button for letter
			b->move((n%16)*w, (n/16)*h);
			b->setFixedSize(w, h);
			n++;
			connect(b, SIGNAL(buttonClicked(QChar)), SLOT(buttonClicked(QChar)));
		}
	}
	setFixedSize((n%16+1)*w, (n/16+1)*h);
}

void CharDialog::buttonClicked(QChar c) {
	cout << "char = " << c.row() << ',' << c.cell() << endl;
	emit charSelected(c);
	done((unsigned int)c);
}
