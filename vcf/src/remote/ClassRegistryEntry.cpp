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


#include "FoundationKit.h"
#include "ClassRegistryEntry.h"

using namespace VCF;

using namespace VCFRemote;

ClassRegistryEntry::ClassRegistryEntry()
{
	m_classID = "";
	m_userID = "";
	m_localServerPath = "";
	m_inProcess = false;
	m_isRemote = false;
	m_remoteServerPath = "";
}

ClassRegistryEntry::ClassRegistryEntry( const VCF::String& classID,
		                const VCF::String& userID,
						const VCF::String& localServerPath,
						const bool& inProcess,
						const bool& remote,
						const VCF::String& remoteServerPath )
{
	m_classID = classID;
	m_userID = userID;
	m_localServerPath = localServerPath;
	m_inProcess = inProcess;
	m_isRemote = remote;
	m_remoteServerPath = remoteServerPath;
}

ClassRegistryEntry::~ClassRegistryEntry()
{

}

void ClassRegistryEntry::saveToStream( OutputStream * stream )
{
	stream->write( m_classID );
	stream->write( m_userID );
	stream->write( m_localServerPath );
	stream->write( m_inProcess );
	stream->write( m_isRemote );
	stream->write( m_remoteServerPath );
}

void ClassRegistryEntry::loadFromStream( InputStream * stream )
{
	stream->read( m_classID );
	stream->read( m_userID );
	stream->read( m_localServerPath );
	stream->read( m_inProcess );
	stream->read( m_isRemote );
	stream->read( m_remoteServerPath );
}

VCF::String ClassRegistryEntry::toString()
{
	return "classID = " + m_classID + "\nLocal Server Path = " + m_localServerPath;
}