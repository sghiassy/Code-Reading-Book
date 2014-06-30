#include "net.h"
#include "qtchat.h"
#include "util.h"
#include <iostream.h>
#include <iomanip.h>
#include <math.h>
#include <unistd.h>		// close(), usleep(), exect(), environ declaration
#include <strstream.h>
#include <sys/time.h>	// struct timeval declaration

// Constants
const char *QTCHAT_PACKET_HEADER = "\x01\x02\x03";
const size_t QTCHAT_PACKET_HEADER_LEN = sizeof(QTCHAT_PACKET_HEADER)-1;
const char *tags[] = {
	"font",
	"/font",
	"url",
	"/url",
	"alt",
	"/alt",
	0
};

// instantiate  static vars
YahooNetConnection* YahooNetConnection::conn_list[256] = {0};
int YahooNetConnection::conn_list_size = 0;

// debug code
void printData(const char *d, int n) {
	char str1[49];			// 16 bytes * 3 chars/byte + '\0' : holds hex string
	char str2[17];			// 16 bytes * 1 char/byte + '\0'	: holds char string
			
	int i;
	int n_ceiling = ((int)ceil((double)n/16.0))*16;
	for(i=0; i<n_ceiling; i++) {	// parse data
		int j = i&15;	// get lower 4 bits of string index as substr index
		if(j == 0) {	// clear our temp strings at i=0, 16, 32...
			memset(str1, 0, 49);
			memset(str2, 0, 17);
		}
		char *cp = str1+(j)*3;	// point cp at place to insert hex string in str1
		if(i<n)
			sprintf(cp, "%02X ", ((int)d[i])&0xff);	// insert hex val of char in str1
		else
			strcpy(cp, "   ");
		if(i<n && isprint((char)d[i])) {	// insert char or space into str2
			str2[j] = (char)d[i];
		} else {
			str2[j] = ' ';
		}
		if((i+1)%16 == 0) {	// print line iff i=15,31,47...
			cerr << str1 << " : ";
			cerr << str2 << endl;;
		}
	}
}

YahooNetConnection::YahooNetConnection
(
	QObject *parent,
  const char *_username,
  const char *_password,
  const char *_server,
  int _port,
  const char *_initial_x,
  bool _initial_room,
  ChattersListWidget *_listbox) :
  QObject(parent), main_widget(0), chat_widget(0), debug_widget(0) {

  if(::debuglevel & DEBUGLEVEL_CONSTRUCTOR)
 	 cerr << "YahooNetConnection constructor called." << endl;

	 // set application cursor to hourglass QCursor prev_cursor;
	QCursor prev_cursor;
	if(qApp->mainWidget()) {
		prev_cursor = qApp->mainWidget()->cursor();
		qApp->mainWidget()->setCursor(waitCursor);
	}
  init(_username, _password, _server, _port, _initial_x, _initial_room, _listbox);
   // restore cursor
	if(qApp->mainWidget())
	  qApp->mainWidget()->setCursor(prev_cursor);
  
	if(!(_username && _password && _server) && (::debuglevel & DEBUGLEVEL_WARNING))
		cerr << "YahooNetConnection: Warning: Unable to init connection... null object created." << endl;

	addToConnectionList();	// add connection object to list of objects

  	// These are no longer needed as the constructor is exiting
	main_widget = 0;
	chat_widget = 0;
	debug_widget = 0;
}



YahooNetConnection::YahooNetConnection(const YahooNetConnection &ync) 
{
	if(::debuglevel & DEBUGLEVEL_COPYCONSTRUCTOR)
		cerr << "YahooNetConnection copy constructor called (empty)." << endl;
	addToConnectionList();	// add connection object to list of objects

}



void YahooNetConnection::init(
	const char *_username,  const char *_password,  
	const char *_server,  int _port,
	const char *_initial_x, bool _initial_room,
	ChattersListWidget *_listbox) {

	char      recvbuffer[4096];
  
	username = _username ? strdup(_username) : 0;
	chat_server = _server ? strdup(_server) : 0;
	chat_port = _port;
	current_room = 0;
	cooky = 0;
	app = 0;
	sock = 0;
	sock_notifier = 0;
	ping_timer = 0;
	listbox = _listbox;

  	// These are for when this object is parsing packets received while it is
	// being constructed (i.e. in the LOGIN state), as no connections have
	// been set up yet and emit won't work.
	main_widget = dynamic_cast<MainWidget *>(parent());
	chat_widget = main_widget ? main_widget->getChatWidget() : 0;
	debug_widget = main_widget ? main_widget->getDebugWidget() : 0;

// ABJ 10/01/1999 04:15 - set keepalive variable to 'enable' on instantiation
  keepalive = 1;	// enable bye default for now
// END ABJ 10/01/1999 04:15 - set keepalive variable to 'enable' on instantiation
	conn_state = CS_INIT;

  // check args, create null object if bad
  
  if(!username || !chat_server || chat_port <= 0) return;
 
  if(pass2cookie(username, _password)) 
   {
	  if(::debuglevel & (DEBUGLEVEL_ERROR | DEBUGLEVEL_NETWORK))
		  cerr << "YahooNetConnection: Error getting cookie from Yahoo." << endl;
     return;
   }

  // borrow recvbuffer[] for a sec...
  
  snprintf(recvbuffer, 4096, "Yahoo Chat - %s", username);
  emit printChatMessage(recvbuffer);
  
  // end borrow

  if (!isSocketGood(sock=linkto(chat_server, chat_port))) 
  {
	if(::debuglevel & (DEBUGLEVEL_ERROR | DEBUGLEVEL_NETWORK))
		cerr << "Error creating socket to chat server." << endl;
   return;
  }

  // borrowing recvbuffer[] again...
  
  snprintf(recvbuffer, 4096, "Logging in as %s...", username);
  fputs(recvbuffer, stderr);
  
  // end borrow
  
	// set up the event handler to get the data from Yahoo
	sock_notifier = new QSocketNotifier(sock, QSocketNotifier::Read, this);	// monitor the socket for read data
	connect(sock_notifier, SIGNAL(activated(int)), SLOT(handleSocketReadEvent(int)));

  if (login() != 0) 
   {
    fputs("Login Failed.\n", stdout);
    return;
   }
	// here we don't actually know if we've logged in yet... we need to
	// wait for the packet from Yahoo saying we have.  This would basically
	// require a state machine... the object has to know we're in state x
	// right after sending login packet... then wait for packet, move to
	// state y,,,

	// wait for return packet from Yahoo
	int waittime = 10000;	// 10 second wait for Yahoo packet
	struct timespec sleeptime = {0,100000000};	// 0.1 seconds
	struct timespec actualtime = {0,0};	// time actually slept
	while(conn_state == CS_LOGIN && waittime > 0) {
		qApp->processEvents(100);	// look for event from the socket notifier
		nanosleep(&sleeptime, &actualtime);	// sleep for 0.1 seconds
		waittime -= (sleeptime.tv_nsec-actualtime.tv_nsec)/1000000;	// decrement time slept
		fputc('.', stderr);
	}
	// Here we've exited the LOGIN state... let's re-enter it to
	// facilitate join()
	conn_state = CS_LOGIN;
	fputc('\n', stderr);
	if(waittime <= 0) {	// failed to login
		cerr << "Error: unable to login... aborting." << endl;
		conn_state = CS_INIT;
		return;
	}

	// borrowing recvbuffer[] again...
//	snprintf(recvbuffer, 4096,
//		_initial_room ? "Attempting to join room %s...\n" : "Attempting to goto user %s...\n",
//		_initial_x);
//	fputs(recvbuffer, stdout);
  // end borrow

	if(_initial_room) {
		cerr << "Attempting to join " << _initial_x << "..." << flush;
		if(join(_initial_x) != 0) {	// need to do something here
			cerr << endl << "join(" << _initial_x << ") failed." << endl;
			if(sock_notifier)
				delete sock_notifier;
			sock_notifier = 0;
			ping_timer = 0;
			return;	// error
		}
		int attempts = 10;
		qApp->processEvents(100);	// look for packet that changes state
		while(attempts && conn_state == CS_LOGIN) {	// keep trying until we get in
			sleep(1);
			qApp->processEvents(100);	// look for packet that changes state
			attempts--;
		}
		cerr << " done." << endl;
	} else {	// goto user
		if(goto_user(_initial_x) != 0) {	// error
			if(sock_notifier)
				delete sock_notifier;
			sock_notifier = 0;
			ping_timer = 0;
			return;	// error
		}
	}
	ping_timer = new QTimer();
	connect(ping_timer, SIGNAL(timeout()), SLOT(handlePingTimerTimeout()));
	ping_timer->start(600000, false);	// (10 min)(60s/min)(1000ms/s), continuous
}

YahooNetConnection::~YahooNetConnection() 
{
  if(::debuglevel & DEBUGLEVEL_DESTRUCTOR)
	  cerr << "YahooNetConnection destructor called." << endl;
  destroy();
  removeFromConnectionList();	// remove object from connections list
}








void YahooNetConnection::destroy() 
{
// ABJ 09/29/1999 15:53 - added setting members to invalid values after delete
	if(sock_notifier) {
		delete sock_notifier;
		sock_notifier = 0;
	}
	if(ping_timer)
		delete ping_timer;

	if(sock) {
		close(sock);
		sock = -1;
		emit printChatMessage("*** Disconnected from Yahoo. ***", 0, CET_SYSTEM_MESSAGE);
	}
	if(username) {
		delete username;
		username = 0;
	}
	if(chat_server) {
		delete chat_server;
		chat_server = 0;
	}
	if(current_room) {
		free(current_room);
		current_room = 0;
	}
// END ABJ 09/29/1999 15:53 - added setting members to invalid values after delete
}


void YahooNetConnection::handleSocketReadEvent(int s) {
	char incomingData[4096];
	int numbytes = 0;

	if(isSocketGood(s)) {
		memset(incomingData, 0, 4096);
		numbytes = SocketRead(s, incomingData, sizeof(incomingData), DEFAULT_SOCK_TIMEOUT);
		if (numbytes <= 0) {
			if(::debuglevel & (DEBUGLEVEL_ERROR | DEBUGLEVEL_NETWORK))
				cerr << "YahooNetConnection::handleSocketReadEvent(): socket read error." << endl;
			close_socket();
			sock_notifier->setEnabled(false);
			return;	/* Socket closed */
		}
		int error_code = processData(incomingData, numbytes);
		if(error_code == -1) {	
			// process the data ourselves first
			if(::debuglevel & (DEBUGLEVEL_ERROR | DEBUGLEVEL_NETWORK))
				cerr << "YahooNetConnection::handleSocketReadEvent(): data processing error." << endl;
			return;
		}
	}
}

int parseCommand(char *input, char **cmd, char **arg1, char **arg2) {

	*arg1 = *arg2 = (char *) 0;

	//Store pointer to first argument.

	*cmd = input;
	while ((*input != ' ') && (*input != '\n') && (*input != '\0'))
		input++;
	if(!(*input)) return 1;
	*input++ = '\0';	// terminate first word with NULL

	// skip over any whitespace
	while ((*input == ' ') || (*input == '\n') || (*input == '\0'))
		input++;
    
	if (*input) {
		*arg1 = input;
		// skip to end of second word
		while ((*input != ' ') && (*input != '\n') && (*input != '\0'))
			input++;
 
		if(!(*input)) return 1;
		*input++ = '\0';	// terminate second word with NULL

		// skip over any whitespace
		while ((*input == ' ') || (*input == '\n') || (*input == '\0'))
			input++;

		if (*input) {
			*arg2 = input;
//	let the rest of the words (3-n) be arg2
//			while (*input != '\n' && *input != '\0')
//				input++;
//
//			*input++ = '\0';
		}
	}
	return 1;
}

int YahooNetConnection::SocketWrite(int s, const char *data, int len) {
	int num_chars = -1;	// number of characters sent

	if(isSocketGood(s)) {
		if((num_chars = send(s, data, len, 0)) < 0) {
			if(::debuglevel & (DEBUGLEVEL_ERROR | DEBUGLEVEL_NETWORK))
				cerr << "YahooNetConnection::SocketWrite(): Error writing to socket." << endl;
			perror("YahooNetConnection::SocketWrite()");
		}
	} else {
		if(::debuglevel & (DEBUGLEVEL_ERROR | DEBUGLEVEL_NETWORK))
			cerr << "YahooNetConnection::SocketWrite(): bad socket." << endl;
	}
	const unsigned char *d = (const unsigned char *)data;
	if(debug_widget)
		debug_widget->dataTransferred(d, num_chars, false);
	else
		emit dataTransferred(d, num_chars, false);
	return num_chars;
}












int YahooNetConnection::SocketRead(int s, char *data, int len, int timeout) {
	fd_set sockset;
	int return_val = -1;
	struct timeval tv, *tvp=0;

	// check arguments
	if(!isSocketGood(s) || !data || len <= 0) {
		if(::debuglevel & (DEBUGLEVEL_ERROR | DEBUGLEVEL_NETWORK))
			cerr << "YahooNetConnection::SocketRead(): Bad argument(s)." << endl;
		return -1;
	}

	FD_ZERO(&sockset);
	FD_SET(s, &sockset);
	if(timeout >= 0) {
		tv.tv_sec = timeout;
		tv.tv_usec = 0;
		tvp = &tv;
	}
	return_val = select(s+1, &sockset, 0, 0, tvp);

	if(return_val == 0) {
		if(::debuglevel & (DEBUGLEVEL_WARNING | DEBUGLEVEL_NETWORK))
			cerr << "YahooNetConnection::SocketRead(): Read timed out." << endl;
		return -1;
	} else if(return_val < 0) {
		if(::debuglevel & (DEBUGLEVEL_ERROR | DEBUGLEVEL_NETWORK))
			cerr << "YahooNetConnection::SocketRead(): Read failed." << endl;
		return -1;
	}
	return_val = read(s, data, len);

	if(return_val < 0) {
		if(::debuglevel & (DEBUGLEVEL_ERROR | DEBUGLEVEL_NETWORK))
			cerr << "YahooNetConnection::SocketRead(): Read failed." << endl;
		return -1;
	}

	if(return_val > len) {
		if(::debuglevel & (DEBUGLEVEL_ERROR | DEBUGLEVEL_NETWORK))
			cerr << "YahooNetConnection::SocketRead(): Overflowed buffer." << endl;
		return -1;
	}
	
	const unsigned char *d = (const unsigned char *)data;
	if(debug_widget)
		debug_widget->dataTransferred(d, return_val, true);
	else
		emit dataTransferred(d, return_val, true);

	return return_val;
}

int YahooNetConnection::pass2cookie(const char *user, const char *passwd) {
	char loginString[1024];
	char *junk;
	static char returnData[2048];
	int cookie_socket;

	if(!isValid()) return -1;

	for(int i=0; i<=14; i++) {	// loop through available cookie servers
		char str[16];
		if(!i)
			strncpy(str, "edit.yahoo.com", 16);
		else
			sprintf(str, "e%1d.yahoo.com", i);
		if (!isSocketGood(cookie_socket = linkto(str, 80)))
			continue;	// try another server if can't connect

		snprintf(loginString, 1024, "GET /config/ncclogin?.src=bl&login=%s&passwd=%s&n=1 HTTP/1.0\nUser-Agent: Yawnhoo\nHost: edit.my.yahoo.com\nCookie:\n\n", user, passwd);
		if(SocketWrite(cookie_socket, loginString, strlen(loginString)) < 0) {
	      if(::debuglevel & (DEBUGLEVEL_ERROR | DEBUGLEVEL_NETWORK))
			  cerr << "YahooNetConnection::pass2cookie(): Error writing to socket" << endl;
			close_socket(cookie_socket);	// close socket
			continue;	// try another server
		}
		if (SocketRead(cookie_socket, returnData, 2048, DEFAULT_SOCK_TIMEOUT) <= 0) {
	      if(::debuglevel & (DEBUGLEVEL_ERROR | DEBUGLEVEL_NETWORK))
			  cerr << "YahooNetConnection::pass2cookie(): error reading from socket." << endl;
			close_socket(cookie_socket);
			continue;	// try another server
		}
		junk = returnData;
		for(int loopcount = 0; loopcount < 3; loopcount++) {
			junk = strstr(junk, "Set-Cookie:");
			if(!junk || *junk == '\0') {
				  cerr << "YahooNetConnection::pass2cookie(): Unable to find suitable cookie..." << endl;
				close_socket(cookie_socket);
				continue;	// try another server
			}
			char *cp = junk;
			while(*cp && *cp != ';') cp++;
			*cp = '\0';
			while(junk && *junk && *junk != 'v') junk++;
			if(*junk != 'v' || *(junk+1)!='=') {
				  cerr << "YahooNetConnection::pass2cookie(): Unable to find suitable cookie..." << endl;
				  junk = cp+1;
			} else {
				cooky = junk;
				printf("Cookie found!: %s\n", cooky);
				break;
			}
		}
		return 0;
	}
	if(::debuglevel & (DEBUGLEVEL_ERROR | DEBUGLEVEL_NETWORK))
		cerr << "Unable to locate suitable cookie server." << endl;
	return -1;
}



char *
YahooNetConnection::SearchBufferForKey(const char *Buffer, size_t BuffSize,
                                       const char *Key,  size_t KeySize)
{
	const char *cp = Buffer;
	// check arguments
	if ( !Buffer || !Key || KeySize<=0 || BuffSize<=0 || KeySize > BuffSize ) {
		return 0;
	}
	while(cp <= (Buffer + BuffSize - KeySize)) {	// loop through buffer
		if (memcmp(cp, (char *)Key, KeySize) == 0)
			// found key
			return const_cast<char *>(cp);
		cp++;	// go to next buffer location
	}
	// not found
	return 0;
}







int YahooNetConnection::processData(char *recvbuffer, int numbytes)
{
  unsigned long	  pktType = 0;
  unsigned short  pktFlag = 0;
  unsigned short  pktLen = 0;
  char		  str[] = "CHT\x00\x00\x01";
  char		  *point2header;

  if(!isValid()) {
		if(::debuglevel & (DEBUGLEVEL_ERROR | DEBUGLEVEL_NETWORK))
			cerr << "YahooNetConnection::processData(): invalid socket." << endl;
		 return -1;
	}
 
  if(numbytes <= 0 || numbytes > 4096 || recvbuffer == 0) {
		// error
		if(::debuglevel & (DEBUGLEVEL_ERROR | DEBUGLEVEL_NETWORK))
			cerr << "YahooNetConnection::processData(): invalid argument." << endl;
		return -1;
	}

	
	while((point2header = SearchBufferForKey(recvbuffer, numbytes, str, 6))) {
		if(point2header >= (recvbuffer + numbytes)) {
			if(::debuglevel & (DEBUGLEVEL_ERROR | DEBUGLEVEL_NETWORK))
				cerr << "YahooNetConnection::processData(): buffer overrun." << endl;
			return -2;
		}
		pktType = ntohl(*((unsigned long *)(point2header + 7)));
		pktFlag = ntohs(*((unsigned short *) (point2header + 11)));
		pktLen = ntohs(*((unsigned short *) (point2header + 13)));

		point2header[pktLen+15] = '\x00';	// terminate subpacket w/ null
		displayMessage(point2header + 15, pktType, pktLen, pktFlag);	// handle packet

		// adjust buffer vars for next loop iteration
		numbytes -= (pktLen+15);
		recvbuffer += (pktLen+15);
	}
	return 0;
}





int YahooNetConnection::displayMessage(char *incomingData,unsigned long pktType,
                                       unsigned short pktLen,unsigned short pktFlag)
{
	// Check connection
	if(!isValid()) return -1;

	// Check arguments
	if(incomingData == 0) {
		if(::debuglevel & (DEBUGLEVEL_ERROR | DEBUGLEVEL_NETWORK))
			cerr << "YahooNetConnection::DisplayMessage(): bad argument." << endl;
		return -1;
	}
 
	char *junk=NULL;
	char *who=NULL;
	char *says=NULL;
	char delimiters[] = {(char)0xc0, (char)0x80, (char)0x00};
	char buffer[1024]={0};
 
	switch (pktType) {
		case 0x01:
		{
			if(conn_state == CS_LOGIN) {	//	 we're in the process of logging in
				char *junk=NULL;
				char *who=NULL;
				if(main_widget)
					main_widget->handleAppSignal(APP_LOGIN);
				else
					emit appSignal(APP_LOGIN);
				junk = strtok (incomingData, delimiters ); 
				who = strtok (NULL, delimiters ); 	
				Chatter chatter(who);
				if(chat_widget)
					chat_widget->addEntry("You have successfully logged on.", &chatter, CET_LOGIN);
				else
					emit printChatMessage("You have successfully logged on.", &chatter, CET_LOGIN);
				conn_state = CS_NORMAL;	// transition state to NORMAL
			} else {	// login packet received when NetConnection not in LOGIN state
				cerr << "YahooNetConnection::displayMessage(): state machine error on login." << endl;
			}
			break;
		}	// end case 0x01

		case 0x02:
		{
			char *junk=NULL;
			char *who=NULL;
			// ABJ 10/01/1999 04:34 - don't emit logout signal if keepalive
			if(!keepalive) emit appSignal(APP_LOGOUT);
			// END ABJ 10/01/1999 04:34 - don't emit logout signal if keepalive
			junk = strtok (incomingData, delimiters ); 
			who = strtok (NULL, delimiters ); 	
			Chatter chatter(who);
			// ABJ 10/01/1999 04:44 - relogin if specified
			if(keepalive) {	// try to relog in with same params as current state
				if(login() != 0) {
					cerr << endl << "login() failed." << endl;
					break;
				}
				int attempts = 10;
				qApp->processEvents(100);	// look for packet that changes state
				cerr << "Attempting to re-login" << flush;
				while (attempts && conn_state == CS_LOGIN) {	// loop until we log in
					fputc('.', stderr);
					sleep(1);
					qApp->processEvents(100);	// look for packet that changes state
					attempts--;
				}
				cerr << " done." << endl;

				if(join(current_room) != 0) {	// need to do something here
					cerr << endl << "join(" << current_room << ") failed." << endl;
					break;
				}
				attempts = 10;
				qApp->processEvents(100);	// look for packet that changes state
				cerr << "Attempting to join " << current_room << flush;
				while(attempts && conn_state == CS_JOIN) {	// keep trying until we get in
					fputc('.', stderr);
					sleep(1);
					qApp->processEvents(100);	// look for packet that changes state
					attempts--;
				}
				cerr << " done." << endl;
			} else
				emit printChatMessage("You have successfully logged off.", &chatter, CET_LOGOUT);
			// END ABJ 10/01/1999 04:44 - relogin if specified
			break;
		}





		case 0x11:	// room enter event
		{
			char *room=NULL;
			char *junk=NULL;
			char *users=NULL;
			char BigStr[2048];
			char delimiters[]    = {(char)0xc0, (char)0x80, (char)0x00};
			char *chatter =0;

			switch(pktFlag) {
				case 0x00:    // normal room enter event
				{
//					cerr << endl;
//					printData(incomingData, pktLen);
					room = strtok (incomingData, delimiters );
					junk = strtok (NULL, delimiters );
//					if(!junk) {
//						cerr << "Error parsing 2nd field of enter packet." << endl;
//					} else {
//						cerr << "Field 2: " << junk << endl;
//					}
					junk = strtok (NULL, delimiters );
//					if(!junk) {
//						cerr << "Error parsing 3rd field of enter packet." << endl;
//					} else {
//						cerr << "Field 3: " << junk << endl;
//					}
					junk = strtok (NULL, delimiters );
//					if(!junk) {
//						cerr << "Error parsing 4th field of enter packet." << endl;
//					} else {
//						cerr << "Field 4: " << junk << endl;
//					}
					users = strtok (NULL, delimiters );
//					if(!users) {
//						cerr << "Error parsing 5th field of enter packet." << endl;
//					} else {
//						cerr << "Field 5: " << users << endl;
//					}

			chatter = strtok(users, ",\x01");
			while(chatter) {
				char name[128];
				int i;
				for(i=0; i<127 && chatter[i] && chatter[i] != 0x02; i++) {
					name[i] = chatter[i];
				}
				name[i] = '\0';
				Chatter *c = new Chatter(name);
				if(conn_state == CS_LOGIN) {
					if(listbox)
						listbox->addChatter(c);
					else {
						cerr << "Error: no ChattersListWidget passed to constructor" << endl;
					}
				} else {
					emit chatterEnters(c);
				}
				if(chat_widget)
					chat_widget->addEntry(0, c, CET_ENTER);
				else
					emit printChatMessage(0, c, CET_ENTER);
				// Break string off at first 0x01 char... new Yahoo protocol
				chatter = strtok(NULL , ",\x01");
			}
// ABJ 10/04/1999 17:59 - added code to change roomname here
					setCurrentRoomName(room);
					if(conn_state == CS_LOGIN && main_widget)
						main_widget->handleRoomNameChanged(room);
					else
						emit roomNameChanged(room);
					break;
				}

				case 1:
					if(conn_state == CS_LOGIN && chat_widget && main_widget) {
						chat_widget->addEntry("*** That Room is Full ***");
						main_widget->handleAppSignal(APP_LOGIN_FAILED_ROOM_FULL);
					} else {
						emit printChatMessage("*** That Room is Full ***");
						emit appSignal(APP_LOGIN_FAILED_ROOM_FULL);
					}
					break;

				case 0x10:    // you are already in room
				case 65503:   // this is a secure room, you must be invited
				case 65525:   // user is not online
						junk = strtok (incomingData, delimiters );
					if(conn_state == CS_LOGIN && chat_widget)
						chat_widget->addEntry(junk);
					else
						emit printChatMessage(junk);
					break;

				default:
					junk = strtok (incomingData, delimiters );
					sprintf(BigStr, "Error(%d) on enter: %s", pktFlag, junk);
					if(conn_state == CS_LOGIN && chat_widget)
						chat_widget->addEntry(BigStr);
					else
						emit printChatMessage(BigStr);
					break;
			}
			if(conn_state == CS_LOGIN || conn_state == CS_JOIN) {
				conn_state = CS_NORMAL;
			}
			break;
		}

		case 0x12:
		{
			char *junk=NULL;
			char *users=NULL;
			char delimiters[]    = {(char)0xc0, (char)0x80, (char)0x00};
			char *chatter = 0;

			junk = strtok (incomingData, delimiters ); 
			users = strtok (NULL, delimiters );
			// parse 'users' for individual chatters and tell the world
			chatter = strtok(users, ",");
			while(chatter) {
				Chatter c(chatter);
				// This is actually wrong... we should emit the 'chatterLeaves()'
				// signal and *then* have that signal handler print the message.
				emit chatterLeaves(&c);
				emit printChatMessage(0, &c, CET_LEAVE);
				chatter = strtok(0, ",");
			}
			break;
		}

		case 0x17:   // invites 
		{   
			char *where=NULL;
			char *junk=NULL;
			char *who=NULL;
			char BigStr[2048];
			char delimiters[]    = {(char)0xc0, (char)0x80, (char)0x00};

			switch(pktFlag) {
				case 0x00:     // user has been invited ..  
					who = strtok (incomingData, delimiters );
					emit printChatMessage(who);
					break;

				case 0x01:  // user has invited you 
				{
					junk = strtok (incomingData, delimiters );
					where = strtok (NULL, delimiters );
					who = strtok (NULL, delimiters );
					sprintf(BigStr, "%s invited you to join room %s", who,where);
					Chatter c(who);
					emit printChatMessage(BigStr, &c, CET_INVITE);
					break;
				}

				case 0xff:
					break;

				default:
				break;
			}
			break;
		}

		case 0x21:   // away - back 
		{
			char name[32] = {0};
			char statement[256] = {0};
			char BigStr[2048];
			char excuse[128] = {0};
			int y = 0;
			int state = 0;
			int s = 0;

			while(incomingData[y]!='\xc0') {
				name[s] = incomingData[y];
				y++;
				s++;
			}
			Chatter c(name);
			y += 2; // skip over delimiter
			while(incomingData[y] != '\xc0') {
				y++;
			}
			y += 2; // skip over delimiter

			state = incomingData[y];
			s = 0;
			if(0x31 == state && 0x30 == incomingData[y + 1]) {
				y += 4;
				while(incomingData[y] != '\0') {
					excuse[s++] = incomingData[y++];
				}
				strcat(statement, "is away... ");
				strcat(statement, excuse);
				sprintf(BigStr, "%s %s", name, statement);
				emit printChatMessage(BigStr, &c, CET_AWAY);
				return 0;
			}

			if(0x31 == state && 0x31 == incomingData[y + 1]) {
				strcat(statement, "is away... Auto-Away");
				sprintf(BigStr, "%s %s", name, statement);
				emit printChatMessage(BigStr, &c, CET_AWAY);
				return 0;
			}
			switch(state) {
				case 0x30:
					strcat(statement, "is back");
					break;

				case 0x31:
					strcat(statement, "is away... Be right back");
					break;

				case 0x32:
					strcat(statement, "is away... Busy");
					break;

				case 0x33:
					strcat(statement, "is away... Not at home");
					break;

				case 0x34:
					strcat(statement, "is away... Not at my desk");
					break;

				case 0x35:
					strcat(statement, "is away... Not in the Office..");
					break;

				case 0x36:
					strcat(statement, "is away... On the phone");
					break;

				case 0x37:
					strcat(statement, "is away... On vacation");
					break;

				case 0x38:
					strcat(statement, "is away... Out to lunch");
					break;

				case 0x39:
					strcat(statement, "is away... Stepped out");
					break;

				default:
				break;
			}

			sprintf(BigStr, "%s %s", name, statement);
			emit printChatMessage(BigStr, &c, CET_AWAY);
			break;
		}






	case 0x41:	// says
	{
	  junk = strtok (incomingData, delimiters );
	  who = strtok (NULL, delimiters ); 	
	  if(who) {
		  Chatter chatter(who);
		  says = strtok (NULL, delimiters );
		  cleanup(says);
		  emit printChatMessage(says, &chatter, CET_SAY_INCOMING);
		} else
			cerr << "YahooNetConnection::displayMessage(): parse error packet type 0x41." << endl;
	  break;
	}

	case 0x42:      // . o O ( thinks )
	{
		junk = strtok (incomingData, delimiters ); 
		who = strtok (NULL, delimiters ); 	
		if(who) {
			Chatter chatter(who);
			says = strtok (NULL, delimiters );
			cleanup(says);
			emit printChatMessage(says, &chatter, CET_SAY_INCOMING);
		} else
			cerr << "YahooNetConnection::displayMessage(): parse error packet type 0x42." << endl;
		break;
	}

	case 0x43:	// emote
	{
		junk = strtok (incomingData, delimiters ); 
		who = strtok (NULL, delimiters ); 
		if(who) {
			Chatter chatter(who);
			says = strtok (NULL, delimiters ); 
			cleanup(says);
			emit printChatMessage(says, &chatter, CET_EMOTION);
		} else
			cerr << "YahooNetConnection::displayMessage(): parse error packet type 0x43." << endl;
		break;
	}








	case 0x44:	// advertisement 
      break;


	case 0x45:	// Private Message
	{
		who = strtok ( incomingData, delimiters);
		if(who) {
			Chatter chatter(who);
			junk = strtok ( NULL, delimiters);
			says = strtok(NULL, delimiters);
			cleanup(says);

//	handle QtChat packets from PM channel
			if(memcmp(says, QTCHAT_PACKET_HEADER, QTCHAT_PACKET_HEADER_LEN)==0)
				handleQtChatPacket(
					says + QTCHAT_PACKET_HEADER_LEN,
					pktLen - QTCHAT_PACKET_HEADER_LEN - (says-incomingData));
			else
				emit printChatMessage(says, &chatter, CET_PM_INCOMING);
		} else
			cerr << "YahooNetConnection::displayMessage(): parse error packet type 0x45." << endl;
		break;
	}

      case 0x47:   // dice 
      break;

      case 0x68:   // buddy lists 
       {
        int y       = 0;
        int state   = 0;
        int z       = 0;
        int more    = 1;
        int service = 0;
        char BigStr[2048] = {0};
        char name[32]        = {0};
        char statement[256]  = {0};

      if (pktLen<2)
      {
        //None of your friends are currently online
        return 0;
      }
      while(more)
      {
        memset(name, 0, sizeof name);
        memset(statement, 0, sizeof statement);
        z = 0;
        state= incomingData[y];

        y+= 2; // skip over delimiter

        while(incomingData[y]!='\xc0')
        {
          if(incomingData[y]=='\x80')
          {
            y++;
          }
          name[z] = incomingData[y];
          y++;
          z++;
        }

        y+= 2; // skip over delimiter

        service= incomingData[y];
        y++;

        if(0x30 == state)
        {
          switch(service)
          {
            case 0x32:
              strcat(statement," leaves chat");
              break;
            case 0x34:
              strcat(statement," leaves pager");
              break;
            case 0x36:
              strcat(statement," leaves pager & chat");
              break;
            default:
              break;
          }
        }
        else
        {
          switch(service)
          {
            case 0x32:
              strcat(statement," is in chat");
              break;
            case 0x34:
              strcat(statement," is on pager");
              break;
            case 0x36:
              strcat(statement," is in chat and pager");
               break;
            default:
              break;
          }
        }

        sprintf(BigStr, "Your Buddy, %s, %s", name, statement);
		  Chatter c(name);
        emit printChatMessage(BigStr, &c, CET_BUDDY);
        if (incomingData[y] !='\xc0')
        {
          more = 0;
        }
      }
     break;     
     }

	case 0x69:
        {
         emit printChatMessage("**** You have Yahoo Mail ****", 0, CET_SYSTEM_MESSAGE);
			emit printChatMessage("*** http://mail.yahoo.com ***", Chatter::getMe(), CET_SYSTEM_MESSAGE);
         break;
        }


      case 0x70:
      break;

      case 0x80:   // grafiti whiteboard 
      break;


	default:
        {
	 snprintf(buffer, 1024, "ERROR Unknown Packet Recieved: 0x%02X",	(unsigned int)pktType);
         emit printChatMessage(buffer);
         break;
	}
      }
	return 0;
}

int YahooNetConnection::cleanup(char *says)
{
	char *dest, *src;


	if(!says) {// return bad code if null received
		if(::debuglevel & (DEBUGLEVEL_ERROR | DEBUGLEVEL_NETWORK))
			cerr << "YahooNetConnection::cleanup(): null pointer received." << endl;
		return -1;
	}
	src = dest = says;	 // init the source pointer
	while(*src) {	// loop until end of string
		switch(*src) {
		 case 0x1b:
			while(*src != 0x6d) src++;
			break;
		 case '<':	// possible HTML tag
		 {
			const char **cp = 0;
			for(cp=tags; *cp; cp++) {
				char *tmpcp = src+1;	// skip whitespace
				while(*tmpcp && isspace(*tmpcp)) tmpcp++;
				if(strncasecmp(tmpcp, *cp, strlen(*cp))==0) {
					tmpcp = src;	// save in case we don't have a valid tag
					while(*src && *src != '>') src++;	// skip tag
					if(*src)
						break;
					else
						src = tmpcp;
				}
			}
			if(*cp)
				break;
		}
		default:
			*dest = *src;
			dest++;
		}
		src++;
	}
	*dest = '\0';	 // don't forget terminating null
	return 0;
}

void YahooNetConnection::close_socket(int s) {
	// close the socket if open
	if(isSocketGood(s))
		close(s);
}

bool YahooNetConnection::isValid() const {
	return (bool)chat_server;	// assume valid if there's a server string
}

bool YahooNetConnection::isSocketGood(int socket) {
	return (socket > 0);
}

int YahooNetConnection::linkto(char *hostname, int cport)
{
	struct sockaddr_in server;
	struct hostent *hp;
	int s;		// socket created by link
	char buf[256];

	if(!isValid()) return -1;

	if(!hostname || cport <= 0) {
		if(::debuglevel & (DEBUGLEVEL_ERROR | DEBUGLEVEL_NETWORK))
			cerr << "YahooNetConnection::linkto(): Invalid argument(s)." << endl;
		return -1;
	}
	s = socket(AF_INET, SOCK_STREAM, 0);

	if (s == -1) {
		if(::debuglevel & (DEBUGLEVEL_ERROR | DEBUGLEVEL_NETWORK)) {
			cerr << "YahooNetConnection::linkto(): Error opening control socket." << endl;
			perror("YahooNetConnection::linkto()");
		}
		return -1;
	}
	snprintf(buf, 256, "Looking up %s...", hostname);
	if(::debuglevel & DEBUGLEVEL_NETWORK) {
		cerr << buf << flush;
	}
	emit printChatMessage(buf);
	server.sin_family = AF_INET;
	hp = gethostbyname(hostname);
	if (hp == (struct hostent *) 0) {
		if(::debuglevel & (DEBUGLEVEL_ERROR | DEBUGLEVEL_NETWORK))
			cerr << endl << hostname << ": Unknown host." << endl;
		close_socket(s);
		if(::debuglevel & DEBUGLEVEL_NETWORK) {
			cerr << "not found." << endl;
		}
		return -2;
	}
	if(::debuglevel & DEBUGLEVEL_NETWORK) {
		cerr << "found." << endl;
	}
	memcpy((char *)(&server.sin_addr), (char *)(hp->h_addr), hp->h_length);
	server.sin_port = htons(cport);
	snprintf(buf, 256, "Connecting to %s:%d...", hostname, cport);
	if(::debuglevel & (DEBUGLEVEL_NETWORK))
		cerr << buf << flush;
	emit printChatMessage(buf);
	if (::connect(s, (struct sockaddr *) & server, sizeof server) == -1) {
		if(::debuglevel & (DEBUGLEVEL_ERROR || DEBUGLEVEL_NETWORK)) {
			cerr << endl << "YahooNetConnection::linkto(): Error connecting socket." << endl;
			perror("YahooNetConnection::linkto()");
		}
		close_socket(s);
		if(::debuglevel & DEBUGLEVEL_NETWORK)
			cerr << "connection failed." << endl;
		return -3;
	}
	if(::debuglevel & (DEBUGLEVEL_NETWORK))
		cerr << "done." << endl;
	return s;
}

void YahooNetConnection::help() {
	static const char *cmd_help_msg =
		"           ***     " APP_NAME "     ***\n"
		"  /goto <username>              Go to room occupied by <username>\n"
		"  /join <roomname>              Move to <roomname>\n"
		"  /join                         Open roomlist dialog\n"
		"  /tell <username> <message...> Send a PM to <username>\n"
		"  /think <message...>           Say <message...> in thought bubble\n"
		"  /who <username>               [unimplemented]\n"
		"  /shell <command> [<args...>]  Invoke a shell <commaand>\n"
		"  /exec  <command> [<args...>]  Alias for /shell\n"
		"  /save [filename]              Save room text to filename\n"
		"  /bye                          Disconnect from chat server\n"
		"  /quit                         Exit " APP_NAME "\n"
		"  /exit                         Alias for /quit\n";
	emit printChatMessage(cmd_help_msg, 0, CET_SYSTEM_MESSAGE);
}






// This command parser *SOOOOO* needs to be dissociated from the network
// code... [dreaming of v1.0...]
void YahooNetConnection::parseInput(QString sendline, bool verbose)
{
	char			  *command, *arg1, *arg2;
//	char			  *commandused;	// unused param
//	char				commandbuf[1024];	// unused param

	if(sendline == 0) {
		if(::debuglevel & (DEBUGLEVEL_ERROR || DEBUGLEVEL_NETWORK))
			cerr << "YahooNetConnection::parseInput(): bad argument." << endl;
		return;
	}

	int buflen = strlen(sendline) + 1;	// length of buf[]
	char *buf = new char[buflen];	// copy the input string
	memset(buf, 0, buflen);
	strncpy(buf, sendline, buflen);

	if (*buf == '/') {
		parseCommand(buf + 1, &command, &arg1, &arg2);

		if (strncasecmp(command, "bye", strlen(command)) == 0) {
			emit appSignal(APP_LOGOUT);	//
		} else if (strncasecmp(command, "connect", strlen(command)) == 0) {
			if(arg1 && *arg1 && arg2 && *arg2) {	// we have args?
				// arg1 == username, arg2 == password
				char *server = chat_server ? strdup(chat_server) : 0;
				int port = chat_port;
				char *room = current_room ? strdup(current_room) : 0;
				destroy();
            // set application cursor to hourglass
            const QCursor &prev_cursor = qApp->mainWidget()->cursor();
            qApp->mainWidget()->setCursor(waitCursor);
				init(arg1, arg2, server, port, room, true, 0);
            qApp->mainWidget()->setCursor(prev_cursor);
			} else {
				emit printChatMessage("Syntax: /connect <username> <password>", 0, CET_SYSTEM_MESSAGE);
			}
		} else if (strncasecmp(command, "goto", strlen(command)) == 0) {
			if(isValid()) {
				if (arg1) {
					char buf[64] = "";
					strncpy(buf, arg1, 64);
					if(arg2) {
						strncat(buf, " ", 64);
						strncat(buf, arg2, 64);
					}
					QString message("*** Attempting to go to ");
					message += buf;
					message += " ***";
					emit printChatMessage(message, 0, CET_SYSTEM_MESSAGE);
					if(goto_user(buf)) {
						if(::debuglevel & (DEBUGLEVEL_WARNING || DEBUGLEVEL_NETWORK))
							cerr << "YahooNetConnection::parseInput(): /goto failed." << endl;
					}
				}
			}
		} else if (strncasecmp(command, "help", strlen(command)) == 0) {
			// print a Usage thing to the user
			help();
		} else if (strncasecmp(command, "join", strlen(command)) == 0) {
			if(isValid()) {
				if (arg1) {
					char buf[64] = "";
					strncpy(buf, arg1, 64);
					if(arg2) {
						strncat(buf, " ", 64);
						strncat(buf, arg2, 64);
					}
					QString message("*** Attempting to join ");
					message += buf;
					message += " ***";
					emit printChatMessage(message, 0, CET_SYSTEM_MESSAGE);
					if(join(buf)) {
						if(::debuglevel & (DEBUGLEVEL_WARNING || DEBUGLEVEL_NETWORK))
							cerr << "YahooNetConnection::parseInput(): /join failed." << endl;
					}
				} else {
					emit activateMenuItem(MENU_VIEW_ROOMLIST);
				}
			}	// end isValid()
		} else if (strncasecmp(command, "me", strlen(command)) == 0) {	// iRC command
			if(isValid()) {
				char buf2[1024];
				if(arg1) {
					strncpy(buf2, arg1, 1024);
					if(arg2) {
						strncat(buf2, " ", 1024);
					}
				} else
					buf2[0] = '\0';
				YahooSendEmote(current_room, buf2);
			}
// ABJ 10-08-1999 10:40 - added shell command
		} else if (strncasecmp(command, "shell", strlen(command)) == 0 || 
			strncasecmp(command, "exec", strlen(command)) == 0) {
			if(arg1) {	// we have a command to invoke
				QString message("*** Invoking '");
				message += arg1;
				if(arg2) {
					message += ' ';
					message += arg2;
				}
				message += "' shell command ***";
				if(verbose)
					emit printChatMessage(message, 0, CET_SYSTEM_MESSAGE);
				else
					emit printChatMessage(QString("*** Invoking command ***"), 0, CET_SYSTEM_MESSAGE);
				shell(arg1, arg2);	//  this'll be changed later, the arg# part
			} else	// bad command
				emit printChatMessage("*** Invalid command ***", 0, CET_SYSTEM_MESSAGE);
// END ABJ 10-08-1999 10:40 - added shell command

		} else if (strncasecmp(command, "save", strlen(command)) == 0) {
			if(arg1)
				emit saveText(arg1);
			else
				emit activateMenuItem(MENU_FILE_SAVE_AS);
		} else if (strncasecmp(command, "tell", strlen(command)) == 0) {
			if(strlen(command) <2) {	// ambiguous command
				emit printChatMessage("*** Ambiguous command ***", 0, CET_SYSTEM_MESSAGE);
				delete buf;
				return;
			}
			if(isValid()) {
				if (arg1 && arg2) {
					YahooSendPM(arg1, arg2);
				} else {
					fprintf(stdout,
						"PM error: bad args: arg1 == 0x%08X, arg2 = 0x%08X\n",
						(unsigned int)arg1, (unsigned int)arg2);
					fflush(stdout);
				}
			}
		} else if (strncasecmp(command, "think", strlen(command)) == 0) {
			if(strlen(command) <2) {	// ambiguous command
				emit printChatMessage("*** Ambiguous command ***", 0, CET_SYSTEM_MESSAGE);
				delete buf;
				return;
			}
			if(isValid()) {
				// create an emote input string & re-call this function
				char buf2[256];
				snprintf(buf2, 1024, ":. o O ( %s ", arg1 ? arg1 : "");
				if(arg2) {
					strncat(buf2, arg2, 1024);
					strncat(buf2, " ", 1024);
				}
				strncat(buf2, ")", 1024);
				parseInput(buf2);
			}
		} else if (strncasecmp(command, "who", strlen(command)) == 0) {
			if(isValid()) {	// nothing now
			}
		} else if (strncasecmp(command, "quit", strlen(command)) == 0 || 
		           strncasecmp(command, "exit", strlen(command)) == 0) {
			emit appSignal(APP_QUIT);	// global signals from this object
// ABJ 10/04/1999 12:21 - modified to send bad commands to the room as text
// ABJ 10/04/1999 12:21 - ...this is to type directory names, etc.
		} else {	// bad argument... treat as ordinary text
//			emit printChatMessage("*** Invalid command ***", 0, CET_SYSTEM_MESSAGE);
			QString message(command);
			if(arg1) {
				message += ' ';
				message += arg1;
				if(arg2) {
					message += ' ';
					message += arg2;
				}
			}
			message.prepend("/");
			YahooSendText(current_room, message);
		}
// END ABJ 10/04/1999 12:21 - modified to send bad commands to the room as text

	} else  {
		// if line starts w/ [semi]colon && isn't a smiley, make it an emote
		if ((*buf == ':'  || *buf == ';')
			&& (!(strchr(")(dDpPxX>", buf[1])) || isalpha(buf[2]))) {
			if(isValid())
				YahooSendEmote(current_room, buf+1);
		} else {	// general entry
			if(isValid()) {
				if(verbose) {
					YahooSendText(current_room, buf );
				} else {
					// Currently the only non-command, non-verbose issuance is
					// the clone warning.
					YahooSendText(current_room, "*** Clone Warning ***");
				}
			}
		}
	}

	// deallocate resources (buf[])
	delete buf;
	return;
}


int YahooNetConnection::YahooSendText(const char *roomname, const char *msg)
{
  char				packetData[1024];
  unsigned long  *ptr, pktLen;

  if(!isValid()) return -1;
  
  // first modify msg[] for URLs... for this we need another buffer
  
  int i=strlen(msg)+90;	// copy the message into our buffer
  char *cp = new char[i];
  strncpy(cp, msg, i);
  processURLs(cp, i);	// convert URLs

	// so now we have like 2 whole buffers, one on the stack

	// form packet
	memset(packetData, 0, 1024);
	strncpy(packetData, "YCHT", 1024);
	ptr = (unsigned long *)(&packetData[4]);
	*ptr = htonl(0x00000100);
	ptr = (unsigned long *)(&packetData[8]);
	*ptr = htonl(0x00000041);
	pktLen = strlen(roomname) + 1 + strlen(cp);
	ptr = (unsigned long *)(&packetData[12]);
	*ptr = htonl(pktLen);
	strncpy(&packetData[16], roomname, 1024-16);
	i = 16 + strlen(roomname);
	packetData[i] = 0x01;
	strncpy(&packetData[i + 1], cp, 1024-i-1);
	delete cp;	// don't need this buffer anymore...
	/* Send it along */
	if(isSocketGood()) {
		if (SocketWrite(sock, packetData, pktLen + 16) != (signed) pktLen + 16) {
			if(::debuglevel & (DEBUGLEVEL_ERROR || DEBUGLEVEL_NETWORK))
				cerr << "YahooNetConnection::YahooSendText(): Error sending text." << endl;
			return -1;
		}
	} else {
		if(::debuglevel & (DEBUGLEVEL_ERROR || DEBUGLEVEL_NETWORK))
			cerr << "YahooNetConnection::YahooSendText(): Bad socket." << endl;
		return -1;
	}
	return 0;
}

int YahooNetConnection::YahooSendPM(const char *who, const char *msg, bool verbose, bool qtchat_packet)
{
	const int      packetDataLen=1024;
	char				packetData[packetDataLen];
	unsigned long  *ptr, pktLen;

	if(!isValid()) return -1;

	// first modify msg[] for URLs
	int i=strlen(msg)+90;	// copy the message into our buffer
	char *cp = new char[i];
	strncpy(cp, msg, i);
	processURLs(cp, i);	// convert URLs

	// form packet
	memset(packetData, 0, packetDataLen);
	strncpy(packetData, "YCHT", packetDataLen);
	ptr = (unsigned long *)(&packetData[4]);
	*ptr = htonl(0x00000100);
	ptr = (unsigned long *)(&packetData[8]);
	*ptr = htonl(0x00000045);
	pktLen = strlen(username) + 1 + strlen(who) + 1 + strlen(cp);
	ptr = (unsigned long *)(&packetData[12]);
	*ptr = htonl(pktLen);
	strncpy(&packetData[16], username, packetDataLen-16);
	packetData[16 + strlen(username)] = 0x01;
	i = 17+strlen(username);
	strncpy(&packetData[i], who, packetDataLen-i);
	i +=  strlen(who);
	packetData[i] = 0x01;
	i++;
	if(qtchat_packet) {	// add qtchat packet header to data
		memcpy(&packetData[i], QTCHAT_PACKET_HEADER, QTCHAT_PACKET_HEADER_LEN);
		i+=QTCHAT_PACKET_HEADER_LEN;
	}
	strncpy(&packetData[i], cp, packetDataLen-i);
	if(SocketWrite(sock, packetData, pktLen + 16) != (signed) pktLen + 16) {
		if(::debuglevel & (DEBUGLEVEL_ERROR || DEBUGLEVEL_NETWORK)) {
			cerr << "YahooNetConnection::YahooSendPM: Error sending PM." << endl;
			perror("YahooNetConnection::YahooSendPM");
		}
		return -1;
	}
	// echo back to user if verbose
	Chatter tempchatter(who);
	if(verbose)
		snprintf(packetData, packetDataLen, "You tell %s: %s", who, msg);
	else
		snprintf(packetData, packetDataLen, "You PM warning to %s.", who);
	emit printChatMessage(packetData, &tempchatter, CET_SYSTEM_MESSAGE);
	delete cp;
	return 0;
}

int YahooNetConnection::YahooSendEmote(const char *roomname, const char *msg)
{
	const int      packetDataLen=1024;
	char				packetData[packetDataLen];
	unsigned long  *ptr, pktLen;

	if(!isValid()) return -1;

	// first modify msg[] for URLs
	int i=strlen(msg)+90;	// copy the message into our buffer
	char *cp = new char[i];
	strcpy(cp, msg);
	processURLs(cp, i);	// convert URLs

	// form packet
	memset(packetData, 0, packetDataLen);
	strncpy(packetData, "YCHT", packetDataLen);
	ptr = (unsigned long *)(&packetData[4]);
	*ptr = htonl(0x00000100);
	ptr = (unsigned long *)(&packetData[8]);
	*ptr = htonl(0x00000043);
	pktLen = strlen(roomname) + 1 + strlen(cp);
	ptr = (unsigned long *)(&packetData[12]);
	*ptr = htonl(pktLen);
	strcpy(&packetData[16], roomname);
	packetData[16 + strlen(roomname)] = 0x01;
	strcpy(&packetData[16 + strlen(roomname) + 1], cp);
	/* Send it along */
	if (SocketWrite(sock, packetData, pktLen + 16) != (signed) pktLen + 16) {
		if(::debuglevel & (DEBUGLEVEL_ERROR || DEBUGLEVEL_NETWORK))
			cerr << "YahooNetConnection::YahooSendEmote: Error sending emotion." << endl;
		delete cp;
		return -1;
	}

	delete cp;
	return 0;
}

int YahooNetConnection::goto_user(const char *user)
{
	char		packetData[256];
	unsigned long  *ptr;
	unsigned short  pktLen;

	if(!isValid()) return -1;

	memset(packetData, 0, 256);
	strcpy(packetData, "YCHT");
	ptr = (unsigned long *)(packetData+4);
	*ptr = htonl(0x00000100);
	ptr = (unsigned long *)(packetData+8);
	*ptr = htonl(0x00000025);	// goto packet type
	pktLen = strlen(user);
	ptr = (unsigned long *)(packetData+12);
	*ptr = htonl(pktLen);
	strcpy(packetData+16, user);
	if(SocketWrite(sock, packetData, pktLen + 16) < 0) {
		if(::debuglevel & (DEBUGLEVEL_ERROR || DEBUGLEVEL_NETWORK))
			cerr << "YahooNetConnection::goto_user(): Error sending packet." << endl;
		return -1;
	}
	// here we can't just change the room... we have to wait for ACK from Yahoo
//	if(current_room) free(current_room);
//	current_room = 0;
	return 0;
}

int YahooNetConnection::join(const char *roomname)
{
	char		packetData[256];
	unsigned long  *ptr;
	unsigned short  pktLen;

	if(!isValid()) return -1;

	memset(packetData, 0, 256);
	strcpy(packetData, "YCHT");
	ptr = (unsigned long *)(&packetData[4]);
	*ptr = htonl(0x00000100);
	ptr = (unsigned long *)(&packetData[8]);
	*ptr = htonl(0x00000011);
	pktLen = strlen(roomname);
	ptr = (unsigned long *)(&packetData[12]);
	*ptr = htonl(pktLen);
	strcpy(&packetData[16], roomname);
	if(SocketWrite(sock, packetData, pktLen + 16) < 0) {
		if(::debuglevel & (DEBUGLEVEL_ERROR || DEBUGLEVEL_NETWORK))
			cerr << "YahooNetConnection::join(): Error joining room." << endl;
		return -1;
	}
	if(current_room) free(current_room);
	current_room = strdup(roomname);
	if(conn_state != CS_LOGIN)	// enter JOIN state iff not in LOGIN state
		conn_state = CS_JOIN;	// set state machine
	return 0;
}

int YahooNetConnection::login()
{
	char				packetData[256];
	unsigned long  *ptr;
	unsigned short  pktLen;

	if(!isValid()) return -1;

	memset(packetData, 0, 256);
	strcpy(packetData, "YCHT");
	ptr = (unsigned long *)(packetData+4);
	*ptr = htonl(0x00000100);
	ptr = (unsigned long *)(packetData+8);
	*ptr = htonl(0x00000001);
	pktLen = strlen(username) + 1 + strlen(cooky);
	ptr = (unsigned long *)(packetData+12);
	*ptr = htonl(pktLen);
	strcpy(&packetData[16], username);
	packetData[16 + strlen(username)] = 0x01;
	strcpy(&packetData[17 + strlen(username)], cooky);
	if(SocketWrite(sock, packetData, pktLen + 16) < 0) {
		if(::debuglevel & (DEBUGLEVEL_ERROR || DEBUGLEVEL_NETWORK))
			cerr << "YahooNetConnection::login(): Error logging in." << endl;
		return -1;
	}
	conn_state = CS_LOGIN;
	return 0;
}

int YahooNetConnection::YahooPing()
{  
	char PacketData[16];  
	unsigned long *ptr;  

	memset( PacketData, 0, 16 );  
	strcpy( PacketData, "YCHT" );  
	ptr = (unsigned long *)(&PacketData[4]);
	*ptr = htonl( 0x00000100 );  
	ptr = (unsigned long *)(&PacketData[8]);  
	*ptr = htonl( 0x00000062 );     
	if(send(sock, PacketData, 16, 0) != (int)16)
		return -1;
	else
		return 0;
}

void YahooNetConnection::handleQtChatPacket(const char *data, size_t len) {
	cerr << "QtChat packet received, length " << len << endl;
};

