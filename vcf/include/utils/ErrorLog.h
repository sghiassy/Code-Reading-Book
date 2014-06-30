//ErrorLog.h

/**
This program is free software; you can redistribute it and/or
modify it as you choose. In fact, you can do anything you would like
with it, so long as credit is given if used in commercial applications.
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

NB: This software will not save the world. 
*/

#ifndef _ERRORLOG_H__
#define _ERRORLOG_H__



namespace VCF  {

/**
*An ErrorLog represents a place to output status 
*during the life of a running application. It takes
*care of managing the existence of a an error log on 
*the system, and updating it with data.
*/
class FRAMEWORK_API ErrorLog : public Object { 
public:
	/**
	*@param String the name of the log file 
	*to create. Must be a fully qualified path
	*/
	ErrorLog( const String& ouputFilename = "" );

	virtual ~ErrorLog();
	
	/**
	*send the string data contained in the text 
	*parameter to the log. This is currently a 
	*a blocking call, adn does not support asynchronous
	*file I/O. 
	*@param String the status data to send to the log file
	*/
	virtual void toLog( const String& text );
protected:

private:
	String m_outputFile;
	FileStream* m_fileStream;
	TextOutputStream* m_textOutStream;
	OutputStream* m_errStream;
};


}; //end of namespace VCF

#endif //_ERRORLOG_H__


