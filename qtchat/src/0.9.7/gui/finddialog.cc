// Constructors / Destructors
FindDialog::FindDialog(QWidget *parent, const char *name, bool modal, WFlags f) :
	QDialog(parent, name, modal, f),
	regex() {

// create components
	regex_entry = new QComboBox(true, this);
	find_button = new QPushButton(QString("&Find"), this);
	cancel_button = new QPushButton(QString("&Cancel"), this);
	find_direction_button_group = new QButtonGroup(2, Qt::Horizontal, "Direction", this);
	find_direction_up_button = new QRadioButton("&Up", find_direction_button_group);
	find_direction_down_button = new QRadioButton("&Down", find_direction_button_group);
	find_direction_label = new QLabel("Direction", this);
	find_case_sensitive_checkbox = new QCheckBox(this);
	find_highlight_enable = new QCheckBox("&Highlight Findings", this);
	find_highlight_color_button = new QPushButton(this);

// init data members
	highlight_color = Qt::yellow;
	find_direction_down = true;	// default to beg.-end search

// layout components
	regex_entry->move(10, 10);
	regex_entry->resize(width()-20, regex_entry->sizeHint().height());
	find_direction_button_group->move(10, regex_entry->rect().bottom()+10);
	find_direction_button_group->resize(width()-20, regex_entry->sizeHint().height());
}

FindDialog::~FindDialog() {
}

// Accessors
bool FindDialog::isFindDirectionDown() const { return find_direction_down; }

protected:
bool find_direction_down;	// true==search from beg. to end
QRegExp regex;	// search pattern

private:
// Components
	QLineEdit *regex_entry;	// Text line entry for search pattern
	QPushButton *find_button;
	QPushButton *cancel_button;
	QRadioButton *find_direction_up_button;
	QRadioButton *find_direction_down_button;
	QButtonGroup *find_direction_button_group;
	QLabel *find_direction_label;
	QCheckBox *find_case_sensitive_checkbox;
};

