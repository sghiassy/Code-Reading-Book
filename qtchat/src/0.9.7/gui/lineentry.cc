#include "debug.h"
#include "lineentry.h"
#include <iostream.h>

LineEntryDialog::LineEntryDialog(QWidget *parent, const char *name, bool modal, WFlags f) :
	QDialog(parent, name, modal, f) {

	if(::debuglevel & DEBUGLEVEL_CONSTRUCTOR)
		cerr << "LineEntryDialog constructor called." << endl;
	entry = new QLineEdit(this);
	label = new QLabel("Enter: ", this);

// connect signals/slots
	connect(entry, SIGNAL(textChanged(const QString&)), SIGNAL(textChanged(const QString&)));
	connect(entry, SIGNAL(returnPressed()), SLOT(returnRedirect()));
	connect(entry, SIGNAL(returnPressed()), SLOT(accept()));

// set widget size
	setSize();

// give the line entry the focus
	entry->setFocus();

// set default result code
	setResult(QDialog::Rejected);
}

LineEntryDialog::LineEntryDialog(const char *prompt, QWidget *parent, const char *name, bool modal, WFlags f) :
	QDialog(parent, name, modal, f) {

	if(::debuglevel & DEBUGLEVEL_CONSTRUCTOR)
		cerr << "LineEntryDialog constructor called." << endl;
	entry = new QLineEdit(this);
	label = new QLabel(prompt, this);

// connect signals/slots
	connect(entry, SIGNAL(textChanged(const char *)), SIGNAL(textChanged(const char *)));
	connect(entry, SIGNAL(returnPressed()), SLOT(returnRedirect()));
	connect(entry, SIGNAL(returnPressed()), SLOT(accept()));

// set widget size
	setSize();

// give the line entry the focus
	entry->setFocus();

// set default result code
	setResult(QDialog::Rejected);
}

LineEntryDialog::~LineEntryDialog() {
	if(::debuglevel & DEBUGLEVEL_DESTRUCTOR)
		cerr << "LineEntryDialog destructor called." << endl;
	delete entry;
	delete label;
}

QLineEdit *LineEntryDialog::getLineEdit() {
	return entry;
}

void LineEntryDialog::setText(const char *str) {
	entry->setText(str);
}

void LineEntryDialog::selectAll() {
	entry->selectAll();
	entry->setFocus();
}

void LineEntryDialog::deselect() {
	entry->deselect();
}

void LineEntryDialog::clear() {
	entry->clear();
}

void LineEntryDialog::returnRedirect() {
	emit text(entry->text());
}

const char *LineEntryDialog::text() const {
	return entry->text();
}

void LineEntryDialog::setSize() {
	label->move(0, 0);
	label->setFixedSize(label->sizeHint().width(), entry->sizeHint().height());
	entry->move(label->width(), 0);
	entry->setFixedSize(250, label->height());
	setFixedSize(label->width() + entry->width(), label->height());
}

