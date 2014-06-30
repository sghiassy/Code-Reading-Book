//WhatsThisHelpListener.h

#ifndef _WHATSTHISHELPLISTENER_H__
#define _WHATSTHISHELPLISTENER_H__


namespace VCF {

	
	/**
	*WhatsThisHelpEventHandler
	*handles the following:
	*onControlHelpRequested
	*/
	template <class SOURCE_TYPE> class APPKIT_API WhatsThisHelpEventHandler : public EventHandlerInstance<SOURCE_TYPE,WhatsThisHelpEvent>  {
	public:
		WhatsThisHelpEventHandler( SOURCE_TYPE* source,
			EventHandlerInstance<SOURCE_TYPE,WhatsThisHelpEvent>::OnEventHandlerMethod handlerMethod, 
			const String& handlerName="") :
				EventHandlerInstance<SOURCE_TYPE,WhatsThisHelpEvent>( source, handlerMethod, handlerName ) {
			
		}
		
		virtual ~WhatsThisHelpEventHandler(){};
	};
};


#endif //_WHATSTHISHELPLISTENER_H__