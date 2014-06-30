#include "menubar.h"
#include "chatmain.h"
#include "defs.h"
#include <iostream.h>

QMenuData *MenuBar::menu_parent[NUM_MENU_ITEMS];

MenuBar::MenuBar(QWidget *parent, const char *name) :
	QMenuBar(parent, name) {

	if(::debuglevel & DEBUGLEVEL_CONSTRUCTOR)
		cerr << "MenuBar constructor called." << endl;
	QPopupMenu *file = new QPopupMenu();
	CHECK_PTR(file);
	file->insertItem("New &Window", (int)MENU_FILE_NEW_WINDOW);
	menu_parent[MENU_FILE_NEW_WINDOW] = file;
	file->insertSeparator();
	file->insertItem("&Connect...", (int)MENU_FILE_CONNECT);
	menu_parent[MENU_FILE_CONNECT] = file;
	file->insertItem("&Disconnect", (int)MENU_FILE_DISCONNECT);
	menu_parent[MENU_FILE_DISCONNECT] = file;
	file->insertSeparator();
	file->insertItem("&Save", (int)MENU_FILE_SAVE);
	menu_parent[MENU_FILE_SAVE] = file;
	file->insertItem("Save &As...", (int)MENU_FILE_SAVE_AS);
	menu_parent[MENU_FILE_SAVE_AS] = file;
	file->insertSeparator();
	file->insertItem("&Print...", (int)MENU_FILE_PRINT);
	menu_parent[MENU_FILE_PRINT] = file;
	file->insertItem("Printer Set&up...", (int)MENU_FILE_PRINTER_SETUP);
	menu_parent[MENU_FILE_PRINTER_SETUP] = file;
	file->insertSeparator();
	file->insertItem("Cl&ose", (int)MENU_FILE_CLOSE);
	menu_parent[MENU_FILE_CLOSE] = file;
	file->insertItem("E&xit", (int)MENU_FILE_EXIT);
	menu_parent[MENU_FILE_EXIT] = file;
	connect(file, SIGNAL(activated(int)), SLOT(activated_redirect(int)));
	
	QPopupMenu *edit = new QPopupMenu();
	CHECK_PTR(edit);
	edit->insertItem("Cu&t", (int)MENU_EDIT_CUT);
	menu_parent[MENU_EDIT_CUT] = edit;
	edit->insertItem("&Copy", (int)MENU_EDIT_COPY);
	menu_parent[MENU_EDIT_COPY] = edit;
	edit->insertItem("&Paste", (int)MENU_EDIT_PASTE);
	menu_parent[MENU_EDIT_PASTE] = edit;
	edit->insertSeparator();
	edit->insertItem("&Find", (int)MENU_EDIT_FIND);
	menu_parent[MENU_EDIT_FIND] = edit;
	edit->insertSeparator();
	edit->insertItem("C&lear Window", (int)MENU_EDIT_CLEAR_WINDOW);
	menu_parent[MENU_EDIT_CLEAR_WINDOW] = edit;
	connect(edit, SIGNAL(activated(int)), SLOT(activated_redirect(int)));

	QPopupMenu *view = new QPopupMenu();
	CHECK_PTR(view);
	view->insertItem("&Menu Bar", (int)MENU_VIEW_MENUBAR);
	menu_parent[MENU_VIEW_MENUBAR] = view;
	view->insertItem("S&tatus Bar", (int)MENU_VIEW_STATUSBAR);
	menu_parent[MENU_VIEW_STATUSBAR] = view;
	view->insertItem("&Chatters List", (int)MENU_VIEW_CHATTERSLIST);
	menu_parent[MENU_VIEW_CHATTERSLIST] = view;
	view->insertItem("&Ignore Bin", (int)MENU_VIEW_IGNOREBIN);
	menu_parent[MENU_VIEW_IGNOREBIN] = view;
	view->insertSeparator();
	view->insertItem("&Room List...", (int)MENU_VIEW_ROOMLIST);
	menu_parent[MENU_VIEW_ROOMLIST] = view;
	view->insertItem("&Statistics...", (int)MENU_VIEW_STATISTICS);
	menu_parent[MENU_VIEW_STATISTICS] = view;
//	view->insertItem("&Ignore List...", (int)MENU_VIEW_IGNORE_LIST);
//	menu_parent[MENU_VIEW_IGNORE_LIST] = view;
	view->insertItem("&Debug Window...", (int)MENU_VIEW_DEBUG_WINDOW);
	menu_parent[MENU_VIEW_DEBUG_WINDOW] = view;
	connect(view, SIGNAL(activated(int)), SLOT(activated_redirect(int)));
	
	QPopupMenu *options_display = new QPopupMenu();
	CHECK_PTR(options_display);
	options_display->insertItem("&Settings...",
		(int)MENU_OPTIONS_DISPLAY_SETTINGS);
	menu_parent[MENU_OPTIONS_DISPLAY_SETTINGS] = options_display;
	options_display->insertItem("&Font...",
		(int)MENU_OPTIONS_DISPLAY_FONT);
	menu_parent[MENU_OPTIONS_DISPLAY_FONT] = options_display;
	options_display->insertSeparator();
	connect(options_display, SIGNAL(activated(int)), this, SLOT(activated_redirect(int)));

	QPopupMenu *options_redirect = new QPopupMenu();
	CHECK_PTR(options_redirect);
	options_redirect->insertItem("To Public Room (Censored)",
		(int)MENU_OPTIONS_REDIRECT_PMS_PUBLIC_CENSORED);
	menu_parent[MENU_OPTIONS_REDIRECT_PMS_PUBLIC_CENSORED] =
		options_redirect;
	options_redirect->insertItem("To Public Room (Uncensored)",
		(int)MENU_OPTIONS_REDIRECT_PMS_PUBLIC);
	menu_parent[MENU_OPTIONS_REDIRECT_PMS_PUBLIC] =
		options_redirect;
	options_redirect->insertItem("To /dev/null",
		(int)MENU_OPTIONS_REDIRECT_PMS_NULL);
	menu_parent[MENU_OPTIONS_REDIRECT_PMS_NULL] =
		options_redirect;
	options_redirect->setCheckable(true);
	connect(options_redirect, SIGNAL(activated(int)), this, SLOT(activated_redirect(int)));

	QPopupMenu *options_ignore = new QPopupMenu();
	CHECK_PTR(options_ignore);
	options_ignore->insertItem("&Settings...",
		(int)MENU_OPTIONS_IGNORE_SETTINGS);
	menu_parent[MENU_OPTIONS_IGNORE_SETTINGS] = options_ignore;
	options_ignore->insertItem("&Case",
		(int)MENU_OPTIONS_IGNORE_CASE);
	menu_parent[MENU_OPTIONS_IGNORE_CASE] = options_ignore;
	options_ignore->insertItem("St&yles",
		(int)MENU_OPTIONS_IGNORE_STYLES);
	menu_parent[MENU_OPTIONS_IGNORE_STYLES] = options_ignore;
	options_ignore->setItemEnabled(	// disable
		(int)MENU_OPTIONS_IGNORE_STYLES,
		false);
	options_ignore->insertItem("&Movement",
		(int)MENU_OPTIONS_IGNORE_MOVEMENT);
	menu_parent[MENU_OPTIONS_IGNORE_MOVEMENT] = options_ignore;
	connect(options_ignore, SIGNAL(activated(int)), this, SLOT(activated_redirect(int)));

	QPopupMenu *options= new QPopupMenu();
	CHECK_PTR(options);
	options->insertItem("&General...", (int)MENU_OPTIONS_GENERAL);
	menu_parent[MENU_OPTIONS_GENERAL] = options;
	options->insertItem("&Display", options_display, (int)MENU_OPTIONS_DISPLAY);
	menu_parent[MENU_OPTIONS_DISPLAY] = options;
	options->insertItem("&Connection...", (int)MENU_OPTIONS_CONNECTION);
	menu_parent[MENU_OPTIONS_CONNECTION] = options;
	options->insertItem("&Ignore", options_ignore, (int)MENU_OPTIONS_IGNORE);
	menu_parent[MENU_OPTIONS_IGNORE] = options;
	options->insertItem("&Diagnostics...", (int)MENU_OPTIONS_DIAGNOSTICS);
	menu_parent[MENU_OPTIONS_DIAGNOSTICS] = options;
	options->insertSeparator();
	options->insertItem("&Redirect PMs", options_redirect,
		(int)MENU_OPTIONS_REDIRECT_PMS);
	menu_parent[MENU_OPTIONS_REDIRECT_PMS] = options;
	options->insertSeparator();
	// ugh... stick AUTOLOGIN here for now... belongs in GENERAL dialog
	options->insertItem("Auto&Login", (int)MENU_OPTIONS_AUTOLOGIN);
	menu_parent[MENU_OPTIONS_AUTOLOGIN] = options;
	options->insertItem("Translate &Escape sequences", (int)MENU_OPTIONS_ENABLE_ESCAPE);
	menu_parent[MENU_OPTIONS_ENABLE_ESCAPE] = options;
	options->insertSeparator();
	options->insertItem("Auto&Warn on PM", (int)MENU_OPTIONS_AUTOWARN_ON_PM);
	menu_parent[MENU_OPTIONS_AUTOWARN_ON_PM] = options;
	options->insertItem("AutoWarn &Message...", (int)MENU_OPTIONS_AUTOWARN_MESSAGE);
	menu_parent[MENU_OPTIONS_AUTOWARN_MESSAGE] = options;
	options->insertItem("&CosyChat", (int)MENU_OPTIONS_COSYCHAT);
	menu_parent[MENU_OPTIONS_COSYCHAT] = options;
	options->setCheckable(TRUE);
	connect(options, SIGNAL(activated(int)), this, SLOT(activated_redirect(int)));

	QPopupMenu *help = new QPopupMenu();
	CHECK_PTR(help);
	help->insertItem("&Contents", (int)MENU_HELP_CONTENTS);
	menu_parent[MENU_HELP_CONTENTS] = help;
	help->insertSeparator();
	help->insertItem("&About ", (int)MENU_HELP_ABOUT);
	menu_parent[MENU_HELP_ABOUT] = help;
	connect(help, SIGNAL(activated(int)), SLOT(activated_redirect(int)));

	insertItem("&File", file, MENU_FILE);
	menu_parent[MENU_FILE] = this;
	insertItem("&Edit", edit, MENU_EDIT);
	menu_parent[MENU_EDIT] = this;
	insertItem("&View", view, MENU_VIEW);
	menu_parent[MENU_VIEW] = this;
	insertItem("&Options", options, MENU_OPTIONS);
	menu_parent[MENU_OPTIONS] = this;
	insertSeparator();
	insertItem("&Help", help, MENU_HELP);
	menu_parent[MENU_HELP] = this;
};

MenuBar::~MenuBar() {
	if(::debuglevel & DEBUGLEVEL_DESTRUCTOR)
		cerr << "MenuBar destructor called." << endl;
}
	
void MenuBar::activated_redirect(int id) {
	emit activated((MenuItem)id);
}

QMenuData *MenuBar::findItem(MenuItem i) const {
	if((int)i >= 0 && (int)i < (int)MENU_END)
		return menu_parent[(int)i];
	else
		return 0;
}

void MenuBar::setItemChecked(MenuItem i, bool checked) {	// set checked status
	QMenuData *item = findItem(i);
	if(item)
		item->setItemChecked((int)i, checked);
}

bool MenuBar::toggleItemChecked(MenuItem i) {	// toggle item, return new state
	QMenuData *item = findItem(i);
	if(item) {
		bool checked = !item->isItemChecked((int)i);
		item->setItemChecked((int)i, checked);
		return checked;
	} else
		return false;
}

bool MenuBar::isItemChecked(MenuItem i) const {	// return checked status of item
	QMenuData *item = findItem(i);
	if(item)
		return item->isItemChecked((int)i);
	else
		return false;
}
