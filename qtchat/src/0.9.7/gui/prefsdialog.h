#ifndef _PREFSDIALOG_H
#define _PREFSDIALOG_H

#include "chatmain.h"
#include "chatter.h"
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qspinbox.h>
#include <qtabdialog.h>

class PrefsDialogMember;

class PrefsDialog : public QTabDialog {
	public:
		PrefsDialog(const ChatWidget *cw, QWidget *parent=0, const char *name=0, bool modal=false, WFlags f=0);
		~PrefsDialog();

		QSize useableSize() const;	// return size of useable widget area
		void setUseableSize(QSize s);	// set size of useable widget area
		void setCurrentTab(int id);	// show id'th tab

		enum PrefsDialogType {
			PREFS_GENERAL,
			PREFS_DISPLAY,
			PREFS_CONNECTION,
			PREFS_IGNORE,
			PREFS_DIAGNOSTICS
		};
		PrefsDialogMember *getDialogWidget(PrefsDialogType t) const;

	private:
		PrefsDialogMember *general;
		PrefsDialogMember *display;
		PrefsDialogMember *connection;
		PrefsDialogMember *ignore;
		PrefsDialogMember *diagnostics;
};

class PrefsDialogMember : public QWidget {
	Q_OBJECT

	public:
		virtual QSize useableSize() const;
//		virtual QSize sizeHint() const;

	public slots:
		virtual void aboutToShow();	// called before member is about to show
		virtual void handleAccept();	// Apply/OK button pressed
		virtual void handleCancel();	// Cancel button pressed (default==hide)
		virtual void handleHelp();	// Help button pressed... show pane-specific help
};

class PrefsGeneralDialog : public PrefsDialogMember {
	Q_OBJECT
	public:
		PrefsGeneralDialog(const ChatWidget *cw, QWidget *parent=0, const char *name=0, WFlags f=0);
		~PrefsGeneralDialog();

		virtual QSize sizeHint() const;

		bool isCloneDetectionEnabled() const;
		int getCloneDetectionThreshold() const;
		bool isIgnorePMsEnabled() const {
			return ignore_pms_enabled->isChecked();
		}
		bool isIgnorePMsOutsideRoomEnabled() const {
			return ignore_pms_outside_room->isChecked();
		}
		FriendLevel getIgnorePMsFriendLevel() const {
			return toFriendLevel(ignore_pms_friend_level->currentItem());
		}
		FriendLevel getCosyChatFriendLevel() const {
			return toFriendLevel(cosychat_friendlevel->currentItem());
		}

	signals:
		void cloneDetectionEnabledChanged(bool enabled);
		void cloneDetectionThresholdChanged(int threshold);
		void ignorePMsEnabledChanged(bool enabled);
		void ignorePMsOutsideRoomEnabledChanged(bool enabled);
		void ignorePMsFriendLevelChanged(FriendLevel fl);
		void cosyChatFriendLevelChanged(FriendLevel fl);

	public slots:
		void setCloneDetectionEnabled(bool on) {
			enable_clone_detection->setChecked(on);
		}
		void setCloneDetectionThreshold(int t) {
			if(t < 0) t = 0;
			else if(t > 100) t = 100;
			clone_detection_threshold->setValue(t);
		}
		void setIgnorePMsEnabled(bool on) {
			ignore_pms_enabled->setChecked(on);
		}
		void setIgnorePMsOutsideRoomEnabled(bool on) {
			ignore_pms_outside_room->setChecked(on);
		}
		void setIgnorePMsFriendLevel(FriendLevel fl) {
			ignore_pms_friend_level->setCurrentItem(toInt(fl));
		}
		void setCosyChatFriendLevel(FriendLevel fl) {
			cosychat_friendlevel->setCurrentItem(toInt(fl));
		}
		virtual void aboutToShow();	// called before member is about to show
		virtual void handleAccept();	// Apply/OK button pressed

	private:
		const ChatWidget *chat_widget;

		bool prev_enable_clone_detection;
		QCheckBox *enable_clone_detection;

		int prev_clone_detection_threshold;
		QSpinBox *clone_detection_threshold;

		bool prev_ignore_pms_enabled;
		QCheckBox *ignore_pms_enabled;	// ignore PMs?

		FriendLevel prev_ignore_pms_friend_level;
		QComboBox *ignore_pms_friend_level;	// ...from people <= friend level?

		bool prev_ignore_pms_outside_room;
		QCheckBox *ignore_pms_outside_room;	// ignore PMs from people not in room?

		FriendLevel prev_cosychat_friendlevel;
		QComboBox *cosychat_friendlevel;	// ignore people <= friendlevel if enabled

};

class PrefsDisplayDialog : public PrefsDialogMember {
	Q_OBJECT
	public:
		PrefsDisplayDialog(QWidget *parent=0, const char *name=0, WFlags f=0);
		~PrefsDisplayDialog();

		virtual QSize sizeHint() const;

	public slots:

	private:
};

class PrefsConnectionDialog : public PrefsDialogMember {
	Q_OBJECT
	public:
		PrefsConnectionDialog(QWidget *parent=0, const char *name=0, WFlags f=0);
		~PrefsConnectionDialog();

		virtual QSize sizeHint() const;

	public slots:

	private:
		QComboBox *chat_server;
		QComboBox *chat_port;
		QComboBox *cookie_server;
		QComboBox *cookie_port;
		QCheckBox *auto_login;
		QComboBox *username;
		QLineEdit *password;
};

class PrefsIgnoreDialog : public PrefsDialogMember {
	Q_OBJECT
	public:
		PrefsIgnoreDialog(QWidget *parent=0, const char *name=0, WFlags f=0);
		~PrefsIgnoreDialog();

		virtual QSize sizeHint() const;
// Accessors
		int getMaxNumDuplicates() const {
			return spam_threshold->value();
		}
		FriendLevel getAutoIgnoreFriendLevel() const {
			return toFriendLevel(autoignore_friend_level_combobox->currentItem());
		}
		bool isAutoIgnoreEnabled() const {
			return autoignore_enable->isChecked();
		}
		bool isAutoIgnoreFirstPostURLEnabled() const {
			return autoignore_first_post_URL_enable->isChecked();
		}
		bool isAutoIgnoreFirstPostPMEnabled() const {
			return autoignore_first_post_PM_enable->isChecked();
		}
		bool isAutoIgnoreSinglePostEnabled() const {
			return autoignore_single_post_enable->isChecked();
		}
		bool isAutoIgnoreCapsEnabled() const {
			return autoignore_caps_enable->isChecked();
		}
		bool isAutoIgnoreMultilineEnabled() const {
			return autoignore_multiline_enable->isChecked();
		}

	signals:
		void autoIgnoreCountChanged(int n);
		void autoIgnoreEnableClicked(bool enabled);
		void autoIgnoreFriendLevelChanged(FriendLevel fl);
		void autoIgnoreFirstPostURLEnableClicked(bool enabled);
		void autoIgnoreFirstPostPMEnableClicked(bool enabled);
		void autoIgnoreSinglePostEnableClicked(bool enabled);
		void autoIgnoreCapsEnableClicked(bool enabled);
		void autoIgnoreMultilineEnableClicked(bool enabled);

	public slots:
		virtual void aboutToShow();
		virtual void handleAccept();
		void setMaxNumDuplicates(int n) {
			if(n >= 0)
				spam_threshold->setValue(n);
			else
				spam_threshold->setValue(0);
		}
		void setAutoIgnoreFriendLevel(FriendLevel fl) {
			autoignore_friend_level_combobox->setCurrentItem(toInt(fl));
		}
		void setAutoIgnoreEnabled(bool on) {
			autoignore_enable->setChecked(on);
		}
		void setAutoIgnoreFirstPostURLEnabled(bool on) {
			autoignore_first_post_URL_enable->setChecked(on);
		}
		void setAutoIgnoreFirstPostPMEnabled(bool on) {
			autoignore_first_post_PM_enable->setChecked(on);
		}
		void setAutoIgnoreSinglePostEnabled(bool on) {
			autoignore_single_post_enable->setChecked(on);
		}
		void setAutoIgnoreCapsEnabled(bool on) {
			autoignore_caps_enable->setChecked(on);
		}
		void setAutoIgnoreMultilineEnabled(bool on) {
			autoignore_multiline_enable->setChecked(on);
		}

	private:
		QSpinBox *spam_threshold;	// max number of repeats before autoignore
		QComboBox autoignore_type;	// type of autoignore
		QComboBox *autoignore_friend_level_combobox;
		QCheckBox *autoignore_enable;	// is autoignore on?
		QGroupBox *autoignore_enable_button_group;	// holds following buttons
		QCheckBox *autoignore_first_post_URL_enable;	// autoignore first post URL?
		QCheckBox *autoignore_first_post_PM_enable;	// autoignore first post PM?
		QCheckBox *autoignore_single_post_enable;	// autoignore single post spam?
		QCheckBox *autoignore_caps_enable;	// autoignore uppercase posts?
		QCheckBox *autoignore_multiline_enable;	// autoignore multiline posts?

		// Methods
		void handleAutoIgnoreEnableClicked(bool enabled);
};

class PrefsDiagnosticsDialog : public PrefsDialogMember {
	Q_OBJECT
	public:
		PrefsDiagnosticsDialog(QWidget *parent=0, const char *name=0, WFlags f=0);
		~PrefsDiagnosticsDialog();

		virtual QSize sizeHint() const;

	public slots:
		virtual void aboutToShow();
		virtual void handleAccept();

	private:
		QGroupBox *debuglevel_enable;
			QCheckBox *debuglevel_network;
			QCheckBox *debuglevel_constructor;
			QCheckBox *debuglevel_copyconstructor;
			QCheckBox *debuglevel_destructor;
			QCheckBox *debuglevel_notice;
			QCheckBox *debuglevel_warning;
			QCheckBox *debuglevel_error;
			QCheckBox *debuglevel_fatal;

		QSpinBox *debuglevel_verbosity;
		QLabel *debuglevel_verbosity_label;
};

#endif	// _PREFSDIALOG_H
