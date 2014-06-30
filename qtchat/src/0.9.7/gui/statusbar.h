#ifndef _STATUSBAR_H
#define _STATUSBAR_H

#include <qstatusbar.h>
#include <qlabel.h>
#include <iostream>
#include "chatmain.h"	// enum PMRedirectType
#include "simpleindicator.h"

class ClickableLabel : public QLabel {
	Q_OBJECT
	public:
		ClickableLabel(const QString &text, QWidget *parent, const char *name=0, WFlags f=0) :
			QLabel(text, parent, name, f) {}
		ClickableLabel(QWidget *parent, const char *name=0, WFlags f=0) :
			QLabel(parent, name, f) {}

	signals:
		void mousePressed(QMouseEvent *e);

	protected:
		virtual void mousePressEvent(QMouseEvent *e) {
			emit mousePressed(e);
		}
};

class MainStatusBar : public QStatusBar {
	Q_OBJECT
	public:
		MainStatusBar(QWidget *parent=0, const char *name=0);
		~MainStatusBar();

	// Accessors
		bool getOnlineState() const {
			return online_indicator->getState();
		}

	public slots:
		void setIgnoredCount(unsigned int n) {	// set no. of ignored chatters in room
			ignored_count = n;
			setLabels();
		}
		void setChattersCount(unsigned int n) {	// set no. of chatters in room
			chatters_count = n;
			setLabels();
		}
		void setOnlineState(bool online);	// set online indicator state
		void setAutoWarn(bool on);	// set AutoWarn indicator state
		void setIgnoreCase(bool on);	// set IgnoreCase indicator state
		void setIgnoreMovement(bool on);	// set IgnoreMovement indicator state
		void setRedirectPMs(PMRedirectType t);

	signals:
		void warnPMClicked(bool new_state);
		void capsClicked(bool new_state);
		void movementClicked(bool new_state);
		void redirectPMsSet(PMRedirectType t);

	protected:

	private slots:
		void handleWarnPMClicked(QMouseEvent *);
		void handleCapsClicked(QMouseEvent *);
		void handleMovementClicked(QMouseEvent *);
		void handleRedirectPMsSet(QMouseEvent *);

	private:
		ClickableLabel *ignored_count_label;
		ClickableLabel *chatters_count_label;
		int ignored_count;
		int chatters_count;
		SimpleIndicatorWidget *online_indicator;
		ClickableLabel *autowarn_label;
		ClickableLabel *ignore_case_label;
		ClickableLabel *ignore_movement_label;
		ClickableLabel *redirect_pms_label;
		bool autowarn;
		bool ignore_case;
		bool ignore_movement;
		PMRedirectType redirect_pms;
		void setLabels();
};

#endif	// _STATUSBAR_H
