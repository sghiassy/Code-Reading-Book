#include "chatter.h"
#include "debug.h"
#include "prefsdialog.h"
#include "util.h"
#include <iomanip.h>
#include <iostream.h>
#include <stdlib.h>
#include <qtabbar.h>
#include <qvbuttongroup.h>

PrefsDialog::PrefsDialog(const ChatWidget *cw, QWidget *parent, const char *name, bool modal, WFlags f) {
	if(::debuglevel & DEBUGLEVEL_CONSTRUCTOR)
		cerr << "PrefsDialog constructor called." << endl;

	// add Preferences panes
	QSize  max_pane_size;
	general = new PrefsGeneralDialog(cw, this);
	addTab(general, "General");
	max_pane_size = max_pane_size.expandedTo(general->sizeHint());
	display = new PrefsDisplayDialog(this);
	addTab(display, "Display");
	max_pane_size = max_pane_size.expandedTo(display->sizeHint());
	connection = new PrefsConnectionDialog(this);
	addTab(connection, "Connection");
	max_pane_size = max_pane_size.expandedTo(connection->sizeHint());
	ignore = new PrefsIgnoreDialog(this);
	addTab(ignore, "Ignore");
	max_pane_size = max_pane_size.expandedTo(ignore->sizeHint());
	diagnostics = new PrefsDiagnosticsDialog(this);
	addTab(diagnostics, "Diagnostics");
	max_pane_size = max_pane_size.expandedTo(diagnostics->sizeHint());

	// define buttons
	setHelpButton();
	setCancelButton();
	setApplyButton();
	setOkButton();

	// notify panes of button clicks/pre-display
	connect(this, SIGNAL(aboutToShow()), general, SLOT(aboutToShow()));
	connect(this, SIGNAL(applyButtonPressed()), general, SLOT(handleAccept()));
	connect(this, SIGNAL(cancelButtonPressed()), general, SLOT(handleCancel()));
	connect(this, SIGNAL(helpButtonPressed()), general, SLOT(handleHelp()));

	connect(this, SIGNAL(aboutToShow()), display, SLOT(aboutToShow()));
	connect(this, SIGNAL(applyButtonPressed()), display, SLOT(handleAccept()));
	connect(this, SIGNAL(cancelButtonPressed()), display, SLOT(handleCancel()));
	connect(this, SIGNAL(helpButtonPressed()), display, SLOT(handleHelp()));

	connect(this, SIGNAL(aboutToShow()), connection, SLOT(aboutToShow()));
	connect(this, SIGNAL(applyButtonPressed()), connection, SLOT(handleAccept()));
	connect(this, SIGNAL(cancelButtonPressed()), connection, SLOT(handleCancel()));
	connect(this, SIGNAL(helpButtonPressed()), connection, SLOT(handleHelp()));

	connect(this, SIGNAL(aboutToShow()), ignore, SLOT(aboutToShow()));
	connect(this, SIGNAL(applyButtonPressed()), ignore, SLOT(handleAccept()));
	connect(this, SIGNAL(cancelButtonPressed()), ignore, SLOT(handleCancel()));
	connect(this, SIGNAL(helpButtonPressed()), ignore, SLOT(handleHelp()));

	connect(this, SIGNAL(aboutToShow()), diagnostics, SLOT(aboutToShow()));
	connect(this, SIGNAL(applyButtonPressed()), diagnostics, SLOT(handleAccept()));
	connect(this, SIGNAL(cancelButtonPressed()), diagnostics, SLOT(handleCancel()));
	connect(this, SIGNAL(helpButtonPressed()), diagnostics, SLOT(handleHelp()));

	// resize widget
//	resize(200, 330);
	setUseableSize(max_pane_size);

	// extras
	setCaption("QtChat Preferences");
}

PrefsDialog::~PrefsDialog() {
	if(::debuglevel & DEBUGLEVEL_DESTRUCTOR)
		cerr << "PrefsDialog destructor called." << endl;
	delete general;
	delete display;
	delete connection;
	delete ignore;
	delete diagnostics;
}

QSize PrefsDialog::useableSize() const {	// return size of useable widget area
	return size() - QSize(20, tabBar()->sizeHint().height() + 60);
}

void PrefsDialog::setUseableSize(QSize s) {	// set size of useable widget area
	resize(s + QSize(20, tabBar()->sizeHint().height() + 60));
}

void PrefsDialog::setCurrentTab(int id) {
	tabBar()->setCurrentTab(id);
}

PrefsDialogMember *PrefsDialog::getDialogWidget(PrefsDialogType t) const {
	PrefsDialogMember *pdm;
	switch(t) {
		case PREFS_GENERAL:
			pdm=general;
			break;
		case PREFS_DISPLAY:
			pdm=display;
			break;
		case PREFS_CONNECTION:
			pdm=connection;
			break;
		case PREFS_IGNORE:
			pdm=ignore;
			break;
		case PREFS_DIAGNOSTICS:
			pdm=diagnostics;
			break;
		default:
			pdm=0;
			break;
	}
	return pdm;
}

void PrefsDialogMember::aboutToShow() {}
void PrefsDialogMember::handleAccept() {}
void PrefsDialogMember::handleCancel() {}
void PrefsDialogMember::handleHelp() {}
QSize PrefsDialogMember::useableSize() const {
	if(parentWidget())
		return (dynamic_cast<PrefsDialog*>(parentWidget()))->useableSize();
	else
		return sizeHint();
}

//QSize PrefsDialogMember::sizeHint() const {
//	return QWidget::sizeHint();
//}

class ClickLabel : public QLabel {
	public:
		ClickLabel(QButton *_buddy, QString text, QWidget *parent, const char *name=0, WFlags f=0) :
			QLabel(_buddy, text, parent, name, f) {buddy = _buddy;}

	protected:
		void mousePressEvent(QMouseEvent *e) {
			if(buddy && e && e->button() == Qt::LeftButton)
				buddy->toggle();
			else
				QLabel::mousePressEvent(e);
		}
	private:
		QButton *buddy;
};

PrefsGeneralDialog::PrefsGeneralDialog(const ChatWidget *cw, QWidget *parent, const char *name, WFlags f) :
	chat_widget(cw) {
	if(::debuglevel & DEBUGLEVEL_CONSTRUCTOR)
		cerr << "PrefsGeneralDialog constructor called." << endl;
	enable_clone_detection = new QCheckBox("Enable Clone Autodetection", this);
	enable_clone_detection->move(10, 10);
	enable_clone_detection->resize(enable_clone_detection->sizeHint());
	QLabel *label = new QLabel("Clone Detection Threshold: ", this);
	label->move(10, enable_clone_detection->geometry().bottom()+10);
	label->resize(label->sizeHint());
	clone_detection_threshold = new QSpinBox(0, 100, 1, this);
	clone_detection_threshold->move(label->geometry().right(), label->geometry().y());
	clone_detection_threshold->resize(clone_detection_threshold->sizeHint());

	int y=clone_detection_threshold->geometry().bottom()+10;
	ignore_pms_enabled = new QCheckBox(this);
	ignore_pms_enabled->move(10, y);
	ignore_pms_enabled->resize(ignore_pms_enabled->sizeHint());
	label = new ClickLabel(ignore_pms_enabled, "Ignore PMs from ", this);
	label->move(ignore_pms_enabled->geometry().right()+3, y);
	label->resize(label->sizeHint());
	ignore_pms_friend_level = new QComboBox(false, this);
	ignore_pms_friend_level->insertItem(toString(STRANGER));
	ignore_pms_friend_level->insertItem(toString(FRIEND));
	ignore_pms_friend_level->insertItem(toString(TRUSTED_FRIEND));
	ignore_pms_friend_level->insertItem(toString(GOD));
	ignore_pms_friend_level->move(label->geometry().right()+3, y);
	ignore_pms_friend_level->resize(ignore_pms_friend_level->sizeHint());
	label = new ClickLabel(ignore_pms_enabled, " and below.", this);
	label->move(ignore_pms_friend_level->geometry().right()+3, y);
	label->resize(label->sizeHint());
	y = ignore_pms_friend_level->geometry().bottom()+10;
	ignore_pms_outside_room = new QCheckBox(this);
	ignore_pms_outside_room->move(10, y);
	ignore_pms_outside_room->resize(ignore_pms_outside_room->sizeHint());
	label = new ClickLabel(ignore_pms_outside_room, "Ignore PMs from chatters outside room", this);
	label->move(ignore_pms_outside_room->geometry().right()+3, y);
	label->resize(label->sizeHint());

	label = new QLabel("CosyChat Friendlevel: ", this);
	label->move(10, ignore_pms_outside_room->geometry().bottom()+3);
	label->resize(label->sizeHint());
	cosychat_friendlevel = new QComboBox(false, this);
	cosychat_friendlevel->insertItem(toString(STRANGER));
	cosychat_friendlevel->insertItem(toString(FRIEND));
	cosychat_friendlevel->insertItem(toString(TRUSTED_FRIEND));
	cosychat_friendlevel->insertItem(toString(GOD));
	cosychat_friendlevel->move(label->geometry().right()+3, label->y());
	cosychat_friendlevel->resize(cosychat_friendlevel->sizeHint());
}

PrefsGeneralDialog::~PrefsGeneralDialog() {
	if(::debuglevel & DEBUGLEVEL_DESTRUCTOR)
		cerr << "PrefsGeneralDialog destructor called." << endl;
}

QSize PrefsGeneralDialog::sizeHint() const {
	return PrefsDialogMember::sizeHint();
}

void PrefsGeneralDialog::aboutToShow() {
	prev_enable_clone_detection = chat_widget->isCloneDetectionEnabled();
	enable_clone_detection->setChecked(prev_enable_clone_detection);

	prev_clone_detection_threshold = chat_widget->getCloneDetectionThreshold();
	clone_detection_threshold->setValue(prev_clone_detection_threshold);

	prev_ignore_pms_enabled = chat_widget->isIgnorePMsEnabled();
	ignore_pms_enabled->setChecked(prev_ignore_pms_enabled);

	prev_ignore_pms_friend_level = chat_widget->getIgnorePMsFriendLevel();
	ignore_pms_friend_level->setCurrentItem(toInt(prev_ignore_pms_friend_level));

	prev_ignore_pms_outside_room = chat_widget->isIgnorePMsOutsideRoomEnabled();
	ignore_pms_outside_room->setChecked(prev_ignore_pms_outside_room);

	prev_cosychat_friendlevel = chat_widget->getCosyChatFriendLevel();
	cosychat_friendlevel->setCurrentItem(toInt(prev_cosychat_friendlevel));
}

void PrefsGeneralDialog::handleAccept() {
	if(prev_enable_clone_detection != enable_clone_detection->isChecked())
		emit cloneDetectionEnabledChanged(enable_clone_detection->isChecked());
	if(prev_clone_detection_threshold != clone_detection_threshold->value())
		emit cloneDetectionThresholdChanged(clone_detection_threshold->value());
	if(prev_ignore_pms_enabled != ignore_pms_enabled->isChecked())
		emit ignorePMsEnabledChanged(ignore_pms_enabled->isChecked());
	if(prev_ignore_pms_outside_room != ignore_pms_outside_room->isChecked())
		emit ignorePMsOutsideRoomEnabledChanged(ignore_pms_outside_room->isChecked());
	if(prev_ignore_pms_friend_level != toFriendLevel(ignore_pms_friend_level->currentItem()))
		emit ignorePMsFriendLevelChanged(toFriendLevel(ignore_pms_friend_level->currentItem()));
	if(prev_cosychat_friendlevel != toFriendLevel(cosychat_friendlevel->currentItem()))
		emit cosyChatFriendLevelChanged(toFriendLevel(cosychat_friendlevel->currentItem()));
}

PrefsDisplayDialog::PrefsDisplayDialog(QWidget *parent, const char *name, WFlags f) {
	if(::debuglevel & DEBUGLEVEL_CONSTRUCTOR)
		cerr << "PrefsDisplayDialog constructor called." << endl;
}

PrefsDisplayDialog::~PrefsDisplayDialog() {
	if(::debuglevel & DEBUGLEVEL_DESTRUCTOR)
		cerr << "PrefsDisplayDialog destructor called." << endl;
}

QSize PrefsDisplayDialog::sizeHint() const {
	return PrefsDialogMember::sizeHint();
}

PrefsConnectionDialog::PrefsConnectionDialog(QWidget *parent, const char *name, WFlags f) {
	if(::debuglevel & DEBUGLEVEL_CONSTRUCTOR)
		cerr << "PrefsConnectionDialog constructor called." << endl;

	// Instantiate widgets
	chat_server = new QComboBox(true, this);
	chat_port = new QComboBox(true, this);
	cookie_server = new QComboBox(true, this);
	cookie_port = new QComboBox(true, this);
	auto_login = new QCheckBox("Auto &Login At Startup", this);
	username = new QComboBox(true, this);
	password = new QLineEdit(this);

	// Layout widgets
	QLabel *label;
	QRect geom;
	label = new QLabel(chat_server, "Chat &Servers", this);
	label->move(10, 10);
	label->resize(label->sizeHint());
	geom = label->geometry();
	chat_server->move(geom.right()+5, geom.top());
	chat_server->resize(chat_server->sizeHint());

	label = new QLabel(chat_port, "Chat &Ports", this);
	label->move(10, geom.bottom()+15);
	label->resize(label->sizeHint());
	geom = label->geometry();
	chat_port->move(geom.right()+5, geom.top());
	chat_port->resize(chat_port->sizeHint());

	label = new QLabel(cookie_server, "Cookie S&ervers", this);
	label->move(10, geom.bottom()+15);
	label->resize(label->sizeHint());
	geom = label->geometry();
	cookie_server->move(geom.right()+5, geom.top());
	cookie_server->resize(cookie_server->sizeHint());

	label = new QLabel(cookie_port, "Cookie P&orts", this);
	label->move(10, geom.bottom()+15);
	label->resize(label->sizeHint());
	geom = label->geometry();
	cookie_port->move(geom.right()+5, geom.top());
	cookie_port->resize(cookie_port->sizeHint());

	auto_login->move(10, geom.bottom()+15);
	auto_login->resize(auto_login->sizeHint());

	label = new QLabel(username, "Yahoo &Username", this);
	label->move(10, auto_login->geometry().bottom()+10);
	label->resize(label->sizeHint());
	geom = label->geometry();
	username->move(geom.right()+5, geom.top());
	username->resize(username->sizeHint());

	label = new QLabel(password, "Yahoo P&assword", this);
	label->move(10, geom.bottom()+15);
	label->resize(label->sizeHint());
	geom = label->geometry();
	password->move(geom.right()+5, geom.top());
	password->resize(password->sizeHint());
}

PrefsConnectionDialog::~PrefsConnectionDialog() {
	if(::debuglevel & DEBUGLEVEL_DESTRUCTOR)
		cerr << "PrefsConnectionDialog destructor called." << endl;
	delete chat_server;
	delete chat_port;
	delete cookie_server;
	delete cookie_port;
	delete auto_login;
	delete username;
	delete password;
}

QSize PrefsConnectionDialog::sizeHint() const {
	return PrefsDialogMember::sizeHint();
}

QSpinBox *spam_threshold;	// max number of repeats before autoignore

PrefsIgnoreDialog::PrefsIgnoreDialog(QWidget *parent, const char *name, WFlags f) {
	if(::debuglevel & DEBUGLEVEL_CONSTRUCTOR)
		cerr << "PrefsIgnoreDialog constructor called." << endl;
	autoignore_enable = new QCheckBox("Enable &Autoignore", this);
	autoignore_enable->move(10, 10);
	autoignore_enable->resize(autoignore_enable->sizeHint());
	spam_threshold = new QSpinBox(0, 20, 1, this);
	spam_threshold->setSpecialValueText("Disabled");
	QLabel *label = new QLabel("Number of repeat posts to autoignore: ", this);

	label->move(10, autoignore_enable->geometry().bottom()+10);
	label->resize(label->sizeHint());
	connect(autoignore_enable, SIGNAL(toggled(bool)),
		label, SLOT(setEnabled(bool)));
	spam_threshold->move(10+label->geometry().right(), autoignore_enable->geometry().bottom()+10);
	spam_threshold->resize(spam_threshold->sizeHint());
	label = new QLabel("Autoignore chatters up to: ", this);
	label->move(10, spam_threshold->geometry().bottom()+10);
	label->resize(label->sizeHint());
	connect(autoignore_enable, SIGNAL(toggled(bool)),
		label, SLOT(setEnabled(bool)));
	autoignore_friend_level_combobox = new QComboBox(false, this);
	autoignore_friend_level_combobox->insertItem(toString(STRANGER));
	autoignore_friend_level_combobox->insertItem(toString(FRIEND));
	autoignore_friend_level_combobox->insertItem(toString(TRUSTED_FRIEND));
	autoignore_friend_level_combobox->insertItem(toString(GOD));
	autoignore_friend_level_combobox->resize(autoignore_friend_level_combobox->sizeHint());
	autoignore_friend_level_combobox->move(label->geometry().right(),  label->y());
	autoignore_enable_button_group = new QGroupBox("AutoIgnore Conditions", this);	// holds following buttons

	autoignore_first_post_URL_enable = new QCheckBox("First Post &URL", autoignore_enable_button_group);
	autoignore_first_post_URL_enable->resize(autoignore_first_post_URL_enable->sizeHint());
	autoignore_first_post_URL_enable->move(10, 20);

	autoignore_first_post_PM_enable = new QCheckBox("First Post &PM", autoignore_enable_button_group);
	autoignore_first_post_PM_enable->resize(autoignore_first_post_PM_enable->sizeHint());
	autoignore_first_post_PM_enable->move(10, autoignore_first_post_URL_enable->geometry().bottom()+5);

	autoignore_single_post_enable = new QCheckBox("Single Post", autoignore_enable_button_group);
	autoignore_single_post_enable->resize(autoignore_single_post_enable->sizeHint());
	autoignore_single_post_enable->move(10, autoignore_first_post_PM_enable->geometry().bottom()+5);

	autoignore_caps_enable = new QCheckBox("Upper&case Post", autoignore_enable_button_group);
	autoignore_caps_enable->resize(autoignore_caps_enable->sizeHint());
	autoignore_caps_enable->move(10, autoignore_single_post_enable->geometry().bottom()+5);

	autoignore_multiline_enable = new QCheckBox("Multiline Post", autoignore_enable_button_group);
	autoignore_multiline_enable->resize(autoignore_multiline_enable->sizeHint());
	autoignore_multiline_enable->move(10, autoignore_caps_enable->geometry().bottom()+5);
	autoignore_enable_button_group->resize(spam_threshold->geometry().right()-10, autoignore_caps_enable->height()*5+45);
	autoignore_enable_button_group->move(10, autoignore_friend_level_combobox->geometry().bottom()+10);
	connect(autoignore_enable, SIGNAL(toggled(bool)),
		spam_threshold, SLOT(setEnabled(bool)));
	connect(autoignore_enable, SIGNAL(toggled(bool)),
		autoignore_friend_level_combobox, SLOT(setEnabled(bool)));
	connect(autoignore_enable, SIGNAL(toggled(bool)),
		autoignore_enable_button_group, SLOT(setEnabled(bool)));
	connect(autoignore_enable, SIGNAL(toggled(bool)),
		autoignore_first_post_URL_enable, SLOT(setEnabled(bool)));
	connect(autoignore_enable, SIGNAL(toggled(bool)),
		autoignore_first_post_PM_enable, SLOT(setEnabled(bool)));
	connect(autoignore_enable, SIGNAL(toggled(bool)),
		autoignore_single_post_enable, SLOT(setEnabled(bool)));
	connect(autoignore_enable, SIGNAL(toggled(bool)),
		autoignore_caps_enable, SLOT(setEnabled(bool)));
	connect(autoignore_enable, SIGNAL(toggled(bool)),
		autoignore_multiline_enable, SLOT(setEnabled(bool)));

}

PrefsIgnoreDialog::~PrefsIgnoreDialog() {
	if(::debuglevel & DEBUGLEVEL_DESTRUCTOR)
		cerr << "PrefsIgnoreDialog destructor called." << endl;
}

QSize PrefsIgnoreDialog::sizeHint() const {
	return PrefsDialogMember::sizeHint();
}

void PrefsIgnoreDialog::aboutToShow() {
}

void PrefsIgnoreDialog::handleAccept() {
	emit autoIgnoreCountChanged(spam_threshold->value());
	emit autoIgnoreEnableClicked(autoignore_enable->isChecked());
	emit autoIgnoreFriendLevelChanged(toFriendLevel(autoignore_friend_level_combobox->currentItem()));
}


PrefsDiagnosticsDialog::PrefsDiagnosticsDialog(QWidget *parent, const char *name, WFlags f) {
	if(::debuglevel & DEBUGLEVEL_CONSTRUCTOR)
		cerr << "PrefsDiagnosticsDialog constructor called." << endl;

	debuglevel_enable = new QGroupBox("Enable Debug Messages", this);
	debuglevel_network = new QCheckBox("Network", debuglevel_enable);
	debuglevel_constructor = new QCheckBox("C++ Constructor", debuglevel_enable);
	debuglevel_copyconstructor = new QCheckBox("C++ Copy Constructor", debuglevel_enable);
	debuglevel_destructor = new QCheckBox("C++ Destructor", debuglevel_enable);

	debuglevel_verbosity = new QSpinBox(0, 15, 1, this);
	debuglevel_verbosity_label = new QLabel("Debug Message Level", this);
	debuglevel_verbosity_label->setBuddy(debuglevel_verbosity);
	debuglevel_notice = new QCheckBox("Notice", debuglevel_enable);
	debuglevel_warning = new QCheckBox("Warning", debuglevel_enable);
	debuglevel_error = new QCheckBox("Error", debuglevel_enable);
	debuglevel_fatal = new QCheckBox("Fatal", debuglevel_enable);

	debuglevel_enable->move(10, 10);
	debuglevel_network->move(10, 25);
	debuglevel_network->resize(debuglevel_network->sizeHint());
	debuglevel_constructor->move(10, debuglevel_network->y()+debuglevel_network->height());
	debuglevel_constructor->resize(debuglevel_constructor->sizeHint());
	debuglevel_copyconstructor->move(10, debuglevel_constructor->y()+debuglevel_constructor->height());
	debuglevel_copyconstructor->resize(debuglevel_copyconstructor->sizeHint());
	debuglevel_destructor->move(10, debuglevel_copyconstructor->y()+debuglevel_copyconstructor->height());
	debuglevel_destructor->resize(debuglevel_destructor->sizeHint());
	debuglevel_notice->move(10, debuglevel_destructor->y()+debuglevel_destructor->height());
	debuglevel_notice->resize(debuglevel_notice->sizeHint());
	debuglevel_warning->move(10, debuglevel_notice->y()+debuglevel_notice->height());
	debuglevel_warning->resize(debuglevel_warning->sizeHint());
	debuglevel_error->move(10, debuglevel_warning->y()+debuglevel_warning->height());
	debuglevel_error->resize(debuglevel_error->sizeHint());
	debuglevel_fatal->move(10, debuglevel_error->y()+debuglevel_error->height());
	debuglevel_fatal->resize(debuglevel_fatal->sizeHint());
	debuglevel_enable->resize(20+debuglevel_copyconstructor->width(), 20+debuglevel_fatal->height()+debuglevel_fatal->y());

	debuglevel_verbosity_label->move(10, debuglevel_enable->y() + debuglevel_enable->height() + 10);
	debuglevel_verbosity_label->resize(debuglevel_verbosity_label->sizeHint());
	debuglevel_verbosity->move(debuglevel_verbosity_label->x() + debuglevel_verbosity_label->width()+15,
		debuglevel_verbosity_label->y());
	debuglevel_verbosity->resize(debuglevel_verbosity->sizeHint());
}

PrefsDiagnosticsDialog::~PrefsDiagnosticsDialog() {
	if(::debuglevel & DEBUGLEVEL_DESTRUCTOR)
		cerr << "PrefsDiagnosticsDialog destructor called." << endl;
}

QSize PrefsDiagnosticsDialog::sizeHint() const {
	return QSize(debuglevel_enable->geometry().right()+10, debuglevel_verbosity->geometry().bottom()+10);
}

void PrefsDiagnosticsDialog::handleAccept() {
	setBitfield(::debuglevel, atoi(debuglevel_verbosity->text()), DEBUGLEVEL_VERBOSITY);
	setBitfield(::debuglevel, (int)debuglevel_network->isChecked(), DEBUGLEVEL_NETWORK);
	setBitfield(::debuglevel, (int)debuglevel_constructor->isChecked(), DEBUGLEVEL_CONSTRUCTOR);
	setBitfield(::debuglevel, (int)debuglevel_copyconstructor->isChecked(), DEBUGLEVEL_COPYCONSTRUCTOR);
	setBitfield(::debuglevel, (int)debuglevel_destructor->isChecked(), DEBUGLEVEL_DESTRUCTOR);
	setBitfield(::debuglevel, (int)debuglevel_notice->isChecked(), DEBUGLEVEL_NOTICE);
	setBitfield(::debuglevel, (int)debuglevel_warning->isChecked(), DEBUGLEVEL_WARNING);
	setBitfield(::debuglevel, (int)debuglevel_error->isChecked(), DEBUGLEVEL_ERROR);
	setBitfield(::debuglevel, (int)debuglevel_fatal->isChecked(), DEBUGLEVEL_FATAL);
} 

void PrefsDiagnosticsDialog::aboutToShow() {
	debuglevel_verbosity->setValue(::debuglevel & DEBUGLEVEL_VERBOSITY);
	debuglevel_network->setChecked((bool)getBitfield(::debuglevel, DEBUGLEVEL_NETWORK));
	debuglevel_constructor->setChecked((bool)getBitfield(::debuglevel, DEBUGLEVEL_CONSTRUCTOR));
	debuglevel_copyconstructor->setChecked((bool)getBitfield(::debuglevel, DEBUGLEVEL_COPYCONSTRUCTOR));
	debuglevel_destructor->setChecked((bool)getBitfield(::debuglevel, DEBUGLEVEL_DESTRUCTOR));
	debuglevel_notice->setChecked((bool)getBitfield(::debuglevel, DEBUGLEVEL_NOTICE));
	debuglevel_warning->setChecked((bool)getBitfield(::debuglevel, DEBUGLEVEL_WARNING));
	debuglevel_error->setChecked((bool)getBitfield(::debuglevel, DEBUGLEVEL_ERROR));
	debuglevel_fatal->setChecked((bool)getBitfield(::debuglevel, DEBUGLEVEL_FATAL));
} 
