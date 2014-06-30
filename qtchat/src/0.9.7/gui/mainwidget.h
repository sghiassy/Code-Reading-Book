#ifndef _MAINWIDGET_H
#define _MAINWIDGET_H

#define INCLUDE_MENUITEM_DEF
#include <iostream.h>
#include <qwidget.h>
#include <qprinter.h>
#include <qsplitter.h>
#include "chatmain.h"
#include "chatterslist.h"
#include "debugwidget.h"
#include "fontdialog.h"
#include "login.h"
#include "menubar.h"
#include "misc.h"
#include "net.h"
#include "prefsdialog.h"
#include "roomlistdialog.h"
#include "simpleindicator.h"
#include "state.h"
#include "statusbar.h"

#define FILE_SAVE_FILENAME_DEFAULT "qtchat.out"

class LoginDialog;

class MainWidget : public QWidget, public StateObject {
	Q_OBJECT
	friend class YahooNetConnection;
	public:
		MainWidget(QWidget *parent=0, const char *name=0, WFlags f=0);
		~MainWidget();

	// StateObjecte virtual methods
		virtual void saveState(ostream &os) const throw (ParamException);
		virtual void restoreState(istream &os) throw (ParamException);

// export most of the ChatWidget's methods/signals/slots
		void removeChatEntry(int index) {
			chat_window->removeEntry(index);
		}
		void clear() {
			chat_window->clear();
		}
		bool isInRoom(const Chatter *c) const {
			return chatters_list->isInRoom(c);
		}
		ChatWidget *getChatWidget() const {
			return chat_window;
		}

		DebugWidget *getDebugWidget() const {
			return debug_window;
		}

	signals:
		void sendText(QString text, bool verbose=true);
		void menuItemActivated(MenuItem item);

	public slots:
		void addChatEntry(const char *text, Chatter *chatter, ChatEntryType t) {
			chat_window->addEntry(text, chatter, t);
		}
		void addChatEntry(const char *text) {
			chat_window->addEntry(text);
		}
		void addChatEntry(const QListBoxItem &entry) {
			chat_window->addEntry(entry);
		}

		void menuItemActivated(int item) {
			emit menuItemActivated((MenuItem)item);
		}

// end export of ChatWidget's methods/signals/slots

		void toggleRedirectPMs(PMRedirectType t);
		void setRedirectPMs(PMRedirectType t);
		void setRedirectPMs(PMRedirectType t, bool state);
		void setAutoWarnOnPM(bool state);
		void setIgnoreCase(bool state);
		void setIgnoreStyles(bool state);
		void setIgnoreMovement(bool state);

		void setYahooConnection(YahooNetConnection *c);

	private slots:
		void handleMenuEvent(MenuItem menu_item);
		void handleMouseClick(QListBoxItem *, int, QMouseEvent *);
		void handleAppSignal(AppSignalType sig);
		void handleMeLeaving(const Chatter *);
		void handleRoomNameChanged(QString name);
		void handleSaveToFile(const char *name);
		void setFont(QFont f);

	protected:
		// GUI components
		QSplitter *main_splitter;
		MenuBar *menubar;
		ChatWidget *chat_window;
		ChattersListWidget *chatters_list;
		MainStatusBar *status_bar;
		SimpleIndicatorWidget *online_indicator;	// for status bar
		SimpleIndicatorWidget *temp_indicator;	// for status bar

		// Dialogs
		DebugWidget *debug_window;
		FontDialog *font_dialog;
		LoginDialog *login_dialog;
		PrefsDialog *prefs_dialog;
		RoomListDialog *roomlist_dialog;

		// Devices
		QPrinter *printer;

		// Network connection
		YahooNetConnection *conn;

		// Data members
		bool enable_menubar;
		bool enable_statusbar;
		bool enable_chatterslist;
		bool enable_ignorebin;

		// private methods
		virtual void resizeEvent(QResizeEvent *);	// override resize
};

#endif	// _MAINWIDGET_H
