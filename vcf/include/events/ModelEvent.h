//ModelEvent.h

#ifndef _MODELEVENT_H__
#define _MODELEVENT_H__


namespace VCF {

class APPKIT_API ModelEvent : public Event {
public:
	ModelEvent( Object* source ) : 
	  Event( source ) {

	  }

	virtual ~ModelEvent(){};

};

};


#endif //_MODELEVENT_H__