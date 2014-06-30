#include "debug.h"
#include "fontdialog.h"
#include "util.h"
#include <iostream.h>
#include <qlabel.h>
#include <qfont.h>
#include <qfontinfo.h>
#include <qgroupbox.h>
#include <qbuttongroup.h>
#include <qpainter.h>
#include <stdio.h>
#include <stdlib.h>

// tab stop set to 3 (':set ts=3'  in vi)

const char * FontDialog::fonts[256] = {
	"Charter",
	"Clean",
	"Courier",
	"Fixed",
	"Helvetica",
	"Lucida",
	"LucidaBright",
	"LucidaTypewriter",
	"New Century Schoolbook",
	"SansSerif",
	"Serif",
	"Symbol",
	"System",
	"Terminal",
	"Times",
	"TypeWriter",
	"Utopia",
	0
};

const char *FontDialog::styles[256] = {
	"Regular",
	"Italic",
	"Bold",
	"Bold Italic",
	0
};

const char *FontDialog::sizes[100-7];

FontDialog::FontDialog(QFont initial_font, QWidget *parent, const char *name, WFlags f) :
	QDialog(parent, name, true, f) {
	if(::debuglevel & DEBUGLEVEL_CONSTRUCTOR)
		cerr << "FontDialog destructor called." << endl;
	init(initial_font);
}

void FontDialog::init(QFont f) {
// construct components
	QLabel *font_label, *style_label, *size_label;
	font	= new QComboBox(false, this);
	font_label = new QLabel(font, "&Font", this);
	style	= new QComboBox(false, this);
	style_label = new QLabel(style, "St&yle", this);
	size	= new QComboBox(false, this);
	size_label= new QLabel(size, "&Size", this);

	QButtonGroup *button_group = new QButtonGroup("Attributes", this);
	bold			= new QCheckBox("Bold", button_group);
	underline	= new QCheckBox("Underline", button_group);
	italic		= new QCheckBox("Italic", button_group);

	ok_button		= new QPushButton("OK", this);
	cancel_button	= new QPushButton("Cancel", this);

	sample = new QGroupBox("Sample", this);

// init components
	font->setSizeLimit(6);
	font->setAutoCompletion(true);
	font->insertStrList(fonts);
	// disable style list
	style->setEnabled(false);
	style->setSizeLimit(6);
	style->setAutoCompletion(true);
	style->insertStrList(styles);
	size->setSizeLimit(6);
	size->setAutoCompletion(true);

	ok_button->setDefault(true);
	cancel_button->setDefault(false);

	for(int i=7; i<100; i++) {
		char *cp = new char[4];
		sizes[i-7] = cp;
		snprintf(cp, 4, "%-d", i+1);
	}
	sizes[100-7] = 0;
	size->insertStrList(sizes);

// layout components
	font_label->move(10, 10);
	font_label->setFixedSize(font_label->sizeHint());
	font->move(font_label->x(), font_label->y() + font_label->height() + 3);
	font->setFixedSize(font->sizeHint());
	style_label->move(font->x()+font->width()+5, 10);
	style_label->setFixedSize(style_label->sizeHint());
	style->move(style_label->x(), style_label->y() + style_label->height() + 3);
	style->setFixedSize(style->sizeHint());
	size_label->move(style->x()+style->width()+5, 10);
	size_label->setFixedSize(size_label->sizeHint());
	size->move(size_label->x(), size_label->y() + size_label->height() + 3);
	size->setFixedSize(size->sizeHint());
	sample->move(10, font->y()+font->height()+5);

	bold->move(5, 15);
	bold->setFixedSize(bold->sizeHint());
	italic->move(5, bold->y()+bold->height()+5);
	italic->setFixedSize(italic->sizeHint());
	underline->move(5, italic->y()+italic->height()+5);
	underline->setFixedSize(underline->sizeHint());
	button_group->setFixedSize(underline->width() + 10*2, underline->height()*3 + 15 + 5*2 + 5);
	button_group->move(font->width() + style->width() + size->width() + 10 + 10 + 5 + 5
		- 10-button_group->width(), font->y()+font->height()+5);

	ok_button->move((size->x() + size->width() + 10)/2 - ok_button->width() - 5, button_group->y() + button_group->height() + 5);
	cancel_button->move((size->x() + size->width() + 10)/2 + 5, button_group->y() + button_group->height() + 5);

	setFixedSize(font->width() + style->width() + size->width() + 10 + 10 + 5 + 5,
		ok_button->y() + ok_button->height() + 10);
	sample->setFixedSize(width() - 10 - 10 - 5 - button_group->width(), button_group->height());

// init connections
	connect(cancel_button, SIGNAL(clicked()), SLOT(reject()));
	connect(ok_button, SIGNAL(clicked()), SLOT(accept()));
	connect(font, SIGNAL(activated(int)), SLOT(handleChangeEvent()));
	connect(style, SIGNAL(activated(int)), SLOT(handleChangeEvent()));
	connect(size, SIGNAL(activated(int)), SLOT(handleChangeEvent()));
	connect(button_group, SIGNAL(clicked(int)), SLOT(handleChangeEvent()));

// misc
	// select initial font, adding to list if not in there already
	QFontInfo fi(f);
	bool font_found = false;
	for(int i=0; i<font->count(); i++) {
		if(strcasecmp(fi.family(), font->text(i))==0) {	// we have a match
			font_found = true;
			font->setCurrentItem(i);	// highlight the matched font
			if(fi.pointSize() >= 8 && fi.pointSize() <= 100) // highlight the size
				size->setCurrentItem(fi.pointSize()-8);
			bold->setChecked(fi.bold());	// set check boxes
			italic->setChecked(fi.italic());
			underline->setChecked(fi.underline());
			break;	// get out of loop
		}
	}
	if(!font_found) {	// not in there?  try adding it
		font->insertItem(fi.family(), 0);
	}
	handleChangeEvent();
}

FontDialog::~FontDialog() {
	if(::debuglevel & DEBUGLEVEL_DESTRUCTOR)
		cerr << "FontDialog destructor called." << endl;
	delete font;
	delete style;
	delete size;
	delete bold;
	delete underline;
	delete italic;
	delete ok_button;
	delete cancel_button;
	delete sample;

	for(int i=7; i<100; i++) {
		delete sizes[i-7];
	}
}

QFont FontDialog::getFont() const {	// return selected font
	QFont f(font->currentText(), atoi(size->currentText()));
	f.setBold(bold->isChecked());
	f.setItalic(italic->isChecked());
	f.setUnderline(underline->isChecked());

	return f;
}

void FontDialog::loadSystemFonts() {}

void FontDialog::setSampleFont(QFont f) {
	sample->setFont(f);
}

void FontDialog::handleChangeEvent() {
	paintEvent(new QPaintEvent(QRect(sample->pos(), sample->size())));
}

void FontDialog::paintEvent(QPaintEvent *e) {
	QPainter p(sample);
	p.eraseRect(rect());
	p.setFont(getFont());
	p.drawText(10, 10, sample->width()-20, sample->height()-20, AlignCenter, "aAbBzZ012345");
}
