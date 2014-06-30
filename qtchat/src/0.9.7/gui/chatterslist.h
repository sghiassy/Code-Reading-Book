#ifndef _CHATTERSLIST_H
#define _CHATTERSLIST_H

#include <qcombobox.h>
#include <qlistbox.h>
#include <qlist.h>
#include <qpopupmenu.h>
#include <qsplitter.h>
#include "chatmain.h"
#include "chatter.h"
#include "debug.h"

class ChattersListBox : public QListBox {	// need the updateItem() method
	public:
		ChattersListBox(QWidget *parent=0, const char *name=0, WFlags f=0) :
			QListBox(parent, name, f) {}
		void updateItem(int index) {QListBox::updateItem(index);}
};

class ChattersListWidget : public QWidget {
	Q_OBJECT
	public:

		ChattersListWidget(QWidget *parent=0, QString name=0, WFlags f=0);
		ChattersListWidget(const ChattersListWidget &clw);
		~ChattersListWidget();

		Chatter *getChatterAt(int index) const;	// does no range checking
		bool isInRoom(const Chatter *c) const;	// checks to see if chatter is in room
		int count() const {
			return chatters_list_window->count();
		}
		bool isIgnoreBinEnabled() const {
			return ignore_list_window->isEnabled();
		}
		ChatterList getChatters() const;
		ChatterList getIgnoredChatters() const;

	public slots:
		void addChatter(Chatter *chatter);
		void removeChatter(const Chatter *chatter);
		void clear() {
			chatters_list_window->clear();
			ignore_list_window->clear();
		}
		void updateChatters();
		void setIgnoreBinEnabled(bool enabled) {
			ignore_list_window->setEnabled(enabled);
			if(enabled)
				ignore_list_window->show();
			else
				ignore_list_window->hide();
		}

	signals:
		void mouseClick(QListBoxItem *chatter, int index, QMouseEvent *e);
		void printChatMessage(QString msg, Chatter *c, ChatEntryType t);
		void numIgnoredChanged(unsigned int n);
		void numChattersChanged(unsigned int n);

	protected:
		void enabledChange(bool) {
			if(isEnabled())
				show();
			else
				hide();
		}
	private slots:
		void handleMouseClick(QListBoxItem *chatter, int index, QMouseEvent *e);
		void handleFriendLevelStatusClick(int index);

	private:
		MyListBox *chatters_list_window;
		MyListBox *ignore_list_window;
		QComboBox *chatters_list_header;
		virtual void resizeEvent(QResizeEvent *);
		QPopupMenu *chatter_menu;
		QPopupMenu *friend_level_menu;
		QSplitter *list_splitter;

		void init();
};

#endif // _CHATTERSLIST_H
