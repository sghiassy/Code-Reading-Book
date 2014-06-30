#include "chatterslist.h"
#include <qcolordialog.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <iostream.h>

	ChattersListWidget::ChattersListWidget(QWidget *parent, QString name, WFlags f)
		: QWidget(parent, name, f) {

		if(::debuglevel & DEBUGLEVEL_CONSTRUCTOR)
			cerr << "ChattersListWidget constructor called." << endl;
		init();
	}

	ChattersListWidget::ChattersListWidget(const ChattersListWidget &clw) {
		if(::debuglevel & DEBUGLEVEL_COPYCONSTRUCTOR)
			cerr << "ChattersListWidget copy constructor called." << endl;
		init();
		for(int i=0; i<clw.count(); i++) {
			addChatter(getChatterAt(i));
		}
	}

	void ChattersListWidget::init() {
		list_splitter = new QSplitter(this);
		list_splitter->setOrientation(QSplitter::Vertical);
		list_splitter->setOpaqueResize(true);
		chatters_list_window = new MyListBox(list_splitter);
		chatters_list_window->setAutoUpdate(true);
		QFont f("Courier", 12, QFont::Normal);
		chatters_list_window->setFont(f);
		ignore_list_window = new MyListBox(list_splitter);
		ignore_list_window->setAutoUpdate(true);
		ignore_list_window->setFont(f);
		chatters_list_header = new QComboBox(this);
		chatters_list_header->insertItem("All");
		chatters_list_header->insertItem("Active");
		chatters_list_header->insertItem("Ignored");
		list_splitter->setResizeMode(chatters_list_window, QSplitter::Stretch);
		list_splitter->setResizeMode(ignore_list_window, QSplitter::Stretch);
//		QValueList<int> sizes;
//		int a, b;	// relative sizes of widgets
//		a = (int)((float)list_splitter->height()*0.7);
//		b = list_splitter->height()-a;
//		sizes.append(a);
//		sizes.append(b);
//		list_splitter->setSizes(sizes);

		chatter_menu = new QPopupMenu();
		chatter_menu->insertItem("", (int)CHATTER_MENU_NAME);
		chatter_menu->insertSeparator();
		chatter_menu->insertItem("&Ignored", (int)CHATTER_MENU_IGNORED);
		chatter_menu->insertItem("&Highlighted", (int)CHATTER_MENU_HIGHLIGHTED);
		chatter_menu->insertItem("Highlight &Color...", (int)CHATTER_MENU_HIGHLIGHT_COLOR);
		friend_level_menu = new QPopupMenu(chatter_menu);
		for(FriendLevel fl = STRANGER; fl < FRIENDLEVEL_END; ++fl) {
			friend_level_menu->insertItem(toString(fl), (int)fl);
		}
		connect(friend_level_menu, SIGNAL(activated(int)), this, SLOT(handleFriendLevelStatusClick(int)));
		friend_level_menu->setCheckable(true);

		chatter_menu->insertItem("&Friend Level", friend_level_menu, (int)CHATTER_MENU_FRIEND_STATUS);
		chatter_menu->insertSeparator();
		chatter_menu->insertItem("&Properties...", (int)CHATTER_MENU_PROPERTIES);
		connect(chatters_list_window,
			SIGNAL(mouseClick(QListBoxItem *, int, QMouseEvent *)),
			SLOT(handleMouseClick(QListBoxItem *, int, QMouseEvent *)));
		connect(ignore_list_window,
			SIGNAL(mouseClick(QListBoxItem *, int, QMouseEvent *)),
			SLOT(handleMouseClick(QListBoxItem *, int, QMouseEvent *)));
		chatters_list_window->resize(chatters_list_header->width(), 80);

	}

	ChattersListWidget::~ChattersListWidget() {
		if(::debuglevel & DEBUGLEVEL_DESTRUCTOR)
			cerr << "ChattersListWidget destructor called." << endl;
		if (chatters_list_window) delete chatters_list_window;
		if (chatters_list_header) delete chatters_list_header;
		delete chatter_menu;
	}

Chatter *ChattersListWidget::getChatterAt(int index) const {
	return (Chatter *)((MyListBox*)chatters_list_window)->getItemAt(index);
}

bool ChattersListWidget::isInRoom(const Chatter *c) const {	// checks to see if chatter is in room
	int n = chatters_list_window->count();
	for(int i=0; i<n; i++) {	// loop through looking for item
		Chatter *cp = dynamic_cast<Chatter *>(chatters_list_window->item(i));
		if(cp && (*cp == *c)) {
			// found chatter... return
			return true;;
		} else if(strcasecmp(cp->getName(), c->getName()) > 0) break;
	}
	n = ignore_list_window->count();
	for(int i=0; i<n; i++) {	// loop through looking for item
		Chatter *cp = dynamic_cast<Chatter *>(ignore_list_window->item(i));
		if(cp && (*cp == *c)) {
			// found chatter... return
			return true;;
		} else if(strcasecmp(cp->getName(), c->getName()) > 0) break;
	}
	return false;
}

	void ChattersListWidget::addChatter(Chatter *chatter) {
		if(!chatter) {// ignore bad arg
			cerr << "ChattersListWidget::addChatter(): bad arg." << endl;
			return;	
		}
		// look through list widgets for duplicate
		int n = chatters_list_window->count();
		for(int i=0; i<n; i++) {	// loop through looking for item
			if(*(dynamic_cast<Chatter *>(chatters_list_window->item(i))) == *chatter) {
				// found a duplicate... ignore addChatter() operation
				return;
			}
		}
		n = ignore_list_window->count();
		for(int i=0; i<n; i++) {	// loop through looking for item
			if(*(dynamic_cast<Chatter *>(ignore_list_window->item(i))) == *chatter) {
				// found a duplicate... ignore addChatter() operation
				return;
			}
		}
		// here we'll duplicate the Chatter so it can be deleted later
		// with removeChatter()
		Chatter *temp = new Chatter(*chatter);
		CHECK_PTR(temp);

		if(temp->isIgnored()) {
			temp->setListBox(chatters_list_window);
			chatters_list_window->inSort(temp);
		} else {
			temp->setListBox(ignore_list_window);
			ignore_list_window->inSort(temp);
		}
		updateChatters();
		emit numChattersChanged(chatters_list_window->count() + ignore_list_window->count());
		emit numIgnoredChanged(ignore_list_window->count());
	}

	void ChattersListWidget::removeChatter(const Chatter *chatter) {
		// basically we're gonna have to loop through the #@%!$#@ list
		// looking for our desired chatter, then remove that one
		if(!chatter) return;	// ignore bad arg
		int n = chatters_list_window->count();
		for(int i=0; i<n; i++) {	// loop through looking for item
			if(*(dynamic_cast<Chatter *>(chatters_list_window->item(i))) == *chatter) {
				// found it... get rid of it & go home
				chatters_list_window->removeItem(i);
				break;	// should be able to return here, but check other list
			}
		}
		n = ignore_list_window->count();
		for(int i=0; i<n; i++) {	// loop through looking for item
			if(*(dynamic_cast<Chatter *>(ignore_list_window->item(i))) == *chatter) {
				// found it... get rid of it & go home
				ignore_list_window->removeItem(i);
				break;
			}
		}
		emit numChattersChanged(chatters_list_window->count() + ignore_list_window->count());
		emit numIgnoredChanged(ignore_list_window->count());
		updateChatters();
	}

	void ChattersListWidget::resizeEvent(QResizeEvent *e) {
		QSize header_size = chatters_list_header->sizeHint();
		chatters_list_header->move(0, 0);
		chatters_list_header->resize(e->size().width(), header_size.height());
		list_splitter->move(0, header_size.height());
		list_splitter->resize(e->size().width(), e->size().height()-header_size.height());
//		chatters_list_window->resize(e->size().width(), chatters_list_window->height());
//		ignore_list_window->resize(e->size().width(), ignore_list_window->height());
	}

void ChattersListWidget::handleFriendLevelStatusClick(int index) {
	Chatter * chatter = new Chatter(chatter_menu->text(CHATTER_MENU_NAME));
	if(chatter) {
		if(chatter->isMe()) {
			cerr << "You cannot change your own friend level from " << toString(chatter->getFriendLevel()) << '.' << endl;
			QString str("*** You cannot change your own friend level. ***");
			emit printChatMessage(str, chatter, CET_SYSTEM_MESSAGE);
			return;
		}
		chatter->setFriendLevel((FriendLevel)index);
		cerr << chatter->getName() << " friend status set to ";
		cerr << toString(chatter->getFriendLevel()) << '.' << endl;
		QString str("*** ");
		str.append(chatter->getName());
		str.append(" friend status set to ");
		str.append(toString(chatter->getFriendLevel()));
		str.append('.');
		emit printChatMessage(str, chatter, CET_SYSTEM_MESSAGE);
		delete chatter;	// get rid of temp Chatter
		updateChatters();	// redraw widget
	} else
		cerr << "ChattersListWidget::handleFriendLevelStatusClick(int): can't find chatter." << endl;
}

void ChattersListWidget::handleMouseClick(QListBoxItem *chatter, int index, QMouseEvent *e) {
	// first check the event to see if we wanna pop up the menu
	if(e->button() == RightButton && chatter) {
		// set the menu state for the particular chatter
		Chatter *c = (Chatter *)chatter;
		chatter_menu->changeItem(c->getName(), (int)CHATTER_MENU_NAME);
		chatter_menu->setItemChecked(
			(int)CHATTER_MENU_IGNORED, c->isIgnored());
		chatter_menu->setItemChecked(
			(int)CHATTER_MENU_HIGHLIGHTED, c->isHighlighted());
		for(FriendLevel fl = STRANGER; fl < FRIENDLEVEL_END; ++fl) {
			friend_level_menu->setItemChecked(
				(int)fl, c->getFriendLevel()==fl);
		}

		int i = chatter_menu->exec(QCursor::pos());	// display popup at cursor
		if(i >= 0) {	// valid item selected
			bool new_state = !chatter_menu->isItemChecked(i);
			QString buf;
			switch((ChatterMenuItem)i) {
				case CHATTER_MENU_IGNORED:
					chatter_menu->setItemChecked(i, new_state);
					buf = "*** ";
					buf += c->getName() + (new_state ? " added to" : " removed from");
					buf += " ignore list ***";
					emit printChatMessage(buf, c, CET_SYSTEM_MESSAGE);
					c->setIgnored(new_state);
					// reset numDuplicatePosts of chatter if unignored
					if(!new_state) {
						QString t;
						t.setNum(time(NULL));
						c->doEvent(t);
					}
					updateChatters();
					break;
				case CHATTER_MENU_HIGHLIGHTED:
					c->setHighlighted(new_state);
					chatter_menu->setItemChecked(i, new_state);
// Disable popping up color dialog when highlight selected
//					if(new_state)
//						c->setHighlightColor(QColorDialog::getColor(c->getHighlightColor(), this));
					buf = "*** ";
					buf += c->getName() + (new_state ? " highlighted" : " unhighlighted");
					buf += " ***";
					emit printChatMessage(buf, c, CET_SYSTEM_MESSAGE);
					break;

				case CHATTER_MENU_HIGHLIGHT_COLOR:
					c->setHighlightColor(QColorDialog::getColor(c->getHighlightColor(), this));
				default:
					break;
			}
			// tell the chat widget
			updateChatters();
			chatters_list_window->repaint(true);	// modify the chatters list to show new status
			ignore_list_window->repaint(true);	// modify the chatters list to show new status
		}
	} else { // left/middle button click... turn off select if selected
		if(chatters_list_window->isSelected(index))
			chatters_list_window->setSelected(index, false);
	}
}

void ChattersListWidget::updateChatters() {
	for(unsigned int i=0; i<chatters_list_window->count(); i++) {
		Chatter *chatter = dynamic_cast<Chatter *>(chatters_list_window->item((int)i));
		if(!chatter) continue;
		if(chatter->isIgnored()) {
			ignore_list_window->inSort(new Chatter(*chatter));
			chatters_list_window->removeItem(i);
		} else
			chatters_list_window->updateItem(i);
	}
	for(unsigned int i=0; i<ignore_list_window->count(); i++) {
		Chatter *chatter = dynamic_cast<Chatter *>(ignore_list_window->item((int)i));
		if(!chatter) continue;
		if(!chatter->isIgnored()) {
			chatters_list_window->inSort(new Chatter(*chatter));
			ignore_list_window->removeItem(i);
		} else
			ignore_list_window->updateItem(i);
	}
	emit numChattersChanged(chatters_list_window->count() + ignore_list_window->count());
	emit numIgnoredChanged(ignore_list_window->count());
	repaint(true);
}

ChatterList ChattersListWidget::getChatters() const {
	ChatterList chatters;
	for(unsigned int i=0; i<chatters_list_window->count(); i++) {
		chatters.push_back(dynamic_cast<Chatter *>(chatters_list_window->item(i)));
	}
	ChatterList temp = getIgnoredChatters();
	chatters.splice(chatters.end(),temp);
	return chatters;
}

ChatterList ChattersListWidget::getIgnoredChatters() const {
	ChatterList chatters;
	for(unsigned int i=0; i<ignore_list_window->count(); i++) {
		chatters.push_back(dynamic_cast<Chatter *>(ignore_list_window->item(i)));
	}
	return chatters;
}

