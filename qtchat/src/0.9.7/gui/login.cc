#include "debug.h"
#include "login.h"
#include <iostream.h>
#include <string.h>
#include <unistd.h>	// close() declaration
#include <qvalidator.h>

const char *login_dialog_labels[] = {
	"Username:",
	"Password:",
	"Server:",
	"Port:",
	"Goto Room:",
	"Goto User:"
};

LoginDialog::LoginDialog(QWidget *parent, QString name, WFlags f) :
	QDialog(parent, name, TRUE, f) {

	if(::debuglevel & DEBUGLEVEL_CONSTRUCTOR)
		cerr << "LoginDialog constructor called." << endl;

// create the GUI components
	username = new QComboBox(true, this);
	username->setAutoResize(false);
	username->setMaxCount(15);
	username->insertItem("Scoobi_FreeBSD");
	username->insertItem("MagnumPI_1970");
	username->insertItem("hangman_bot");
	username->setAutoCompletion(true);
	password = new QLineEdit(this);
	password->setEchoMode(QLineEdit::Password);
	chat_server = new QComboBox(false, this);
	chat_server->setAutoResize(false);
	chat_server->setMaxCount(10);
	chat_server->insertItem("cs1.chat.yahoo.com");
	chat_server->insertItem("cs2.chat.yahoo.com");
	chat_server->insertItem("cs3.chat.yahoo.com");
	chat_server->insertItem("cs4.chat.yahoo.com");
	chat_server->insertItem("cs5.chat.yahoo.com");
	chat_server->insertItem("cs6.chat.yahoo.com");
	chat_server->insertItem("cs7.chat.yahoo.com");
	chat_server->insertItem("cs8.chat.yahoo.com");
	chat_server->insertItem("cs9.chat.yahoo.com");
	chat_server->setCurrentItem(1);
	chat_server->setAutoCompletion(true);

	chat_port = new QLineEdit(this);
	chat_port->setFocus();
	QString buf = QString::number(CHAT_PORT_DEFAULT);
	chat_port->setValidator(
		new QIntValidator(CHAT_PORT_MIN, CHAT_PORT_MAX, this));
	chat_port->insert(buf);

	goto_room_button = new QPushButton("Go!", this);

	goto_user_button = new QPushButton("Go!", this);

	goto_room = new QComboBox(true, this);
	goto_room->insertItem("Programming:2");
	goto_room->insertItem("Programming:1");
	goto_room->insertItem("Programming");
	goto_room->insertItem("Lobby:1");
	goto_room->insertItem("Lobby:2");
	goto_room->insertItem("Lobby");
	goto_room->insertItem("Linux, FreeBSD, Solaris");
	goto_room->setAutoCompletion(true);
	goto_user = new QComboBox(true, this);
	goto_user->setAutoCompletion(true);
	for(int i=0; i<6; i++) {
		labels[i] = new QLabel(login_dialog_labels[i], this);
	}
	cancel = new QPushButton("Cancel", this);

	setTabOrder(username, password);
	setTabOrder(password, chat_server);
	setTabOrder(chat_server, chat_port);
	setTabOrder(chat_port, goto_room);
	setTabOrder(goto_room, goto_room_button);
	setTabOrder(goto_room_button, goto_user);
	setTabOrder(goto_user, goto_user_button);
	setTabOrder(goto_user_button, cancel);

	goto_room_button->setDefault(true);


// layout the widget components
	int max_label_width = 0;
	for(int i=0; i<6; i++) {
		labels[i]->move(5, (i*(username->sizeHint().height()+10))+10);
		labels[i]->resize(labels[i]->sizeHint());
		int j;
		if((j=labels[i]->sizeHint().width()) > max_label_width)
			max_label_width = j;
	}
	max_label_width += 3;
	username->move(max_label_width, 5);
	username->resize(200, username->sizeHint().height());
	password->move(max_label_width, labels[1]->y());
	password->resize(200, password->sizeHint().height());
	chat_server->move(max_label_width, labels[2]->y());
	chat_server->resize(200, chat_server->sizeHint().height());
	chat_port->move(max_label_width, labels[3]->y());
	chat_port->resize(75, chat_port->sizeHint().height());
	goto_room_button->resize(goto_room_button->sizeHint());
	goto_room_button->move(max_label_width + 210 - goto_room_button->width(),
		labels[4]->y());
	goto_user_button->resize(goto_user_button->sizeHint());
	goto_user_button->move(max_label_width + 210 - goto_user_button->width(),
		labels[5]->y());
	goto_room->move(max_label_width+5, labels[4]->y());
	goto_room->resize(200 - goto_room_button->width(),
		goto_room->sizeHint().height());
	goto_user->move(max_label_width+5, labels[5]->y());
	goto_user->resize(200 - goto_user_button->width(),
		goto_user->sizeHint().height());
	cancel->resize(cancel->sizeHint());
	cancel->move((username->x()+username->width()-cancel->width())/2,
		labels[5]->y()+labels[5]->height() + 70);
	resize(username->x()+username->width(), cancel->y()+cancel->height());
	setFixedSize(size());

	// set up connections
	connect(cancel, SIGNAL(clicked()), SLOT(reject()));
	connect(goto_room_button, SIGNAL(clicked()), SLOT(handleGotoUserClicked()));
	connect(goto_user_button, SIGNAL(clicked()), SLOT(handleGotoUserClicked()));
	password->setFocus();	// password has initial focus

	goto_room_clicked= true;
}

LoginDialog::~LoginDialog() {
	if(::debuglevel & DEBUGLEVEL_DESTRUCTOR)
		cerr << "LoginDialog destructor called." << endl;
	delete username;
	delete password;
	delete chat_server;
	delete goto_room_button;
	delete goto_user_button;
	delete goto_room;
	delete goto_user;
	for(int i=0; i<6; i++) delete labels[i];
	delete cancel;
}

void setParam(ostream &os, const char *name, const QStringList &list) {
	int i=0;
	for(QStringList::ConstIterator s=list.begin(); s!=list.end(); ++s, ++i) {
		os << name << '[' << i << "] = " << *s << endl;
	}
}

void getParam(istream &is, const char *name, QStringList &list) {
	int i, last_i=-1; 
	char *cp;
	char *cp2;
	streampos old_pos = is.tellg();
	while(is) {
		old_pos = is.tellg();
		char buf[1024];
		memset(buf, 0, 1024);
		is.getline(buf, 1024);	// read in candidate for list
		if(!is || strncasecmp(name, buf, strlen(name)) != 0) {
			break;
		}
		cp = buf;
		while(*cp && *cp != '[') cp++;
		cp2 = cp;
		while(*cp && *cp != ']') cp++;
		i=(int)strtol(cp, &cp2, 10);	// get index
		if(cp == cp2 || (i != last_i+1)) {	// nothing read
			break;
		}
		last_i = i;
		while(*cp2 && *cp2 != '=') cp++;
		cp2++;	// skip leading whitespace
		list += QString(cp2);
	}
	is.seekg(old_pos);	// reset stream to before error occurred
}

void LoginDialog::saveState(ostream &os) const {
	setParam(os, "Username", getUserNames());
	setParam(os, "ChatServer", getChatServers());
	setParam(os, "GotoRoom", getGotoRooms());
	setParam(os, "GotoUser", getGotoUsers());
}

void LoginDialog::restoreState(istream &is) {
	QStringList list;
	getParam(is, "Username", list); if(list.count()) setUserNames(list);
	getParam(is, "ChatServer", list); if(list.count()) setChatServers(list);
	getParam(is, "GotoRoom", list); if(list.count()) setGotoRooms(list);
	getParam(is, "GotoUser", list); if(list.count()) setGotoUsers(list);
}

void LoginDialog::handleGotoRoomClicked() {
	goto_room_clicked = true;
	accept();
}

void LoginDialog::handleGotoUserClicked() {
	goto_room_clicked = false;
	accept();
}

