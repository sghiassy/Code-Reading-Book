#include "debug.h"
#include "lineentry.h"
#include "mainwidget.h"
#include "nc.h"
#include "prefsdialog.h"
#include "util.h"
#include <iostream.h>
#include <qdialog.h>
#include <qfiledialog.h>
#include <qmessagebox.h>
#include <qstring.h>
#include <qtabbar.h>

#define ROOMLIST_SERVER "chat.yahoo.com"
#define ROOMLIST_PATH "/c/roomlist.html"
#define HTTP_PORT 80

MainWidget::MainWidget(QWidget *parent, const char *name, WFlags f) :
	QWidget(parent, name, f) {

	if(::debuglevel & DEBUGLEVEL_CONSTRUCTOR)
		cerr << "MainWidget constructor called." << endl;
	main_splitter = new QSplitter(QSplitter::Horizontal, this);
	menubar = new MenuBar(this);
	chat_window = new ChatWidget(main_splitter);
	chatters_list = new ChattersListWidget(main_splitter);
	status_bar = new MainStatusBar(this);

	// Dialogs
	debug_window = new DebugWidget();
	font_dialog = new FontDialog(QFont());
	login_dialog = new LoginDialog();
	prefs_dialog = new PrefsDialog(chat_window, this);

	NetConnection conn2(ROOMLIST_SERVER, HTTP_PORT);	// attempt to create connection
	ChatRoomTree *roomtree=0;
	if(!conn2) {
		cerr << "Unable to connect to " ROOMLIST_SERVER ":" << HTTP_PORT << '.' << endl;
	} else {
		// send request to web server
		conn2.write("GET " ROOMLIST_PATH "\n", 5+strlen(ROOMLIST_PATH));

		// construct tree using stream from web server
		roomtree = new ChatRoomTree(conn2);
	}
	roomlist_dialog = new RoomListDialog(roomtree);
	conn2.close();

	// Devices
	printer = new QPrinter();
	printer->setCreator(APP_NAME "-" APP_VER);

	// Network
	conn = 0;
	setYahooConnection(0);

	// init chat window
	chat_window->clear();
	chat_window->setAutoWarnOnPM(true);
	status_bar->setAutoWarn(true);

	// init menubar
	menubar->setItemChecked(MENU_OPTIONS_AUTOWARN_ON_PM, chat_window->getAutoWarnOnPM());
	menubar->setItemChecked(MENU_OPTIONS_AUTOLOGIN, true);
	menubar->setItemChecked(MENU_OPTIONS_ENABLE_ESCAPE, chat_window->getTranslateEscapeEnabled());
	menubar->setItemChecked(MENU_OPTIONS_COSYCHAT, chat_window->isCosyChatEnabled());
	menubar->setItemChecked(MENU_VIEW_MENUBAR, true);
	menubar->setItemChecked(MENU_VIEW_STATUSBAR, true);
	menubar->setItemChecked(MENU_VIEW_CHATTERSLIST, true);
	menubar->setItemChecked(MENU_VIEW_IGNOREBIN, true);

	// init debug window
	debug_window->setCaption("QtChat: Debug Packet Trace");

	// init splitter container
	main_splitter->setOpaqueResize(true);
	main_splitter->setResizeMode(chat_window, QSplitter::Stretch);
	main_splitter->setResizeMode(chatters_list, QSplitter::Stretch);
	QValueList<int> sizes;
	int a, b;	// relative sizes of widgets
	a = (int)((float)main_splitter->width()*0.7);
	b = main_splitter->width()-a;
	sizes.append(a);
	sizes.append(b);
	main_splitter->setSizes(sizes);

	// init preferences dialogs
	PrefsIgnoreDialog *ignore_prefs =
		dynamic_cast<PrefsIgnoreDialog *>(prefs_dialog->getDialogWidget(PrefsDialog::PREFS_IGNORE));
	if(ignore_prefs) {
		ignore_prefs->setMaxNumDuplicates(chat_window->getMaxNumDuplicates());
		ignore_prefs->setAutoIgnoreEnabled(chat_window->isAutoIgnoreEnabled());
		ignore_prefs->setAutoIgnoreFriendLevel(chat_window->getAutoIgnoreFriendLevel());

		ignore_prefs->setAutoIgnoreFirstPostPMEnabled(chat_window->isAutoIgnoreFirstPostPMEnabled());
		ignore_prefs->setAutoIgnoreFirstPostURLEnabled(chat_window->isAutoIgnoreFirstPostURLEnabled());
		ignore_prefs->setAutoIgnoreSinglePostEnabled(chat_window->isAutoIgnoreSinglePostEnabled());
		ignore_prefs->setAutoIgnoreCapsEnabled(chat_window->isAutoIgnoreCapsEnabled());
		ignore_prefs->setAutoIgnoreMultilineEnabled(chat_window->isAutoIgnoreMultilineEnabled());
	}

// Connections
	// connect the chat_window's sendText() signal to send it ourself
	connect(chat_window, SIGNAL(sendText(QString, bool)), SIGNAL(sendText(QString, bool)));

	// update chatter list when someone ignored
	connect(chat_window, SIGNAL(ignoreStateChanged(Chatter *)),
		chatters_list, SLOT(updateChatters()));

	// update chatter list when someone's friend level changed
	connect(chat_window, SIGNAL(updateChatters()),
		chatters_list, SLOT(updateChatters()));

	// update the status bar when someone added to/removed from list
	connect(chatters_list, SIGNAL(numChattersChanged(unsigned int)),
		status_bar, SLOT(setChattersCount(unsigned int)));
	connect(chatters_list, SIGNAL(numIgnoredChanged(unsigned int)),
		status_bar, SLOT(setIgnoredCount(unsigned int)));
	// connect the menubar's activated(int) signal to send it ourself
	connect(menubar, SIGNAL(activated(int)), SLOT(menuItemActivated(int)));

	// also handle the menubar events we care about
	connect(menubar, SIGNAL(activated(MenuItem)), SLOT(handleMenuEvent(MenuItem)));

	// reenable the menubar when middle mouse button clicked on chat window
	connect(chat_window, SIGNAL(mouseClick(QListBoxItem *, int, QMouseEvent *)),
		SLOT(handleMouseClick(QListBoxItem *, int, QMouseEvent *)));

	// get any messages the ChattersList wants to display
	connect(
		chatters_list,
		SIGNAL(printChatMessage(QString, Chatter *, ChatEntryType)),
		chat_window,
		SLOT(addEntry(QString, Chatter *, ChatEntryType))
	);

	// get mode change signals from statusbar clicks
	connect(
		status_bar, SIGNAL(redirectPMsSet(PMRedirectType)),
		SLOT(setRedirectPMs(PMRedirectType)));
	connect(
		status_bar, SIGNAL(warnPMClicked(bool)),
		SLOT(setAutoWarnOnPM(bool)));
	connect(
		status_bar, SIGNAL(capsClicked(bool)),
		SLOT(setIgnoreCase(bool)));
	connect(
		status_bar, SIGNAL(movementClicked(bool)),
		SLOT(setIgnoreMovement(bool)));

	// connect PrefsDialog signals
	if(ignore_prefs) {
		connect(
			ignore_prefs,
			SIGNAL(autoIgnoreCountChanged(int)),
			chat_window,
			SLOT(setMaxNumDuplicates(int)));
		connect(
			ignore_prefs,
			SIGNAL(autoIgnoreEnableClicked(bool)),
			chat_window,
			SLOT(setAutoIgnoreEnabled(bool)));
		connect(
			ignore_prefs,
			SIGNAL(autoIgnoreFriendLevelChanged(FriendLevel)),
			chat_window,
			SLOT(setAutoIgnoreFriendLevel(FriendLevel)));
		connect(
			ignore_prefs,
			SIGNAL(autoIgnoreFirstPostPMEnableClicked(bool)),
			chat_window,
			SLOT(setAutoIgnoreFirstPostPMEnabled(bool)));
		connect(
			ignore_prefs,
			SIGNAL(autoIgnoreFirstPostURLEnableClicked(bool)),
			chat_window,
			SLOT(setAutoIgnoreFirstPostURLEnabled(bool)));
		connect(
			ignore_prefs,
			SIGNAL(autoIgnoreSinglePostEnableClicked(bool)),
			chat_window,
			SLOT(setAutoIgnoreSinglePostEnabled(bool)));
		connect(
			ignore_prefs,
			SIGNAL(autoIgnoreCapsEnableClicked(bool)),
			chat_window,
			SLOT(setAutoIgnoreCapsEnabled(bool)));
		connect(
			ignore_prefs,
			SIGNAL(autoIgnoreMultilineEnableClicked(bool)),
			chat_window,
			SLOT(setAutoIgnoreMultilineEnabled(bool)));
	}
		
	PrefsGeneralDialog *general_prefs =
		dynamic_cast<PrefsGeneralDialog *>(prefs_dialog->getDialogWidget(PrefsDialog::PREFS_GENERAL));
	if(general_prefs) {
		connect(
			general_prefs,
			SIGNAL(cloneDetectionEnabledChanged(bool)),
			chat_window,
			SLOT(setCloneDetectionEnabled(bool)));
		connect(
			general_prefs,
			SIGNAL(cloneDetectionThresholdChanged(int)),
			chat_window,
			SLOT(setCloneDetectionThreshold(int)));
		connect(
			general_prefs,
			SIGNAL(ignorePMsEnabledChanged(bool)),
			chat_window,
			SLOT(setIgnorePMsEnabled(bool)));
		connect(
			general_prefs,
			SIGNAL(ignorePMsFriendLevelChanged(FriendLevel)),
			chat_window,
			SLOT(setIgnorePMsFriendLevel(FriendLevel)));
		connect(
			general_prefs,
			SIGNAL(ignorePMsOutsideRoomEnabledChanged(bool)),
			chat_window,
			SLOT(setIgnorePMsOutsideRoomEnabled(bool)));
		connect(
			general_prefs,
			SIGNAL(cosyChatFriendLevelChanged(FriendLevel)),
			chat_window,
			SLOT(setCosyChatFriendLevel(FriendLevel)));
	}

	// that's about it... let's go home for cornflakes
	QSize sz;
	sz = chat_window->minimumSize();
	sz += chatters_list->minimumSize();
	sz += menubar->minimumSize();
	setMinimumSize(sz);
	setCaption("QtChat");

	// start up the connection if there is no main widget yet
	handleMenuEvent(MENU_FILE_CONNECT);
}

MainWidget::~MainWidget() {
	if(::debuglevel & DEBUGLEVEL_DESTRUCTOR)
		cerr << "MainWidget destructor called." << endl;
	delete menubar;
	delete chat_window;
	delete chatters_list;
	delete debug_window;
	if(conn) delete conn;
}

// StateObject virtual methods
void MainWidget::saveState(ostream &os) const throw (ParamException) {
	try {
		os << "[Chat Window]" << endl;
		chat_window->saveState(os);
		os << endl;

		os << "[Menu Bar]" << endl;
		setParam(os, "ViewMenuBar", menubar->isEnabled());
		setParam(os, "ViewStatusBar", status_bar->isEnabled());
		setParam(os, "ViewChattersList", chatters_list->isEnabled());
		setParam(os, "ViewIgnoreList", chatters_list->isIgnoreBinEnabled());
		os << endl;

		os << "[Login Dialog]" << endl;
		login_dialog->saveState(os);
		os << endl;

	} catch(ParamException pe) {
		cerr << "MainWidget::saveState(): exception caught." << endl;
		throw pe;
	}
}

int findHeader(istream &is, const char *header) {
	ios::iostate old_state = is.rdstate();	// save state in case of error
	streampos old_pos = is.tellg();	// save position in case of error
	is.seekg(0, ios::beg);	// rewind stream
	char buf[1024];
	while(is) {
		is.getline(buf, 1024);
		if(is) {
			char *cp = strstr(buf, header);
			if(cp) {
				// adjust stream position
				return 0;
			}	// else continue (not found)
		} else {	// end of file?
			if(!is.bad()) {
				is.seekg(old_pos);	// restore position
				is.setstate(old_state);	// restore state
			}
			return -1;	// return fail;
		}
	}
	return -1;	// this should never be reached
}

void MainWidget::restoreState(istream &is) throw (ParamException) {
	bool tmpb;

	try {
		if(findHeader(is, "[Chat Window]") == 0) {
			chat_window->restoreState(is);
		} else {
			cerr << endl << "Unable to find chat window state." << endl;
		}

		if(findHeader(is, "[Menu Bar]") == 0) {
			getParam(is, "ViewMenuBar", tmpb);
				menubar->setEnabled(tmpb);
				menubar->setItemChecked(MENU_VIEW_MENUBAR, tmpb);
				if(!tmpb) {	// tell user how to get menubar back
					chat_window->addEntry("*** Click middle mouse button in chat window to reenable menubar ***", 0, CET_SYSTEM_MESSAGE);
					menubar->hide();
				} else
					menubar->show();
			getParam(is, "ViewStatusBar", tmpb);
			status_bar->setEnabled(tmpb);
			menubar->setItemChecked(MENU_VIEW_STATUSBAR, tmpb);
			if(tmpb)
				status_bar->show();
			else
				status_bar->hide();
			getParam(is, "ViewChattersList", tmpb);
				chatters_list->setEnabled(tmpb);
				menubar->setItemChecked(MENU_VIEW_CHATTERSLIST, tmpb);
			getParam(is, "ViewIgnoreList", tmpb);
				chatters_list->setIgnoreBinEnabled(tmpb);
				menubar->setItemChecked(MENU_VIEW_IGNOREBIN, tmpb);
		} else {
			cerr << endl << "Unable to find menubar state." << endl;
		}

		if(findHeader(is, "[Login Dialog]") == 0) {
			login_dialog->restoreState(is);
		} else {
			cerr << endl << "Unable to find login dialog state." << endl;
		}
	} catch(ParamException e) {
		cerr << "MainWidget::restoreState(): exception caught." << endl;
		throw e;
	}	// end try block

	status_bar->setIgnoreCase(chat_window->getIgnoreCase());
	status_bar->setIgnoreMovement(chat_window->getIgnoreMovement());
	status_bar->setAutoWarn(chat_window->getAutoWarnOnPM());
	status_bar->setRedirectPMs(chat_window->getRedirectPMs());

	// init preferences dialogs
	PrefsGeneralDialog *general_prefs =
		dynamic_cast<PrefsGeneralDialog *>(prefs_dialog->getDialogWidget(PrefsDialog::PREFS_GENERAL));
	if(general_prefs) {
		general_prefs->setCloneDetectionEnabled(chat_window->isCloneDetectionEnabled());
		general_prefs->setCloneDetectionThreshold(chat_window->getCloneDetectionThreshold());
		general_prefs->setIgnorePMsEnabled(chat_window->isIgnorePMsEnabled());
		general_prefs->setIgnorePMsFriendLevel(chat_window->getIgnorePMsFriendLevel());
		general_prefs->setIgnorePMsOutsideRoomEnabled(chat_window->isIgnorePMsOutsideRoomEnabled());
	}
	PrefsIgnoreDialog *ignore_prefs =
		dynamic_cast<PrefsIgnoreDialog *>(prefs_dialog->getDialogWidget(PrefsDialog::PREFS_IGNORE));
	if(ignore_prefs) {
		ignore_prefs->setMaxNumDuplicates(chat_window->getMaxNumDuplicates());
		ignore_prefs->setAutoIgnoreEnabled(chat_window->isAutoIgnoreEnabled());
		ignore_prefs->setAutoIgnoreFriendLevel(chat_window->getAutoIgnoreFriendLevel());

		ignore_prefs->setAutoIgnoreFirstPostPMEnabled(chat_window->isAutoIgnoreFirstPostPMEnabled());
		ignore_prefs->setAutoIgnoreFirstPostURLEnabled(chat_window->isAutoIgnoreFirstPostURLEnabled());
		ignore_prefs->setAutoIgnoreSinglePostEnabled(chat_window->isAutoIgnoreSinglePostEnabled());
		ignore_prefs->setAutoIgnoreCapsEnabled(chat_window->isAutoIgnoreCapsEnabled());
		ignore_prefs->setAutoIgnoreMultilineEnabled(chat_window->isAutoIgnoreMultilineEnabled());
	}

		resizeEvent(0);	// resize widget
		update();	// redraw widget
}


void MainWidget::resizeEvent(QResizeEvent *) {
	bool menubar_enabled = menubar->isEnabled();
	bool statusbar_enabled = status_bar->isEnabled();
	if(menubar_enabled) {
		menubar->move(0, 0);
		menubar->resize(width(), menubar->height());
		main_splitter->move(0, menubar->height());
	} else {
		main_splitter->move(0, 0);
	}
	if(statusbar_enabled) {
		status_bar->move(0, height()-status_bar->height());
		status_bar->resize(width(), status_bar->height());
	}
	main_splitter->resize(
		width(),
		height()
			- (menubar_enabled ? menubar->height() : 0)
			- (statusbar_enabled ? status_bar->height() : 0));
}

void MainWidget::setYahooConnection(YahooNetConnection *c) {
	if(conn) {
			// here we don't delete the connection.  Perhaps in the future
			// we'll add a reference count to the NetConnection object and
			// delete the connection when it reaches 0
			//online_indicator->setState(false);
// This is crashing now for some bizare reason
//			disconnect(conn, SIGNAL(dataTransferred(const unsigned char *, int, bool)),
//				debug_window, SLOT(dataTransferred(const unsigned char *, int, bool)));
//			delete conn;
			conn = 0;
	}
	if(!c) c = new YahooNetConnection(	// instantiate a null connection
		this,0,0,0,0,0);
	if(c && c->isValid()) {
	   // connect the sendText signal to our connection object
	   connect(this, SIGNAL(sendText(QString, bool)),
		   c, SLOT(parseInput(QString, bool)));
	   connect(c, SIGNAL(printChatMessage(QString)),
		   chat_window, SLOT(addEntry(QString)));
	   connect(c, SIGNAL(printChatMessage(QString, Chatter *, ChatEntryType)),
		   chat_window, SLOT(addEntry(QString, Chatter *, ChatEntryType)));
	   connect(c, SIGNAL(chatterEnters(Chatter *)),
		   chatters_list, SLOT(addChatter(Chatter *)));
	   connect(c, SIGNAL(chatterLeaves(const Chatter *)),
		   chatters_list, SLOT(removeChatter(const Chatter *)));
		// also need to handle possibility of 'me' leaving
	   connect(c, SIGNAL(chatterLeaves(const Chatter *)),
		   SLOT(handleMeLeaving(const Chatter *)));
		connect(c, SIGNAL(appSignal(AppSignalType)),
			SLOT(handleAppSignal(AppSignalType)));
		connect(c, SIGNAL(dataTransferred(const unsigned char *, int, bool)),
			debug_window, SLOT(dataTransferred(const unsigned char *, int, bool)));
		connect(c, SIGNAL(roomNameChanged(QString)),
			SLOT(handleRoomNameChanged(QString)));
		connect(roomlist_dialog, SIGNAL(roomSelected(const char *)),
			c, SLOT(join(const char *)));
		connect(c, SIGNAL(activateMenuItem(MenuItem)),
			SLOT(handleMenuEvent(MenuItem)));
		connect(c, SIGNAL(saveText(const char *)),
			SLOT(handleSaveToFile(const char *)));
	} else {	// connection marked down
//		chatters_list->clear();
	}
	conn = c;
	if(conn)
		status_bar->setOnlineState(conn->isValid());
}

void MainWidget::handleMenuEvent(MenuItem menu_item) {
	switch(menu_item) {
		case MENU_FILE_NEW_WINDOW:
		{
			MainWidget *mw = new MainWidget(0, 0, WDestructiveClose);
			mw->resize(600, 400);
			mw->show();
			break;
		}

		case MENU_FILE_CONNECT:	  	// "Connect..." selected
		{
			login_dialog->clearPassword();
			char *username = getenv("QTCHAT_USER");
			char *password = getenv("QTCHAT_PASS");
			char *server   = getenv("QTCHAT_SRVR");
			char *port     = getenv("QTCHAT_PORT");
			char *room     = getenv("QTCHAT_ROOM");
			if(username)
				login_dialog->setUsername(username);
			if(password)
				login_dialog->setPassword(password);
			if(server)
				login_dialog->setChatServer(server);
			if(port)
				login_dialog->setChatPort(atoi(port));
			if(room)
				login_dialog->setGotoRoom(room);

			// pop up the filled-in dialog if this is not the 1st invocation...
			// or username/password aren't already set or user isn't already connected
			if(!(username && password) || Chatter(username).isConnected())
				if(login_dialog->exec()!=QDialog::Accepted) break;
			// Here the dialog box, whether shown or not, has our values
			// and we want a connection
			if(conn) delete conn;
			bool b=login_dialog->gotoRoomClicked();
			YahooNetConnection *c = new YahooNetConnection(
				this,
				login_dialog->getUsername(),
				login_dialog->getPassword(),
				login_dialog->getChatServer(),
				login_dialog->getChatPort(),
				b ? login_dialog->getGotoRoom() : login_dialog->getGotoUser(),
				b,
				chatters_list
			);
			setYahooConnection(c);
			login_dialog->clearPassword();
			// clear chatter list widget
//			chatters_list->clear();

			// set the 'me' person's name
			if(c) {
				Chatter *me = new Chatter(c->getUser());	// create 'me'
				me->setMe(true);
				chatters_list->addChatter(me);	// stick 'me' in listbox
			}
			login_dialog->clearPassword();

			break;
		}

		case MENU_FILE_DISCONNECT:
			if(conn)
				delete conn;
			conn = 0;
			setYahooConnection(0);
			break;

		case MENU_FILE_SAVE:
			chat_window->writeToFile(FILE_SAVE_FILENAME_DEFAULT);
			break;

		case MENU_FILE_SAVE_AS:
		{
			QString filename = QFileDialog::getSaveFileName();	// get filename
			if(!filename.isEmpty()) {
				chat_window->writeToFile((const char *)filename);
			}
			break;
		}

		case MENU_FILE_PRINT:
			printer->setup(this);
			break;

		case MENU_FILE_PRINTER_SETUP:
			printer->setup(this);
			break;

		case MENU_FILE_CLOSE:
			close();	// destroy this widget
			break;

		case MENU_FILE_EXIT:
			qApp->closeAllWindows();
			break;

		case MENU_EDIT_CUT:
		{
			chat_window->cutText();
			break;
		}

		case MENU_EDIT_COPY:
		{
			chat_window->copyText();
			break;
		}

		case MENU_EDIT_PASTE:
		{
			chat_window->pasteText();
			break;
		}

		case MENU_EDIT_FIND:
		{
			break;
		}

		case MENU_EDIT_CLEAR_WINDOW:
			chat_window->clear();
			break;

		case MENU_VIEW_MENUBAR:
		{
			menubar->setEnabled(menubar->toggleItemChecked(menu_item));
			if(!menubar->isItemChecked(menu_item))	// tell user how to get it back
				chat_window->addEntry("*** Click middle mouse button in chat window to reenable menubar ***", 0, CET_SYSTEM_MESSAGE);
			resizeEvent(0);
			break;
		}

		case MENU_VIEW_STATUSBAR:
		{
			status_bar->setEnabled(menubar->toggleItemChecked(menu_item));
			if(status_bar->isEnabled())
				status_bar->show();
			else
				status_bar->hide();
//			online_indicator->setEnabled(menubar->isItemChecked(menu_item));
			resizeEvent(0);
			repaint(true);
			break;
		}

		case MENU_VIEW_CHATTERSLIST:
		{
			bool b = menubar->toggleItemChecked(menu_item);
			chatters_list->setEnabled(b);
			chat_window->resize(b ? (size() - chatters_list->size()) : size());
			resizeEvent(0);
			repaint(true);
			break;
		}

		case MENU_VIEW_IGNOREBIN:
		{
			bool new_state = menubar->toggleItemChecked(menu_item);
			chatters_list->setIgnoreBinEnabled(new_state);
			if(new_state) {
				chatters_list->setEnabled(true);
				menubar->setItemChecked(MENU_VIEW_CHATTERSLIST, true);
			}
			chat_window->repaint(true);
			resizeEvent(0);
			repaint(true);
			break;
		}

		case MENU_VIEW_ROOMLIST:
			roomlist_dialog->show();
			break;

		case MENU_VIEW_DEBUG_WINDOW:
			debug_window->show();
			debug_window->raise();
			break;

		case MENU_OPTIONS_GENERAL:
			prefs_dialog->setCurrentTab(0);
			prefs_dialog->show();
			prefs_dialog->raise();
			break;

		case MENU_OPTIONS_CONNECTION:
			prefs_dialog->setCurrentTab(2);
			prefs_dialog->show();
			prefs_dialog->raise();
			break;

		case MENU_OPTIONS_DIAGNOSTICS:
			prefs_dialog->setCurrentTab(4);
			prefs_dialog->show();
			prefs_dialog->raise();
			break;

		case MENU_OPTIONS_IGNORE_SETTINGS:
			prefs_dialog->setCurrentTab(3);
			prefs_dialog->show();
			prefs_dialog->raise();
			break;

		case MENU_OPTIONS_AUTOLOGIN:
		{
			// code to automatically relogin to Yahoo when disconnected
			bool autologin = menubar->toggleItemChecked(menu_item);
			conn->setKeepAlive(autologin);	// keep connection alive unless explicit disconnect
			if(autologin)
				chat_window->addEntry("*** Connection KeepAlive enabled. ***", 0, CET_SYSTEM_MESSAGE);
			else
				chat_window->addEntry("*** Connection KeepAlive disabled. ***", 0, CET_SYSTEM_MESSAGE);
			break;
		}
		case MENU_OPTIONS_ENABLE_ESCAPE:
		{
			bool enable_escape = menubar->toggleItemChecked(menu_item);
			chat_window->enableTranslateEscape(enable_escape);
			if(enable_escape)
				chat_window->addEntry("*** Escape characters entered will be translated. ***", 0, CET_SYSTEM_MESSAGE);
			else
				chat_window->addEntry("*** Characters entered will be unmodified. ***", 0, CET_SYSTEM_MESSAGE);
			break;
		}

		case MENU_OPTIONS_COSYCHAT:
		{
			bool enable_cosychat = menubar->toggleItemChecked(menu_item);
			chat_window->setCosyChatEnabled(enable_cosychat);
			if(enable_cosychat) {
				char str[256] = "*** CosyChat enabled: posts from ";
				strncat(str, toString(chat_window->getCosyChatFriendLevel()), 256);
				strncat(str, " and below not shown. ", 256);
				chat_window->addEntry(str, 0, CET_SYSTEM_MESSAGE);
			} else
				chat_window->addEntry("*** CosyChat disabled ***", 0, CET_SYSTEM_MESSAGE);
			break;
		}


		case MENU_OPTIONS_DISPLAY_SETTINGS:
			prefs_dialog->setCurrentTab(1);
			prefs_dialog->show();
			prefs_dialog->raise();
			break;

		case MENU_OPTIONS_DISPLAY_FONT:
			if(font_dialog->exec() == QDialog::Accepted) {
// uncomment if using FontDialog, comment otherwise
				chat_window->setFont(font_dialog->getFont());
				chat_window->repaint(true);
// end uncomment
			}
			break;

		case MENU_OPTIONS_IGNORE_CASE:
		{
			setIgnoreCase(!menubar->isItemChecked(menu_item));
			break;
		}

		case MENU_OPTIONS_IGNORE_STYLES:
		{
			setIgnoreStyles(!menubar->isItemChecked(menu_item));
			break;
		}
		case MENU_OPTIONS_IGNORE_MOVEMENT:
			setIgnoreMovement(!menubar->isItemChecked(menu_item));
			break;

		case MENU_OPTIONS_REDIRECT_PMS_PUBLIC_CENSORED:
		case MENU_OPTIONS_REDIRECT_PMS_PUBLIC:
		case MENU_OPTIONS_REDIRECT_PMS_NULL:
			toggleRedirectPMs((PMRedirectType)(menu_item-MENU_OPTIONS_REDIRECT_PMS_PUBLIC+1));
			break;

		case MENU_OPTIONS_AUTOWARN_ON_PM:
			setAutoWarnOnPM(!menubar->isItemChecked(menu_item));
			break;

		case MENU_OPTIONS_AUTOWARN_MESSAGE:
		{
			LineEntryDialog autowarn_message_widget("AutoWarn Message: ", this, 0, true);	// create toplevel widget
			autowarn_message_widget.setText(chat_window->getAutoWarnMessage());
			autowarn_message_widget.selectAll();
			autowarn_message_widget.exec();
			if(autowarn_message_widget.result() == QDialog::Accepted) {
				setAutoWarnOnPM(!(strlen(autowarn_message_widget.text()) == 0));
			}
			break;
		}

		case MENU_HELP_CONTENTS:
			conn->parseInput(QString("/help"));
			break;
			
		case MENU_HELP_ABOUT:
			QMessageBox::about(this, "About QtChat",
				"QtChat Version " APP_VER "\n\n"
				"Authors: Scoobi_FreeBSD      gui development\n"
				"                             C++ code structure\n"
				"         127001              networking code\n"
				"                             protocol issues");
			break;

		default:
			break;
	}

}

void MainWidget::setIgnoreCase(bool state) {
	chat_window->setIgnoreCase(state);
	status_bar->setIgnoreCase(state);
	char buf[64] = "*** Capitalization ";
	strncat(buf, state ? "ignored ***" : "enabled ***", 64);
	chat_window->addEntry(buf, 0, CET_SYSTEM_MESSAGE);
	menubar->setItemChecked(MENU_OPTIONS_IGNORE_CASE, state);
}

void MainWidget::setIgnoreStyles(bool state) {
	char buf[64] = "*** Rich text styles ";
	strncat(buf, state ? "disabled ***" : "enabled ***", 64);
	chat_window->addEntry(buf, 0, CET_SYSTEM_MESSAGE);
	menubar->setItemChecked(MENU_OPTIONS_IGNORE_STYLES, state);
}

void MainWidget::setIgnoreMovement(bool state) {
	chat_window->setIgnoreMovement(state);
	status_bar->setIgnoreMovement(state);
	char buf[64] = "*** Movement ";
	strncat(buf, state ? "ignored ***" : "enabled ***", 64);
	chat_window->addEntry(buf, 0, CET_SYSTEM_MESSAGE);
	menubar->setItemChecked(MENU_OPTIONS_IGNORE_MOVEMENT, state);
}

void MainWidget::toggleRedirectPMs(PMRedirectType t) {
	if(t == REDIRECT_OFF) return;
	setRedirectPMs(
		t, 
		!menubar->isItemChecked(	// set to opposite of current state
			(MenuItem)(
				(int)MENU_OPTIONS_REDIRECT_PMS_PUBLIC
				+(int)t
				-1
			)	// end arg to (MenuItem) cast operator
		)	// end arg to isItemChecked() 
	);	// end args to setRedirectPMs()
}

void MainWidget::setRedirectPMs(PMRedirectType t) {
	setRedirectPMs(t, true);
}

void MainWidget::setRedirectPMs(PMRedirectType t, bool state) {
	// first save old status of redirect, maintained by ChatWidget
	PMRedirectType prev_redirect = chat_window->getRedirectPMs();

	// reset all menu items
	menubar->setItemChecked(MENU_OPTIONS_REDIRECT_PMS_PUBLIC, false);
	menubar->setItemChecked(MENU_OPTIONS_REDIRECT_PMS_PUBLIC_CENSORED, false);
	menubar->setItemChecked(MENU_OPTIONS_REDIRECT_PMS_NULL, false);

	// update menubar
	if(t != REDIRECT_OFF) {
		menubar->setItemChecked(
			(MenuItem)((int)MENU_OPTIONS_REDIRECT_PMS_PUBLIC +(int)t - 1),	// adjust for REDIRECT_OFF
			state);
	}
	QString txt("*** PM Redirect: ");
	if(state) {
		switch(t) {
			case REDIRECT_OFF:
				txt += "off ***";
				break;
			case REDIRECT_PUBLIC:
				txt += "public ***";
				break;
			case REDIRECT_PUBLIC_CENSORED:
				txt += "public (censored) ***";
				break;
			case REDIRECT_NULL:
				txt += "/dev/null (PMs ignored) ***";
				break;
		}
	} else
		txt += "off ***";

	// notify user of change
	chat_window->addEntry(txt, 0, CET_SYSTEM_MESSAGE);

	chat_window->setRedirectPMs(t, state);

	// tell room if Redirect PMs Public state changed
	PMRedirectType current_redirect = chat_window->getRedirectPMs();
	if(prev_redirect != current_redirect) {
		if(current_redirect == REDIRECT_PUBLIC)
			emit sendText("*** Warning: All PMs redirected to public messages ***");
		else if(prev_redirect == REDIRECT_PUBLIC)
			emit sendText("*** PMs not made public ***");
	}
	// update status bar
//	status_bar->setRedirectPMs((current_redirect!=prev_redirect ? current_redirect :
//		(current_redirect==REDIRECT_OFF ? current_redirect : REDIRECT_OFF)));
	status_bar->setRedirectPMs(current_redirect);
}

void MainWidget::setAutoWarnOnPM(bool state) {
	menubar->setItemChecked(MENU_OPTIONS_AUTOWARN_ON_PM, state);
	chat_window->setAutoWarnOnPM(state);
	char buf[64] = "*** AutoWarn on PM ";
	strncat(buf, state ? "enabled ***" : "disabled ***", 64);
	chat_window->addEntry(buf, 0, CET_SYSTEM_MESSAGE);
	status_bar->setAutoWarn(state);
}

void MainWidget::handleMouseClick(QListBoxItem *, int, QMouseEvent *e) {
	if(e->button() == Qt::MidButton) {	// reenable the menubar
		menubar->setEnabled(true);
		menubar->setItemChecked(MENU_VIEW_MENUBAR, true);
		resizeEvent(0);
	}
}

void MainWidget::handleAppSignal(AppSignalType sig) {
	switch(sig) {
		case APP_LOGIN:
//			online_indicator->setState(true);
			break;
		case APP_LOGOUT:	// request to logout received
			handleMenuEvent(MENU_FILE_DISCONNECT);
//			online_indicator->setState(false);
			break;
		case APP_QUIT:		// request to terminate received
			handleMenuEvent(MENU_FILE_EXIT);
			break;
		case APP_LOGIN_FAILED_ROOM_FULL:	// try again
			handleMenuEvent(MENU_FILE_CONNECT);
			break;
		default:
			break;
	}
}

void MainWidget::handleMeLeaving(const Chatter *c) {
	if(c && conn && c->isMe() && *c == Chatter(conn->getUser()))
		chatters_list->clear();
}

void MainWidget::handleRoomNameChanged(QString name) {
	QString s("QtChat: ");
	s += name;
	setCaption(s);
}

void MainWidget::handleSaveToFile(const char *name) {
	chat_window->writeToFile(name);
}
void MainWidget::setFont(QFont f) {
	chat_window->setFont(f);
}
