//ACTIVEKBLib.h

#ifndef _ACTIVEKBLIB_H__
#define _ACTIVEKBLIB_H__

#include "FoundationKit.h"

namespace ACTIVEKBLib  {

//Typelibrary typedefs


//End of Typelibrary typedefs

/**
*Interface _DActiveKB
*UUID: E9B53FC1-761E-11D0-AF02-00009290C4DB
*/
class _DActiveKB : public Interface { 
public:
	virtual ~_DActiveKB(){};

	virtual long GetCurrentKBID(  ) = 0;//[id(10)]

	virtual void SetCurrentKBID(  ) = 0;//[id(9)]

	virtual long TemporaryTest(  ) = 0;//[id(8)]

	virtual void AboutBox(  ) = 0;//[id(-552)]

};


/**
*Interface _DActiveKBEvents
*UUID: E9B53FC2-761E-11D0-AF02-00009290C4DB
*/
class _DActiveKBEvents : public Interface { 
public:
	virtual ~_DActiveKBEvents(){};

};


#define ACTIVEKBLIB_ACTIVEKB_CLASSID 		"E9B53FC3-761E-11D0-AF02-00009290C4DB"
/**
*class ActiveKB
*UUID: E9B53FC3-761E-11D0-AF02-00009290C4DB
*ProgID: 
*/
class ActiveKB : public VCF::Object, public _DActiveKB, public _DActiveKBEvents {
public:
	BEGIN_CLASSINFO( ActiveKB, "ACTIVEKBLib::ActiveKB", "VCF::Object", ACTIVEKBLIB_ACTIVEKB_CLASSID );
	END_CLASSINFO( ActiveKB );

	ActiveKB();

	virtual ~ActiveKB();

	virtual long GetCurrentKBID(  );//[id(10)]

	virtual void SetCurrentKBID(  );//[id(9)]

	virtual long TemporaryTest(  );//[id(8)]

	virtual void AboutBox(  );//[id(-552)]

};

}  //end of namespace ACTIVEKBLib

#endif //_ACTIVEKBLIB_H__

