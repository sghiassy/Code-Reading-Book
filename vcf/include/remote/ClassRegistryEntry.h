//ClassRegistryEntry.h

/**
This program is free software; you can redistribute it and/or
modify it as you choose. In fact, you can do anything you would like
with it, so long as credit is given if used in commercial applications.
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

NB: This software will not save the world. 
*/

#ifndef _CLASSREGISTRYENTRY_H__
#define _CLASSREGISTRYENTRY_H__

#include "RemoteCommon.h"
#include "Object.h"


namespace VCFRemote  {

/**
*A Class Registry entry contains the neccessary information to create a class 
*from a server, either locally (more often than not), or remotely, if the 
*remote class has been cached for faster lookup.
*/
class REMOTEKIT_API ClassRegistryEntry : public VCF::Object, public VCF::Persistable { 
public:
	ClassRegistryEntry();

	ClassRegistryEntry( const VCF::String& classID,
		                const VCF::String& userID,
						const VCF::String& localServerPath,
						const bool& inProcess,
						const bool& remote,
						const VCF::String& remoteServerPath );

	virtual ~ClassRegistryEntry();
	
	/**
     * returns the string value of the UUID that associates the entry 
	 *with a particular class 
     */
	VCF::String getClassID() {
		return m_classID;	
	}

	/**
     * the user ID is a short hand for the ClassID, such as "MyApp.Foo" 
     */
	VCF::String getUserID() {
		return m_userID;
	}

	/**
     * the fully qualified file name to the local server for the class. 
     */
	VCF::String getLocalServerPath() {
		return m_localServerPath;
	}

	/**
     * is the server in process, i.e. a DLL or SO, as opposed to a 
	 *self executing file. 
     */
	bool isInProcess() {
		return m_inProcess;
	}

	/**
     * is the server remote. False by default. A remote server is a 
	 *server whose executable code resides a another machine.
     */
	bool isRemote() {
		return m_isRemote;
	}

	/**
     * a fully qualified URL that describes the name of a machine 
	 *(or IP address ?) and the path to the server on that machine. 
	 *This is only valid if the m_isRemote member is true, which 
	 *automatically means m_inProcess is false. 
     */
	VCF::String getRemoteServerPath()  {
		return m_remoteServerPath;
	}

	/**
	*Write the object to the specified output stream
	*/
    virtual void saveToStream( VCF::OutputStream * stream );
	
	/**
	**Read the object from the specified input stream
	*/
    virtual void loadFromStream( VCF::InputStream * stream );

	virtual VCF::String toString();
protected:

private:
	/**
     * this member variable represents the string value of the UUID 
	 *that associates the entry with a particular class 
     */
	VCF::String m_classID;
	/**
     * the user ID is a short hand for the ClassID, such as "MyApp.Foo" 
     */
    VCF::String m_userID;

    /**
     * the fully qualified file name to the local server for the class. 
     */
    VCF::String m_localServerPath;

    /**
     * is the server in process, i.e. a DLL or SO, as opposed to a self 
	 *executing file. 
     */
    bool m_inProcess;

    /**
     * is the server remote. False by default 
     */
    bool m_isRemote;

    /**
     * a fully qualified URL that describes the name of a machine (or IP address ?)
	 *and the path to the server on that machine. This is only valid if the 
	 *m_isRemote member is true, which automatically means m_inProcess is false. 
     */
    VCF::String m_remoteServerPath;
};


}; //end of namespace VCFRemote

#endif //_CLASSREGISTRYENTRY_H__


