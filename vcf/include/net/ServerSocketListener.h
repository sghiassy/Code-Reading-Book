// ServerSocketListener.h

#ifndef _SERVERSOCKETLISTENER_H__
#define _SERVERSOCKETLISTENER_H__


namespace VCFNet
{

class NETKIT_API ServerSocketListener : public VCF::Listener  {
public:	
	virtual ~ServerSocketListener();

	virtual void onMessage( ServerSocketEvent* event ) = 0;
	
	virtual void onClientCreated( ServerSocketEvent* event ) = 0;

	virtual void onClientDeleted( ServerSocketEvent* event ) = 0;
};

typedef void (VCF::Object:: *OnServerSocketEvent)( ServerSocketEvent* event );

class NETKIT_API ServerSocketHandler : public VCF::Object, public ServerSocketListener{
public:
	ServerSocketHandler( VCF::Object* source ){
		m_source = source;

	};

	virtual ~ServerSocketHandler(){};

	virtual void onMessage( ServerSocketEvent* event ) = 0;
	
	virtual void onClientCreated( ServerSocketEvent* event ) = 0;

	virtual void onClientDeleted( ServerSocketEvent* event ) = 0;

	OnServerSocketEvent m_message;
	OnServerSocketEvent m_clientCreated;
	OnServerSocketEvent m_clientDeleted;
private:
	VCF::Object* m_source;

};


};

#endif 
