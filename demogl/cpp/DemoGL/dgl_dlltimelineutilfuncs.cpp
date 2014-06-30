//////////////////////////////////////////////////////////////////////
// FILE: dgl_dlltimelineutilfuncs.cpp
// PURPOSE: in here are all the timeline related functions 
//////////////////////////////////////////////////////////////////////
// SHORT DESCRIPTION:
// this is the file with the timeline related functions. If it's timeline
// related it's in here, except wrapper functions who are called / used
// by the kernel.
// 
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
// v1.3: For 80% recoded.
// v1.2: Added major changes/fixes
// v1.1: Added to codebase.
//////////////////////////////////////////////////////////////////////

#include "DemoGL\dgl_dllstdafx.h"

////////////////////////////////////////////////////////////////////////
//                          
//						CODE
//
////////////////////////////////////////////////////////////////////////


// Purpose: deletes the passed event from the double linked list. It assumes it is PART of that list.
// it fixes the pointers on the prev and next side, and also checks if the head is deleted now or not.
void
DeleteTimeLineEvent(CTimeLineEvent *pEvent)
{
	if(!pEvent)
	{
		return;
	}
	if(pEvent==m_gpTimeLineHead)
	{
		m_gpTimeLineHead=pEvent->GetNext();
		delete pEvent;
		return;
	}
	pEvent->GetPrev()->SetNext(pEvent->GetNext());
	pEvent->GetNext()->SetPrev(pEvent->GetPrev());
	delete pEvent;
}


// Purpose: inserts a new eventobject in the double linked timeline list
void
InsertTimeLineEvent(CTimeLineEvent *pEvent)
{
	CTimeLineEvent	*pCurr;

	pCurr=m_gpTimeLineHead;
	if(!pCurr)
	{
		// first node.
		m_gpTimeLineHead=pEvent;
		return;
	}

	// find the first event that has a timespot that is > than the timespot of the
	// to be inserted event. 
	for(;(pCurr->GetTimeSpot()<=pEvent->GetTimeSpot())&&(pCurr->GetNext());)
	{
		if(pCurr->GetNext())
		{
			pCurr=pCurr->GetNext();
		}
	}

	// OR pcurr's timespot is > than pEvent's OR we're at the last one.
	if(!pCurr->GetNext())
	{
		// pCurr's timespot is <= pEvent's
		pCurr->SetNext(pEvent);
		pEvent->SetPrev(pCurr);
		return;
	}

	// pCurr's timespot is > pEvent's. 
	if(!pCurr->GetPrev())
	{
		// pEvent will get the head.
		m_gpTimeLineHead=pEvent;
		pEvent->SetNext(pCurr);
		pCurr->SetPrev(pEvent);
		return;
	}
	else
	{
		pCurr->GetPrev()->SetNext(pEvent);
		pEvent->SetPrev(pCurr->GetPrev());
		pEvent->SetNext(pCurr);
		pCurr->SetPrev(pEvent);
	}
}


// Purpose: moves the CurrentTLE pointer (pointer to Current TimeLineEvent(TLE)) to the timespot
// given. If a timespot of a TLE is >= than the timespot given, that TLE is selected as current.
// As ExecuteTimeLine, but without execution. lTimeSpot in milliseconds
void
MoveCurrentTLEToTimeSpot(long lTimeSpot)
{
	bool				bFinished;
	CTimeLineEvent		*pTLE;

	pTLE=m_gpTimeLineHead;
	bFinished=(pTLE==NULL);
	while(!bFinished)
	{
		if(!pTLE)
		{
			bFinished=true;
			continue;
		}
		if(pTLE->GetTimeSpot()>=lTimeSpot)
		{
			// finished for this cycle
			bFinished = true;
			continue;
		}
		// valid, move to next.
		pTLE=pTLE->GetNext();
	}
	// store new current timeline event pointer.
	m_gpDemoDat->SetCurrentTLE(pTLE);
}


// Purpose: displayes the complete timeline with all events defined and the current spot on that timeline
void
DisplayTimeLineInConsole()
{
	CTimeLineEvent		*pTLE;
	int					i;
	char				sYesNo[3];
	long				lCurrentTimeSpotMS;

	lCurrentTimeSpotMS=(long)(m_gpDemoDat->GetfElapsedTimeInSecs() * 1000.0f);

	// display header
	DEMOGL_ConsoleLogLine(true, " ");
	DEMOGL_ConsoleLogLineV(true, "Current timespot: %.10d ms.",lCurrentTimeSpotMS);
	DEMOGL_ConsoleLogLine(true, "Executed | TimeSpot(ms)| Event data");
	DEMOGL_ConsoleLogLine(true, "---------+-------------+--------------------------------------------------");

	if(!m_gpTimeLineHead)
	{
		// no events defined.
		DEMOGL_ConsoleLogLine(true, " No timeline events defined.");
		DEMOGL_ConsoleLogLine(true, "---------+-------------+--------------------------------------------------");
	}
	else
	{
		// walk every timeline event in the chain
		for(pTLE=m_gpTimeLineHead;pTLE;pTLE=pTLE->GetNext())
		{
			if(pTLE->GetTimeSpot() > lCurrentTimeSpotMS)
			{
				sprintf(sYesNo,"%3s","No ");
			}
			else
			{
				sprintf(sYesNo,"%3s","Yes");
			}
			if(pTLE->GetTimeSpot() < 0)
			{
				      DEMOGL_ConsoleLogLineV(true, "  %3s    | %.10d | Object      : %-s",sYesNo,pTLE->GetTimeSpot(),pTLE->GetObject());
			}
			else
			{
				     DEMOGL_ConsoleLogLineV(true, "  %3s    |  %.10d | Object      : %-s",sYesNo,pTLE->GetTimeSpot(),pTLE->GetObject());
			}
	        DEMOGL_ConsoleLogLineV(true, "         |             | Command     : %-s",ScriptCommandToText(pTLE->GetCommand()));
	
			for(i=0;i<pTLE->GetAmountParams();i++)
			{
		        DEMOGL_ConsoleLogLineV(true, "         |             | Parameter %02d: %s",i,pTLE->GetCommandParamString(i));
			}
			DEMOGL_ConsoleLogLine(true, "---------+-------------+--------------------------------------------------");
		}
	}
}


// Purpose: browses the timeline and executes all events that have timespots <= lMSPassed.
// The current milliseconds passed does not have to be the same as the global amount
// stored in the this object, due to pre-run-execution of the timeline in the prepare()
// function.
// Additional info: the timeline is sorted from 0 to maxtimespot specified. So if we spot a
// timespot > lMSPassed, we can savely return.
void
ExecuteTimeLine(long lMSPassed)
{
	bool				bFinished;
	CTimeLineEvent		*pTLE;

	pTLE=m_gpDemoDat->GetCurrentTLE();
	bFinished=(pTLE==NULL);
	while(!bFinished)
	{
		if(!pTLE)
		{
			bFinished=true;
			continue;
		}
		if(pTLE->GetTimeSpot()>lMSPassed)
		{
			// finished for this cycle
			bFinished = true;
			continue;
		}
		// it's a valid timeline event... execute it!
		DoExecuteTimelineEvent(pTLE, lMSPassed>=0);
		pTLE=pTLE->GetNext();
	}
	// store new current timeline event pointer.
	m_gpDemoDat->SetCurrentTLE(pTLE);
}


// Purpose: executes the timelineevent object passed.
void
DoExecuteTimelineEvent(CTimeLineEvent *pTLE, bool bSameThread)
{
	int				iCommand, iElementType, iResult, iElementCodeID, iChannelCodeID;
	char			*sObject;
	CEffect			*pEffectObject;
	CStdString		sError, sToLog;
	long			lLongParam;
	bool			bReturnValue;	


	sObject=pTLE->GetObject();
	iCommand = pTLE->GetCommand();

	/////////////////////////////////////////////////////////
	// NOTE to readers of this sourcecode:
	//
	// This is the largest routine in DemoGL. There is NO size optimising code in here (as in: use 1 command
	// interpreter for 2 commands that look alike (like LOADMP3 and LOADMOD), because these things will be
	// BUG intensive when the syntax of the commands changes. Now every command of every object
	// (_MUSICSYSTEM, _SYSTEM, _SOUNDSYSTEM, or effectobject) has it's own commandexecutor routine. 
	//
	// Keep it that way, because only THIS way it's the most flexible implementation!.
	// There is a lot of errorreporting code in this routine.
	// This can't be stripped from the routine IMHO because it's important that errors are logged back into the
	// console so a developer can see what went wrong where and why. (without this code it would be fairly small :))
	//
	// This routine should probably move to it's own .cpp file, but it's the heart of the control system together
	// with the kernel, therefor it stays in this file IMHO for now.
	//
	// [FB] on 25th of october 2000
	////////////////////////////////////////////////////////

	
	/////////////////////////////////////////////////////////
	//
	//		_MUSICSYSTEM. Deprecated system. For backwards compatibility.
	//      Will use _SOUNDSYSTEM to produce music. Only 1 element will work: 0.
	//      If you load 2 mp3's the first will work.
	//
	//      The code seems redundant, but the _MUSICSYSTEM will keep it's own set of code
	//      so the setup of the code in _SOUNDSYSTEM can be totally different without worries about
	//      breaking backwards compatibility.
	//
	//		The soundelements, MOD or MP3, are loaded using the ElementScriptID "__MSE" and ChannelScriptID "__MSC".
	//
	////////////////////////////////////////////////////////

	if(!strcmp(sObject,_MUSICSYSTEM))
	{
		// it's the musicsystem. 
		// check if the user has sound enabled.
		if(m_gpDemoDat->GetbSound())
		{
			switch(m_gpDemoDat->GetMusicSubSystemType())
			{
				case MP3SUBSYSTEM:
				{
					iElementType=DGL_SS_MP3ELEMENT;
				}; break;
				case MODSUBSYSTEM:
				{
					iElementType=DGL_SS_MODELEMENT;
				}; break;
			}
			switch(iCommand)
			{
				case CT_LOAD:
				{
					// load the file.
					iResult=m_gpSoundSystem->LoadSoundElement(pTLE->GetCommandParamString(0),iElementType,"__MSE",false);
					if(iResult < 0)
					{
						sError.Format("Error in command: %d;_MUSICSYSTEM;LOAD;%s;",pTLE->GetTimeSpot(),
										pTLE->GetCommandParamString(0));
						LogFeedback(&sError[0],true,"DoExecuteTimeLine:_MS:CT_LOAD",false);
						// done
						break;
					}
					sToLog.Format("Sound element with name %s loaded succesfully.",pTLE->GetCommandParamString(0));
					LogFeedback(&sToLog[0],true,"DoExecuteTimeLine:_MS:CT_LOAD",true);
				}; break;
				case CT_START:
				{
					// Can't use float/int values of parameter
					// check for hexadecimal parameter.
					if(!strchr(pTLE->GetCommandParamString(0),'x'))
					{
						// not hexadecimal
						lLongParam = (long)pTLE->GetCommandParamInt(0);
					}
					else
					{
						// hexadecimal, use sscanf trick 
						sscanf(pTLE->GetCommandParamString(0),"%x",&lLongParam);
					}
					if(lLongParam<0)
					{
						// illegal parameter passed.
						sError.Format("Illegal parameter passed to START of _MUSICSYSTEM command: %s. Ignored.", pTLE->GetCommandParamString(0));
						LogFeedback(&sError[0],true,"ExecuteTimeLine:_MS:CT_START",false);
						break;
					}
					iElementCodeID=m_gpSoundSystem->ConvertElementScriptID2ElementCodeID("__MSE");
					iResult=m_gpSoundSystem->StartElement(iElementCodeID,iElementType,lLongParam,false,"__MSC",NULL);
					if(iResult >= 0)
					{
						sToLog.Format("Sound element %d started on channel %d.",iElementCodeID,iResult);
						LogFeedback(&sToLog[0],true,"ExecuteTimeLine:_MS:CT_START",true);
					}
				}; break;
				case CT_PAUSE:
				{
					iChannelCodeID=m_gpSoundSystem->ConvertChannelScriptID2ChannelCodeID("__MSC");
					iResult=m_gpSoundSystem->PauseChannel(iChannelCodeID);
					if(iResult==SYS_OK)
					{
						sToLog.Format("Sound channel %d play paused.",iChannelCodeID);
						LogFeedback("",true,"ExecuteTimeLine:_MS:CT_PAUSE",true);
					}
				}; break;
				case CT_STOP:
				{
					iChannelCodeID=m_gpSoundSystem->ConvertChannelScriptID2ChannelCodeID("__MSC");
					iResult=m_gpSoundSystem->StopChannel(iChannelCodeID);
					if(iResult==SYS_OK)
					{
						sToLog.Format("Sound channel %d play stopped.",iChannelCodeID);
						LogFeedback(&sToLog[0],true,"ExecuteTimeLine:_MS:CT_STOP",true);
					}
				}; break;
				case CT_END:
				{
					iElementCodeID=m_gpSoundSystem->ConvertElementScriptID2ElementCodeID("__MSE");
					iResult=m_gpSoundSystem->FreeSoundElement(iElementCodeID,iElementType, false);
					if(iResult==SYS_OK)
					{
						sToLog.Format("Sound channel %d ended and removed.",iElementCodeID);
						LogFeedback(&sToLog[0],true,"ExecuteTimeLine:_MS:CT_END",true);
					}
				}; break;
				case CT_SEEK:
				{
					// check for hexadecimal parameter.
					if(!strchr(pTLE->GetCommandParamString(0),'x'))
					{
						// not hexadecimal
						lLongParam = (long)pTLE->GetCommandParamInt(0);
					}
					else
					{
						// hexadecimal, use sscanf trick 
						sscanf(pTLE->GetCommandParamString(0),"%x",&lLongParam);
					}
					if(lLongParam<0)
					{
						// illegal. ignore command
						sError.Format("Illegal parameter passed to SEEK of _MUSICSYSTEM command: %s. Ignored.", pTLE->GetCommandParamString(0));
						LogFeedback(&sError[0],true,"ExecuteTimeLine:_MS:CT_SEEK",false);
					}
					else
					{
						iChannelCodeID=m_gpSoundSystem->ConvertChannelScriptID2ChannelCodeID("__MSC");
						iResult=m_gpSoundSystem->SeekInChannel(iChannelCodeID,lLongParam);
						if(iResult==SYS_OK)
						{
							sToLog.Format("Seeking to timeposition %d in sound channel %d succeeded.", lLongParam, iChannelCodeID);
							LogFeedback(&sToLog[0],true,"ExecuteTimeLine:_MS:CT_SEEK",true);
						}
					}
				}; break;
				case CT_CONTINUE:
				{
					iChannelCodeID=m_gpSoundSystem->ConvertChannelScriptID2ChannelCodeID("__MSC");
					iResult=m_gpSoundSystem->ContinueChannel(iChannelCodeID);
					if(iResult==SYS_OK)
					{
						sToLog.Format("Sound channel %d play continued.", iChannelCodeID);
						LogFeedback(&sToLog[0],true,"ExecuteTimeLine:_MS:CT_CONTINUE",true);
					}
				}; break;
				case CT_NOP:
				{
					// no operation.
				}; break;
				///////////
				//// ADD MORE COMMANDS FOR _MUSICSYSTEM HERE
				///////////
				default:
				{
					// Illegal command.
					sError.Format("Illegal Command specified with _MUSICSYSTEM: %d",iCommand);
					LogFeedback(&sError[0],true,"ExecuteTimeLine:_MS:default",false);
				}; break;
			}
		}
		// done
		return;
	}

	///////////////////////////////////////////
	//
	//			_SYSTEM
	//
	///////////////////////////////////////////
	if(!strcmp(sObject,_SYSTEM))
	{
		// it's the main system. 
		 // execute command
		switch(iCommand)
		{
			case CT_END:
			{
				// post message we should end.
				PostQuitMessage(0);
				LogFeedback("Ending application.",true,"ExecuteTimeLine:_SYS:CT_END",true);
				bReturnValue=true;
			}; break;
			case CT_HIDE:
			{
				// Hide the layer.
				lLongParam = atol(pTLE->GetCommandParamString(0));
				if(lLongParam<0)
				{
					// illegal parameter passed.
					sError.Format("Illegal parameter passed to HIDE command: %s. Ignored.", pTLE->GetCommandParamString(0));
					LogFeedback(&sError[0],true,"ExecuteTimeLine:_SYS:CT_HIDE",false);
				}
				else
				{
					HideLayer(lLongParam);
					sToLog.Format("Hiding layer %d.", atol(pTLE->GetCommandParamString(0)));
					LogFeedback(&sToLog[0],true,"ExecuteTimeLine:_SYS:CT_HIDE",true);
				}
			}; break;
			case CT_SHOW:
			{
				// Show the layer.
				lLongParam = atol(pTLE->GetCommandParamString(0));
				if(lLongParam<0)
				{
					// illegal parameter passed.
					sError.Format("Illegal parameter passed to SHOW command: %s. Ignored.", pTLE->GetCommandParamString(0));
					LogFeedback(&sError[0],true,"ExecuteTimeLine:_SYS:CT_SHOW",false);
				}
				else
				{
					ShowLayer(lLongParam);
					sToLog.Format("Showing layer %d.", lLongParam);
					LogFeedback(&sToLog[0],true,"ExecuteTimeLine:_SYS:CT_SHOW",true);
				}
			}; break;
			case CT_SETTIME:
			{
				// sets the current passed amount of milliseconds to the value in the parameter.
				lLongParam = atol(pTLE->GetCommandParamString(0));
				if(lLongParam<=0)
				{
					// illegal. ignore command
					sError.Format("Illegal parameter passed to SETTIME command: %s. Ignored.", pTLE->GetCommandParamString(0));
					LogFeedback(&sError[0],true,"ExecuteTimeLine:_SYS:CT_SETTIME",false);
				}
				else
				{
					m_gpDemoDat->SetTimePassed(lLongParam);
					sToLog.Format("Setting time to %d",lLongParam);
					LogFeedback(&sToLog[0],true,"ExecuteTimeLine:_SYS:CT_SETTIME",true);
				}
			}; break;
			case CT_CAPTUREMOUSE:
			{
				if(!strcmp(pTLE->GetCommandParamString(0),"ON"))
				{
					// set capturing on.
					SetCapture(m_gpDemoDat->GethWnd());
					LogFeedback("Mousecapturing is switched ON",true,"ExecuteTimeLine:_SYS:CT_CAPTUREMOUSE",true);
					break;
				}
				if(!strcmp(pTLE->GetCommandParamString(0),"OFF"))
				{
					// set capturing off
					ReleaseCapture();
					LogFeedback("Mousecapturing is switched OFF",true,"ExecuteTimeLine:_SYS:CT_CAPTUREMOUSE",true);
					break;
				}
				// illegal. ignore command
				sError.Format("Illegal parameter passed to CAPTUREMOUSE command: %s. Ignored.", pTLE->GetCommandParamString(0));
				LogFeedback(&sError[0],true,"ExecuteTimeLine:_SYS:CT_CAPTUREMOUSE",false);
			}; break;
			case CT_RESTART:
			{
				// test if the user wants all soundchannels to be killed.
				if(pTLE->GetCommandParamInt(3)==1)
				{
					// stop all sound.
					m_gpSoundSystem->StopAll();
				}

				// set starttime.
				m_gpDemoDat->SetCurrentTimeAtTickerReset((long)pTLE->GetCommandParamInt(0));

				// Test if the system should re-init effectobjects
				if(pTLE->GetCommandParamInt(1)==1)
				{
					// init effects. Ignore return result for now.
					InitAllRegisteredEffectObjects();
				}

				// set the exec pre-timespot events flag to the value requested in the script
				m_gpDemoDat->SetbExecPriorStartTimeTLEsAtReset(pTLE->GetCommandParamInt(2)==1);

				// restart the application. 
				PostMessage(m_gpDemoDat->GethWnd(),WM_DEMOGL_STARTKERNELLOOP,NULL,NULL);
			}; break;
		}
		// done
		return;
	}

	///////////////////////////////////////////
	//
	//			_SOUNDSYSTEM
	//
	///////////////////////////////////////////
	if(!strcmp(sObject,_SOUNDSYSTEM))
	{
		// it's the soundsystem. 
		// check if the user has sound enabled.
		if(m_gpDemoDat->GetbSound())
		{
			switch(iCommand)
			{
				/////////
				// MP3 Element type Commands
				/////////
				case CT_LOADMP3:
				{
					iResult=m_gpSoundSystem->LoadSoundElement(pTLE->GetCommandParamString(0),
										DGL_SS_MP3ELEMENT, pTLE->GetCommandParamString(1),
										(pTLE->GetCommandParamInt(2)==1));
					if(iResult<0)
					{
						// error while loading.
						sError.Format("Error in command: %d;_SOUNDSYSTEM;LOADMP3;%s;%s;%d;",pTLE->GetTimeSpot(),
								pTLE->GetCommandParamString(0),pTLE->GetCommandParamString(1),
								pTLE->GetCommandParamInt(2));
						LogFeedback(&sError[0],true,"ExecuteTimeLine:_SS:CT_LOADMP3",false);
						break;
					}
					sToLog.Format("On %d ms.: _SS: Sound element '%s' with Element ID %s loaded.",(long)(m_gpDemoDat->GetfElapsedTimeInSecs()*1000.0f),
							pTLE->GetCommandParamString(0),pTLE->GetCommandParamString(1));
					LogFeedback(&sToLog[0],true,"DoExecuteTimeLine:_SS:CT_LOADMP3",true);
				};break;
				case CT_PLAYMP3:
				{
					// Get the ElementCodeID using the scriptID
					iElementCodeID=m_gpSoundSystem->ConvertElementScriptID2ElementCodeID(pTLE->GetCommandParamString(0));
					if(iElementCodeID==DGL_SS_ERR_ILLEGALELEMENTID)
					{
						sError.Format("Error in command: %d;_SOUNDSYSTEM;PLAYMP3;%s;%d;%d;%d;%d;%s;. Element ID %s not found.",
								pTLE->GetTimeSpot(), pTLE->GetCommandParamString(0),
								pTLE->GetCommandParamInt(1),pTLE->GetCommandParamInt(2),
								pTLE->GetCommandParamInt(3),pTLE->GetCommandParamInt(4),
								pTLE->GetCommandParamString(5), pTLE->GetCommandParamString(0));
						LogFeedback(&sError[0],true,"ExecuteTimeLine:_SS:CT_PLAYMP3",false);
						// done
						break;
					}
					iResult=m_gpSoundSystem->StartElement(iElementCodeID, DGL_SS_MP3ELEMENT, 
									(long)pTLE->GetCommandParamInt(1),(pTLE->GetCommandParamInt(4)==1),
									pTLE->GetCommandParamString(5),NULL);
					if(iResult<0)
					{
						sError.Format("Error in command: %d;_SOUNDSYSTEM;PLAYMP3;%s;%d;%d;%d;%d;%s;. Couldn't start play.",
								pTLE->GetTimeSpot(), pTLE->GetCommandParamString(0),
								pTLE->GetCommandParamInt(1),pTLE->GetCommandParamInt(2),
								pTLE->GetCommandParamInt(3),pTLE->GetCommandParamInt(4),pTLE->GetCommandParamString(5));
						LogFeedback(&sError[0],true,"ExecuteTimeLine:_SS:CT_PLAYMP3",false);
						// done
						break;
					}
					// the result returned from the startelement call is the ChannelCodeID we need to use in the next calls.
					iChannelCodeID=iResult;
					// play is started. Now set some attributes. For frequency -1 is specified: leave current
					iResult=m_gpSoundSystem->SetChannelAttributes(iChannelCodeID, pTLE->GetCommandParamInt(2),
								pTLE->GetCommandParamInt(3),-1,	DGL_SS_SA_FREQUENCY|DGL_SS_SA_VOLUME|DGL_SS_SA_PAN);
					if(iResult==SYS_NOK)
					{
						sError.Format("Error in command: %d;_SOUNDSYSTEM;PLAYMP3;%s;%d;%d;%d;%d;%s;. Attribute error.",
								pTLE->GetTimeSpot(), pTLE->GetCommandParamString(0),
								pTLE->GetCommandParamInt(1),pTLE->GetCommandParamInt(2),
								pTLE->GetCommandParamInt(3),pTLE->GetCommandParamInt(4),pTLE->GetCommandParamString(5));
						LogFeedback(&sError[0],true,"ExecuteTimeLine:_SS:CT_PLAYMP3",false);
						break;
					}
					sToLog.Format("On %d ms.: _SS: Play of sound element %s started at pos. %d ms.",(long)(m_gpDemoDat->GetfElapsedTimeInSecs()*1000.0f),
							pTLE->GetCommandParamString(0),pTLE->GetCommandParamInt(1));
					LogFeedback(&sToLog[0],true,"DoExecuteTimeLine:_SS:CT_PLAYMP3",true);
				};break;
				case CT_STOPMP3:
				{
					// Get the ChannelCodeID using the scriptID
					iChannelCodeID=m_gpSoundSystem->ConvertChannelScriptID2ChannelCodeID(pTLE->GetCommandParamString(0));
					if(iChannelCodeID==DGL_SS_ERR_ILLEGALCHANNELID)
					{
						sError.Format("Error in command: %d;_SOUNDSYSTEM;STOPMP3;%s;. Channel ID %s not found.",
								pTLE->GetTimeSpot(), pTLE->GetCommandParamString(0), pTLE->GetCommandParamString(0));
						LogFeedback(&sError[0],true,"ExecuteTimeLine:_SS:CT_STOPMP3",false);
						// done
						break;
					}
					iResult=m_gpSoundSystem->StopChannel(iChannelCodeID);
					if(iResult==SYS_NOK)
					{
						sError.Format("Error in command: %d;_SOUNDSYSTEM;STOPMP3;%s;. Couldn't stop channel.",
								pTLE->GetTimeSpot(), pTLE->GetCommandParamString(0));
						LogFeedback(&sError[0],true,"ExecuteTimeLine:_SS:CT_STOPMP3",false);
						break;
					}
					sToLog.Format("On %d ms.: _SS: Play of sound channel %s stopped.",(long)(m_gpDemoDat->GetfElapsedTimeInSecs()*1000.0f),
							pTLE->GetCommandParamString(0));
					LogFeedback(&sToLog[0],true,"DoExecuteTimeLine:_SS:CT_STOPMP3",true);
				};break;
				case CT_PAUSEMP3:
				{
					// Get the ChannelCodeID using the scriptID
					iChannelCodeID=m_gpSoundSystem->ConvertChannelScriptID2ChannelCodeID(pTLE->GetCommandParamString(0));
					if(iChannelCodeID==DGL_SS_ERR_ILLEGALCHANNELID)
					{
						sError.Format("Error in command: %d;_SOUNDSYSTEM;PAUSEMP3;%s;. Channel ID %s not found.",
								pTLE->GetTimeSpot(), pTLE->GetCommandParamString(0), pTLE->GetCommandParamString(0));
						LogFeedback(&sError[0],true,"ExecuteTimeLine:_SS:CT_PAUSEMP3",false);
						// done
						break;
					}
					iResult=m_gpSoundSystem->PauseChannel(iChannelCodeID);
					if(iResult==SYS_NOK)
					{
						sError.Format("Error in command: %d;_SOUNDSYSTEM;PAUSEMP3;%s;. Couldn't pause channel.",
								pTLE->GetTimeSpot(), pTLE->GetCommandParamString(0));
						LogFeedback(&sError[0],true,"ExecuteTimeLine:_SS:CT_PAUSEMP3",false);
						break;
					}
					sToLog.Format("On %d ms.: _SS: Play of sound channel %s paused.",(long)(m_gpDemoDat->GetfElapsedTimeInSecs()*1000.0f),
							pTLE->GetCommandParamString(0));
					LogFeedback(&sToLog[0],true,"DoExecuteTimeLine:_SS:CT_PAUSEMP3",true);
				};break;
				case CT_CONTINUEMP3:
				{
					// Get the ChannelCodeID using the scriptID
					iChannelCodeID=m_gpSoundSystem->ConvertChannelScriptID2ChannelCodeID(pTLE->GetCommandParamString(0));
					if(iChannelCodeID==DGL_SS_ERR_ILLEGALCHANNELID)
					{
						sError.Format("Error in command: %d;_SOUNDSYSTEM;CONTINUEMP3;%s;. Channel ID %s not found.",
								pTLE->GetTimeSpot(), pTLE->GetCommandParamString(0), pTLE->GetCommandParamString(0));
						LogFeedback(&sError[0],true,"ExecuteTimeLine:_SS:CT_CONTINUEMP3",false);
						// done
						break;
					}
					iResult=m_gpSoundSystem->ContinueChannel(iChannelCodeID);
					if(iResult==SYS_NOK)
					{
						sError.Format("Error in command: %d;_SOUNDSYSTEM;CONTINUEMP3;%s;. Couldn't continue channel.",
								pTLE->GetTimeSpot(), pTLE->GetCommandParamString(0));
						LogFeedback(&sError[0],true,"ExecuteTimeLine:_SS:CT_CONTINUMP3",false);
						break;
					}
					sToLog.Format("On %d ms.: _SS: Play of sound channel %s resumed.",(long)(m_gpDemoDat->GetfElapsedTimeInSecs()*1000.0f),
							pTLE->GetCommandParamString(0));
					LogFeedback(&sToLog[0],true,"DoExecuteTimeLine:_SS:CT_CONTINUEMP3",true);
				};break;
				case CT_SEEKINMP3:
				{
					// Get the ChannelCodeID using the scriptID
					iChannelCodeID=m_gpSoundSystem->ConvertChannelScriptID2ChannelCodeID(pTLE->GetCommandParamString(0));
					if(iChannelCodeID==DGL_SS_ERR_ILLEGALCHANNELID)
					{
						sError.Format("Error in command: %d;_SOUNDSYSTEM;SEEKINMP3;%s;%d;. Channel ID %s not found.",
								pTLE->GetTimeSpot(), pTLE->GetCommandParamString(0), pTLE->GetCommandParamInt(1),
								pTLE->GetCommandParamString(0));
						LogFeedback(&sError[0],true,"ExecuteTimeLine:_SS:CT_SEEKINMP3",false);
						// done
						break;
					}
					iResult=m_gpSoundSystem->SeekInChannel(iChannelCodeID, pTLE->GetCommandParamInt(1));
					if(iResult==SYS_NOK)
					{
						sError.Format("Error in command: %d;_SOUNDSYSTEM;SEEKINMP3;%s;%d;. Couldn't seek to position.",
								pTLE->GetTimeSpot(), pTLE->GetCommandParamString(0),pTLE->GetCommandParamInt(1));
						LogFeedback(&sError[0],true,"ExecuteTimeLine:_SS:CT_SEEKINMP3",false);
						break;
					}
					sToLog.Format("On %d ms.: _SS: seek in sound channel %s to pos. %d ms done.",(long)(m_gpDemoDat->GetfElapsedTimeInSecs()*1000.0f),
							pTLE->GetCommandParamString(0),pTLE->GetCommandParamInt(1));
					LogFeedback(&sToLog[0],true,"DoExecuteTimeLine:_SS:CT_SEEKINMP3",true);
				};break;
				case CT_EAXMIXMP3:
				{
					// Get the ChannelCodeID using the scriptID
					iChannelCodeID=m_gpSoundSystem->ConvertChannelScriptID2ChannelCodeID(pTLE->GetCommandParamString(0));
					if(iChannelCodeID==DGL_SS_ERR_ILLEGALCHANNELID)
					{
						sError.Format("Error in command: %d;_SOUNDSYSTEM;EAXMIXMP3;%s;%f;. Channel ID %s not found.",
								pTLE->GetTimeSpot(), pTLE->GetCommandParamString(0), pTLE->GetCommandParamFloat(1),
								pTLE->GetCommandParamString(0));
						LogFeedback(&sError[0],true,"ExecuteTimeLine:_SS:CT_EAXMIXMP3",false);
						// done
						break;
					}
					iResult=m_gpSoundSystem->SetEAXMixChannel(iChannelCodeID,pTLE->GetCommandParamFloat(1));
					if(iResult==SYS_NOK)
					{
						sError.Format("Error in command: %d;_SOUNDSYSTEM;EAXMIXMP3;%s;%f;. Couldn't set EAX mix value.",
								pTLE->GetTimeSpot(), pTLE->GetCommandParamString(0),pTLE->GetCommandParamFloat(1));
						LogFeedback(&sError[0],true,"ExecuteTimeLine:_SS:CT_EAXMIXMP3",false);
						break;
					}
					sToLog.Format("On %d ms.: _SS: EAX mix value %f set for sound channel %s.",
							(long)(m_gpDemoDat->GetfElapsedTimeInSecs()*1000.0f),
							pTLE->GetCommandParamFloat(1),pTLE->GetCommandParamString(0));
					LogFeedback(&sToLog[0],true,"DoExecuteTimeLine:_SS:CT_EAXMIXMP3",true);
				};break;
				case CT_SETVOLUMEMP3:
				{
					// Get the ChannelCodeID using the scriptID
					iChannelCodeID=m_gpSoundSystem->ConvertChannelScriptID2ChannelCodeID(pTLE->GetCommandParamString(0));
					if(iChannelCodeID==DGL_SS_ERR_ILLEGALCHANNELID)
					{
						sError.Format("Error in command: %d;_SOUNDSYSTEM;SETVOLUMEMP3;%s;%d;. Channel ID %s not found.",
								pTLE->GetTimeSpot(), pTLE->GetCommandParamString(0), pTLE->GetCommandParamInt(1),
								pTLE->GetCommandParamString(0));
						LogFeedback(&sError[0],true,"ExecuteTimeLine:_SS:CT_SETVOLUMEMP3",false);
						// done
						break;
					}
					iResult=m_gpSoundSystem->SetChannelAttributes(iChannelCodeID, pTLE->GetCommandParamInt(1),0,0,DGL_SS_SA_VOLUME);
					if(iResult==SYS_NOK)
					{
						sError.Format("Error in command: %d;_SOUNDSYSTEM;SETVOLUMEMP3;%s;%d;. Couldn't set volume.",
								pTLE->GetTimeSpot(), pTLE->GetCommandParamString(0),pTLE->GetCommandParamInt(1));
						LogFeedback(&sError[0],true,"ExecuteTimeLine:_SS:CT_SETVOLUMEMP3",false);
						break;
					}
					sToLog.Format("On %d ms.: _SS: Volume set to %d for sound channel %s.",
							(long)(m_gpDemoDat->GetfElapsedTimeInSecs()*1000.0f),
							pTLE->GetCommandParamInt(1),pTLE->GetCommandParamString(0));
					LogFeedback(&sToLog[0],true,"DoExecuteTimeLine:_SS:CT_SETVOLUMEMP3",true);
				};break;
				case CT_SETPANMP3:
				{
					// Get the ChannelCodeID using the scriptID
					iChannelCodeID=m_gpSoundSystem->ConvertChannelScriptID2ChannelCodeID(pTLE->GetCommandParamString(0));
					if(iChannelCodeID==DGL_SS_ERR_ILLEGALCHANNELID)
					{
						sError.Format("Error in command: %d;_SOUNDSYSTEM;SETPANMP3;%s;%d;. Channel ID %s not found.",
								pTLE->GetTimeSpot(), pTLE->GetCommandParamString(0), pTLE->GetCommandParamInt(1),
								pTLE->GetCommandParamString(0));
						LogFeedback(&sError[0],true,"ExecuteTimeLine:_SS:CT_SETPANMP3",false);
						// done
						break;
					}
					iResult=m_gpSoundSystem->SetChannelAttributes(iChannelCodeID,0,pTLE->GetCommandParamInt(1),0,DGL_SS_SA_PAN);
					if(iResult==SYS_NOK)
					{
						sError.Format("Error in command: %d;_SOUNDSYSTEM;SETPANMP3;%s;%d;. Couldn't set panning.",
								pTLE->GetTimeSpot(), pTLE->GetCommandParamString(0),pTLE->GetCommandParamInt(1));
						LogFeedback(&sError[0],true,"ExecuteTimeLine:_SS:CT_SETPANMP3",false);
						break;
					}
					sToLog.Format("On %d ms.: _SS: Panning set to %d for sound channel %s.",
							(long)(m_gpDemoDat->GetfElapsedTimeInSecs()*1000.0f),
							pTLE->GetCommandParamInt(1),pTLE->GetCommandParamString(0));
					LogFeedback(&sToLog[0],true,"DoExecuteTimeLine:_SS:CT_SETPANMP3",true);
				};break;
				case CT_FREEMP3:
				{
					// Get the ElementCodeID using the scriptID
					iElementCodeID=m_gpSoundSystem->ConvertElementScriptID2ElementCodeID(pTLE->GetCommandParamString(0));
					if(iElementCodeID==DGL_SS_ERR_ILLEGALELEMENTID)
					{
						sError.Format("Error in command: %d;_SOUNDSYSTEM;FREEMP3;%s;. Element ID %s not found.",
								pTLE->GetTimeSpot(), pTLE->GetCommandParamString(0), pTLE->GetCommandParamString(0));
						LogFeedback(&sError[0],true,"ExecuteTimeLine:_SS:CT_FREEMP3",false);
						// done
						break;
					}
					iResult=m_gpSoundSystem->FreeSoundElement(iElementCodeID, DGL_SS_MP3ELEMENT, false);
					if(iResult==SYS_NOK)
					{
						sError.Format("Error in command: %d;_SOUNDSYSTEM;FREEMP3;%s;. Couldn't free this element.",
								pTLE->GetTimeSpot(), pTLE->GetCommandParamString(0));
						LogFeedback(&sError[0],true,"ExecuteTimeLine:_SS:CT_FREEMP3",false);
						break;
					}
					sToLog.Format("On %d ms.: _SS: the sound element %s is removed from memory.",(long)(m_gpDemoDat->GetfElapsedTimeInSecs()*1000.0f),
							pTLE->GetCommandParamString(0));
					LogFeedback(&sToLog[0],true,"DoExecuteTimeLine:_SS:CT_FREEMP3",true);
				};break;
				/////////
				// MOD Element type Commands
				/////////
				case CT_LOADMOD:
				{
					iResult=m_gpSoundSystem->LoadSoundElement(pTLE->GetCommandParamString(0),
										DGL_SS_MODELEMENT, pTLE->GetCommandParamString(1),
										(pTLE->GetCommandParamInt(2)==1));
					if(iResult<0)
					{
						// error while loading.
						sError.Format("Error in command: %d;_SOUNDSYSTEM;LOADMOD;%s;%s;%d",pTLE->GetTimeSpot(),
								pTLE->GetCommandParamString(0),pTLE->GetCommandParamString(1),
								pTLE->GetCommandParamInt(2));
						LogFeedback(&sError[0],true,"ExecuteTimeLine:_SS:CT_LOADMOD",false);
						break;
					}
					sToLog.Format("On %d ms.: _SS: Sound element '%s' with Element ID %s loaded.",(long)(m_gpDemoDat->GetfElapsedTimeInSecs()*1000.0f),
							pTLE->GetCommandParamString(0),pTLE->GetCommandParamString(1));
					LogFeedback(&sToLog[0],true,"DoExecuteTimeLine:_SS:CT_LOADMOD",true);
				};break;
				case CT_PLAYMOD:
				{
					// Get the ElementCodeID using the scriptID
					iElementCodeID=m_gpSoundSystem->ConvertElementScriptID2ElementCodeID(pTLE->GetCommandParamString(0));
					if(iElementCodeID==DGL_SS_ERR_ILLEGALELEMENTID)
					{
						sError.Format("Error in command: %d;_SOUNDSYSTEM;PLAYMOD;%s;%d;%d;%d;%d;%s;. ID %s not found.",
								pTLE->GetTimeSpot(), pTLE->GetCommandParamString(0),
								pTLE->GetCommandParamInt(1),pTLE->GetCommandParamInt(2),
								pTLE->GetCommandParamInt(3),pTLE->GetCommandParamInt(4),
								pTLE->GetCommandParamString(5), pTLE->GetCommandParamString(0));
						LogFeedback(&sError[0],true,"ExecuteTimeLine:_SS:CT_PLAYMOD",false);
						// done
						break;
					}
					// Seek parameter (startoffset)
					// check for hexadecimal parameter.
					if(!strchr(pTLE->GetCommandParamString(1),'x'))
					{
						// not hexadecimal
						lLongParam = (long)pTLE->GetCommandParamInt(1);
					}
					else
					{
						// hexadecimal, use sscanf trick 
						sscanf(pTLE->GetCommandParamString(1),"%x",&lLongParam);
					}
					iResult=m_gpSoundSystem->StartElement(iElementCodeID, DGL_SS_MODELEMENT,lLongParam, 
									(pTLE->GetCommandParamInt(4)==1), pTLE->GetCommandParamString(5),NULL);
					if(iResult < 0)
					{
						sError.Format("Error in command: %d;_SOUNDSYSTEM;PLAYMOD;%s;%d;%d;%d;%d;%s;. Couldn't start play.",
								pTLE->GetTimeSpot(), pTLE->GetCommandParamString(0),
								pTLE->GetCommandParamInt(1),pTLE->GetCommandParamInt(2),
								pTLE->GetCommandParamInt(3),pTLE->GetCommandParamInt(4), pTLE->GetCommandParamString(5));
						LogFeedback(&sError[0],true,"ExecuteTimeLine:_SS:CT_PLAYMOD",false);
						// done
						break;
					}
					// play is started. Now set some attributes. For frequency -1 is specified: leave current
					// iResult is the channelcodeid returned by StartElement()
					iChannelCodeID=iResult;
					iResult=m_gpSoundSystem->SetChannelAttributes(iChannelCodeID, pTLE->GetCommandParamInt(2),
								pTLE->GetCommandParamInt(3),-1,	DGL_SS_SA_FREQUENCY|DGL_SS_SA_VOLUME|DGL_SS_SA_PAN);
					if(iResult==SYS_NOK)
					{
						sError.Format("Error in command: %d;_SOUNDSYSTEM;PLAYMOD;%s;%d;%d;%d;%d;%s;. Attribute error.",
								pTLE->GetTimeSpot(), pTLE->GetCommandParamString(0),
								pTLE->GetCommandParamInt(1),pTLE->GetCommandParamInt(2),
								pTLE->GetCommandParamInt(3),pTLE->GetCommandParamInt(4), pTLE->GetCommandParamString(5));
						LogFeedback(&sError[0],true,"ExecuteTimeLine:_SS:CT_PLAYMOD",false);
						break;
					}
					sToLog.Format("On %d ms.: _SS: Play of sound element %s started at pos. %d ms.",(long)(m_gpDemoDat->GetfElapsedTimeInSecs()*1000.0f),
							pTLE->GetCommandParamString(0),pTLE->GetCommandParamInt(1));
					LogFeedback(&sToLog[0],true,"DoExecuteTimeLine:_SS:CT_PLAYMOD",true);
				};break;
				case CT_STOPMOD:
				{
					// Get the ChannelCodeID using the scriptID
					iChannelCodeID=m_gpSoundSystem->ConvertChannelScriptID2ChannelCodeID(pTLE->GetCommandParamString(0));
					if(iChannelCodeID==DGL_SS_ERR_ILLEGALCHANNELID)
					{
						sError.Format("Error in command: %d;_SOUNDSYSTEM;STOPMOD;%s;. Channel ID %s not found.",
								pTLE->GetTimeSpot(), pTLE->GetCommandParamString(0), pTLE->GetCommandParamString(0));
						LogFeedback(&sError[0],true,"ExecuteTimeLine:_SS:CT_STOPMOD",false);
						// done
						break;
					}
					iResult=m_gpSoundSystem->StopChannel(iChannelCodeID);
					if(iResult==SYS_NOK)
					{
						sError.Format("Error in command: %d;_SOUNDSYSTEM;STOPMOD;%s;. Couldn't stop channel.",
								pTLE->GetTimeSpot(), pTLE->GetCommandParamString(0));
						LogFeedback(&sError[0],true,"ExecuteTimeLine:_SS:CT_STOPMOD",false);
						break;
					}
					sToLog.Format("On %d ms.: _SS: Play of sound channel %s stopped.",(long)(m_gpDemoDat->GetfElapsedTimeInSecs()*1000.0f),
							pTLE->GetCommandParamString(0));
					LogFeedback(&sToLog[0],true,"DoExecuteTimeLine:_SS:CT_STOPMOD",true);
				};break;
				case CT_PAUSEMOD:
				{
					// Get the ChannelCodeID using the scriptID
					iChannelCodeID=m_gpSoundSystem->ConvertChannelScriptID2ChannelCodeID(pTLE->GetCommandParamString(0));
					if(iChannelCodeID==DGL_SS_ERR_ILLEGALCHANNELID)
					{
						sError.Format("Error in command: %d;_SOUNDSYSTEM;PAUSEMOD;%s;. Channel ID %s not found.",
								pTLE->GetTimeSpot(), pTLE->GetCommandParamString(0), pTLE->GetCommandParamString(0));
						LogFeedback(&sError[0],true,"ExecuteTimeLine:_SS:CT_PAUSEMOD",false);
						// done
						break;
					}
					iResult=m_gpSoundSystem->PauseChannel(iChannelCodeID);
					if(iResult==SYS_NOK)
					{
						sError.Format("Error in command: %d;_SOUNDSYSTEM;PAUSEMOD;%s;. Couldn't pause channel.",
								pTLE->GetTimeSpot(), pTLE->GetCommandParamString(0));
						LogFeedback(&sError[0],true,"ExecuteTimeLine:_SS:CT_PAUSEMOD",false);
						break;
					}
					sToLog.Format("On %d ms.: _SS: Play of sound channel %s paused.",(long)(m_gpDemoDat->GetfElapsedTimeInSecs()*1000.0f),
							pTLE->GetCommandParamString(0));
					LogFeedback(&sToLog[0],true,"DoExecuteTimeLine:_SS:CT_PAUSEMOD",true);
				};break;
				case CT_CONTINUEMOD:
				{
					// Get the ChannelCodeID using the scriptID
					iChannelCodeID=m_gpSoundSystem->ConvertChannelScriptID2ChannelCodeID(pTLE->GetCommandParamString(0));
					if(iChannelCodeID==DGL_SS_ERR_ILLEGALCHANNELID)
					{
						sError.Format("Error in command: %d;_SOUNDSYSTEM;CONTINUEMOD;%s;. Channel ID %s not found.",
								pTLE->GetTimeSpot(), pTLE->GetCommandParamString(0), pTLE->GetCommandParamString(0));
						LogFeedback(&sError[0],true,"ExecuteTimeLine:_SS:CT_CONTINUEMOD",false);
						// done
						break;
					}
					iResult=m_gpSoundSystem->ContinueChannel(iChannelCodeID);
					if(iResult==SYS_NOK)
					{
						sError.Format("Error in command: %d;_SOUNDSYSTEM;CONTINUEMOD;%s;. Couldn't continue channel.",
								pTLE->GetTimeSpot(), pTLE->GetCommandParamString(0));
						LogFeedback(&sError[0],true,"ExecuteTimeLine:_SS:CT_CONTINUMOD",false);
						break;
					}
					sToLog.Format("On %d ms.: _SS: Play of sound channel %s resumed.",(long)(m_gpDemoDat->GetfElapsedTimeInSecs()*1000.0f),
							pTLE->GetCommandParamString(0));
					LogFeedback(&sToLog[0],true,"DoExecuteTimeLine:_SS:CT_CONTINUEMOD",true);
				};break;
				case CT_SEEKINMOD:
				{
					// Get the ChannelCodeID using the scriptID
					iChannelCodeID=m_gpSoundSystem->ConvertChannelScriptID2ChannelCodeID(pTLE->GetCommandParamString(0));
					if(iChannelCodeID==DGL_SS_ERR_ILLEGALCHANNELID)
					{
						sError.Format("Error in command: %d;_SOUNDSYSTEM;SEEKINMOD;%s;%s;. Channel ID %s not found.",
								pTLE->GetTimeSpot(), pTLE->GetCommandParamString(0), pTLE->GetCommandParamString(1),
								pTLE->GetCommandParamString(0));
						LogFeedback(&sError[0],true,"ExecuteTimeLine:_SS:CT_SEEKINMOD",false);
						// done
						break;
					}
					// Seek parameter (startoffset)
					// check for hexadecimal parameter.
					if(!strchr(pTLE->GetCommandParamString(1),'x'))
					{
						// not hexadecimal
						lLongParam = (long)pTLE->GetCommandParamInt(1);
					}
					else
					{
						// hexadecimal, use sscanf trick 
						sscanf(pTLE->GetCommandParamString(1),"%x",&lLongParam);
					}
					iResult=m_gpSoundSystem->SeekInChannel(iChannelCodeID, lLongParam);
					if(iResult==SYS_NOK)
					{
						sError.Format("Error in command: %d;_SOUNDSYSTEM;SEEKINMOD;%s;%s;. Couldn't seek to position.",
								pTLE->GetTimeSpot(), pTLE->GetCommandParamString(0),pTLE->GetCommandParamString(1));
						LogFeedback(&sError[0],true,"ExecuteTimeLine:_SS:CT_SEEKINMOD",false);
						break;
					}
					sToLog.Format("On %d ms.: _SS: seek in sound channel %s to pos. %s done.",(long)(m_gpDemoDat->GetfElapsedTimeInSecs()*1000.0f),
							pTLE->GetCommandParamString(0),pTLE->GetCommandParamString(1));
					LogFeedback(&sToLog[0],true,"DoExecuteTimeLine:_SS:CT_SEEKINMOD",true);
				};break;
				case CT_EAXMIXMOD:
				{
					// Get the ChannelCodeID using the scriptID
					iChannelCodeID=m_gpSoundSystem->ConvertChannelScriptID2ChannelCodeID(pTLE->GetCommandParamString(0));
					if(iChannelCodeID==DGL_SS_ERR_ILLEGALCHANNELID)
					{
						sError.Format("Error in command: %d;_SOUNDSYSTEM;EAXMIXMOD;%s;%f;. Channel ID %s not found.",
								pTLE->GetTimeSpot(), pTLE->GetCommandParamString(0), pTLE->GetCommandParamFloat(1),
								pTLE->GetCommandParamString(0));
						LogFeedback(&sError[0],true,"ExecuteTimeLine:_SS:CT_EAXMIXMOD",false);
						// done
						break;
					}
					iResult=m_gpSoundSystem->SetEAXMixChannel(iChannelCodeID, pTLE->GetCommandParamFloat(1));
					if(iResult==SYS_NOK)
					{
						sError.Format("Error in command: %d;_SOUNDSYSTEM;EAXMIXMOD;%s;%f;. Couldn't set EAX mix value.",
								pTLE->GetTimeSpot(), pTLE->GetCommandParamString(0),pTLE->GetCommandParamFloat(1));
						LogFeedback(&sError[0],true,"ExecuteTimeLine:_SS:CT_EAXMIXMOD",false);
						break;
					}
					sToLog.Format("On %d ms.: _SS: EAX mix value %f set for sound channel %s.",
							(long)(m_gpDemoDat->GetfElapsedTimeInSecs()*1000.0f),
							pTLE->GetCommandParamFloat(1),pTLE->GetCommandParamString(0));
					LogFeedback(&sToLog[0],true,"DoExecuteTimeLine:_SS:CT_EAXMIXMOD",true);
				};break;
				case CT_SETVOLUMEMOD:
				{
					// Get the ChannelCodeID using the scriptID
					iChannelCodeID=m_gpSoundSystem->ConvertChannelScriptID2ChannelCodeID(pTLE->GetCommandParamString(0));
					if(iChannelCodeID==DGL_SS_ERR_ILLEGALCHANNELID)
					{
						sError.Format("Error in command: %d;_SOUNDSYSTEM;SETVOLUMEMOD;%s;%d;. Channel ID %s not found.",
								pTLE->GetTimeSpot(), pTLE->GetCommandParamString(0), pTLE->GetCommandParamInt(1),
								pTLE->GetCommandParamString(0));
						LogFeedback(&sError[0],true,"ExecuteTimeLine:_SS:CT_SETVOLUMEMOD",false);
						// done
						break;
					}
					iResult=m_gpSoundSystem->SetChannelAttributes(iChannelCodeID, pTLE->GetCommandParamInt(1),0,0,DGL_SS_SA_VOLUME);
					if(iResult==SYS_NOK)
					{
						sError.Format("Error in command: %d;_SOUNDSYSTEM;SETVOLUMEMOD;%s;%d;. Couldn't set volume.",
								pTLE->GetTimeSpot(), pTLE->GetCommandParamString(0),pTLE->GetCommandParamInt(1));
						LogFeedback(&sError[0],true,"ExecuteTimeLine:_SS:CT_SETVOLUMEMOD",false);
						break;
					}
					sToLog.Format("On %d ms.: _SS: Volume set to %d for sound channel %s.",
							(long)(m_gpDemoDat->GetfElapsedTimeInSecs()*1000.0f),
							pTLE->GetCommandParamInt(1),pTLE->GetCommandParamString(0));
					LogFeedback(&sToLog[0],true,"DoExecuteTimeLine:_SS:CT_SETVOLUMEMOD",true);
				};break;
				case CT_SETPANMOD:
				{
					// Get the ChannelCodeID using the scriptID
					iChannelCodeID=m_gpSoundSystem->ConvertChannelScriptID2ChannelCodeID(pTLE->GetCommandParamString(0));
					if(iChannelCodeID==DGL_SS_ERR_ILLEGALCHANNELID)
					{
						sError.Format("Error in command: %d;_SOUNDSYSTEM;SETPANMOD;%s;%d;. Channel ID %s not found.",
								pTLE->GetTimeSpot(), pTLE->GetCommandParamString(0), pTLE->GetCommandParamInt(1),
								pTLE->GetCommandParamString(0));
						LogFeedback(&sError[0],true,"ExecuteTimeLine:_SS:CT_SETPANMOD",false);
						// done
						break;
					}
					iResult=m_gpSoundSystem->SetChannelAttributes(iChannelCodeID, 0,pTLE->GetCommandParamInt(1),0,DGL_SS_SA_PAN);
					if(iResult==SYS_NOK)
					{
						sError.Format("Error in command: %d;_SOUNDSYSTEM;SETPANMOD;%s;%d;. Couldn't set panning.",
								pTLE->GetTimeSpot(), pTLE->GetCommandParamString(0),pTLE->GetCommandParamInt(1));
						LogFeedback(&sError[0],true,"ExecuteTimeLine:_SS:CT_SETPANMOD",false);
						break;
					}
					sToLog.Format("On %d ms.: _SS: Panning set to %d for sound channel %s.",
							(long)(m_gpDemoDat->GetfElapsedTimeInSecs()*1000.0f),
							pTLE->GetCommandParamInt(1),pTLE->GetCommandParamString(0));
					LogFeedback(&sToLog[0],true,"DoExecuteTimeLine:_SS:CT_SETPANMOD",true);
				};break;
				case CT_FREEMOD:
				{
					// Get the ElementCodeID using the scriptID
					iElementCodeID=m_gpSoundSystem->ConvertElementScriptID2ElementCodeID(pTLE->GetCommandParamString(0));
					if(iElementCodeID==DGL_SS_ERR_ILLEGALELEMENTID)
					{
						sError.Format("Error in command: %d;_SOUNDSYSTEM;FREEMOD;%s;. ID %s not found.",
								pTLE->GetTimeSpot(), pTLE->GetCommandParamString(0), pTLE->GetCommandParamString(0));
						LogFeedback(&sError[0],true,"ExecuteTimeLine:_SS:CT_FREEMOD",false);
						// done
						break;
					}
					iResult=m_gpSoundSystem->FreeSoundElement(iElementCodeID, DGL_SS_MODELEMENT, false);
					if(iResult==SYS_NOK)
					{
						sError.Format("Error in command: %d;_SOUNDSYSTEM;FREEMOD;%s;. Couldn't free this element.",
								pTLE->GetTimeSpot(), pTLE->GetCommandParamString(0));
						LogFeedback(&sError[0],true,"ExecuteTimeLine:_SS:CT_FREEMOD",false);
						break;
					}
					sToLog.Format("On %d ms.: _SS: the sound element %s is removed from memory.",(long)(m_gpDemoDat->GetfElapsedTimeInSecs()*1000.0f),
							pTLE->GetCommandParamString(0));
					LogFeedback(&sToLog[0],true,"DoExecuteTimeLine:_SS:CT_FREEMOD",true);
				};break;
				/////////
				// SMPL Element type Commands
				/////////
				case CT_LOADSMPL:
				{
					iResult=m_gpSoundSystem->LoadSoundElement(pTLE->GetCommandParamString(0),
										DGL_SS_SAMPLEELEMENT, pTLE->GetCommandParamString(1), false);
					if(iResult<0)
					{
						// error while loading.
						sError.Format("Error in command: %d;_SOUNDSYSTEM;LOADSMPL;%s;%s",pTLE->GetTimeSpot(),
								pTLE->GetCommandParamString(0),pTLE->GetCommandParamString(1));
						LogFeedback(&sError[0],true,"ExecuteTimeLine:_SS:CT_LOADSMPL",false);
						break;
					}
					sToLog.Format("On %d ms.: _SS: Sound element '%s' with Element ID %s loaded.",(long)(m_gpDemoDat->GetfElapsedTimeInSecs()*1000.0f),
							pTLE->GetCommandParamString(0),pTLE->GetCommandParamString(1));
					LogFeedback(&sToLog[0],true,"DoExecuteTimeLine:_SS:CT_LOADSMPL",true);
				};break;
				case CT_PLAYSMPL:
				{
					// Get the ElementCodeID using the scriptID
					iElementCodeID=m_gpSoundSystem->ConvertElementScriptID2ElementCodeID(pTLE->GetCommandParamString(0));
					if(iElementCodeID==DGL_SS_ERR_ILLEGALELEMENTID)
					{
						sError.Format("Error in command: %d;_SOUNDSYSTEM;PLAYSMPL;%s;%d;%d;%d;%d;%d;%s;. Element ID %s not found.",
								pTLE->GetTimeSpot(), pTLE->GetCommandParamString(0),
								pTLE->GetCommandParamInt(1),pTLE->GetCommandParamInt(2),
								pTLE->GetCommandParamInt(3),pTLE->GetCommandParamInt(4),pTLE->GetCommandParamInt(5),
								pTLE->GetCommandParamString(6), pTLE->GetCommandParamString(0));
						LogFeedback(&sError[0],true,"ExecuteTimeLine:_SS:CT_PLAYSMPL",false);
						// done
						break;
					}
					iResult=m_gpSoundSystem->StartElement(iElementCodeID, DGL_SS_SAMPLEELEMENT, 
									(long)pTLE->GetCommandParamInt(1),(pTLE->GetCommandParamInt(5)==1),
									pTLE->GetCommandParamString(6),NULL);
					if(iResult==SYS_NOK)
					{
						sError.Format("Error in command: %d;_SOUNDSYSTEM;PLAYSMPL;%s;%d;%d;%d;%d;%d;%s;. Couldn't start play.",
								pTLE->GetTimeSpot(), pTLE->GetCommandParamString(0),
								pTLE->GetCommandParamInt(1),pTLE->GetCommandParamInt(2),
								pTLE->GetCommandParamInt(3),pTLE->GetCommandParamInt(4),
								pTLE->GetCommandParamInt(5), pTLE->GetCommandParamString(6));
						LogFeedback(&sError[0],true,"ExecuteTimeLine:_SS:CT_PLAYSMPL",false);
						// done
						break;
					}
					// play is started. Now set some attributes. 
					// iResult is channelcodeid this sample element is started on. 
					iChannelCodeID=iResult;
					iResult=m_gpSoundSystem->SetChannelAttributes(iChannelCodeID, pTLE->GetCommandParamInt(2),
								pTLE->GetCommandParamInt(3),pTLE->GetCommandParamInt(4),
								DGL_SS_SA_FREQUENCY|DGL_SS_SA_VOLUME|DGL_SS_SA_PAN);
					if(iResult==SYS_NOK)
					{
						sError.Format("Error in command: %d;_SOUNDSYSTEM;PLAYMP3;%s;%d;%d;%d;%d;%d;%s;. Attribute error.",
								pTLE->GetTimeSpot(), pTLE->GetCommandParamString(0),
								pTLE->GetCommandParamInt(1),pTLE->GetCommandParamInt(2),
								pTLE->GetCommandParamInt(3),pTLE->GetCommandParamInt(4),
								pTLE->GetCommandParamInt(5), pTLE->GetCommandParamString(6));
						LogFeedback(&sError[0],true,"ExecuteTimeLine:_SS:CT_PLAYMP3",false);
						break;
					}
					sToLog.Format("On %d ms.: _SS: Play of sound element %s started at pos. %d ms.",(long)(m_gpDemoDat->GetfElapsedTimeInSecs()*1000.0f),
							pTLE->GetCommandParamString(0),pTLE->GetCommandParamInt(1));
					LogFeedback(&sToLog[0],true,"DoExecuteTimeLine:_SS:CT_PLAYSMPL",true);
				};break;
				case CT_STOPSMPL:
				{
					// Get the ChannelCodeID using the scriptID
					iChannelCodeID=m_gpSoundSystem->ConvertChannelScriptID2ChannelCodeID(pTLE->GetCommandParamString(0));
					if(iChannelCodeID==DGL_SS_ERR_ILLEGALCHANNELID)
					{
						sError.Format("Error in command: %d;_SOUNDSYSTEM;STOPSMPL;%s;. Channel ID %s not found.",
								pTLE->GetTimeSpot(), pTLE->GetCommandParamString(0), pTLE->GetCommandParamString(0));
						LogFeedback(&sError[0],true,"ExecuteTimeLine:_SS:CT_STOPSMPL",false);
						// done
						break;
					}
					iResult=m_gpSoundSystem->StopChannel(iChannelCodeID);
					if(iResult==SYS_NOK)
					{
						sError.Format("Error in command: %d;_SOUNDSYSTEM;STOPSMPL;%s;. Couldn't stop channel.",
								pTLE->GetTimeSpot(), pTLE->GetCommandParamString(0));
						LogFeedback(&sError[0],true,"ExecuteTimeLine:_SS:CT_STOPSMPL",false);
						break;
					}
					sToLog.Format("On %d ms.: _SS: Play of sound channel %s stopped.",(long)(m_gpDemoDat->GetfElapsedTimeInSecs()*1000.0f),
							pTLE->GetCommandParamString(0));
					LogFeedback(&sToLog[0],true,"DoExecuteTimeLine:_SS:CT_STOPSMPL",true);
				};break;
				case CT_PAUSESMPL:
				{
					// Get the ChannelCodeID using the scriptID
					iChannelCodeID=m_gpSoundSystem->ConvertChannelScriptID2ChannelCodeID(pTLE->GetCommandParamString(0));
					if(iChannelCodeID==DGL_SS_ERR_ILLEGALCHANNELID)
					{
						sError.Format("Error in command: %d;_SOUNDSYSTEM;PAUSESMPL;%s;. Channel ID %s not found.",
								pTLE->GetTimeSpot(), pTLE->GetCommandParamString(0), pTLE->GetCommandParamString(0));
						LogFeedback(&sError[0],true,"ExecuteTimeLine:_SS:CT_PAUSESMPL",false);
						// done
						break;
					}
					iResult=m_gpSoundSystem->PauseChannel(iChannelCodeID);
					if(iResult==SYS_NOK)
					{
						sError.Format("Error in command: %d;_SOUNDSYSTEM;PAUSESMPL;%s;. Couldn't pause channel.",
								pTLE->GetTimeSpot(), pTLE->GetCommandParamString(0));
						LogFeedback(&sError[0],true,"ExecuteTimeLine:_SS:CT_PAUSESMPL",false);
						break;
					}
					sToLog.Format("On %d ms.: _SS: Play of sound channel %s paused.",(long)(m_gpDemoDat->GetfElapsedTimeInSecs()*1000.0f),
							pTLE->GetCommandParamString(0));
					LogFeedback(&sToLog[0],true,"DoExecuteTimeLine:_SS:CT_PAUSESMPL",true);
				};break;
				case CT_CONTINUESMPL:
				{
					// Get the ChannelCodeID using the scriptID
					iChannelCodeID=m_gpSoundSystem->ConvertChannelScriptID2ChannelCodeID(pTLE->GetCommandParamString(0));
					if(iChannelCodeID==DGL_SS_ERR_ILLEGALCHANNELID)
					{
						sError.Format("Error in command: %d;_SOUNDSYSTEM;CONTINUESMPL;%s;. Channel ID %s not found.",
								pTLE->GetTimeSpot(), pTLE->GetCommandParamString(0), pTLE->GetCommandParamString(0));
						LogFeedback(&sError[0],true,"ExecuteTimeLine:_SS:CT_CONTINUESMPL",false);
						// done
						break;
					}
					iResult=m_gpSoundSystem->ContinueChannel(iChannelCodeID);
					if(iResult==SYS_NOK)
					{
						sError.Format("Error in command: %d;_SOUNDSYSTEM;CONTINUESMPL;%s;. Couldn't continue element's play.",
								pTLE->GetTimeSpot(), pTLE->GetCommandParamString(0));
						LogFeedback(&sError[0],true,"ExecuteTimeLine:_SS:CT_CONTINUESMPL",false);
						break;
					}
					sToLog.Format("On %d ms.: _SS: Play of sound channel %s resumed.",(long)(m_gpDemoDat->GetfElapsedTimeInSecs()*1000.0f),
							pTLE->GetCommandParamString(0));
					LogFeedback(&sToLog[0],true,"DoExecuteTimeLine:_SS:CT_CONTINUESMPL",true);
				};break;
				case CT_EAXMIXSMPL:
				{
					// Get the ChannelCodeID using the scriptID
					iChannelCodeID=m_gpSoundSystem->ConvertChannelScriptID2ChannelCodeID(pTLE->GetCommandParamString(0));
					if(iChannelCodeID==DGL_SS_ERR_ILLEGALCHANNELID)
					{
						sError.Format("Error in command: %d;_SOUNDSYSTEM;EAXMIXSMPL;%s;%f;. Channel ID %s not found.",
								pTLE->GetTimeSpot(), pTLE->GetCommandParamString(0), pTLE->GetCommandParamFloat(1),
								pTLE->GetCommandParamString(0));
						LogFeedback(&sError[0],true,"ExecuteTimeLine:_SS:CT_EAXMIXSMPL",false);
						// done
						break;
					}
					iResult=m_gpSoundSystem->SetEAXMixChannel(iChannelCodeID, pTLE->GetCommandParamFloat(1));
					if(iResult==SYS_NOK)
					{
						sError.Format("Error in command: %d;_SOUNDSYSTEM;EAXMIXSMPL;%s;%f;. Couldn't set EAX mix value.",
								pTLE->GetTimeSpot(), pTLE->GetCommandParamString(0),pTLE->GetCommandParamFloat(1));
						LogFeedback(&sError[0],true,"ExecuteTimeLine:_SS:CT_EAXMIXSMPL",false);
						break;
					}
					sToLog.Format("On %d ms.: _SS: EAX mix value %f set for sound channel %s.",
							(long)(m_gpDemoDat->GetfElapsedTimeInSecs()*1000.0f),
							pTLE->GetCommandParamFloat(1),pTLE->GetCommandParamString(0));
					LogFeedback(&sToLog[0],true,"DoExecuteTimeLine:_SS:CT_EAXMIXSMPL",true);
				};break;
				case CT_SEEKINSMPL:
				{
					// Get the ChannelCodeID using the scriptID
					iChannelCodeID=m_gpSoundSystem->ConvertChannelScriptID2ChannelCodeID(pTLE->GetCommandParamString(0));
					if(iChannelCodeID==DGL_SS_ERR_ILLEGALCHANNELID)
					{
						sError.Format("Error in command: %d;_SOUNDSYSTEM;SEEKINSMPL;%s;%d;. Channel ID %s not found.",
								pTLE->GetTimeSpot(), pTLE->GetCommandParamString(0), pTLE->GetCommandParamInt(1),
								pTLE->GetCommandParamString(0));
						LogFeedback(&sError[0],true,"ExecuteTimeLine:_SS:CT_SEEKINSMPL",false);
						// done
						break;
					}
					iResult=m_gpSoundSystem->SeekInChannel(iChannelCodeID, pTLE->GetCommandParamInt(1));
					if(iResult==SYS_NOK)
					{
						sError.Format("Error in command: %d;_SOUNDSYSTEM;SEEKINSMPL;%s;%d;. Couldn't seek to position.",
								pTLE->GetTimeSpot(), pTLE->GetCommandParamString(0),pTLE->GetCommandParamInt(1));
						LogFeedback(&sError[0],true,"ExecuteTimeLine:_SS:CT_SEEKINSMPL",false);
						break;
					}
					sToLog.Format("On %d ms.: _SS: seek in sound channel %s to pos. %d done.",(long)(m_gpDemoDat->GetfElapsedTimeInSecs()*1000.0f),
							pTLE->GetCommandParamString(0),pTLE->GetCommandParamInt(1));
					LogFeedback(&sToLog[0],true,"DoExecuteTimeLine:_SS:CT_SEEKINSMPL",true);
				};break;
				case CT_SETVOLUMESMPL:
				{
					// Get the ChannelCodeID using the scriptID
					iChannelCodeID=m_gpSoundSystem->ConvertChannelScriptID2ChannelCodeID(pTLE->GetCommandParamString(0));
					if(iChannelCodeID==DGL_SS_ERR_ILLEGALCHANNELID)
					{
						sError.Format("Error in command: %d;_SOUNDSYSTEM;SETVOLUMESMPL;%s;%d;. Channel ID %s not found.",
								pTLE->GetTimeSpot(), pTLE->GetCommandParamString(0), pTLE->GetCommandParamInt(1),
								pTLE->GetCommandParamString(0));
						LogFeedback(&sError[0],true,"ExecuteTimeLine:_SS:SETVOLUMESMPL",false);
						// done
						break;
					}
					iResult=m_gpSoundSystem->SetChannelAttributes(iChannelCodeID, pTLE->GetCommandParamInt(1),0,0,DGL_SS_SA_VOLUME);
					if(iResult==SYS_NOK)
					{
						sError.Format("Error in command: %d;_SOUNDSYSTEM;SETVOLUMESMPL;%s;%d;. Couldn't set volume.",
								pTLE->GetTimeSpot(), pTLE->GetCommandParamString(0),pTLE->GetCommandParamInt(1));
						LogFeedback(&sError[0],true,"ExecuteTimeLine:_SS:SETVOLUMESMPL",false);
						break;
					}
					sToLog.Format("On %d ms.: _SS: Volume set to %d for sound channel %s.",
							(long)(m_gpDemoDat->GetfElapsedTimeInSecs()*1000.0f),
							pTLE->GetCommandParamInt(1),pTLE->GetCommandParamString(0));
					LogFeedback(&sToLog[0],true,"DoExecuteTimeLine:_SS:SETVOLUMESMPL",true);
				};break;
				case CT_SETPANSMPL:
				{
					// Get the ChannelCodeID using the scriptID
					iChannelCodeID=m_gpSoundSystem->ConvertChannelScriptID2ChannelCodeID(pTLE->GetCommandParamString(0));
					if(iChannelCodeID==DGL_SS_ERR_ILLEGALCHANNELID)
					{
						sError.Format("Error in command: %d;_SOUNDSYSTEM;SETPANSMPL;%s;%d;. Channel ID %s not found.",
								pTLE->GetTimeSpot(), pTLE->GetCommandParamString(0), pTLE->GetCommandParamInt(1),
								pTLE->GetCommandParamString(0));
						LogFeedback(&sError[0],true,"ExecuteTimeLine:_SS:CT_SETPANSMPL",false);
						// done
						break;
					}
					iResult=m_gpSoundSystem->SetChannelAttributes(iChannelCodeID, 0,pTLE->GetCommandParamInt(1),0,DGL_SS_SA_PAN);
					if(iResult==SYS_NOK)
					{
						sError.Format("Error in command: %d;_SOUNDSYSTEM;SETPANSMPL;%s;%d;. Couldn't set panning.",
								pTLE->GetTimeSpot(), pTLE->GetCommandParamString(0),pTLE->GetCommandParamInt(1));
						LogFeedback(&sError[0],true,"ExecuteTimeLine:_SS:CT_SETPANSMPL",false);
						break;
					}
					sToLog.Format("On %d ms.: _SS: Panning set to %d for sound channel %s.",
							(long)(m_gpDemoDat->GetfElapsedTimeInSecs()*1000.0f),
							pTLE->GetCommandParamInt(1),pTLE->GetCommandParamString(0));
					LogFeedback(&sToLog[0],true,"DoExecuteTimeLine:_SS:CT_SETPANSMPL",true);
				};break;
				case CT_FREESMPL:
				{
					// Get the ElementCodeID using the scriptID
					iElementCodeID=m_gpSoundSystem->ConvertElementScriptID2ElementCodeID(pTLE->GetCommandParamString(0));
					if(iElementCodeID==DGL_SS_ERR_ILLEGALELEMENTID)
					{
						sError.Format("Error in command: %d;_SOUNDSYSTEM;FREESMPL;%s;. ID %s not found.",
								pTLE->GetTimeSpot(), pTLE->GetCommandParamString(0), pTLE->GetCommandParamString(0));
						LogFeedback(&sError[0],true,"ExecuteTimeLine:_SS:CT_FREESMPL",false);
						// done
						break;
					}
					iResult=m_gpSoundSystem->FreeSoundElement(iElementCodeID, DGL_SS_SAMPLEELEMENT, false);
					if(iResult==SYS_NOK)
					{
						sError.Format("Error in command: %d;_SOUNDSYSTEM;FREESMPL;%s;. Couldn't free this element.",
								pTLE->GetTimeSpot(), pTLE->GetCommandParamString(0));
						LogFeedback(&sError[0],true,"ExecuteTimeLine:_SS:CT_FREESMPL",false);
						break;
					}
					sToLog.Format("On %d ms.: _SS: the sound element %s is removed from memory.",(long)(m_gpDemoDat->GetfElapsedTimeInSecs()*1000.0f),
							pTLE->GetCommandParamString(0));
					LogFeedback(&sToLog[0],true,"DoExecuteTimeLine:_SS:CT_FREESMPL",true);
				};break;
				/////////
				// General Commands
				/////////
				case CT_SETEAXENV:
				{
					iResult=m_gpSoundSystem->SetEAXEnv(pTLE->GetCommandParamInt(0), pTLE->GetCommandParamFloat(1),
									pTLE->GetCommandParamFloat(2),pTLE->GetCommandParamFloat(3));
					if(iResult==SYS_NOK)
					{
						sError.Format("Error in command %d;_SOUNDSYSTEM;SETEAXENV;%d;%f;%f;%f;. Couldn't set EAX parameters",
								pTLE->GetTimeSpot(),pTLE->GetCommandParamInt(0), pTLE->GetCommandParamFloat(1),
									pTLE->GetCommandParamFloat(2),pTLE->GetCommandParamFloat(3));
						LogFeedback(&sError[0],true,"ExecuteTimeLine:_SS:CT_SETEAXENV",false);
						break;
					}
					sToLog.Format("On %d ms.: _SS: EAX overall parameters set.",
							(long)(m_gpDemoDat->GetfElapsedTimeInSecs()*1000.0f));
					LogFeedback(&sToLog[0],true,"DoExecuteTimeLine:_SS:CT_SETEAXENV",true);
				};break;
				case CT_SETVOLUME:
				{
					iResult=m_gpSoundSystem->SetVolume(pTLE->GetCommandParamInt(0));
					if(iResult==SYS_NOK)
					{
						sError.Format("Error in command %d;_SOUNDSYSTEM;SETVOLUME;%d;. Couldn't set volume.",
								pTLE->GetTimeSpot(),pTLE->GetCommandParamInt(0));
						LogFeedback(&sError[0],true,"ExecuteTimeLine:_SS:CT_SETVOLUME",false);
						break;
					}
					sToLog.Format("On %d ms.: _SS: Overall volume set to %d.",
							(long)(m_gpDemoDat->GetfElapsedTimeInSecs()*1000.0f),pTLE->GetCommandParamInt(0));
					LogFeedback(&sToLog[0],true,"DoExecuteTimeLine:_SS:CT_SETVOLUME",true);
				};break;
				case CT_STOPALL:
				{
					iResult=m_gpSoundSystem->StopAll();
					if(iResult==SYS_NOK)
					{
						sError.Format("Error in command %d;_SOUNDSYSTEM;STOPALL;. Couldn't stop play.",
								pTLE->GetTimeSpot());
						LogFeedback(&sError[0],true,"ExecuteTimeLine:_SS:CT_STOPALL",false);
						break;
					}
					sToLog.Format("On %d ms.: _SS: All soundelements are stopped.",
							(long)(m_gpDemoDat->GetfElapsedTimeInSecs()*1000.0f));
					LogFeedback(&sToLog[0],true,"DoExecuteTimeLine:_SS:CT_STOPALL",true);
				};break;
				case CT_PAUSEALL:
				{
					iResult=m_gpSoundSystem->PauseAll();
					if(iResult==SYS_NOK)
					{
						sError.Format("Error in command %d;_SOUNDSYSTEM;PAUSEALL;. Couldn't pause play.",
								pTLE->GetTimeSpot());
						LogFeedback(&sError[0],true,"ExecuteTimeLine:_SS:CT_PAUSEALL",false);
						break;
					}
					sToLog.Format("On %d ms.: _SS: All soundelements are paused.",
							(long)(m_gpDemoDat->GetfElapsedTimeInSecs()*1000.0f));
					LogFeedback(&sToLog[0],true,"DoExecuteTimeLine:_SS:CT_PAUSEALL",true);
				};break;
				case CT_CONTINUEALL:
				{
					iResult=m_gpSoundSystem->ContinueAll();
					if(iResult==SYS_NOK)
					{
						sError.Format("Error in command %d;_SOUNDSYSTEM;CONTINUEALL;. Couldn't continue paused play.",
								pTLE->GetTimeSpot());
						LogFeedback(&sError[0],true,"ExecuteTimeLine:_SS:CT_CONTINUEALL",false);
						break;
					}
					sToLog.Format("On %d ms.: _SS: Play of all soundelements is continued.",
							(long)(m_gpDemoDat->GetfElapsedTimeInSecs()*1000.0f));
					LogFeedback(&sToLog[0],true,"DoExecuteTimeLine:_SS:CT_CONTINUEALL",true);
				};break;
			}
		}
		// done
		return;
	}


	///////////////////
	// ADD MORE SYSTEMOBJECTS _HERE_
	///////////////////
	
	// it's an effectobject. Find it and get a pointer to it.
	pEffectObject=GetEffectObject(sObject);
	if(!pEffectObject)
	{
		// effectobject not found.
		sError.Format("Effect %s not registered. Command %s ignored",sObject,ScriptCommandToText(iCommand));
		LogFeedback(&sError[0],true,"ExecuteTimeLine:EFF:Unknown",true);
	}
	else
	{
		 // execute command
		switch(iCommand)
		{
			case CT_NOP:
			{
				// no operation
				sToLog.Format("On %d ms.: %s: NOP executed.",(long)(m_gpDemoDat->GetfElapsedTimeInSecs()*1000.0f),sObject);
				LogFeedback(&sToLog[0],true,"ExecuteTimeLine:EFF:CT_NOP",true);
			}; break;
			case CT_PREPARE:
			{
				// call the preparefunction of the object.
				sToLog.Format("On %d ms.: %s: Preparing effect.",(long)(m_gpDemoDat->GetfElapsedTimeInSecs()*1000.0f),sObject);
				LogFeedback(&sToLog[0],true,"ExecuteTimeLine:EFF:CT_PREPARE",true);
				if(bSameThread)
				{
					// same thread
					pEffectObject->Prepare();
				}
				else
				{
					// different thread, sent message.

					// sent message to mainthread to prepare this object. We can't do this here because we run in a 
					// different thread and the Rendercontext we use for rendering is owned by the mainthread. Thus texture
					// uploading or other fancy stuff in the prepare thread is useless then if we do it in our own thread.
					// We store the effectpointer in the FIFO Prepare queue gpPrepareQueue.
					m_gpPrepareQueue->Add(pEffectObject);
					PostMessage(m_gpDemoDat->GethWnd(),WM_DEMOGL_PREPAREEFFECT,NULL,NULL);
				}
			}; break;	
			case CT_START:
			{
				// create new layer and place the effectobject on it.
				lLongParam = atol(pTLE->GetCommandParamString(0));
				if(lLongParam<0)
				{
					// illegal.
					sError.Format("START Command supplied for object %s has invalid layernumber. Ignored.", sObject);
					LogFeedback(&sError[0],true,"ExecuteTimeLine:EFF:CT_START",false);
					break;
				}
				CreateNewLayer(pEffectObject,sObject,lLongParam);
				sToLog.Format("On %d ms.: %s: Effect started.",(long)(m_gpDemoDat->GetfElapsedTimeInSecs()*1000.0f),sObject);
				LogFeedback(&sToLog[0],true,"ExecuteTimeLine:EFF:CT_START",true);
			}; break;
			case CT_STOP:
			{
				// remove the layer(s) where the effectobject is running on
				RemoveLayers(sObject);
				sToLog.Format("On %d ms.: %s: Effect stopped. Layer(s) removed.",
						(long)(m_gpDemoDat->GetfElapsedTimeInSecs()*1000.0f),sObject);
				LogFeedback(&sToLog[0],true,"ExecuteTimeLine:EFF:CT_STOP",true);
			}; break;
			case CT_END:
			{
				// call the endfunction of the object, remove any layers where the effect is running on, 
				// plus unload the effectstorage object
				pEffectObject->End();
				RemoveLayers(sObject);
				RemoveEffectStore(sObject);
				sToLog.Format("On %d ms.: %s: Effect ended. Layer(s) removed. Object removed.",
						(long)(m_gpDemoDat->GetfElapsedTimeInSecs()*1000.0f),sObject);
				LogFeedback(&sToLog[0],true,"ExecuteTimeLine:EFF:CT_END",true);
			}; break;
			////////////////////////////////////////
			// DEPRECATED
			////////////////////////////////////////
				case CT_SEND:
				{
					// send the param to the object by calling the objects 'ReceiveParam()' function
					pEffectObject->ReceiveParam(pTLE->GetCommandParamString(0));
					sToLog.Format("On %d ms.: %s: Sent %s to effect.",(long)(m_gpDemoDat->GetfElapsedTimeInSecs()*1000.0f),
							sObject, pTLE->GetCommandParamString(0));
					LogFeedback(&sToLog[0],true,"ExecuteTimeLine:EFF:CT_SEND",true);
				}; break;
			////////////////////////////////////////
			// END DEPRECATED
			////////////////////////////////////////
			case CT_MOVETOLYR:
			{
				// move object from 1 layerobject to the other. this can be complicated, so don't do this a lot :)
				// layer that is abandoned is removed
				lLongParam = atol(pTLE->GetCommandParamString(0));
				if(lLongParam<0)
				{
					// illegal.
					sError.Format("MOVETOLYR Command supplied for object %s has invalid layernumber. Ignored.", sObject);
					LogFeedback(&sError[0],true,"ExecuteTimeLine:EFF:CT_MOVETOLYR",false);
					break;
				}
				MoveEffectToLayer(sObject,lLongParam);
				sToLog.Format("On %d ms.: %s: Effect moved to layer %d.",(long)(m_gpDemoDat->GetfElapsedTimeInSecs()*1000.0f)
							,sObject,lLongParam);
				LogFeedback(&sToLog[0],true,"ExecuteTimeLine:EFF:CT_MOVETOLYR",true);
			}; break;
			case CT_SENDINT:
			{
				// send the param to the object as integer by calling the objects 'ReceiveInt()' function
				pEffectObject->ReceiveInt(pTLE->GetCommandParamString(0), pTLE->GetCommandParamInt(1),
								pTLE->GetCommandParamInt(2));
				sToLog.Format("On %d ms.: %s: Sent integer %d with name %s to effect on layer %d.",
						(long)(m_gpDemoDat->GetfElapsedTimeInSecs()*1000.0f), sObject, 
						pTLE->GetCommandParamInt(1), pTLE->GetCommandParamString(0),pTLE->GetCommandParamInt(2));
				LogFeedback(&sToLog[0],true,"ExecuteTimeLine:EFF:CT_SENDINT",true);
			}; break;
			case CT_SENDFLOAT:
			{
				// send the param to the object as float by calling the objects 'ReceiveFloat()' function
				pEffectObject->ReceiveFloat(pTLE->GetCommandParamString(0), pTLE->GetCommandParamFloat(1),
								pTLE->GetCommandParamInt(2));
				sToLog.Format("On %d ms.: %s: Sent float %f with name %s to effect on layer %d.",
						(long)(m_gpDemoDat->GetfElapsedTimeInSecs()*1000.0f), sObject, 
						pTLE->GetCommandParamFloat(1), pTLE->GetCommandParamString(0),pTLE->GetCommandParamInt(2));
				LogFeedback(&sToLog[0],true,"ExecuteTimeLine:EFF:CT_SENDFLOAT",true);
			}; break;
			case CT_SENDSTRING:
			{
				// send the param to the object as string by calling the objects 'ReceiveString()' function
				pEffectObject->ReceiveString(pTLE->GetCommandParamString(0), pTLE->GetCommandParamString(1),
								pTLE->GetCommandParamInt(2));
				sToLog.Format("On %d ms.: %s: Sent string %s with name %s to effect on layer %d.",
						(long)(m_gpDemoDat->GetfElapsedTimeInSecs()*1000.0f), sObject, 
						pTLE->GetCommandParamString(1), pTLE->GetCommandParamString(0),pTLE->GetCommandParamInt(2));
				LogFeedback(&sToLog[0],true,"ExecuteTimeLine:EFF:CT_SENDSTRING",true);
			}; break;
			///////////
			//// ADD MORE COMMANDS FOR EFFECTS HERE
			///////////
			default:
			{
				// Illegal command.
				sError.Format("Illegal Command specified with effect: %s: %d. Ignored.",sObject,iCommand);
				LogFeedback(&sError[0],true,"ExecuteTimeLine:EFF:default",false);
			}; break;
		}
	}
}

