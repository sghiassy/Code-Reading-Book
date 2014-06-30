#include "debug.h"
#include "qtchat.h"
#include "statusbar.h"
#include <qtooltip.h>

	MainStatusBar::MainStatusBar(QWidget *parent, const char *name) :
		QStatusBar(parent, name) {
		if(::debuglevel & DEBUGLEVEL_CONSTRUCTOR)
			cerr << "MainStatusBar constructor called." << endl;

		chatters_count = 0;
		ignored_count = 0;
		autowarn = false;
		ignore_case = false;
		ignore_movement = false;
		redirect_pms = REDIRECT_OFF;

		chatters_count_label = new ClickableLabel(this);
		chatters_count_label->setAlignment(AlignCenter);
		addWidget(chatters_count_label, 1, false);
		QToolTip::add(chatters_count_label, "Number of chatters in room (including ignored).");

		ignored_count_label = new ClickableLabel(this);
		ignored_count_label->setAlignment(AlignCenter);
		addWidget(ignored_count_label, 1, false);
		QToolTip::add(ignored_count_label, "Number of ignored chatters in room.");

		autowarn_label = new ClickableLabel("<b>WarnPM</b>", this);
		autowarn_label->setTextFormat(Qt::RichText);
		autowarn_label->setFixedWidth(62);
		autowarn_label->setAlignment(AlignCenter);
		connect(autowarn_label, SIGNAL(mousePressed(QMouseEvent *)),
			SLOT(handleWarnPMClicked(QMouseEvent *)));
		addWidget(autowarn_label, 1, true);
		QToolTip::add(autowarn_label, "Automatically send warning to PMers.");

		ignore_case_label = new ClickableLabel("<b>CAPS</b>", this);
		ignore_case_label->setTextFormat(Qt::RichText);
		ignore_case_label->setFixedWidth(40);
		ignore_case_label->setAlignment(AlignCenter);
		connect(ignore_case_label, SIGNAL(mousePressed(QMouseEvent *)),
			SLOT(handleCapsClicked(QMouseEvent *)));
		addWidget(ignore_case_label, 1, true);
		QToolTip::add(ignore_case_label, "Display uppercase letters from chatters.");

		ignore_movement_label = new ClickableLabel("<b>Move</b>", this);
		ignore_movement_label->setTextFormat(Qt::RichText);
		ignore_movement_label->setFixedWidth(50);
		ignore_movement_label->setAlignment(AlignCenter);
		connect(ignore_movement_label, SIGNAL(mousePressed(QMouseEvent *)),
			SLOT(handleMovementClicked(QMouseEvent *)));
		addWidget(ignore_movement_label, 1, true);
		QToolTip::add(ignore_movement_label, "Display enter/leave messages.");

		redirect_pms_label = new ClickableLabel(" <b>Redirect PMs:</b> off", this);
		redirect_pms_label->setTextFormat(Qt::RichText);
		redirect_pms_label->setFixedWidth(150);
		redirect_pms_label->setAlignment(AlignLeft | AlignVCenter);
		connect(redirect_pms_label, SIGNAL(mousePressed(QMouseEvent *)),
			SLOT(handleRedirectPMsSet(QMouseEvent *)));
		addWidget(redirect_pms_label, 2, true);
		QToolTip::add(redirect_pms_label, "Redirection of PM messages from chatters.");

		online_indicator= new SimpleIndicatorWidget(Qt::green, Qt::red, this);
		addWidget(online_indicator, height(), true);
		QToolTip::add(online_indicator, "Connection status.");

		setLabels();	// init the labels' text
	}

	MainStatusBar::~MainStatusBar() {
		if(::debuglevel & DEBUGLEVEL_DESTRUCTOR)
			cerr << "MainStatusBar destructor called." << endl;
		delete online_indicator;
	}

void MainStatusBar::setLabels() {
	chatters_count_label->setText(QString("<b>Chatters:</b> ") + QString::number(chatters_count));
	chatters_count_label->setFixedWidth(82);

	ignored_count_label->setText(QString("<b>Ignored:</b> ")+QString::number(ignored_count));
	ignored_count_label->setFixedWidth(70);

	QToolTip::remove(autowarn_label);
	if(autowarn) {
		autowarn_label->setText("<b>WarnPM</b>");
		QToolTip::add(autowarn_label, "Automatically send warning to PMers.");
	} else {
		autowarn_label->setText("WarnPM");
		QToolTip::add(autowarn_label, "Do not automatically warn PMers.");
	}

	QToolTip::remove(ignore_case_label);
	if(ignore_case) {
		QToolTip::add(ignore_case_label, "Force all text from chatters to lowercase.");
		ignore_case_label->setText("caps");
	} else {
		QToolTip::add(ignore_case_label, "Display uppercase letters from chatters.");
		ignore_case_label->setText("<b>CAPS</b>");
	}

	ignore_movement_label->setText("Move");
	if(ignore_movement) {
		QToolTip::remove(ignore_movement_label);
		QToolTip::add(ignore_movement_label, "Do not display enter/leave messages.");
	} else {
		ignore_movement_label->setText("<b>Move</b>");
		QToolTip::add(ignore_movement_label, "Display enter/leave messages.");
	}

	QString txt;
	if(redirect_pms != REDIRECT_OFF)
		txt = QString("<b>Redirect PMs:</b> ");
	else
		txt = QString("Redirect PMs: ");
	switch(redirect_pms) {
		case REDIRECT_OFF:
			txt += QString("off");
			break;
		case REDIRECT_PUBLIC:
			txt += QString("<font color=\"red\">public</font>");
			break;
		case REDIRECT_PUBLIC_CENSORED:
			txt += QString("censored");
			break;
		case REDIRECT_NULL:
			txt += QString("/dev/null");
			break;
	}
	redirect_pms_label->setText(txt);
}

void MainStatusBar::setOnlineState(bool online) {	// set online indicator state
	online_indicator->setState(online);
}

void MainStatusBar::setAutoWarn(bool on) {	// set AutoWarn indicator state
	autowarn = on;
	setLabels();
}

void MainStatusBar::setIgnoreCase(bool on) {	// set IgnoreCase indicator state
	ignore_case = on;
	setLabels();
}

void MainStatusBar::setIgnoreMovement(bool on) {	// set IgnoreMovement indicator state
	ignore_movement = on;
	setLabels();
}

void MainStatusBar::setRedirectPMs(PMRedirectType t) {
	redirect_pms = t;
	setLabels();
}

void MainStatusBar::handleWarnPMClicked(QMouseEvent *e) {
	if(e->button() == QMouseEvent::LeftButton) {
		// tell world requested autowarn state
		emit warnPMClicked(!autowarn);
	}
}

void MainStatusBar::handleCapsClicked(QMouseEvent *e) {
	if(e->button() == QMouseEvent::LeftButton) {
		// tell world requested ignore case state
		emit capsClicked(!ignore_case);
	}
}

void MainStatusBar::handleMovementClicked(QMouseEvent *e) {
	if(e->button() == QMouseEvent::LeftButton) {
		// tell world requested ignore movement state
		emit movementClicked(!ignore_movement);
	}
}

void MainStatusBar::handleRedirectPMsSet(QMouseEvent *e) {
	if(e->button() == QMouseEvent::LeftButton) {
		// tell world requested PM redirect state
		emit redirectPMsSet((PMRedirectType)((redirect_pms+1)%4));
	}
}

