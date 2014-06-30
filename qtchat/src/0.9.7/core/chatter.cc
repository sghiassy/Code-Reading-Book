#include <fstream.h>
#include <iomanip.h>
#include <list>	// STL linked list
#include <string>	// STL string class

#include <stdio.h>
#include <stdlib.h>
#include <qfontmetrics.h>

#include "chatter.h"
#include "net.h"

typedef list<string> StringList;

class ChatterRef : public StateObject {
	public:
		// Constructors / Destructors
		ChatterRef(QString _name, bool _highlighted=false, bool _ignored=false);
		ChatterRef(const char *_name, bool _highlighted=false, bool _ignored=false);
		virtual ~ChatterRef();

		static int numChatters() {
			return num_chatters;
		}
		QString getName() const {
			return QString(name);
		}
		QString getiName() const {
			return QString(name).lower();
		}
		bool isMe() const {
			return is_me;
		}
		void setMe(bool b) {
			is_me = b;
			if(b)
				friend_level = (FriendLevel)((int)FRIENDLEVEL_END - 1);
		}
		bool isConnected() const;	// check if chatter is me and connected
		bool isIgnored() const {
			return ignored;
		}
		bool isHighlighted() const {
			return highlighted;
		}
		void setIgnored(bool b) {
			ignored = b;
			if(b)
				addToIgnoredList(name);
			else
				removeFromIgnoredList(name);
		}

		void addToIgnoredList(const char *name) {}
		void removeFromIgnoredList(const char *name) {}

		void setHighlighted(bool b) {
			highlighted = b;
		}
		void setHighlightColor(QColor c) {
			highlight_color=c;
		}
		QColor getHighlightColor() {
			return highlight_color;
		}
		int incRefCount() {
			return ++refs;
		}
		int decRefCount() {
			return --refs;
		}
		FriendLevel getFriendLevel() const {
			return friend_level;
		}
		void setFriendLevel(FriendLevel fl) {
			friend_level = fl;
		}
		void setFriendLevel(char fl) {
			switch(fl) {
				case 'F': friend_level = FRIEND; break;
				case 'T': friend_level = TRUSTED_FRIEND; break;
				case 'G': friend_level = GOD; break;
				case 'S':
				default:  friend_level = STRANGER; break;
			}
		}
		ChatEntryType getLastEventType() const {
			return last_event_type;
		}
		void setLastEventType(ChatEntryType t) {
			last_event_type = t;
		}
		float getEventsPerSec() const {
			return events_per_sec;
		}
		float getEventsPerSecPerSec() const {
			return events_per_sec_per_sec;
		}
		float getLastEventsPerSec() const {
			return last_events_per_sec;	// last value returned by getE.P.S.()
		}
		void doEvent(QString txt);	// register event done by chatter & update counters
		QString getLastLine() const {	// last line posted by chatter
			return last_line;
		}
		int numPMEvents() const {	// number of PMs emitted by Chatter
			return num_pm_events;
		}
		void incNumPMEvents() {	// increment number of PMs emitted by Chatter
			num_pm_events++;
		}
		int numCapsEvents() const {	// num. uppercase posts emitted by Chatter
			return num_caps_events;
		}
		void incNumCapsEvents() {	// inc. num. uppercase posts emitted by Chatter
			num_caps_events++;
		}
		void setEventQueueSize(int size);
		int getEventQueueSize() const {
			return queue_size;
		}
		int numEvents() const {	// number of events recorded in circular queue
			return num_events;
		}
		int numDuplicateEvents() const {	// number of duplicates of most recent event
			return num_duplicates;
		}
		int getNumTotalEvents() const { 	// total number of events posted by Chatter
			return num_total_events;
		}

		void saveState(ostream &s) const;
		void restoreState(istream &s);
//		static int createFromStream(istream &s);
		static int createFromStream(istream &is, int version);

	private:
		int refs;		// number of references to this object in existence
		static int num_chatters;	// total number of ChatterRef objects in existence
		FriendLevel friend_level;	// level of trust in chatter
		char *name;	// chatter's name as known to Yahoo
		bool is_me;	// this chatter is 'me'
		bool highlighted;	// chatter is highlighted
		bool ignored;	// chatter is ignored
		static QTime elapsed_time;	// 24 hr elapsed time
		int num_pm_events;	// number of PMs emitted by Chatter
		int num_caps_events;	// number of uppercase posts emitted by Chatter
		ChatEntryType last_event_type;	// last event issued by event
		int event_time[MAX_QUEUE_SIZE];	// circular queue of event times
		int event_time_index;	// current index into queue, next time goes here
		int num_events;	// number of events in queue
		int queue_size;	// number of events queue can hold (max MAX_QUEUE_SIZE)
		float events_per_sec;	// no. of events registered per second
		float last_events_per_sec;	// last value of above
		float events_per_sec_per_sec;	// derivative of above
		int num_total_events;	// total number of events by chatter
		QColor highlight_color;	// duh
		QString last_line;	// last line posted by chatter
		int num_duplicates;	// number of identical last posts
		void init(const char *_name);	// used by constructors

		static StringList ignored_list;	// list of people on ignore
};


const char *FriendLevelString[] = {	// string rep. of FriendLevel
	"Stranger",
	"Friend",
	"Trusted Friend",
	"God",
	0
};

int toInt(FriendLevel fl) {	// function to get int representation of FriendLevel
	return (int)(fl - STRANGER);
}

FriendLevel toFriendLevel(int i) {	// companion to toInt(FriendLevel)
	return (FriendLevel)(i+(int)STRANGER);
}

const char *toString(FriendLevel fl) {	// function to get string rep. of FriendLevel
	if(fl >= STRANGER && fl < FRIENDLEVEL_END)
		return FriendLevelString[fl-STRANGER];
	else
		return FriendLevelString[0];	// return STRANGER by default
}

FriendLevel toFriendLevel(const char *str) {	// companion to toString(FriendLevel)
	for(int i=0; FriendLevelString[i]; i++)
		if(strcasecmp(str, FriendLevelString[i])==0) return toFriendLevel(i);
	return STRANGER;
}

FriendLevel operator++(FriendLevel &fl) {
	FriendLevel tmp = fl;
	fl = (FriendLevel)(((int)fl)+1);
	return tmp;
}


// Init static variables
StringList ChatterRef::ignored_list;
ChatterRef *Chatter::chatters[MAX_NUM_CHATTERS] = {0};
int ChatterRef::num_chatters = 0;	// total number of ChatterRef objects in existance
QTime ChatterRef::elapsed_time;	// init elapsed timer to 0:00:00

bool ChatterRef::isConnected() const {	// check if chatter is me and connected
	if(!isMe()) return false;	// non-me's can't be connected
	return YahooNetConnection::getMeConnection(Chatter(name)) != 0;
}

ChatterRef::ChatterRef(QString _name, bool _highlighted, bool _ignored) :
	refs(0),highlighted(_highlighted),ignored(_ignored),
	highlight_color(Qt::yellow),
	last_line() {

	init(_name);
}

ChatterRef::ChatterRef(const char *_name, bool _highlighted, bool _ignored) :
	refs(0),highlighted(_highlighted),ignored(_ignored),
	highlight_color(Qt::yellow),
	last_line() {

	init(_name);
}

void ChatterRef::init(const char *_name) {
	if(::debuglevel & DEBUGLEVEL_CONSTRUCTOR)
		cerr << "ChatterRef(\"" << _name << "\") constructor called." << endl;
	if(_name) {
		name = new char[strlen(_name)+1];
		strcpy(name, _name);
	} else
		name = 0;
	friend_level = STRANGER;
	is_me = false;
	num_pm_events = 0;
	num_caps_events = 0;
	for(int i=0; i<MAX_QUEUE_SIZE; i++)
		event_time[i] = 0;
	last_event_type = CET_NULL;
	event_time_index = 0;
	num_events = 0;
	queue_size = 4;
	events_per_sec = 0.0;
	last_events_per_sec = 0.0;
	events_per_sec_per_sec = 0.0;
	if(elapsed_time.isNull()) {
		elapsed_time.setHMS(0, 0, 0, 1);
		elapsed_time.start();
	}
	num_duplicates = 0;
	num_chatters++;
	if(ignored)
		addToIgnoredList(name);
}

ChatterRef::~ChatterRef() {
	if(debuglevel & DEBUGLEVEL_DESTRUCTOR)
		cerr << "ChatterRef(\"" << name << "\") destructor called." << endl;
	if(name) {
		removeFromIgnoredList(name);
		delete name;
	}
	num_chatters--;
}

void addToIgnoreList(const char *name) {
}

void removeFromIgnoreList(const char *name) {
}

void ChatterRef::doEvent(QString txt) {
	// first determine duplicate events
	if(txt == last_line) {	// this also catches null strings though...
		num_duplicates++;
	} else {
		num_duplicates = 1;
		last_line = txt;
	}

	// now calculate rate/acceleration of chatter events
	if(queue_size) {
		int current_event_time = elapsed_time.elapsed();	// time of current event (ms)
		event_time[event_time_index] = current_event_time;
		int prev_event_time_index = (event_time_index-1) % queue_size;
		event_time_index = (event_time_index+1) % queue_size;	// adjust index to range of queue
		if(num_events < queue_size) num_events++;
		int first_event_time = 	// time of first event (ms)
			event_time[(queue_size + (event_time_index-num_events)) % queue_size];
		last_events_per_sec = events_per_sec;
		if(current_event_time != first_event_time)
			events_per_sec = (float)num_events*1000.0/(float)(current_event_time - first_event_time);
		else
			events_per_sec = 0.0;
		if(current_event_time != event_time[prev_event_time_index])
			events_per_sec_per_sec = (events_per_sec - last_events_per_sec)* 1000.0/(current_event_time-event_time[prev_event_time_index]);
		else
			events_per_sec_per_sec = 0.0;
	} else
		events_per_sec = 0;
}

void ChatterRef::setEventQueueSize(int size) {
	if(size < queue_size) {
	}
}

void ChatterRef::saveState(ostream &s) const {
	if(name) {
//**** DISABLED Version 1*****
//		s << "\tName=\"" << name << '\"' << endl;
//		s << "\tFriendLevel=\"" << toString(friend_level) << '\"' << endl;
//		s << "\tHighlighted=\"" << (highlighted ? "true" : "false") << '\"' << endl;
//		s << "\tIgnored=\"" << (ignored ? "true" : "false") << '\"' << endl;
//		s << "\tHighlightColor=\"" << setw(6) << setfill('0') << hex << highlight_color.rgb() << '\"' << endl << endl;
//***** END DISABLED *****

// Version 2
		s << name << ends;
		s << toString(friend_level)[0];
		s << (char)highlighted;
		s << (char)ignored;
		s << (char)highlight_color.red()
			<< (char)highlight_color.green()
			<< (char)highlight_color.blue();
// End Version 2
	}
}

int readTag(const char *tag, istream &is, char *buf, int len, unsigned int version) {
	// Get tag into buffer 'buf' of length 'len'.
	// Tag is formatted:	*tag*"buf"
	// Calling readTag("Name", buf, is, len) will read ` Name="Joe" '.

	// check args
	if(!tag || !buf || !is || len <=0) {
		cerr << "readTag(): Bad argument(s)" << endl;
		return -1;
	}

	switch(version) {
		case 1:
			(is >> ws).getline(buf, len, '\"');	// read line up to first quote
			if(!is || strstr(buf, tag) == 0) {	// look for tag
				cerr << "readTag(): Unable to find tag '" << tag << "' in '" << buf << "'." << endl;
				return -1;	// error... no tag found.
			}
			is.getline(buf, len, '\"');	// read Name
		//	is.getline(buf, len) >> ws;	// read Name
			if(!is) {
				cerr << "readTag(): Unable to read value of tag '" << tag << "'." << endl;
				return -1;
			}
			// buf now has tag value in it
			break;

		case 2:	// new version of chatters.ini
			switch(tag[0]) {
				case 'N':	// "Name" tag
					is.getline(buf, len, '\0');
					break;
				case 'I':	// "Ignored" tag
				case 'H':	// "Highlighted" tag
					is.read(buf, 1);
					break;
				case 'C':	// "Highlight Color" tag
					is.read(buf, 3);
					break;
				case 'F':	// "FriendLevel" tag
					is.read(buf, 1);
					break;
			break;
		}

		default:
			break;
	}
	return 0;
}

int ChatterRef::createFromStream(istream &is, int version) {
	char buf[1024];
	unsigned long color = 0;
	ChatterRef *c = 0;

	// Get name
	if(readTag("Name", is, buf, 1024, version)) {
		cerr << "ChatterRef::createFromStream(): Error parsing 'Name' tag'" << endl;
		return -1;
	}
	int i;	// gen. purpose int

	// see if this chatter already exists
	// this is O(n^2), so we'll specify that all chatters in file will be unique
//	i=findChatter(buf);
//	if(i >= 0) { // found one.. we'll set the properties of existing chatter
//		c = Chatter::chatters[i];
//	} else {	// doesn't already exist... make a new one
		c = new ChatterRef((const char *)buf);
		CHECK_PTR(c);
		i=Chatter::findFirstSpace();
		if(i >=0) {	// we have an empty slot... stick it in!!!
			Chatter::chatters[i] = c;
			num_chatters++;
		} else {	// doh!
			cerr << "ChatterRef::createFromStream(): Unable to insert new chatter." << endl;
			delete c;
			Chatter::chatters[i] = 0;
			return -1;
		}
//	}

	// get FriendLevel
	if(readTag("FriendLevel", is, buf, 1024, version)) {
		cerr << "ChatterRef::createFromStream(): Error parsing 'FriendLevel' tag'" << endl;
		return -1;
	}
	if(version == 1)
		c->setFriendLevel(toFriendLevel(buf));
	else if(version == 2)
		c->setFriendLevel(buf[0]);

	// get Highlighted
	if(readTag("Highlighted", is, buf, 1024, version)) {
		cerr << "ChatterRef::createFromStream(): Error parsing 'Highlighted' tag'" << endl;
		return -1;
	}
	if(version == 1)
		c->setHighlighted(strcasecmp(buf, "true")==0 ? true : false);
	else if(version == 2)
		c->setHighlighted(buf[0]);

	// get Ignored
	if(readTag("Ignored", is, buf, 1024, version)) {
		cerr << "ChatterRef::createFromStream(): Error parsing 'Ignored' tag'" << endl;
		return -1;
	}
	if(version == 1)
		c->setIgnored(strcasecmp(buf, "true")==0 ? true : false);
	else if(version == 2)
		c->setIgnored(buf[0]);

	// get HighlightColor
	if(version == 1) {
		if(readTag("HighlightColor", is, buf, 1024, version)) {
			cerr << "ChatterRef::createFromStream(): Error parsing 'HighlightColor' tag'" << endl;
			return -1;
		}
		sscanf(buf, "%lx", &color);
		c->setHighlightColor(QColor(color));
	} else if(version == 2) {
		if(readTag("C", is, buf, 1024, version)) {
			cerr << "ChatterRef::createFromStream(): Error parsing 'HighlightColor' tag'" << endl;
			return -1;
		}
		c->setHighlightColor(QColor((unsigned int)buf[0] & 0xff, (unsigned int)buf[1] & 0xff, (unsigned int)buf[2] & 0xff));
	}

	return 0;
}

void ChatterRef::restoreState(istream &is) {
	return;
}

Chatter::Chatter(QString name)  {
	if(::debuglevel & DEBUGLEVEL_CONSTRUCTOR)
		cerr << "Chatter(\"" << name << "\") constructor called." << endl;
	if(name.isNull()) {
		if(::debuglevel & (DEBUGLEVEL_WARNING | DEBUGLEVEL_CONSTRUCTOR))
			cerr << "Chatter::Chatter(QString): Null Chatter created.  loadChatter() call needed." << endl;
	}
	int i = findChatter(name);
	if(i >=0) {
		chatters[i]->incRefCount();
		index = i;
	} else {
		i = findFirstSpace();
		if(i >= 0) {
			chatters[i] = new ChatterRef(name);
			chatters[i]->incRefCount();
			index = i;
		} else {
			cerr << "Chatter::Chatter(QString): Unable to create ChatterRef for";
			cerr << name << '.' << endl;
		}
	}
	lb = 0;
	if(index >= MAX_NUM_CHATTERS)
		cerr << "Chatter::Chatter(): Error: index = " << index << endl;
}

Chatter::Chatter(const char *name)  {
	if(::debuglevel & DEBUGLEVEL_CONSTRUCTOR)
		cerr << "Chatter(\"" << name << "\") constructor called." << endl;
	if(!name) {
		if(::debuglevel & (DEBUGLEVEL_WARNING | DEBUGLEVEL_CONSTRUCTOR))
			cerr << "Chatter::Chatter(const char *): Null Chatter created.  loadChatter() call needed." << endl;
	}
	int i = findChatter(QString(name));
	if(i >=0) {
		chatters[i]->incRefCount();
		index = i;
	} else {
		i = findFirstSpace();
		if(i >= 0) {
			chatters[i] = new ChatterRef(name);
			chatters[i]->incRefCount();
			index = i;
		} else {
			cerr << "Chatter::Chatter(const char *): Unable to create ChatterRef for";
			cerr << name << '.' << endl;
		}
	}
	lb = 0;
	if(index >= MAX_NUM_CHATTERS)
		cerr << "Chatter::Chatter(): Error: index = " << index << endl;
}

Chatter::Chatter(const Chatter &c) {
	index = c.index;
	lb = c.lb;
	int i = chatters[index]->incRefCount();
	if(::debuglevel & DEBUGLEVEL_COPYCONSTRUCTOR)
		cerr << "Chatter(\"" << c.getName() << "\") copy constructor called (" << i << ")." << endl;
}

Chatter::~Chatter() {
	if(::debuglevel & DEBUGLEVEL_DESTRUCTOR)
		cerr << "Chatter(\"" << getName() << "\") destructor called (";
	int i;
	if((i = chatters[index]->decRefCount()) == 0) {
// don't delete the last chatter
//		delete chatters[index];
//		chatters[index] = 0;
	}
	if(::debuglevel & DEBUGLEVEL_DESTRUCTOR)
		cerr << i << ")." << endl;
}

void Chatter::destroyAll() {
	for(int i=0; i<MAX_NUM_CHATTERS; i++) {
		if(chatters[i]) {
			delete chatters[i];
			chatters[i] = 0;
		}
	}
}

int Chatter::findChatter(QString name) {
	if(!name.isNull()) {
		for(int i=0; i<MAX_NUM_CHATTERS; i++) {
			if(chatters[i] && name.lower() == chatters[i]->getName().lower())
				return i;
		}
	}
	return -1;
}

int Chatter::findFirstSpace() {
	for(int i=0; i<MAX_NUM_CHATTERS; i++) {
		if(chatters[i]==0) return i;
	}
	return -1;
}

bool Chatter::loadChatters(istream &is) {
	int n;	// number of chatters saved in state stream
	int version;	// version no.

	if(!is) return false;	// error getting state
	char header[256];	// <Chatter> tag
	is.getline(header, 256, '>');
	if(!is) {	// error getting state
		cerr << "Chatter::loadChatters(istream&): Error reading header." << endl;
	} else {
		// make sure tag is proper
		char *cp;
		if((cp=strstr(header, "<Chatter")) == NULL)
			return false;
		cp += 8;	// skip the "<Chatter" substring
		if((cp=strstr(header, "count=")) == NULL ||
			sscanf(cp+6, "%d", &n)!=1)
			return false;
		if((cp=strstr(header, "version=")) == NULL
			|| (sscanf(cp+8, "%d>", &version)!=1))
			version = 1;

		if(n > MAX_NUM_CHATTERS) { // capacity exceeded
			cerr << "*** Error!!! Too many chatters in file... not loading." << endl;
			return false;
		} else if(((float)n / (float)MAX_NUM_CHATTERS) > 0.9) { // about to exceed capacity
			cerr << "*** Warning!!! Chatter chatters[] at critical level." << endl;
			cerr << "*** Increase MAX_NUM_CHATTERS and recompile." << endl;
		}
		cout << "Loading Chatters...  0%" << flush;
		for(int i=0; i<n; i++) {
			if(ChatterRef::createFromStream(is, version) || !is) {
				cerr << "Chatter::loadChatters(istream&): error creating Chatter #";
				cerr << setw(3) << setfill('0') << i << " of ";
				cerr << setw(3) << setfill('0') << n  << '.' << endl;
				cerr << "Chatter::loadChatters(istream&): assuming end of file." << endl;
				return false;
			}
			if(i%5) {
				printf("\b\b\b\b%3d%%", (i+1)*100/n);
				fflush(stdout);
//				cout << "\b\b\b\b\b" << setw(3) << setfill(' ') << ios::right << (i+1)*100/n << '%' << flush;
			}
		}

		is.scan("</Chatters>");
	}
//	cout << "\b\b\b\b\b100%%" << endl;
	printf("\b\b\b\b100%%\n");
	return (bool)is;
}

bool Chatter::saveChatters(ostream &os) {

	int num_saveable_chatters = 0;
	for(int i=0; i<MAX_NUM_CHATTERS; i++)
		if(chatters[i] && (
			   chatters[i]->isIgnored()
			|| chatters[i]->isHighlighted()
			|| chatters[i]->getFriendLevel() > STRANGER))
			num_saveable_chatters++;
	os << "<Chatters count=" << num_saveable_chatters;
	os << " version=2>" << endl;	// set version of chatters.ini
	int num_saveable_chatters_check = 0;
	for(int i=0; i<MAX_NUM_CHATTERS; i++) {
		if(chatters[i] && 
			(chatters[i]->getFriendLevel() > STRANGER ||	// don't save strangers
			 chatters[i]->isHighlighted()	// save people w/ interesting states
			 || chatters[i]->isIgnored()
			)
		  ) {
			chatters[i]->saveState(os);
			num_saveable_chatters_check++;
		}	// end if
	}	// end for()
	if(num_saveable_chatters != num_saveable_chatters_check) {
		cerr << "Chatter::saveChatters(ostream &): Chatter count discrepancy." << endl;
		cerr << "                                : ";
		cerr << num_saveable_chatters << " != " << num_saveable_chatters_check << endl;
	}
	os << "</Chatters>" << endl;
	return (bool)os;
}

void Chatter::paint(QPainter *p) {
	if(p) {
		p->save();
		if(isMe()) {	// do something special for 'me'
			p->setPen(Qt::red);
		} else {
			if(isIgnored()) {
				p->setPen(Qt::gray);
			} else {
				switch(getFriendLevel()) {
					case FRIEND:
						p->setPen(QColor(176,48,96));	// maroon
						break;
					case TRUSTED_FRIEND:
						p->setPen(QColor(225,69,25));
						break;
					case GOD:
						p->setPen(Qt::red);
						break;
					case STRANGER:
					default:
						p->setPen(Qt::black);
						break;
				}
			}
			if(isHighlighted()) {
				p->setBackgroundMode(Qt::OpaqueMode);
				p->setBackgroundColor(getHighlightColor());
				p->setPen(Qt::darkBlue);
			}
		}
		QFontMetrics fm = p->fontMetrics();
		p->drawText(3, fm.ascent() + fm.leading()/2, getName());
		p->restore();
	}
}

int Chatter::height(const QListBox *list) const {
	if(!list)
		list = lb;
	return list->fontMetrics().lineSpacing();
}

int Chatter::width(const QListBox *list) const {
	if(!list)
		list = lb;
	QFont f=list->font();
	f.setBold(true);
	return QFontMetrics(f).width(getName())+6;
}

int Chatter::height(QPainter *p) const {
   if(p)
		return p->fontMetrics().lineSpacing();
	else
		return 0;
}

int Chatter::width(QPainter *p) const {
	if(p)
		return p->fontMetrics().width(getName())+6;
	else
		return 0;
}

// Mutators
void Chatter::setFriendLevel(FriendLevel fl) {
	chatters[index]->setFriendLevel(fl);
}
void Chatter::setIgnored(bool _ignored) {
	chatters[index]->setIgnored(_ignored);
}
void Chatter::setHighlighted(bool _highlighted) {
	chatters[index]->setHighlighted(_highlighted);
}
void Chatter::setListBox(const QListBox *_lb) {
	lb = _lb;
}
void Chatter::setHighlightColor(QColor c) {
	chatters[index]->setHighlightColor(c);
}

void Chatter::setLastEventType(ChatEntryType t) {
	chatters[index]->setLastEventType(t);
}
void Chatter::doEvent(QString txt) {
	chatters[index]->doEvent(txt);
}
void Chatter::doEvent() {	// register event done by chatter & update counters
	doEvent(QString());
}
void Chatter::setEventQueueSize(int size) {
	chatters[index]->setEventQueueSize(size);
}

// Accessors
int Chatter::numChatters() {
	return ChatterRef::numChatters();
}
QString Chatter::text() const {
	return chatters[index]->getiName();
}
Chatter *Chatter::getMe() {
	for(int i=0; i < MAX_NUM_CHATTERS && i<ChatterRef::numChatters(); i++)
		if(chatters[i] && chatters[i]->isMe())
			return new Chatter(chatters[i]->getName());
	return 0;
}

QString Chatter::getName() const {
	return chatters[index]->getName();
}
FriendLevel Chatter::getFriendLevel() const {
	return chatters[index]->getFriendLevel();
}
QColor Chatter::getHighlightColor() {
	return chatters[index]->getHighlightColor();
}
Chatter::operator QString() {
	return text();
}
bool Chatter::operator==(const Chatter &c) const {
	return index == c.index;
}
bool Chatter::operator!=(const Chatter &c) const {
	return index != c.index;
}
bool Chatter::isIgnored() const {
	return chatters[index]->isIgnored();
}
bool Chatter::isHighlighted() const {
	return chatters[index]->isHighlighted();
}
bool Chatter::isMe() const {
	return chatters[index]->isMe();
}
void Chatter::setMe(bool b) {
	chatters[index]->setMe(b);
}

bool Chatter::isConnected() const {	// check if chatter is me and connected
	return chatters[index]->isConnected();
}

int Chatter::numPMEvents() const {	// number of PMs emitted by Chatter
	return chatters[index]->numPMEvents();
}
void Chatter::incNumPMEvents() {	// increment number of PMs emitted by Chatter
	chatters[index]->incNumPMEvents();
}
int Chatter::numCapsEvents() const {	// num. uppercase posts emitted by Chatter
	return chatters[index]->numCapsEvents();
}
void Chatter::incNumCapsEvents() {	// inc. num. of uppercase posts emitted by Chatter
	chatters[index]->incNumCapsEvents();
}

ChatEntryType Chatter::getLastEventType() const {
	return chatters[index]->getLastEventType();
}

float Chatter::getEventsPerSec() const {
	return chatters[index]->getEventsPerSec();
}
float Chatter::getLastEventsPerSec() const {
	return chatters[index]->getLastEventsPerSec();
}
float Chatter::getEventsPerSecPerSec() const {
	return chatters[index]->getEventsPerSecPerSec();
}
int Chatter::getEventQueueSize() const {
	return chatters[index]->getEventQueueSize();
}
int Chatter::numEvents() const {
	return chatters[index]->numEvents();
}
int Chatter::numDuplicateEvents() const {	// number of duplicates of most recent event
	return chatters[index]->numDuplicateEvents();
}
int Chatter::getNumTotalEvents() const { 	// total number of events posted by Chatter
	return chatters[index]->getNumTotalEvents();
}
QString Chatter::getLastLine() const {	// last line posted by chatter
	return chatters[index]->getLastLine();
}
const QListBox *Chatter::getListBox() const {
	return lb;
}

	// detect clones
QString Chatter::isClone(float threshold, FriendLevel compare_friendlevel, float &similarity) const {
	int count=0;
	int chatters_index=0;
	ChatterRef *chatterptr;

	// only check valid Chatters
	if(chatters[index] == 0) return QString();

	// loop through chatters
	while(chatters_index < MAX_NUM_CHATTERS && count < numChatters()) {
		if((chatterptr=chatters[chatters_index]) != 0) {	// valid ChatterRef?

			// only compare against people who are >= compare_friendlevel
			if(chatterptr->getFriendLevel() >= compare_friendlevel) {

				// calculate similarity to current chatter
				float tmp_similarity = getSimilarity(chatterptr->getName(), getName());
				if(tmp_similarity > threshold && fabs(1.0-tmp_similarity) > 0.000001 ) {
					similarity = tmp_similarity;
					return chatterptr->getName();	// clone found
				}

			}	// end friendlevel comparison

			count++;	// incr. num chatters tested
		}	// end valid ChatterRef test

		chatters_index++;	// try next ChatterRef
	}	// end loop
	return QString();	// not a clone
}	// end function

