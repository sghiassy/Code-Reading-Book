//////////////////////////////////////////////////////////////////////
// FILE: dgl_dllscriptfuncs.cpp
// PURPOSE: in here are all the script functions that are needed to handle scripts
//////////////////////////////////////////////////////////////////////
// Part of DemoGL Demo System sourcecode. See version information
//////////////////////////////////////////////////////////////////////
// COPYRIGHTS:
// Copyright (c)1999-2001 Solutions Design. All rights reserved.
// Central DemoGL Website: www.demogl.com.
// 
// Released under the following license: (BSD)
// -------------------------------------------
// Redistribution and use in source and binary forms, with or without modification, 
// are permitted provided that the following conditions are met: 
//
// 1) Redistributions of source code must retain the above copyright notice, this list of 
//    conditions and the following disclaimer. 
// 2) Redistributions in binary form must reproduce the above copyright notice, this list of 
//    conditions and the following disclaimer in the documentation and/or other materials 
//    provided with the distribution. 
// 
// THIS SOFTWARE IS PROVIDED BY SOLUTIONS DESIGN ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, 
// INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A 
// PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL SOLUTIONS DESIGN OR CONTRIBUTORS BE LIABLE FOR 
// ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
// NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR 
// BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, 
// STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE 
// USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
//
// The views and conclusions contained in the software and documentation are those of the authors 
// and should not be interpreted as representing official policies, either expressed or implied, 
// of Solutions Design. 
//
// See also License.txt in the sourcepackage.
//
//////////////////////////////////////////////////////////////////////
// Contributers to the code:
//		- Frans Bouma [FB]
//////////////////////////////////////////////////////////////////////
// VERSION INFORMATION.
//
// v1.3: New parser and extensible syntax support, some fixes.
// v1.2: Some fixes.
// v1.1: Added to codebase.
//////////////////////////////////////////////////////////////////////

#include "DemoGL\dgl_dllstdafx.h"

///////////////////////////////////////////////////////////////////////
//
// Global variables for tokens from script. Have file (.obj) scope.
//
///////////////////////////////////////////////////////////////////////

// scripttokens
CToken			m_garrtTokens[SCR_MAXTOKENSPERROW];
int				m_giAmTokensInLine;

// CmdToken array of SCmdTokens. Needed for lexical analyzer and ScriptCommandToText routine
SCmdToken	m_garrCmdTokens[CT_MAXCMDTOKENS]=
{
	{CT_NOP,"NOP"},
	{CT_START,"START"},
	{CT_LOAD,"LOAD"},
	{CT_PAUSE,"PAUSE"},
	{CT_END,"END"},
	{CT_CONTINUE,"CONTINUE"},
	{CT_PREPARE,"PREPARE"},
	{CT_STOP,"STOP"},
	{CT_MOVETOLYR,"MOVETOLYR"},
	{CT_HIDE,"HIDE"},
	{CT_SHOW,"SHOW"},
	{CT_SEND,"SEND"},
	{CT_SETTIME,"SETTIME"},
	{CT_SEEK,"SEEK"},
	{CT_CAPTUREMOUSE,"CAPTUREMOUSE"},
	{CT_LOADMP3,"LOADMP3"},
	{CT_PLAYMP3,"PLAYMP3"},
	{CT_STOPMP3,"STOPMP3"},
	{CT_PAUSEMP3,"PAUSEMP3"},
	{CT_CONTINUEMP3,"CONTINUEMP3"},
	{CT_SEEKINMP3,"SEEKINMP3"},
	{CT_EAXMIXMP3,"EAXMIXMP3"},
	{CT_SETVOLUMEMP3,"SETVOLUMEMP3"},
	{CT_SETPANMP3,"SETPANMP3"},
	{CT_FREEMP3,"FREEMP3"},
	{CT_LOADMOD,"LOADMOD"},
	{CT_PLAYMOD,"PLAYMOD"},
	{CT_STOPMOD,"STOPMOD"},
	{CT_PAUSEMOD,"PAUSEMOD"},
	{CT_CONTINUEMOD,"CONTINUEMOD"},
	{CT_SEEKINMOD,"SEEKINMOD"},
	{CT_EAXMIXMOD,"EAXMIXMOD"},
	{CT_SETVOLUMEMOD,"SETVOLUMEMOD"},
	{CT_SETPANMOD,"SETPANMOD"},
	{CT_FREEMOD,"FREEMOD"},
	{CT_LOADSMPL,"LOADSMPL"},
	{CT_PLAYSMPL,"PLAYSMPL"},
	{CT_STOPSMPL,"STOPSMPL"},
	{CT_PAUSESMPL,"PAUSESMPL"},
	{CT_CONTINUESMPL,"CONTINUESMPL"},
	{CT_SEEKINSMPL,"SEEKINSMPL"},
	{CT_EAXMIXSMPL,"EAXMIXSMPL"},
	{CT_SETVOLUMESMPL,"SETVOLUMESMPL"},
	{CT_SETPANSMPL,"SETPANSMPL"},
	{CT_FREESMPL,"FREESMPL"},
	{CT_SETEAXENV,"SETEAXENV"},
	{CT_SETVOLUME,"SETVOLUME"},
	{CT_STOPALL,"STOPALL"},
	{CT_PAUSEALL,"PAUSEALL"},
	{CT_CONTINUEALL,"CONTINUEALL"},
	{CT_RESTART,"RESTART"},
	{CT_SENDINT,"SENDINT"},
	{CT_SENDFLOAT,"SENDFLOAT"},
	{CT_SENDSTRING,"SENDSTRING"}
	//////////////////////////
	// ADD MORE HERE (don't forget the comma-issue ;))
	//////////////////////////
};

////////////////////////////////////////////////////////////////////////
//                          
//						CODE
//
////////////////////////////////////////////////////////////////////////
// Purpose: this routine is the general script loader and parser. It loads the script, parses every line, and
// generates TimeLineEvent objects, if defined. 
//
// Additional info: The textfile is loaded with FLoadFile, and therefor is padded with a '0'. It's then safe to use
// string search routines.
int
LoadAndParseScript()
{
	int				iLineLength,iFileLen, iCLCounter, iLAResult, iLenRead, iLenToRead;
	CStdString		sError;
	char			sCLContent[SCR_MAXLINELENGTH+1];	// add 1 for appended 0
	char			*pCurrentLine, *pCRLFPos;
	bool			bEOFreached;
	byte			*pFileData; 
	CTimeLineEvent	*pTLE;

	pFileData = FLoadFile(m_gpDemoDat->GetScriptFilename());
	if(!pFileData)
	{
		sError.Format("Could not open or load scriptfile %s.",m_gpDemoDat->GetScriptFilename());
		LogFeedback(&sError[0],true,"LoadAndParseScript",false);
		return SYS_LPS_FILENOTFOUND;
	}

	// get length
	iFileLen = FFileLength(m_gpDemoDat->GetScriptFilename());

	if(iFileLen == SYS_FILENOTFOUND)
	{
		free(pFileData);
		sError.Format("Could not open or load scriptfile %s.",m_gpDemoDat->GetScriptFilename());
		LogFeedback(&sError[0],true,"LoadAndParseScript",false);
		return SYS_LPS_FILENOTFOUND;
	}

	// parse it line for line
	// NOTE: pCurrentLine is updated during the routine. 
	for(pCurrentLine = (char *)pFileData, iCLCounter=0, bEOFreached=false, iLenRead=0;(!bEOFreached)&&(iLenRead < iFileLen);)
	{
		// clear linebuffer
		memset(sCLContent,0,SCR_MAXLINELENGTH+1);

		// read line
		if((iFileLen - iLenRead) > SCR_MAXLINELENGTH)
		{
			// we'll NOT reach the EOF this readcycle.
			iLenToRead=SCR_MAXLINELENGTH;
		}
		else
		{
			// we CAN reach the end this cycle. 
			iLenToRead=(iFileLen-iLenRead)+1;
		}
		// find CRLF. THerefor search for the pos of '13'. 
		// file is padded with 0 so search will always stop.
		pCRLFPos = strchr(pCurrentLine,13);
		// now we have to copy the line into the linebuffer to get it scanned.
		if(!pCRLFPos)
		{
			// not found. we reached the end of the file.
			iLineLength=iLenToRead;
			bEOFreached = true;
			iCLCounter++;
		}
		else
		{
			iLineLength = pCRLFPos - pCurrentLine;
			if(iLineLength> SCR_MAXLINELENGTH)
			{
				iLineLength = SCR_MAXLINELENGTH;
			}
			else
			{
				iCLCounter++;
			}
		}
		strncpy(sCLContent, pCurrentLine, iLineLength);

		if(!bEOFreached)
		{
			// only add these when we're not at the end of the file. 
			pCurrentLine += iLineLength+2;		// add 2 to skip CRLF
			iLenRead+=iLineLength+2;
		}

		// tokenize read line
		iLAResult=Lex(sCLContent);
		switch(iLAResult)
		{
			case LA_ISCOMMENT:
			case LA_ISEMPTY:
			{
				// lines will be skipped.
				continue;
			}; break;
			case LA_TOKENIZED:
			{
				// line is tokenized. Enter tokenparser, which will convert the tokens into objects which can be used.
				pTLE = ParseTokens(iCLCounter, true);
				if(!pTLE)
				{
					// there was an error. Log the line to the console
					sError.Format("Error in scriptline %d: %s. Line Skipped.", iCLCounter, &sCLContent[0]);
					LogFeedback(&sError[0],true,"LoadAndParseScript",false);
					continue;
				}
				else
				{
					// went ok, insert in timeline
					InsertTimeLineEvent(pTLE);
					continue;
				}
			}; break;
		}
	}
	free(pFileData);

	// Clear tokenarray, we don't need the data in there.
	InitTokenArray();
	// return ok.
	return SYS_OK;
}


// Purpose: parses the tokens, that are a result of the lex scan. It will then emit a TimeLineEvent object
//          and return a pointer to that timeline event. If error, then NULL is returned.
// Additional Info: It can now only parse TimeLineEvents (because that are the only things that are specified
//                  in the scriptfile.
CTimeLineEvent
*ParseTokens(int iCLCounter, bool bErrorMsgs)
{
	CStdString			sLog;
	CTimeLineEvent		*pNewTLE;
	int					iToken;

	// first token has to be a numeric token (i.e. the timeline spot)
	if(m_garrtTokens[0].GetToken()!=TK_NUMERIC)
	{
		// syntax error
		if(bErrorMsgs)
		{
			sLog.Format("Syntax error in line %d: Missing timespecification.", iCLCounter);
			LogFeedback(&sLog[0],true,"ParseTokens",false);
		}
		return NULL;
	}

	// second token has to be a string. (i.e. the objectname)
	if(m_garrtTokens[1].GetToken()!=TK_STRING)
	{
		// syntax error
		if(bErrorMsgs)
		{
			sLog.Format("Syntax error in line %d: Missing Objectname.", iCLCounter);
			LogFeedback(&sLog[0],true,"ParseTokens",false);
		}
		return NULL;
	}

	// third token has to be a command.
	if(m_garrtTokens[2].GetToken()!=TK_COMMAND)
	{
		// syntax error
		if(bErrorMsgs)
		{
			sLog.Format("Syntax error in line %d: Missing commandspecification.", iCLCounter);
			LogFeedback(&sLog[0],true,"ParseTokens",false);
		}
		return NULL;
	}

	// Check parameters. If there are 'TK_COMMAND' or 'TK_NUMERIC' tokens, modify these into TK_STRING tokens.
	// All tokens are stored as string, so numeric tokens can be converted to stringtokens without trouble.
	for(iToken=3;iToken<m_giAmTokensInLine;)
	{
		if((m_garrtTokens[iToken].GetToken()==TK_COMMAND)||(m_garrtTokens[iToken].GetToken()==TK_NUMERIC))
		{
			m_garrtTokens[iToken].SetToken(TK_STRING);
		}
		iToken++;
	}

	// TimeLineEvent is specified correctly. Emit timeline event and add it to the linked list.
	pNewTLE = new CTimeLineEvent();
	
	// store command and properties
	pNewTLE->SetCommand(m_garrtTokens[2].GetTokenDataC());
	pNewTLE->SetObject(m_garrtTokens[1].GetTokenDataS());
	pNewTLE->SetTimeSpot(atol(m_garrtTokens[0].GetTokenDataS()));
	
	// store commandparameters if any.
	for(iToken=3;iToken<m_giAmTokensInLine;)
	{
		pNewTLE->SetCommandParam(m_garrtTokens[iToken].GetTokenDataS(),(iToken-3));				// string value
		pNewTLE->SetCommandParam((float)atof(m_garrtTokens[iToken].GetTokenDataS()),(iToken-3));		// float value (will be 0 if not a float)
		pNewTLE->SetCommandParam(atoi(m_garrtTokens[iToken].GetTokenDataS()),(iToken-3));		// int value (will be 0 if not an int)
		iToken++;
	}
	// Done, return TimeLineEvent object
	return pNewTLE;
}


// Purpose: initializes tokenarray so ParseTokens can stop with parsing when it sees a TK_EMPTY
void
InitTokenArray()
{
	int			i;
	
	for(i=0;i<SCR_MAXTOKENSPERROW;i++)
	{
		m_garrtTokens[i].SetToken(TK_EMPTY);
		m_garrtTokens[i].SetTokenDataS("");
	}
	m_giAmTokensInLine=0;
}


// Purpose: skips empty space in given string. it will return a pointer to the first NON Space character OR
// NULL if EOL reached.
// Whitespace are all characters <= 32
char
*SkipWhiteSpace(char *sToSkip)
{
	int		iLen, iCurrChar, i;

	iLen = strlen(sToSkip);
	for(i=0;(i<iLen)&&((int)sToSkip[i]!=0);i++)
	{
		iCurrChar = (int)sToSkip[i];
		if(iCurrChar>32)
		{
			// first nonwhitespace found.
			return sToSkip+i;
		}
	}

	// not found.. return NULL
	return NULL;
}


// Purpose: trims whitespace behind the string passed. It will move the'\0' marker to the front in the buffer
// if whitespace is found at the end.
void
TrimWhiteSpaceR(char *sString)
{
	int			iStringLen, i;
	char		iCurrChar;

	iStringLen=strlen(sString);
	if(!iStringLen)
	{
		return;
	}

	for(i=0;i<iStringLen;i++)
	{
		iCurrChar=(int)sString[(iStringLen-1)-i];
		if(iCurrChar>32)
		{
			sString[((iStringLen-1)-i)+1]='\0';
			return;
		}
	}
}


// Purpose: checks if a given string consists of ONLY numeric digits. it will stop at the first whitespace, if available and will
// skip those whitespace. If it encounters a nonwhitespace nonnumeric digit, false is returned, true otherwise.
bool
IsNumeric(char *sToCheck)
{
	int			i, iLen;

	iLen=strlen(sToCheck);
	if(!iLen)
	{
		return false;
	}

	for(i=0;i<iLen;i++)
	{
		if((isdigit((int)sToCheck[i]))||(isspace((int)sToCheck[i]))||(sToCheck[i]=='-'))
		{
			continue;
		}
		// bad character... return false (it's not numeric)
		return false;
	}

	return true;
}


// Purpose: lexical analizes a passed line. If it's a tokenizable line, it will tokenize it and place the tokens into
// the tokenstructure that is defined for Lex and ParseTokens, m_arrtTokens[].
// It returns it's results by using predefined returnconstants. See DemoGL_Main.h for details.
// In: sCLContent, current line in characters. EOF/EOL included.
//     iCLCounter, current line counter for errorlogging.
// Out: resultvalue. See DemoGL_Main.h
int
Lex(char *sCLContent)
{
	int		iLen, iToken, i;
	char	*pCurrent;
	char	sCurrentToken[SCR_MAXLINELENGTH], sCTOrgTxt[SCR_MAXLINELENGTH];
	bool	bCmdFound;

	// initialize array and tokencounter.
	InitTokenArray();
	
	pCurrent = SkipWhiteSpace(sCLContent);
	if(!pCurrent)
	{
		// empty line.
		return LA_ISEMPTY;
	}

	// check if commentline. 
	if(pCurrent[0]==(char)SCR_COMMENTMARKER)
	{
		//yup. 
		return LA_ISCOMMENT;
	}

	// not empty, not comment. should be a timeline event.
	for(iToken=0;(pCurrent!=NULL);pCurrent=SkipWhiteSpace(pCurrent+iLen+1),iToken++, m_giAmTokensInLine++)
	{
		iLen = strcspn(pCurrent,SCR_DELIMITER);
		strncpy(sCurrentToken,pCurrent,iLen);
		sCurrentToken[iLen]=0;
		// trim whitespace behind the token
		TrimWhiteSpaceR(sCurrentToken);
		// copy the original text of the token to a temp string, so if it's a param we can
		// store the original into the param string slot.
		strcpy(sCTOrgTxt,sCurrentToken);
		// Convert to uppercase;
		_strupr(sCurrentToken);
		
		// check what kind of thing this is we just scanned.
		// check numeric.
		if(IsNumeric(sCurrentToken))
		{
			// yup.
			m_garrtTokens[iToken].SetToken(TK_NUMERIC);
			m_garrtTokens[iToken].SetTokenDataS(sCurrentToken);
			continue;
		}

		// it is a string. Check if it's a command. If a person specifies a commandstring as a parameter
		// it will end up ok. All parsers do this internally :)
		for(i=0, bCmdFound=false;i<CT_MAXCMDTOKENS;i++)
		{
			if(!strcmp(sCurrentToken,m_garrCmdTokens[i].m_sCmdToken))
			{
				m_garrtTokens[iToken].SetToken(TK_COMMAND);
				m_garrtTokens[iToken].SetTokenDataS(sCurrentToken);
				m_garrtTokens[iToken].SetTokenDataC(m_garrCmdTokens[i].m_iCmdToken);
				bCmdFound=true;
				break;
			}
		}

		if(!bCmdFound)
		{
			// it's NOT a command. set it as a string
			m_garrtTokens[iToken].SetToken(TK_STRING);
			m_garrtTokens[iToken].SetTokenDataS(sCTOrgTxt);
		}
	}

	return LA_TOKENIZED;
}


// Purpose: returns a string according to the passed Scriptcommand.
char
*ScriptCommandToText(int iCommand)
{
	int		i;

	for(i=0;i<CT_MAXCMDTOKENS;i++)
	{
		if(m_garrCmdTokens[i].m_iCmdToken==iCommand)
		{
			return m_garrCmdTokens[i].m_sCmdToken;
		}
	}

	// Not found
	return "Unkown";
}

