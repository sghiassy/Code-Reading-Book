#include "prototypes.h"
#include "chatmain.h"
#include "chatter.h"
#include "lineedit.h"
#include "mainwidget.h"
#include "util.h"
#include <ctype.h>	// isblah() char functions
#include <fstream.h>
#include <iomanip.h>
#include <iostream.h>
#include <math.h>	// fabs()
#include <stdio.h>
extern "C" {
#include <string.h>
}
#include <sys/types.h>	// stat()
#include <sys/stat.h>	// stat()
#include <time.h>
#include <qclipboard.h>
#include <qcolordialog.h>
#include <qmessagebox.h>
#include <qpopupmenu.h>


const char *PMRedirectTypeString[] = {
	"REDIRECT_OFF",
	"REDIRECT_PUBLIC",
	"REDIRECT_PUBLIC_CENSORED",
	"REDIRECT_NULL",
	0
};
const char *toString(PMRedirectType t) {
	return PMRedirectTypeString[(int)(t-REDIRECT_OFF)];
}

PMRedirectType toPMRedirectType(const char *str) {
	for(int i=0; PMRedirectTypeString[i]; i++)
		if(strcasecmp(str, PMRedirectTypeString[i])==0) return (PMRedirectType)i;
	cerr << "toPMRedirectType(): type not found for '" << str << "'."<< endl;
	return REDIRECT_OFF;
}

static const int num_0d0a_limit = 4;

void printChatEventType(ChatEntryType t) {
	switch(t) {
	case CET_SAY_OUTGOING:
		cout << "CET_SAY_OUTGOING" << endl;
		break;
	case CET_SAY_INCOMING:
		cout << "CET_SAY_INCOMING" << endl;
		break;
	case CET_PM_OUTGOING:
		cout << "CET_PM_OUTGOING" << endl;
		break;
	case CET_PM_INCOMING:
		cout << "CET_PM_INCOMING" << endl;
		break;
	case CET_EMOTION:
		cout << "CET_EMOTION" << endl;
		break;
	case CET_ENTER:
		cout << "CET_ENTER" << endl;
		break;
	case CET_LEAVE:
		cout << "CET_LEAVE" << endl;
		break;
	case CET_LOGIN:
		cout << "CET_LOGIN" << endl;
		break;
	case CET_LOGOUT:
		cout << "CET_LOGOUT" << endl;
		break;
	case CET_INVITE:
		cout << "CET_INVITE" << endl;
		break;
	case CET_AWAY:
		cout << "CET_AWAY" << endl;
		break;
	case CET_BUDDY:
		cout << "CET_BUDDY" << endl;
		break;
	case CET_SYSTEM_MESSAGE:
		cout << "CET_SYSTEM_MESSAGE" << endl;
		break;
	case CET_NULL:
		cout << "CET_NULL" << endl;
		break;
	}
}

	ChatEntry::ChatEntry(ChatEntryType et, const Chatter *c, QString t, QListBox *_parent) {
		if(::debuglevel & DEBUGLEVEL_CONSTRUCTOR)
			cerr << "ChatEntry constructor called." << endl;
		if(c)
			chatter = new Chatter(*c);
		else
			chatter = 0;
		txt = t;
		lb = _parent;
		type = et;
		cached_height=0;	// holds cached height
		old_listbox_width=-1;	// last listbox width
	}

	ChatEntry::ChatEntry(const ChatEntry &c) {
		if(::debuglevel & DEBUGLEVEL_COPYCONSTRUCTOR)
			cerr << "ChatEntry copy constructor called." << endl;
		if(c.chatter)
			chatter = new Chatter(*c.chatter);
		else
			chatter = 0;
		txt = c.txt;
		type = c.type;
		cached_height=c.cached_height;	// holds cached height
		old_listbox_width = c.old_listbox_width;
	}

	ChatEntry::~ChatEntry() {
		if(::debuglevel & DEBUGLEVEL_DESTRUCTOR)
			cerr << "ChatEntry destructor called." << endl;
		if(chatter) delete chatter;
	}

	QString ChatEntry::text() const {
		QString buf;
		if(chatter) {
			buf = chatter->text();
			switch(type) {
			  case CET_EMOTION:
				buf += " ";
				break;
			  case CET_PM_INCOMING:
				buf += " tells you: ";
				break;
			  case CET_ENTER:
				buf.prepend("*** ");
				buf.append(" enters ***");
				break;
			  case CET_LEAVE:
				buf.prepend("*** ");
				buf.append(" leaves ***");
				break;

			  default:
				buf += ": ";
				break;
			}
		}
		if(txt)
			buf += txt;
		return buf;
	}

	int ChatEntry::height(const QListBox *b) const {

		if(!b) {
			b = listBox();
		}
		int *old_listbox_width_ptr = const_cast<int*>(&old_listbox_width);
		int *cached_height_ptr = const_cast<int*>(&cached_height);
		if(old_listbox_width != b->width()) {	// widget has been resized horizontally
			*old_listbox_width_ptr = b->width();
			int chatter_pixel_width = 0;
			// print the chatter first
			if(chatter) {
				chatter_pixel_width = chatter->width(b);
			}
			QFont f = b->font();	// get the font of the painter
			f.setWeight(QFont::Normal);
			int text_pixel_width = old_listbox_width - chatter_pixel_width - 10 - b->verticalScrollBar()->width() - 3;
			QFontMetrics fm(f);
			*cached_height_ptr = fm.boundingRect(chatter_pixel_width+10, 0,
				text_pixel_width, 2048,
				Qt::AlignLeft|Qt::WordBreak,
				txt).height();
		}
		return cached_height;
	}

	int ChatEntry::width(const QListBox *b) const {
		if(b)
			return b->width();
		else if(listBox())
			return listBox()->width();
		else
			return 0;
	}

	void ChatEntry::paint(QPainter *p) {
		QFont f = p->font();	// get the font of the painter
		QFontMetrics fm(f);	// get a font metrics for the font
		p->save();
		int text_width = width(listBox())-10-listBox()->verticalScrollBar()->width() - 3;
		switch(type) {
		  case CET_SAY_INCOMING:
		  case CET_PM_INCOMING:
		  case CET_EMOTION:
		  {
			int chatter_width = 0;
			if(chatter) {	// draw the chatter's name
				p->save();
				f.setBold(true);
				p->setFont(f);
				chatter_width = chatter->width(p);
				p->translate(10,0);
				chatter->paint(p);	// draw chatter's name
				p->restore();
				if(chatter->isHighlighted()) {
					p->setBackgroundMode(Qt::OpaqueMode);
					p->setBackgroundColor(chatter->getHighlightColor());
				}
			}
			// now we have to draw the rest of the crap
			switch(type) {	// need another switch statement, easier
			  case CET_SAY_INCOMING:
			  {
				p->drawText(10+chatter_width, 0, text_width-chatter_width, 1024,
					Qt::WordBreak, ": ");
				int i = p->fontMetrics().boundingRect(": ").width();
				drawMarkupText(p, 10+chatter_width+i, 0,
					text_width-chatter_width-i, 1024, txt);
				break;
			  }

			  case CET_PM_INCOMING:
			  {
				p->setBackgroundMode(Qt::OpaqueMode);
				p->setBackgroundColor(QColor(230, 230, 230));
				p->drawText(10+chatter_width, 0, text_width-chatter_width, 1024,
					Qt::WordBreak, " tells you:");
				int i = p->fontMetrics().boundingRect(" tells you:").width();
				drawMarkupText(p, 10+chatter_width+i, 0,
					text_width-chatter_width-i, 1024, txt);
				break;
			  }

			  case CET_EMOTION:
				p->setPen(Qt::red);
				drawMarkupText(p, 15+chatter_width, 0,
					text_width-chatter_width-5, 1024, txt);
				break;

			  default:
				break;

			}
			break;
		  }

		  case CET_ENTER:
		  case CET_LEAVE:
		  {
			QString s = chatter->getName();
			if(type==CET_ENTER) {
				s.prepend("--- ");
				s.append(" enters -->");
				p->setPen(QColor(100,100,200));
			} else {
				s.prepend("<-- ");
				s.append(" leaves ---");
				p->setPen(QColor(100,100,140));
			}
			f.setBold(true);
			p->setFont(f);
			p->drawText(10, 0, text_width, 1024,
				Qt::WordBreak, (QString)s);
			break;
		  }

		  case CET_LOGIN:
		  case CET_LOGOUT:
		  {
			p->setPen(QColor(100,100,200));
			QString s("*** You have successfully logged ");
			if(type==CET_LOGIN)
				s.append("in ***");
			else
				s.append("out ***");
			f.setBold(true);
			p->setFont(f);
			p->drawText(10, 0, text_width, 1024,
				Qt::WordBreak, (QString )s);
			break;
		  }

		  case CET_SYSTEM_MESSAGE:
		  default:	// CET_AWAY, CET_INVITE messages for now
			p->setPen(QColor(100,100,200));
			f.setBold(true);
			p->setFont(f);
			p->drawText(10, 0, text_width, 1024,
				Qt::WordBreak, txt);
			break;
		}
		p->restore();
	}

void ChatEntry::drawMarkupText(QPainter *p, int x, int y, int w, int h, QString txt) {
	// right now just paint the crap up there, regardless of what's in the string
	p->drawText(x, y, w, h, Qt::WordBreak, txt);
}

void MyListBox::mousePressEvent(QMouseEvent *e) {
	int y_coord = e->y();	// get y-coordinate of click
	int item_index = findItem(y_coord);	// get index of item under click
	emit mouseClick(item(item_index), item_index, e);
}

void MyListBox::resizeEvent(QResizeEvent *e) {
//	QListBoxItem *item = itemAt(QPoint(1, e->size().height()-1));
//	int item_index = item ? index(item) : count()-1;
	QListBox::resizeEvent(e);
	if(count())
		setBottomItem(count()-1);
}

// Instantiate/initialize static variables
const int ChatWidget::MAX_LINE_LENGTH = 512;
const char *ChatWidget::AUTOWARN_PM_MSG_DEFAULT = "*** WARNING: PMs are intensely disliked by this user; CUT IT OUT ***";

ChatWidget::ChatWidget(QWidget *parent, QString name, WFlags f) :
	QWidget(parent, name, f) {

	if(::debuglevel & DEBUGLEVEL_CONSTRUCTOR)
		cerr << "ChatWidget constructor called." << endl;

	chat_window = new MyListBox(this);
	chat_window->setSelectionMode(QListBox::Multi);	// disable selection of several lines
	chat_window->setBottomScrollBar(false);	// disable bottom scroll bar
	chat_window->setAutoBottomScrollBar(false);	// ditto

	chat_entry_line = new LineEdit(-1, this);
	chat_entry_line->setMaxLength(MAX_LINE_LENGTH);	// set max line entry length
		// anything longer will be truncated.

	chatter_menu = new QPopupMenu();
	chatter_menu->insertItem("", (int)CHATTER_MENU_NAME);
	chatter_menu->insertSeparator();

	chatter_menu->insertItem("&Ignored", (int)CHATTER_MENU_IGNORED);
	chatter_menu->insertItem("&Highlighted", (int)CHATTER_MENU_HIGHLIGHTED);
	chatter_menu->insertItem("Highlight C&olor...", (int)CHATTER_MENU_HIGHLIGHT_COLOR);

	friend_level_menu = new QPopupMenu(chatter_menu);
	for(FriendLevel fl = STRANGER; fl < FRIENDLEVEL_END; ++fl) {
		friend_level_menu->insertItem(toString(fl), (int)fl);
	}
	connect(friend_level_menu, SIGNAL(activated(int)), this, SLOT(handleFriendLevelStatusClick(int)));
	friend_level_menu->setCheckable(true);

	chatter_menu->insertItem("&Friend Status...", friend_level_menu, (int)CHATTER_MENU_FRIEND_STATUS);

	chatter_menu->insertSeparator();
	chatter_menu->insertItem("&Copy", (int)CHATTER_MENU_COPY);
	chatter_menu->insertItem("Re&move All Entries", (int)CHATTER_MENU_REMOVE_ENTRIES);
	chatter_menu->insertSeparator();
	chatter_menu->insertItem("&PM...", (int)CHATTER_MENU_PM);
	chatter_menu->setItemEnabled((int)CHATTER_MENU_PM, false);	// disable for now
	chatter_menu->insertItem("P&roperties...", (int)CHATTER_MENU_PROPERTIES);
	chatter_menu->setItemEnabled((int)CHATTER_MENU_PROPERTIES, true);	// disable for now

	send_button = new QPushButton("Send", this);
	enter_label = new QLabel(chat_entry_line, "&Enter:", this);

// Initialize widget state
	ignore_case = false;
	ignore_movement = false;
	ignore_pms = false;	// ignore PMs from strangers
	ignore_pms_friend_level = STRANGER;	// ignore PMs below & including this FriendLevel
	ignore_pms_outside_room = true;	// ignore PMs from strangers outside room
	redirect_public = false;	// do not redirect PMs to public channel
	redirect_public_censored = false;	// do not redirect PMs to public channel
	redirect_null = false;	// do not redirect PMs to /dev/null
	autoignore_enabled = true;	// enable autoignore
	autoignore_friend_level = STRANGER;	// ignore STRANGER & below
	autowarn_pm = false;	// do not autowarn on PM
	autowarn_pm_msg = QString(0);
	autoignore_first_post_URL_enabled = true;	// autoignore first post URL?
	autoignore_first_post_PM_enabled = false;	// autoignore first post PM?
	autoignore_multiline_enabled = true;	// autoignore multiline posts?
	autoignore_single_post_enabled = false;	// autoignore single posts?
	autoignore_caps_enabled = true;	// autoignore uppercase?
	enableTranslateEscape(true);
	max_num_duplicates = 3;
	max_num_caps_posts = 3;
	max_num_entries = 500;	// maximum number of entries this widget can hold
	clone_detection_threshold = 70;	// 70%
	clone_detection_enabled = true;
	verbosity = true;	// verbose message printing
	cosychat_enabled = false;
	cosychat_friendlevel = STRANGER;

	// have chat entry line click button when Enter pressed
	QObject::connect(chat_entry_line, SIGNAL(returnPressed()), send_button, SLOT(animateClick()));
	// set up the click-the-send-button actions
	connect(send_button, SIGNAL(clicked()), SLOT(processSendClick()));
	// handle mouse clicks in the chat window
	connect(chat_window,
		SIGNAL(mouseClick(QListBoxItem *, int, QMouseEvent *)),
		SLOT(handleMouseClick(QListBoxItem *, int, QMouseEvent *)));
	// also emit mouse clicks to outside world
	connect(chat_window,
		SIGNAL(mouseClick(QListBoxItem *, int, QMouseEvent *)),
		SIGNAL(mouseClick(QListBoxItem *, int, QMouseEvent *)));
	// give the chat_entry_line widget the focus
	chat_entry_line->setFocus();
	// keep bottom scroll bar offa that motha
	chat_window->setBottomScrollBar(false);
	setMinimumSize(150,100);
}
	
ChatWidget::~ChatWidget() {
	if(::debuglevel & DEBUGLEVEL_DESTRUCTOR)
		cerr << "ChatWidget destructor called." << endl;
	delete chat_window;
	delete chat_entry_line;
	delete chatter_menu;
	delete send_button;
	delete enter_label;
}

void ChatWidget::saveState(ostream &os) const {
	try {
		setParam(os, "IgnoreCase", getIgnoreCase());
		setParam(os, "IgnoreMovement", getIgnoreMovement());
		setParam(os, "IgnorePMs", isIgnorePMsEnabled());
		setParam(os, "IgnorePMsFriendLevel", toString(getIgnorePMsFriendLevel()));
		setParam(os, "IgnorePMsOutsideRoom", isIgnorePMsOutsideRoomEnabled());
		setParam(os, "AutoWarnOnPM", getAutoWarnOnPM());
		setParam(os, "AutoWarnMessage", getAutoWarnMessage());
		setParam(os, "AutoIgnore", isAutoIgnoreEnabled());
		setParam(os, "AutoIgnoreFriendLevel", toString(getAutoIgnoreFriendLevel()));
		setParam(os, "AutoIgnoreFPURL", isAutoIgnoreFirstPostURLEnabled());
		setParam(os, "AutoIgnoreFPPM", isAutoIgnoreFirstPostPMEnabled());
		setParam(os, "AutoIgnoreSinglePost", isAutoIgnoreSinglePostEnabled());
		setParam(os, "AutoIgnoreCaps", isAutoIgnoreCapsEnabled());
		setParam(os, "AutoIgnoreMultiline", isAutoIgnoreMultilineEnabled());
		setParam(os, "RedirectPMs", toString(getRedirectPMs()));

		QFont f = getFont();
		setParam(os, "ChatWindowFont.Family", f.family());
		setParam(os, "ChatWindowFont.PointSize", f.pointSize());
		setParam(os, "ChatWindowFont.Weight", f.weight());
		setParam(os, "ChatWindowFont.Italic", f.italic());

		setParam(os, "MaxNumDuplicates", getMaxNumDuplicates());
		setParam(os, "MaxNumCapsPosts", getMaxNumCapsPosts());
		setParam(os, "TranslateEscape", getTranslateEscapeEnabled());
		setParam(os, "CloneDetection", isCloneDetectionEnabled());
		setParam(os, "CloneDetectionThreshold", getCloneDetectionThreshold());
		setParam(os, "Verbosity", getVerbosity());
		setParam(os, "CosyChat", isCosyChatEnabled());
		setParam(os, "CosyChatFriendLevel", toString(getCosyChatFriendLevel()));
	} catch (ParamException) { 
		cerr << "ChatWidget::saveState(): Error reading value" << endl;
	}
}

void ChatWidget::restoreState(istream &is) {
	bool tmpb;
	int tmpi;
	const size_t tmps_size = 1024;
	char tmps[tmps_size];

	try {
		getParam(is, "IgnoreCase", tmpb);
			setIgnoreCase(tmpb);
		getParam(is, "IgnoreMovement", tmpb);
			setIgnoreMovement(tmpb);
		getParam(is, "IgnorePMs", tmpb);
			setIgnorePMsEnabled(tmpb);
		getParam(is, "IgnorePMsFriendLevel", tmps, tmps_size);
			setIgnorePMsFriendLevel(toFriendLevel(tmps));
		getParam(is, "IgnorePMsOutsideRoom", tmpb);
			setIgnorePMsOutsideRoomEnabled(tmpb);
		getParam(is, "AutoWarnOnPM", tmpb);
			setAutoWarnOnPM(tmpb);
		getParam(is, "AutoWarnMessage", tmps, tmps_size); setAutoWarnMessage(tmps);
		getParam(is, "AutoIgnore", tmpb); setAutoIgnoreEnabled(tmpb);
		getParam(is, "AutoIgnoreFriendLevel", tmps, tmps_size);
			setAutoIgnoreFriendLevel(toFriendLevel(tmps));
		getParam(is, "AutoIgnoreFPURL", tmpb); setAutoIgnoreFirstPostURLEnabled(tmpb);
		getParam(is, "AutoIgnoreFPPM", tmpb); setAutoIgnoreFirstPostPMEnabled(tmpb);
		getParam(is, "AutoIgnoreSinglePost", tmpb); setAutoIgnoreSinglePostEnabled(tmpb);
		getParam(is, "AutoIgnoreCaps", tmpb); setAutoIgnoreCapsEnabled(tmpb);
		getParam(is, "AutoIgnoreMultiline", tmpb); setAutoIgnoreMultilineEnabled(tmpb);
		getParam(is, "RedirectPMs", tmps, tmps_size);
			setRedirectPMs(toPMRedirectType(tmps));
			setRedirectPMs(toPMRedirectType(tmps));

		int ps, w;
		getParam(is, "ChatWindowFont.Family", tmps, tmps_size);
		getParam(is, "ChatWindowFont.PointSize", ps);
		getParam(is, "ChatWindowFont.Weight", w);
		getParam(is, "ChatWindowFont.Italic", tmpb);
		setFont(QFont(tmps, ps, w, tmpb));

		getParam(is, "MaxNumDuplicates", tmpi); setMaxNumDuplicates(tmpi);
		getParam(is, "MaxNumCapsPosts", tmpi); setMaxNumCapsPosts(tmpi);
		getParam(is, "TranslateEscape", tmpb); enableTranslateEscape(tmpb);
		getParam(is, "CloneDetection", tmpb); setCloneDetectionEnabled(tmpb);
		getParam(is, "CloneDetectionThreshold", tmpi); setCloneDetectionThreshold(tmpi);
		getParam(is, "Verbosity", tmpb); setVerbosity(tmpb);
		getParam(is, "CosyChat", tmpb); setCosyChatEnabled(tmpb);
		getParam(is, "CosyChatFriendLevel", tmps, 1024);
			setCosyChatFriendLevel(toFriendLevel(tmps));
	} catch (ParamException) { 
		cerr << "ChatWidget::restoreState(): Error reading value" << endl;
	}
}

void ChatWidget::resizeEvent(QResizeEvent *e) {
	chat_window->move(0,0);
	QSize entry_line_size = chat_entry_line->sizeHint();
	QSize send_button_size = send_button->sizeHint();
	QSize enter_label_size = enter_label->sizeHint();
	QSize new_size = (e ? e->size() : size());

	chat_window->resize(new_size.width(), new_size.height() - entry_line_size.height());
	chat_entry_line->setGeometry(enter_label_size.width(), new_size.height()-entry_line_size.height(),
		new_size.width()-send_button_size.width()-enter_label_size.width(), entry_line_size.height());
	send_button->move(new_size.width()-send_button_size.width(), chat_window->height());
	send_button->resize(send_button_size);
	enter_label->move(0, chat_window->height());
	enter_label->resize(enter_label_size);
}

void ChatWidget::addEntry(QString text, Chatter *chatter, ChatEntryType t) {	// add line from chatter

	// do event for chatter's entry
	if(chatter && !chatter->isIgnored() &&
		(t == CET_SAY_INCOMING ||
		 t == CET_PM_INCOMING ||
		 t == CET_AWAY ||
		 t == CET_EMOTION  /*||
		 t == CET_ENTER ||
		 t == CET_LEAVE */))
	chatter->doEvent(text);

	// set number of PMs by chatter
	if(chatter && t == CET_PM_INCOMING)
		chatter->incNumPMEvents();

	// detect clones before doing any ignoring of packets
	if(t == CET_ENTER && chatter)
		detectClones(chatter);

	// ignore messages from ignored people
	if(chatter && chatter->isIgnored() && t!=CET_SYSTEM_MESSAGE) return;

	// ignore people below cosychat friendlevel if enabled
	if(cosychat_enabled && chatter && chatter->getFriendLevel() <= cosychat_friendlevel) return;

	// ignore conditions
	const char *cp;
	if((cp=isIgnorable(chatter, text, t)) != 0) {
		if(::debuglevel & DEBUGLEVEL_NOTICE)
			cerr << chatter->getName() << " is ignored: " << cp << endl;
		QString buf("*** ");
		buf += chatter->getName();
		buf += " ignored: ";
		buf += cp;
		addEntry(buf, chatter, CET_SYSTEM_MESSAGE);
		chatter->setIgnored(true);
		emit updateChatters();	// tell the world
	}

	// exiting conditions
	if(ignore_movement && (t == CET_ENTER || t == CET_LEAVE)) return;
	if(t!=CET_SYSTEM_MESSAGE && chatter && chatter->isIgnored() && !chatter->isMe()) return;
	if(t == CET_PM_INCOMING && chatter && !chatter->isMe()) {
		if(ignore_pms && chatter->getFriendLevel() <= ignore_pms_friend_level) return;
		if(ignore_pms_outside_room) {
			const MainWidget *mwp = dynamic_cast<const MainWidget *>(parent());
			if(mwp && mwp->isInRoom(chatter)) return;
		}
		if(redirect_null) return;
	}

	// set last event type of chatter
	if(chatter) {
		chatter->setLastEventType(t);
	}


	// modify entries
	ChatEntry *ce;
	if(!text.isNull() && !text.isEmpty()) {
		chat_entry_line->setFocus();
		char buf[1024];
		strncpy(buf, text.latin1(), 1024);
		removeTags(buf, 1024);
		QString text_string = QString(buf);
		// lowercase if IgnoreCase on, not a system message, and not my text
		if(ignore_case && t != CET_SYSTEM_MESSAGE && t != CET_AWAY && 
			!chatter->isMe())
			text_string = text_string.lower();
		ce = new ChatEntry(t, chatter, text_string, chat_window);
	} else ce = new ChatEntry(t, chatter, 0, chat_window);
	addEntry(*ce);	// stick it in the list box
	// scroll down to new item if we're already at the bottom
	// ...or if the entered item is from me
	// ...or if it's a system message
	if((unsigned int)(chat_window->topItem()+chat_window->numItemsVisible()) >= chat_window->count()-3
		|| (chatter && chatter->isMe())
		|| t == CET_SYSTEM_MESSAGE) {
		chat_window->setBottomItem(chat_window->count()-1);
		chat_window->clearSelection();
	}

	// handle autowarn on pm
	if(t == CET_PM_INCOMING && getAutoWarnOnPM() && !chatter->isMe() && chatter->getFriendLevel() < FRIEND)	{	// warn PMers who are strangers
		const int buflen = 128;		// build PM command
		char buf[buflen] = "/tell ";
		memset(buf+6, 0, buflen-6);	// zero the rest of the buffer
		strncat(buf, chatter->text(), buflen);
		strncat(buf, " ", buflen);
		strncat(buf, getAutoWarnMessage(), buflen);
		emit sendText(buf, verbosity);	// ship off the command
	}

	// send PMs back to room if 2nd or greater PM and person is STRANGER
	if(chatter && t == CET_PM_INCOMING && (redirect_public || redirect_public_censored)) {
		if(chatter->numPMEvents() <= 1 && chatter->getFriendLevel() < FRIEND) {
			// warn PMer about subsequent redirection
			const int buflen = 128;		// build PM command
			char buf[buflen] = "/tell ";
			memset(buf+6, 0, buflen-6);	// zero the rest of the buffer
			strncat(buf, chatter->text(), buflen);
			strncat(buf, " *** WARNING: subsequent PMs will be posted to the public chatroom ***", buflen);
			emit sendText(buf, verbosity);	// ship off the command
		} else if(chatter->numPMEvents() > 1 && chatter->getFriendLevel() < FRIEND) {
			QString say;
			say = chatter->getName();
			say.append(" tells me: ");
			if(redirect_public && !redirect_public_censored)
				say.append(text);
			else
				say.append("[text omitted]");
			emit sendText(say);
		}
	}

	// limit number of posts in widget
	if(chat_window->count() >= (unsigned int)max_num_entries) {
		int num_extra =
			(chat_window->count() - max_num_entries) > 0 ?
			chat_window->count() - max_num_entries : 0;
		for(int i=0; i<num_extra; i++)
			chat_window->removeItem(0);
	}
}

void ChatWidget::addEntry(QString text) {	// add line
	addEntry(text, 0, CET_SYSTEM_MESSAGE);	// ...as a system message
}

void ChatWidget::addEntry(const QListBoxItem &entry) {
	((ChatEntry&)entry).setListBox(chat_window);
	chat_window->insertItem(&entry);
}

void ChatWidget::removeEntry(int index) {
	chat_window->removeItem(index);
}

void ChatWidget::removeAllEntries(Chatter *c) {	// purge widget of all entries related to Chatter
	Chatter *chatter;
	for(int i=chat_window->count()-1; i>=0; i--) {
		chatter = (dynamic_cast<ChatEntry*>(chat_window->getItemAt(i)))->getChatter();
		if(chatter && (*chatter == *c))
			removeEntry(i);
	}
}

void ChatWidget::clear() {	// clear the screen
	chat_window->clear();
}

//	void ChatWidget::setFont(QFont &f) {	// set the font of the main
//chat window 		chat_window->setFont(f);
//	}

// Accessors
//	QFont ChatWidget::getFont() {	// return the font used for chat
//		QFont *fp = new QFont(chat_window->font());
//		return *fp;
//	}

// Slots
void ChatWidget::processSendClick() {
	if(strlen(chat_entry_line->text())) {	// we have text
		QString txt = chat_entry_line->text();	// make a copy of the line
		int i=0;	// index into string
		if(getTranslateEscapeEnabled()) {
			while((i=txt.find('\\', i, true)) >= 0) {	// look for escape seq.
				char ch=(char)(const_cast<const QString*>(&txt))->at(i+1);
				switch(ch) {
					case 'n':
						txt.remove(i, 2);
						txt.insert(i, 0x0a);
						txt.insert(i, 0x0d);
						txt.insert(i, ' ');
						i+=2;
						break;
					case 't':
						txt.remove(i, 2);
						txt.insert(i, QString("   "));	// replace tabs w/ 3 spc
						i+=3;
						break;
					case '\\':
						txt.remove(i, 1);
						i++;
						break;
					default:
						i++;
						break;
				}
			}
		}
		// clear the line edit widget, now that we have its precious text
		chat_entry_line->clear();

		// let's notify the outside world
		emit sendText(txt);
	}
}

extern int addNetscapePID(pid_t pid, const char *url);

void ChatWidget::handleFriendLevelStatusClick(int index) {
	Chatter * chatter = new Chatter(chatter_menu->text(CHATTER_MENU_NAME));
	if(chatter) {
		if(chatter->isMe()) {
			cerr << "You cannot change your friend level from " << toString(chatter->getFriendLevel()) << '.' << endl;
			QString str("*** You cannot change your friend level. ***");
			addEntry(str, chatter, CET_SYSTEM_MESSAGE);
			return;
		}
		// 'index' is a direct integer value of FriendLevel
		chatter->setFriendLevel((FriendLevel)index);
		cerr << chatter->getName() << " friend status set to ";
		cerr << toString(chatter->getFriendLevel()) << '.' << endl;
		QString str("*** ");
		str.append(chatter->getName());
		str.append(" friend status set to ");
		str.append(toString(chatter->getFriendLevel()));
		str.append('.');
		addEntry(str, chatter, CET_SYSTEM_MESSAGE);
		emit updateChatters();	// tell the world
		delete chatter;
	} else
		cerr << "ChatWidget::handleFriendLevelStatusClick(int): can't find chatter." << endl;
}

void ChatWidget::handleMouseClick(QListBoxItem *chatentry, int index, QMouseEvent *e) {
	// first check the event to see if we wanna pop up the menu
	if(e->button() == RightButton && chatentry) {
		// set the menu state for the particular chatter
		ChatEntry *entry = (ChatEntry *)chatentry;
		Chatter *c = entry->getChatter();
		if(!c) return;
		chatter_menu->changeItem(c->getName(), (int)CHATTER_MENU_NAME);
		chatter_menu->setItemChecked(
			(int)CHATTER_MENU_IGNORED, c->isIgnored());
		chatter_menu->setItemChecked(
			(int)CHATTER_MENU_HIGHLIGHTED, c->isHighlighted());
		for(FriendLevel fl = STRANGER; fl < FRIENDLEVEL_END; ++fl) {
			friend_level_menu->setItemChecked(
				(int)fl, c->getFriendLevel()==fl);
		}

		// Add any URLs to the menu
		char line[512];
		memset(line, 0, 512);
		if(!entry->text().isNull())
			strncpy(line, (const char *)entry->text(), 512);
		int num_URLs = 0;
		const char *cp=line;
		while((cp = findURL(cp))) {
			if(!num_URLs)
				chatter_menu->insertSeparator();
			const char *cp2 = cp;	// search for end of URL
			QString URLstring("");
			while(*cp2 && !isspace(*cp2)) {
				URLstring += *cp2;
				cp2++;
			}
			chatter_menu->insertItem(URLstring, CHATTER_MENU_URL_BASE+num_URLs);
			num_URLs++;
			while(*cp && !isspace(*cp)) cp++;
		}


		int i = chatter_menu->exec(QCursor::pos());	// display popup at cursor
		if(i >= 0 && i < CHATTER_MENU_URL_BASE) {	// valid item selected
			bool new_state = !chatter_menu->isItemChecked(i);
			QString buf;
			switch((ChatterMenuItem)i) {

				case CHATTER_MENU_IGNORED:
					chatter_menu->setItemChecked(i, new_state);
					buf = "*** ";
					buf += c->getName() + (new_state ? " added to" : " removed from");
					buf += " ignore list ***";
					addEntry(buf, c, CET_SYSTEM_MESSAGE);
					// clear numDuplicatePosts value of chatter if ignore cleared
					if(!new_state) {
						QString t;
						t.setNum(time(NULL));
						c->doEvent(t);
					}
					c->setIgnored(new_state);
					break;

				case CHATTER_MENU_HIGHLIGHTED:
					c->setHighlighted(new_state);
					chatter_menu->setItemChecked(i, new_state);
					if(new_state) {
						QColor new_color = QColorDialog::getColor(c->getHighlightColor(), this);
						if(new_color.isValid())
							c->setHighlightColor(new_color);
					}
					buf = "*** ";
					buf += c->getName() + (new_state ? " highlighted" : " unhighlighted");
					buf += " ***";
					addEntry(buf, c, CET_SYSTEM_MESSAGE);
					break;

				case CHATTER_MENU_HIGHLIGHT_COLOR:
					c->setHighlightColor(QColorDialog::getColor(c->getHighlightColor(), this));
					break;

				case CHATTER_MENU_COPY:
					qApp->clipboard()->setText(entry->text());
					break;

				case CHATTER_MENU_REMOVE_ENTRIES:
					removeAllEntries(c);
					break;

				case CHATTER_MENU_PM:
					emit openPMWindow(c);
					break;

				case CHATTER_MENU_PROPERTIES:
				{
					char argstr[512];
					char urlstr[256];
					snprintf(urlstr, 256, "http://profiles.yahoo.com/%s",
						(const char *)c->getName());
					snprintf(argstr, 512, "-geometry =500x700 -dont-save-geometry-prefs -noraise -remote openURL(%s,new-window)",
						urlstr);
					
					// This is a fallible solution, since there is a period
					// in which Netscape is running but the pid hasn't been
					// added to the list of Netscape pid's. It's non-fatal though.
					addNetscapePID(shell("netscape", argstr), urlstr);
					break;
				}

				default:
					break;
			}	// end switch
			// tell the chat widget
			chat_window->update();	// modify the chatters list to show new status
			emit updateChatters();	// tell everyone else (i.e. the chatters list)
		} else if(i >= CHATTER_MENU_URL_BASE) {
			char argstr[512];
			const char *cp;
			snprintf(argstr, 512, "-noraise -remote openURL(%s,new-window)",
				cp=(const char *)chatter_menu->text(i));
			
			// This is a fallible solution, since there is a period
			// in which Netscape is running but the pid hasn't been
			// added to the list of Netscape pid's. It's non-fatal though.
			addNetscapePID(shell("netscape", argstr), cp);
		}	// end check for valid menu item selected
		for(int i=0; i<num_URLs; i++)	// remove URLs
			chatter_menu->removeItem(CHATTER_MENU_URL_BASE+i);
		if(num_URLs)	// remove separator
			chatter_menu->removeItemAt(chatter_menu->count()-1);
	} else {
		chat_window->clearSelection();
	}
}

		QListBoxItem *ChatWidget::toQListBoxItem(QString text) {
			return new QListBoxText(text);
		}

		void ChatWidget::writeToFile(QString path) {	// write contents to file
			struct stat sb;

			if(chat_window->count() == 0) {	// nothing to save
				QMessageBox::information(this, 0,
					"There is nothing to save.");
				return;
			}
			if(!stat((const char *)path, &sb)) {
				QString msg("File ");
				msg  += path + " exists... Overwrite?";
				if(QMessageBox::warning(this, 0, msg,
					"Overwrite", "Cancel", 0,
					0, 1) == 1) return;
			}
		ofstream ofs(path);

		if(ofs) {
			if(::debuglevel & DEBUGLEVEL_NOTICE)
				cerr << "Writing chat contents to " << path << "..." << flush;
			for(unsigned int i=0; i<chat_window->count(); i++) {
				ofs << chat_window->text(i) << endl;
			}
			if(::debuglevel & DEBUGLEVEL_NOTICE)
				cerr << "done." << endl;
		} else {
			if(::debuglevel & DEBUGLEVEL_ERROR) {
				cerr << endl << "Error opening file." << endl;
				perror(0);
			}
		}
	}

	void ChatWidget::setAutoWarnMessage(QString msg) {
		if(!msg.isEmpty() && !msg.isNull()) {	// only change if arg is valid
			autowarn_pm_msg = msg;	// copy string into storage
		}
	}

const char *ChatWidget::isIgnorable(Chatter *c, const QString &txt, ChatEntryType t) {
	if(!autoignore_enabled) return 0;
	if(!c || c->isIgnored()) return 0;	// no chatter or chatter already ignored
	if(c->isMe()) return 0;	// me isn't ignorable
	if(c->getFriendLevel() > autoignore_friend_level) return 0;
	if(t == CET_SYSTEM_MESSAGE) return 0;

	if(autoignore_first_post_URL_enabled && c->getLastEventType() == CET_ENTER && !txt.isNull() && findURL(txt))
			return "first post URL";

	if(autoignore_first_post_PM_enabled && t==CET_PM_INCOMING && c->numEvents() <= 1)
		return "first post PM";

	

	if(autoignore_multiline_enabled && (t==CET_SAY_INCOMING || t==CET_PM_INCOMING || t==CET_EMOTION)) {	// check for 0d0a chars
		int num_0d=0;
		
		for(const char *cp = txt.latin1(); cp && *cp; cp++) {
			if(*cp=='\x0d') num_0d++;
		}
		if(num_0d > num_0d0a_limit)
			return "multiline posting.";
	}

	// check for caps in post
	if(!ignore_case && autoignore_caps_enabled && (t==CET_SAY_INCOMING || t==CET_PM_INCOMING)) {
		unsigned int num_letters = 0;
		unsigned int num_caps = 0;
//		unsigned int len = txt.length();
		for(const char *cp = txt.latin1(); cp && *cp; cp++) {
			if(isalpha(*cp)) {
				num_letters++;
				if(isupper(*cp))
					num_caps++;
			}
		}
		if(num_letters && ((float)num_caps / (float)num_letters > 0.90)) {
			c->incNumCapsEvents();
			if(c->numCapsEvents() > max_num_caps_posts)
				return "uppercase post";
		}
	}

	if(autoignore_single_post_enabled && t==CET_LEAVE && c->numEvents()<2 && (c->getLastEventType()==CET_PM_INCOMING || c->getLastEventType()==CET_SAY_INCOMING))
		return "single post spamming.";

	int num_duplicates = c->numDuplicateEvents();
	if(max_num_duplicates && num_duplicates >= max_num_duplicates)
		return "duplicate posts";

	return 0;
}

void ChatWidget::detectClones(const Chatter *chatter) {
	// detect clones before doing any ignoring of packets
	QString clonee;	// person being cloned
	float similarity=0.0;	// similarity returned from isClone()
	if(clone_detection_enabled && !(clonee=chatter->isClone((float)clone_detection_threshold/100.0, FRIEND, similarity)).isNull()) {

		QString str("*** Clone warning: ");
		str.append(chatter->getName());
		str.append(" may be clone of ");
		str.append(clonee);
		str.append(", similarity ");
		str.append(QString::number(similarity*100.0));
		str.append("%.");
		emit sendText(str, verbosity);	// alert room of clone
	}
}
