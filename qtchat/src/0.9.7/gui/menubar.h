#ifndef _MENUBAR_H
#define _MENUBAR_H

#include <qmenubar.h>
#define INCLUDE_MENUITEM_DEF
#include <qmenudata.h>
#include <qapplication.h>
#include <iostream.h>


enum MenuItem {
	MENU_FILE=0,
	MENU_FILE_NEW_WINDOW,
	MENU_FILE_CONNECT,
	MENU_FILE_DISCONNECT,
	MENU_FILE_SAVE,
	MENU_FILE_SAVE_AS,
	MENU_FILE_PRINT,
	MENU_FILE_PRINTER_SETUP,
	MENU_FILE_CLOSE,
	MENU_FILE_EXIT,

	MENU_EDIT,
	MENU_EDIT_CUT,
	MENU_EDIT_COPY,
	MENU_EDIT_PASTE,
	MENU_EDIT_FIND,
	MENU_EDIT_CLEAR_WINDOW,

	MENU_VIEW,
	MENU_VIEW_STATUSBAR,	// controls StatusBar presence
	MENU_VIEW_MENUBAR,	// controls MenuBar presence
	MENU_VIEW_CHATTERSLIST,	// controls entire ChattersListWidget presence
	MENU_VIEW_IGNOREBIN,	// controls ignore bin of ChattersListWidget
	MENU_VIEW_ROOMLIST,
	MENU_VIEW_STATISTICS,
	MENU_VIEW_IGNORE_LIST,
	MENU_VIEW_DEBUG_WINDOW,

	MENU_OPTIONS,
	MENU_OPTIONS_GENERAL,
	MENU_OPTIONS_IGNORE,
	MENU_OPTIONS_IGNORE_SETTINGS,
	MENU_OPTIONS_IGNORE_CASE,
	MENU_OPTIONS_IGNORE_STYLES,
	MENU_OPTIONS_IGNORE_MOVEMENT,
	MENU_OPTIONS_AUTOLOGIN,	// ugh... stick it here for now... belongs in GENERAL
	MENU_OPTIONS_ENABLE_ESCAPE,
	MENU_OPTIONS_DISPLAY,
	MENU_OPTIONS_DISPLAY_SETTINGS,
	MENU_OPTIONS_DISPLAY_FONT,
	MENU_OPTIONS_CONNECTION,
	MENU_OPTIONS_DIAGNOSTICS,
	MENU_OPTIONS_REDIRECT_PMS,
	MENU_OPTIONS_REDIRECT_PMS_PUBLIC,
	MENU_OPTIONS_REDIRECT_PMS_PUBLIC_CENSORED,
	MENU_OPTIONS_REDIRECT_PMS_NULL,
	MENU_OPTIONS_AUTOWARN_ON_PM,
	MENU_OPTIONS_AUTOWARN_MESSAGE,
	MENU_OPTIONS_COSYCHAT,

	MENU_HELP,
	MENU_HELP_CONTENTS,
	MENU_HELP_ABOUT,

	MENU_END
};

static const int NUM_MENU_ITEMS = (int)MENU_END;

class MenuBar : public QMenuBar {
	Q_OBJECT
	public:
		MenuBar(QWidget *parent=0, const char *name=0);
		~MenuBar();

		QMenuData *findItem(MenuItem i) const;

		void setItemChecked(MenuItem i, bool checked);	// set checked status
		bool toggleItemChecked(MenuItem i);	// toggle item, return new state
		bool isItemChecked(MenuItem i) const;	// return checked status of item

	signals:
		void activated(MenuItem id);

	protected:
		void enabledChange(bool oldEnabled) {
			if(isEnabled()) {
				show();
			} else {
				hide();
			}
		}

	private slots:
		void activated_redirect(int id);	// redirects activated(int) to use MenuItem
	private:
		static QMenuData *menu_parent[NUM_MENU_ITEMS];
};

#endif	// _MENUBAR_H
