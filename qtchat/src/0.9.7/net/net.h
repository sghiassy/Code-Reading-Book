#ifndef _NET_H
#define _NET_H

#include "chatmain.h"
#include "chatterslist.h"
#include "debugwidget.h"
#include "mainwidget.h"
#include "menubar.h"
#include "misc.h"	// AppSignalType
#include "prototypes.h"
#include "util.h"
#include <sys/types.h>
#include <ctype.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <pwd.h>
#include <stdlib.h>
extern "C" {
#include <string.h>
}
#include <stdio.h>
#include <sys/file.h>
#include <sys/utsname.h>

#include <qapplication.h>
#include <qlist.h>
#include <qobject.h>
#include <qsocketnotifier.h>

#include "chatter.h"
#include "chatmain.h"
#include "debug.h"
#include "qtchat.h"

#define SERVER "cs4.chat.yahoo.com"
#define CHAT_PORT_DEFAULT 8001
#define CHAT_PORT_MIN 8000
#define CHAT_PORT_MAX 8999
#define DEFAULT_SOCK_TIMEOUT 20

class MainWidget;	// forward declaration

class YahooNetConnection: public QObject {
	Q_OBJECT
	public:
		enum ConnectionState {
			CS_INIT,
			CS_NORMAL,
			CS_LOGIN,
			CS_LOGOUT,
			CS_JOIN
		};
		static const char *toString(ConnectionState cs) {
			switch(cs) {
				case CS_INIT:
					return "Init";
					break;
				case CS_NORMAL:
					return "Normal";
					break;
				case CS_LOGIN:
					return "Login";
					break;
				case CS_LOGOUT:
					return "Logout";
					break;
				case CS_JOIN:
					return "Join";
					break;
				default:
					return "(Unknown)";
			}
		};

		YahooNetConnection(
			QObject *parent,
			const char *username,
			const char *password,
			const char *server=SERVER,
			int port=CHAT_PORT_DEFAULT,
			const char *initial_x=0,
			bool initial_room=true,
			ChattersListWidget *listbox=0);
		YahooNetConnection(const YahooNetConnection &ync);
		~YahooNetConnection();

		char *SearchBufferForKey (const char *Buffer, size_t BuffSize, const char *Key, size_t KeySize);
		int displayMessage (char *incomingData, unsigned long pktType, unsigned short pktLen, unsigned short pktFlag);
		int processData (char *recvbuffer, int numbytes);
		int YahooSendEmote (const char *roomname, const char *msg);
		int YahooSendText (const char *roomname, const char *msg);
		int YahooSendPM (const char *who, const char *msg, bool verbose=true, bool qtchat_packet=false);
		int  pass2cookie(const char *user, const char *passwd);
		int linkto (char *hostname, int cport);
		int SocketWrite( int s, const char *data, int len);
		int SocketRead( int s, char *data, int len, int timeout);

		int cleanup(char *says);
		bool isValid() const;
		int login (void);
// ABJ 10/01/1999 04:10 - keepalive accessor method added
		bool isKeepAlive() const {
			return keepalive;
		}
		ConnectionState getState() const {
			return conn_state;
		}

	public slots:
		void parseInput(QString sendline, bool verbose=true);
		int join(const char *roomname);
		int goto_user(const char *user);
		void setApp(QApplication *a) {
			app = a;
		}
// ABJ 10/01/1999 04:10 - keepalive mutator method added
		void setKeepAlive(bool enable) {
			keepalive = enable;
		}
// ABJ 10/04/1999 10:52 - added accessor method to get current room
	QString getCurrentRoom() const {
		return current_room;
	}
// ABJ 10/04/1999 18:00 - added method to set room name
	void setCurrentRoomName(QString roomname) {
		if(!roomname.isNull()) {
			if(current_room) delete current_room;
			current_room = strdup((const char *)roomname);
		} else {
			if(::debuglevel & DEBUGLEVEL_WARNING)
				cerr << "YahooNetConnection::setCurrentRoomName(): null argument... name not set." << endl;
		}
	}
// ABJ 10/16/1999 15:39 - added method to get name of person logged in
	const char *getUser() const {
		if(isValid())
			return username;
		else
			return 0;
	}

// ABJ 10/16/1999 16:20 - added method to get connection that has a 'me' logged in
	static YahooNetConnection* getMeConnection(const Chatter &me) {
		for(int i=0; i<conn_list_size; i++) {
			if(conn_list[i] && conn_list[i]->isValid() && Chatter(conn_list[i]->getUser()) == me)
				return conn_list[i];
		}
		return 0;
	}

	private slots:
		void handleQtChatPacket(const char *data, size_t len);
		void handleSocketReadEvent(int s);	// socket s has data to read
		void handlePingTimerTimeout() {	// handles timeouts of ping_timer
			YahooPing();
			emit printChatMessage("*** ping emitted ***");
		}
		void setState(ConnectionState s) {
			bool changed = (s != conn_state);
			conn_state = s;
			if(changed)
				emit stateChanged(conn_state);
		}

	signals:
		void printChatMessage(QString msg);
		void printChatMessage(QString msg, Chatter *ch, ChatEntryType t=CET_SAY_INCOMING);
		// emitted when any data is transferred via connection
		void dataTransferred(const unsigned char *_data, int num_data, bool incoming);
		void chatterEnters(Chatter *chatter);
		void chatterLeaves(const Chatter *chatter);
		void appSignal(AppSignalType s);	// global signals from this object
			// should be moved to core
		void roomNameChanged(QString name);
		void stateChanged(ConnectionState s);
		void activateMenuItem(MenuItem m);
		void saveText(const char *filename);

	protected:
		int sock;		// connection socket
		ConnectionState conn_state;	// connection state
		char *chat_server;	// hostname of Yahoo chat server
		int chat_port;		// TCP/IP port to which to communicate
		QSocketNotifier *sock_notifier;
		char *username;		// username of person logged in
		char *current_room;	// current room
		char *cooky;
		QApplication *app;	// pointer to this application... needed for event processing

	private:
		ChattersListWidget *listbox;
		MainWidget *main_widget;
		ChatWidget *chat_widget;
		DebugWidget *debug_widget;
		void init(
			const char *_username,
			const char *_password,
			const char *_server,
			int _port,
			const char *_initial_x,
			bool _initial_room,
			ChattersListWidget *listbox);
		void destroy();

		void close_socket(){	// close this connection socket if open
			close_socket(sock);
			sock = 0;
		}
		void close_socket(int s);	// close socket if open
		bool isSocketGood() {	// return socket valid indicator
			return isSocketGood(sock);
		}
		bool isSocketGood(int s);	// return socket valid indicator
		void help();

// ABJ 10/01/1999 04:10 - added keepalive variable
		bool keepalive;
// END ABJ 10/01/1999 04:10 - added keepalive variable

// ABJ 10/01/1999 07:38 - loopy fix for autologout
		QTimer *ping_timer;	// 10-minute timer for pinging Yahoo
		int YahooPing();	// function called every 10 min
// END ABJ 10/01/1999 07:38 - loopy fix for autologout

// ABJ 10/16/1999 15:54 - added ability to keep track of all existing connections
		static YahooNetConnection *conn_list[256];
		static int conn_list_size;

		void addToConnectionList() {
			if(conn_list_size == 256) {
				cerr << "YahooNetConnection::addToConnectionList(): out of space." << endl;
				return;	// this should be a fatal error, but not at the moment
			}
			for(int i=0; i<conn_list_size; i++)
				if(conn_list[i] == this) return;
			conn_list[conn_list_size++] = this;
		}

		void removeFromConnectionList() {
			for(int i=0; i<conn_list_size; i++)
				if(conn_list[i] == this) {
					conn_list[i] = conn_list[--conn_list_size];
					return;
				}
			cerr << "YahooNetConnection::removeFromConnectionList(): connection not found." << endl;
			return;	// this should be a fatal error, but not at the moment
		}
// END ABJ 10/16/1999 15:54 - added ability to keep track of all existing connections
};

#endif	// _NET_H
