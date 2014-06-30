#ifndef _FINDDIALOG_H
#define _FINDDIALOG_H

#include <qcolor.h>
#include <qcombobox.h>
#include <qdialog.h>
#include <qpushbutton.h>
#include <qregexp.h>

class FindDialog : public QDialog {
	public:
	// Constructors / Destructors
		FindDialog(QWidget *parent=0, const char *name=0, bool modal=true, WFlags f=0);
		~FindDialog();

	// Accessors
		bool isFindDirectionDown() const { return find_direction_down; }

	protected:
		QRegExp regex;	// search pattern

	// Components
		QComboBox *regex_entry;	// Text line entry for search pattern
		QPushButton *find_button;	// Start Search button
		QPushButton *cancel_button;	// Cancel Search button

		bool find_direction_down;	// true==search from beg. to end
		QRadioButton *find_direction_up_button;	// Find direction (up/down)
		QRadioButton *find_direction_down_button;
		QButtonGroup *find_direction_button_group;	// group for direction buttons
		QLabel *find_direction_label;	// duh

		QCheckBox *find_case_sensitive_checkbox;	// case sensitive search?

		QColor	highlight_color;	// highlight color
		QCheckBox *find_highlight_enable;
		QPushButton *find_highlight_color_button;
};

#endif	// _FINDDIALOG_H
