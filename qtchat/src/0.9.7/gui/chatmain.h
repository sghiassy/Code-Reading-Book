#ifndef _CHATMAIN_H
#define _CHATMAIN_H

#include <iostream.h>
#include <qapplication.h>
#include <qfont.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qlistbox.h>
#include <qmainwindow.h>
#include <qpopupmenu.h>
#include <qpushbutton.h>
//#include "chatter.h"
#include "debug.h"
#include "misc.h"
#include "state.h"

enum ChatEntryType {
	CET_SAY_OUTGOING,
	CET_SAY_INCOMING,
	CET_PM_OUTGOING,
	CET_PM_INCOMING,
	CET_EMOTION,
	CET_ENTER,
	CET_LEAVE,
	CET_LOGIN,
	CET_LOGOUT,
	CET_INVITE,
	CET_AWAY,
	CET_BUDDY,
	CET_SYSTEM_MESSAGE,
	CET_NULL
};

enum PMRedirectType {
	REDIRECT_OFF=0,
	REDIRECT_PUBLIC,
	REDIRECT_PUBLIC_CENSORED,
	REDIRECT_NULL
};

const char *toString(PMRedirectType);
PMRedirectType toPMRedirectType(const char *);

class Chatter;

class ChatEntry : public QListBoxItem {
	public:

		ChatEntry(ChatEntryType t, const Chatter *c, QString text, QListBox *parent=0);
		ChatEntry(const ChatEntry &c);
		~ChatEntry();

		virtual QString text() const;

		operator Chatter*() {
			return chatter;
		}
		Chatter* getChatter() const {
			return chatter;
		}
		ChatEntryType getType() const {
			return type;
		}
		const QListBox *getListBox() const {
			return lb;
		}
		void setListBox(const QListBox *_lb) {
			lb = _lb;
		}

	protected:
		virtual void paint(QPainter *p);
		virtual int height(const QListBox *b) const;
		virtual int width(const QListBox *b) const;

	private:
		Chatter *chatter;
		QString txt;
		ChatEntryType type;
		const QListBox *lb;
		int cached_height;
		int old_listbox_width;

		void drawMarkupText(QPainter *p, int x, int y, int w, int h, QString txt);
};

// QListBox class that exports the mouse click signal and redefines resize
class MyListBox : public QListBox {
	Q_OBJECT
	public:
		MyListBox(QWidget *parent=0, QString name=0, WFlags f=0) :
			QListBox(parent, name, f) {

			if(::debuglevel & DEBUGLEVEL_CONSTRUCTOR)
				cerr << "MyListBox constructor called." << endl;
		}

		QListBoxItem *getItemAt(int index) {
			return item(index);
		}
		int findItem(int i) { return QListBox::findItem(i); }
		void updateItem(int index) {QListBox::updateItem(index);}
		void updateItem(QListBoxItem *i) {QListBox::updateItem(i);}

	signals:
		void mouseClick(QListBoxItem *chatter, int index, QMouseEvent *e);

	protected:
		virtual void mousePressEvent(QMouseEvent *e);
		virtual void resizeEvent(QResizeEvent *e);
};


class ChatWidget : public QWidget, public StateObject {
	Q_OBJECT
	public:
		static const int MAX_LINE_LENGTH;
		static const char *AUTOWARN_PM_MSG_DEFAULT;

		ChatWidget(QWidget *parent=0, QString name=0, WFlags f=0);
		~ChatWidget();

		bool getIgnoreCase() const {
			return ignore_case;
		}
		bool getIgnoreMovement() const {
			return ignore_movement;
		}
		bool isIgnorePMsEnabled() const {
			return ignore_pms;
		}
		FriendLevel getIgnorePMsFriendLevel () const {
			return ignore_pms_friend_level;
		}
		bool isIgnorePMsOutsideRoomEnabled() const {
			return ignore_pms_outside_room;
		}
		bool getAutoWarnOnPM() const {
			return autowarn_pm;
		}
		bool isAutoIgnoreEnabled() const {
			return autoignore_enabled;
		}
		FriendLevel getAutoIgnoreFriendLevel() const {
			return autoignore_friend_level;
		}
		bool isAutoIgnoreFirstPostURLEnabled() const {
			return autoignore_first_post_URL_enabled;
		}
		bool isAutoIgnoreFirstPostPMEnabled() const {
			return autoignore_first_post_PM_enabled;
		}
		bool isAutoIgnoreSinglePostEnabled() const {
			return autoignore_single_post_enabled;
		}
		bool isAutoIgnoreCapsEnabled() const {
			return autoignore_caps_enabled;
		}
		bool isAutoIgnoreMultilineEnabled() const {
			return autoignore_multiline_enabled;
		}
		PMRedirectType getRedirectPMs() const {
			if(redirect_public) return REDIRECT_PUBLIC;
			else if(redirect_public_censored) return REDIRECT_PUBLIC_CENSORED;
			else if(redirect_null) return REDIRECT_NULL;
			else return REDIRECT_OFF;
		}

		bool getRedirectPMs(PMRedirectType to) const {
			return to == getRedirectPMs();
		}

		QString getAutoWarnMessage() const {
			if(autowarn_pm_msg)
				return autowarn_pm_msg;
			else
				return QString(AUTOWARN_PM_MSG_DEFAULT);
		}
		QFont getFont() const {
			return chat_window->font();
		}
		int getMaxNumDuplicates() const {
			return max_num_duplicates;
		}
		int getMaxNumCapsPosts() const {
			return max_num_caps_posts;
		}
		int getMaxNumEntries() const {
			return max_num_entries;
		}
		bool getTranslateEscapeEnabled() const {	// translation of escape chars enabled?
			return translate_escape_chars;
		}
		bool isCloneDetectionEnabled() const {
			return clone_detection_enabled;
		}
		int getCloneDetectionThreshold() const {
			return clone_detection_threshold;
		}
		bool getVerbosity() const {
			return verbosity;
		}
		bool isCosyChatEnabled() const {
			return cosychat_enabled;
		}
		FriendLevel getCosyChatFriendLevel() const {
			return cosychat_friendlevel;
		}

	// Mutators
		void saveState(ostream &os) const;
		void restoreState(istream &is);

		void removeEntry(int index);
		void clear();	// clear the screen
		void setFont(QFont f) {
			chat_window->setFont(f);
		}

	// Signals
	signals:
		void sendText(QString text, bool display=true);	// activated when Send button pressed
		                                	// with non-null text in text entry line
		void ignoreStateChanged(Chatter *c);
		void updateChatters();	// change to Chatter state made
		void mouseClick(QListBoxItem *chatter, int index, QMouseEvent *e);
		void openPMWindow(const Chatter *c);

	// Public Slots
	public slots:
		void addEntry(QString text, Chatter *chatter, ChatEntryType t);	// add line from chatter
		void addEntry(QString text);	// add line from me
		void addEntry(const QListBoxItem &entry);	// add preformatted entry
		void removeAllEntries(Chatter *c);	// purge widget of all entries related to Chatter
		void setIgnoreCase(bool b) {
			ignore_case = b;
		}
		void setIgnoreMovement(bool b) {
			ignore_movement = b;
		}
		void setIgnorePMsEnabled(bool b) {
			ignore_pms = b;
		}
		void setIgnorePMsFriendLevel(FriendLevel f) {
			ignore_pms_friend_level = f;
		}
		void setIgnorePMsOutsideRoomEnabled(bool b) {
			ignore_pms_outside_room = b;
		}

		void setAutoIgnoreEnabled(bool on) {
			autoignore_enabled = on;
		}

		void setAutoIgnoreFriendLevel(FriendLevel fl) {
			autoignore_friend_level = fl;
		}
		void setAutoIgnoreFirstPostURLEnabled(bool on) {
			autoignore_first_post_URL_enabled = on;
		}
		void setAutoIgnoreFirstPostPMEnabled(bool on) {
			autoignore_first_post_PM_enabled = on;
		}
		void setAutoIgnoreSinglePostEnabled(bool on) {
			autoignore_single_post_enabled = on;
		}
		void setAutoIgnoreCapsEnabled(bool on) {
			autoignore_caps_enabled = on;
		}
		void setAutoIgnoreMultilineEnabled(bool on) {
			autoignore_multiline_enabled = on;
		}

		void setRedirectPMs(PMRedirectType to) {
			setRedirectPMs(to, true);
		}

		void setRedirectPMs(PMRedirectType to, bool enable) {
			switch(to) {
				case REDIRECT_PUBLIC:	// redirect to public message
					redirect_public = enable;
					if(enable) {
						redirect_public_censored = false;
						redirect_null = false;
					}
					break;
				case REDIRECT_PUBLIC_CENSORED:	// redirect to public message
					redirect_public_censored = enable;
					if(enable) {
						redirect_public = false;
						redirect_null = false;
					}
					break;
				case REDIRECT_NULL:	// redirect to public message
					redirect_null= enable;
					if(enable) {
						redirect_public_censored = false;
						redirect_public = false;
					}
					break;
				case REDIRECT_OFF:	// turn off all of 'em
				default:
					redirect_public = false;
					redirect_public_censored = false;
					redirect_null = false;
					break;
			}
		}
		void setAutoWarnOnPM(bool b) {
			autowarn_pm = b;
		}
		void setAutoWarnMessage(QString msg);
		void setMaxNumDuplicates(int n) {
			max_num_duplicates = n;
		}

		void setMaxNumCapsPosts(int n) {
			max_num_caps_posts = n;
		}

		void setMaxNumEntries(int n) {
			max_num_entries = n;
		}

		void enableTranslateEscape(bool y) {	// enable translation of escape chars?
			translate_escape_chars = y;
		}

		void writeToFile(QString path);	// write contents to file, no checking
		void cutText() {	// cut selected text from chat entry line
			chat_entry_line->cut();
		}

		void copyText() {	// copy selected text from chat entry line
			chat_entry_line->copy();
		}

		void pasteText() {	// paste text into chat entry line
			chat_entry_line->paste();
		}

		void clearText() {	// clear selected text in chat entry line
// *** DISABLED ***
//			if(chat_entry_line->hasMarkedText())
//				chat_entry_line->del();
		}
		void setCloneDetectionEnabled(bool enabled) {
			clone_detection_enabled = enabled;
		}
		void setCloneDetectionThreshold(int threshold) {
			clone_detection_threshold = threshold;
		}
		void setVerbosity(bool on) {
			verbosity = on;
		}
		void setCosyChatEnabled(bool enabled) {
			cosychat_enabled = enabled;
		}
		void setCosyChatFriendLevel(FriendLevel fl) {
			cosychat_friendlevel = fl;
		}

	protected:
		MyListBox *chat_window;	// holds room conversation
		QLineEdit *chat_entry_line;	// enter text here
		QPushButton *send_button;	// duh
		QLabel *enter_label;
		bool ignore_case;	// do we ignore case?
		bool ignore_movement;	// do we ignore movement?
		bool ignore_pms;	// do we ignore pms?
		FriendLevel ignore_pms_friend_level;	// ignore PMs below & including this FriendLevel
		bool ignore_pms_outside_room;	// do we ignore pms?
		bool redirect_public;	// redirect PMs to public view?
		bool redirect_public_censored;	// redirect PMs to public view?
		bool redirect_null;	// redirect PMs to public view?
		bool autoignore_enabled;	// autoignore enabled?
		bool autoignore_first_post_URL_enabled;	// autoignore first post URL?
		bool autoignore_first_post_PM_enabled;	// autoignore first post PM?
		bool autoignore_multiline_enabled;	// autoignore multiline posts?
		bool autoignore_single_post_enabled;	// autoignore single posts?
		bool autoignore_caps_enabled;	// autoignore uppercase?
		FriendLevel autoignore_friend_level;	// friend level <= ignored
		bool autowarn_pm;	// do we automatically warn PMers?
		bool translate_escape_chars;	// translate C escape chars?
		bool verbosity;	// display messages/warnings verbosely?
		bool cosychat_enabled;	// show only messages from people above friendlevel
		FriendLevel cosychat_friendlevel;	// see above
		QString autowarn_pm_msg;	// message to use
		int max_num_duplicates;	// number of spams before autoignore (0==disable)
		int max_num_caps_posts;	// number of uppercase posts before autoignore
		int max_num_entries;	// max. number of entries widget can hold
		bool clone_detection_enabled;	// duh
		int clone_detection_threshold;	// percent match for clones (0-100)

		virtual void resizeEvent(QResizeEvent *);	// redefine the resize func

	private slots:
		void processSendClick();	// handle clicking the Send button
		void handleMouseClick(QListBoxItem *, int, QMouseEvent *);
		void handleFriendLevelStatusClick(int index);

	private:
		QListBoxItem *toQListBoxItem(QString text);
		QPopupMenu *chatter_menu;
		QPopupMenu *friend_level_menu;
		const char *isIgnorable(Chatter *c, const QString &txt, ChatEntryType t);
		void detectClones(const Chatter *chatter);
};
#endif	//	_CHATMAIN_H
