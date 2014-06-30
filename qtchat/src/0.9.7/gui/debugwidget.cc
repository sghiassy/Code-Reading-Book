#include "debugwidget.h"
#include <ctype.h>
#include <fstream.h>
#include <iostream.h>
#include <math.h>	// ceil() function
#include <stdio.h>
#include <qfiledialog.h>
#include <qstring.h>

class MyMultiLineEdit : public QMultiLineEdit {
	public:
		MyMultiLineEdit(QWidget *parent=0, const char *name=0) :
			QMultiLineEdit(parent, name) {
			if(::debuglevel & DEBUGLEVEL_CONSTRUCTOR)
				cerr << "MyMultiLineEdit constructor called." << endl;
		}
		QPoint cursorPoint() const {
			return QMultiLineEdit::cursorPoint();
		}
		void setTopCell(int row) {
			QTableView::setTopCell(row);
		}
		int topCell() const {
			return QTableView::topCell();
		}
};

DebugWidget::DebugWidget(QWidget *parent, const char *name, WFlags f) :
	QWidget(parent, name, f) {

	frozen = false;
	show_incoming = true;
	show_outgoing = true;
	if(::debuglevel & DEBUGLEVEL_CONSTRUCTOR)
		cerr << "DebugWidget constructor called." << endl;
	edit_widget = new MyMultiLineEdit(this);
	edit_widget->setReadOnly(true);
	edit_widget->setAutoUpdate(true);

	menu_bar = new QMenuBar(this);
	QPopupMenu *popup;

	popup = new QPopupMenu(menu_bar);
	popup->insertItem("Save &As...", this, SLOT(saveTextAs()));
	popup->insertItem("&Close", this, SLOT(hide()));
	menu_bar->insertItem("&File", popup, 0);

	popup = new QPopupMenu(menu_bar);
	popup->insertItem("Cu&t", this, SLOT(cut()), CTRL+Key_X);
	popup->insertItem("&Copy", this, SLOT(copyText()), CTRL+Key_C);
	popup->insertItem("&Paste", this, SLOT(paste()), CTRL+Key_V);
//	popup->insertItem("&Delete", this, SLOT(deleteText()));
	popup->insertSeparator();
	popup->insertItem("&Find...", this, SLOT(findText()));
	popup->insertSeparator();
	popup->insertItem("&Clear Window", this, SLOT(clear()));
	menu_bar->insertItem("&Edit", popup, 1);

	popup = new QPopupMenu(menu_bar);
	popup->insertItem("Show &Incoming Packets", this, SLOT(toggleIncoming()), 0, 0);
	popup->setItemChecked(0, true);
	show_incoming = true;
	popup->insertItem("Show &Outgoing Packets", this, SLOT(toggleOutgoing()), 0, 1);
	popup->setItemChecked(1, true);
	show_outgoing = true;

	QPopupMenu *color = new QPopupMenu(popup);
	color->insertItem("&Incoming Data...", this, SLOT(setIncomingColor()));
	color->insertItem("&Outgoing Data...", this, SLOT(setOutgoingColor()));
	popup->insertItem("&Color", color);

	popup->insertItem("Free&ze Window", this, SLOT(toggleFreeze()), 0, 2);
	popup->insertItem("&Filter...", 0, 3);
	popup->setCheckable(true);
	menu_bar->insertItem("&Options", popup, 2);

	// status bar
	status_bar = new QStatusBar(this);
	resize(300, 400);
	edit_widget->setFont(QFont("Fixed", 10));
	setFixedWidth(QFontMetrics(QFont("Fixed", 10)).width('x')*(16*4+10));
}  

DebugWidget::~DebugWidget() {
	if(::debuglevel & DEBUGLEVEL_DESTRUCTOR)
		cerr << "DebugWidget destructor called." << endl;
	delete edit_widget;
}  

void DebugWidget::resizeEvent(QResizeEvent *e) {
	edit_widget->move(0, menu_bar->height());
	edit_widget->resize(width(), height() - menu_bar->height() - status_bar->height());
	status_bar->move(0, edit_widget->y() + edit_widget->height());
}

//void DebugWidget::handleTextChanged() {
	// only move to the bottom if somewhat near the bottom already
//	int line, col;
//	edit_widget->cursorPosition(&line, &col);	// get cursor position
	// apparently after looking, I can't do it without getting at the
	// protected methods of QMultiLineEdit, so this is postponed and the
	// scroll_to_end_of_text_when_changed() is temporarily disabled
//	edit_widget->setCursorPosition(edit_widget->numLines(), 0, false);
//}

//void DebugWidget::deleteText() {
//	QClipboard *clipboard = QApplication::clipboard();
//	QString txt = clipboard->text();	// save clipboard contents
//	edit_widget->cut();	// cut text to clipboard
//	clipboard->setText(txt);	// restore contents
//}

void DebugWidget::dataTransferred(const unsigned char *d, int n, bool incoming) {
	if(frozen) return;
	if(incoming && !show_incoming) return;
	if(!incoming && !show_outgoing) return;

	char str1[49];			// 16 bytes * 3 chars/byte + '\0' : holds hex string
	char str2[17];			// 16 bytes * 1 char/byte + '\0'	: holds char string
	QString color_str;
	QString line;
	int num_lines = 0;
			

	int i;
	int n_ceiling = ((int)ceil((double)n/16.0))*16;
	for(i=0; i<n_ceiling; i++) {	// parse data
		int j = i&15;	// get lower 4 bits of string index as substr index
		if(j == 0) {	// clear our temp strings at i=0, 16, 32...
			memset(str1, 0, 49);
			memset(str2, 0, 17);
		}
		char *cp = str1+(j)*3;	// point cp at place to insert hex string in str1
		if(i<n)
			sprintf(cp, "%02X ", ((int)d[i])&0xff);	// insert hex val of char in str1
		else
			strcpy(cp, "   ");
		if(i<n && isprint((char)d[i])) {	// insert char or space into str2
			str2[j] = (char)d[i];
		} else {
			str2[j] = ' ';
		}
		if((i+1)%16 == 0) {	// add line to widget iff i=15,31,47...
//			const QColor &c = incoming ? incoming_color : outgoing_color;
			
//			color_str.sprintf("<font color=\"#%02x%02x%02x\">", c.red(), c.green(), c.blue());
			line.append(color_str);
			line.append(str1);
			if(incoming)
				line.append("-> ");
			else
				line.append("<- ");
			line.append(str2);
//			line.append("</font><br/>");
			line.append('\n');
			num_lines++;
		}
	}
	edit_widget->setAutoUpdate(false);
	int top_cell = dynamic_cast<MyMultiLineEdit *>(edit_widget)->topCell();
	edit_widget->append(line);
	dynamic_cast<MyMultiLineEdit *>(edit_widget)->setTopCell(top_cell+num_lines+1);
	edit_widget->setAutoUpdate(true);
	edit_widget->repaint();
}

void DebugWidget::saveTextAs() {
	QString filename;
	if(!(filename = QFileDialog::getSaveFileName("qtchatdebug.txt")).isNull()) {
		ofstream ofs(filename);
		if(ofs) {
			ofs << edit_widget->text();
		}
	}
}

void DebugWidget::setIncoming(bool b) {
	show_incoming = b;
	QMenuData *item;
	item = menu_bar->findItem(2)->popup();
	item->setItemChecked(0, show_incoming);
}

void DebugWidget::toggleIncoming() {
	setIncoming(!show_incoming);
}

void DebugWidget::setOutgoing(bool b) {
	show_outgoing = b;
	QMenuData *item;
	item = menu_bar->findItem(2)->popup();
	item->setItemChecked(1, show_outgoing);
}

void DebugWidget::toggleOutgoing() {
	setOutgoing(!show_outgoing);
}

void DebugWidget::setFreeze(bool b) {
	frozen = b;
	QMenuData *item;
	item = menu_bar->findItem(2)->popup();
	item->setItemChecked(2, frozen);
}

void DebugWidget::toggleFreeze() {
	setFreeze(!frozen);
}
