#ifndef _CHATTER_H
#define _CHATTER_H

#include <ctype.h>
#include <iostream>
#include <fstream>
extern "C" {
#include <string.h>
}
#include <limits.h>
#include <list>
#include <stdlib.h>
#include <qlistbox.h>
#include <qpainter.h>
#include <qdatetime.h>
#include "chatmain.h"	// ChatEntryType def.
#include "debug.h"	// debug code
#include "state.h"	// state persistence code
#include "util.h"	// getSimilarity()
#include "misc.h"

static const int MAX_QUEUE_SIZE = 25;

#define MAX_NUM_CHATTERS 4096

const char *toString(FriendLevel fl);	// function to get string rep. of FriendLevel
FriendLevel toFriendLevel(const char *);	// companion to toString(FriendLevel)

int toInt(FriendLevel fl);	// function to get int representation of FriendLevel
FriendLevel toFriendLevel(int i);	// companion to toInt(FriendLevel)

FriendLevel operator++(FriendLevel &fl);

class ChatterRef;

class Chatter : public QListBoxItem {
	public:
	// Constructors/Destructors
		Chatter(QString _name);
		Chatter(const char *_name);
		Chatter(const Chatter &c);
		~Chatter();
		static void destroyAll();

	// Mutators
		inline void setFriendLevel(FriendLevel fl);
		inline void setIgnored(bool _ignored);
		inline void setHighlighted(bool _highlighted);
		inline void setListBox(const QListBox *_lb);
		inline void setHighlightColor(QColor c);
		inline void setLastEventType(ChatEntryType t);
		inline void doEvent(QString txt);
		inline void doEvent();	// register event done by chatter & update counters
		inline void setEventQueueSize(int size);

		static bool loadChatters(istream &is);
		static bool saveChatters(ostream &os);
		bool loadState(istream &is);	// get chatter's state from stream
		bool saveState(ostream &os) const;	// set chatter's state to stream

	// Accessors
		static inline int numChatters();
		virtual inline QString text() const;

		static inline Chatter *getMe();

		inline QString getName() const;
		inline FriendLevel getFriendLevel() const;
		inline QColor getHighlightColor();
		inline operator QString();

		inline bool operator==(const Chatter &c) const;
		inline bool operator!=(const Chatter &c) const;

		virtual void paint(QPainter *p);
		virtual int height(const QListBox *) const;
		virtual int width(const QListBox *) const;
		virtual int height(QPainter *p) const;
		virtual int width(QPainter *p) const;


		inline bool isIgnored() const;
		inline bool isHighlighted() const;
		// Clone detection:
		//		threshold: 0.0-1.0, similarity above which isClone() returns match
		//		compare_friendlevel: clonee FriendLevel >= comparison made
		//		similarity: return value of similarity to clonee, or unmodified if no clone found
		QString isClone(float threshold, FriendLevel compare_friendlevel, float &similarity) const;
		inline bool isMe() const;
		inline void setMe(bool b);

		inline bool isConnected() const;	// check if chatter is me and connected

		inline int numPMEvents() const;	// number of PMs emitted by Chatter
		inline void incNumPMEvents();	// increment number of PMs emitted by Chatter
		inline int numCapsEvents() const;	// num. uppercase posts emitted by Chatter
		inline void incNumCapsEvents();	// inc. num. of uppercase posts emitted by Chatter

		inline ChatEntryType getLastEventType() const;

		inline float getEventsPerSec() const;
		inline float getLastEventsPerSec() const;
		inline float getEventsPerSecPerSec() const;
		inline int getEventQueueSize() const;
		inline int numEvents() const;
		inline int numDuplicateEvents() const;	// number of duplicates of most recent event
		inline int getNumTotalEvents() const; 	// total number of events posted by Chatter
		inline QString getLastLine() const;	// last line posted by chatter
		inline const QListBox *getListBox() const;


	private:
		static ChatterRef *chatters[MAX_NUM_CHATTERS];
		int index;			// index into chatters[] of this Chatter
		const QListBox *lb;		// listbox (for height()/width()

		int findChatter(QString name);	// find ChatterRef index in chatters[]
		static int findFirstSpace();	// find first empty slot in chatters[]

		friend ChatterRef;
		friend void ChatWidget::addEntry(QString, Chatter *, ChatEntryType);
		friend int main(int, char *argv[]);	// to destroy chatters[]
	
};

typedef list<Chatter *> ChatterList;

#endif // _CHATTER_H
