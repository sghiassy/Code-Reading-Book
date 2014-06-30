#include "lineedit.h"
#include <iostream.h>
#include <qdragobject.h>

LineEdit::LineEdit(int size, QWidget *parent, const char *name) :
	QLineEdit(parent, name) {

	if(::debuglevel & DEBUGLEVEL_CONSTRUCTOR)
		cerr << "LineEdit constructor called." << endl;
	histsize = size;
	history = new QStrList();	// deep copy
	history->setAutoDelete(false);	// don't delete removed items
	history->append("");	// hidden list item to hold LineEdit contents
	char_dialog = new CharDialog(this, 0, true);
	setAcceptDrops(true);	// accept drops of URLs
}

LineEdit::~LineEdit() {
	if(::debuglevel & DEBUGLEVEL_DESTRUCTOR)
		cerr << "LineEdit destructor called." << endl;
	delete history;
}

// History list methods
void LineEdit::setHistSize(int size) {
	if(size >= 0) {	// list possibly being reduced in size
		if(size < histsize) { // list is reduced in size
			for(int i=size; i<histsize; i++) {
				history->removeLast();	// delete oldest items
			}
		}
	}
	histsize = size;
}

void LineEdit::setHistory(QStrList *list) {
	history = list;
	if((int)list->count() > histsize) {	// setting bigger list than histsize
		histsize = list->count();	// bump size up to size of list
	}
}

// Events

void LineEdit::keyPressEvent(QKeyEvent *e) {
	switch(e->key()) {
	 case Qt::Key_Up:	// history list scrolling
	 case Qt::Key_Down:
	 {
		// save current line
		int i = history->at();
		history->insert(history->at(), text());
		history->remove(history->at()+1);
		if(i+1 == history->at())
			history->prev();
		// get new item
		const char *cp;
		if(e->key() == Qt:: Key_Up) {
			if(!(cp=history->next()))
				cp=history->last();
		} else {
			if(!(cp=history->prev()))
				cp=history->first();
		}
		setText(cp);
		break;
	 }

	 case Qt::Key_Enter:
	 case Qt::Key_Return:
		if(history->at() == 0) {	// at hidden element in list... add to history
			history->insert(1, text());	// insert new item
			if(histsize >= 0 && (int)(history->count()-1) > histsize)
				history->removeLast();
		} else {	// entering history list item...
			history->insert(history->at(), text());
			history->remove(history->at()+1);
		}
		history->first();	// move to beginning of list
		emit returnPressed();
		break;

	 default:
		QLineEdit::keyPressEvent(e);
		break;
	}
}

void LineEdit::mousePressEvent(QMouseEvent *e) {
	if(e->button() == QMouseEvent::RightButton) {
		QChar c;
		if((c=(QChar)char_dialog->exec()) != (QChar)(unsigned int)QDialog::Rejected) {
			insert((QString)c);
		}
	} else
		QLineEdit::mousePressEvent(e);
}

void LineEdit::dragEnterEvent(QDragEnterEvent *e) {
	cerr << "LineEdit::dragEnterEvent(" << e->format() << ") called." << endl;
	e->accept(QTextDrag::canDecode(e) ||
		QUriDrag::canDecode(e));
}

void LineEdit::dropEvent(QDropEvent *e) {
	cerr << "LineEdit::dropEvent(" << e->format() << ") called." << endl;
	QString t;
	QStrList tl;
	if(QTextDrag::decode(e, t)) {
		insert(t);
	} else if (QUriDrag::decode(e, tl)) {
		char *cp = tl.first();
		while(cp) {
			insert(cp);
			cp = tl.next();
			if(cp)
				insert(" ");
		}
	}
}
