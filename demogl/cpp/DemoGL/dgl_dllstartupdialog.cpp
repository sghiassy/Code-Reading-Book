//////////////////////////////////////////////////////////////////////
// FILE: dgl_dllstartupdialog.cpp
// PURPOSE: the code for the startup dialog and the aboutdialog, accessable
// from the startup dialog.
//////////////////////////////////////////////////////////////////////
// SHORT DESCRIPTION:
// The startup dialog is opened in DEMOGL_AppRun(), using runtype RUNTYPE_NORMAL
// or in RUNTYPE_SCREENSAVER in screensaver execmode DGL_SSAVERET_CONFIG
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
// v1.3: Added to codebase.
// v1.2: --
// v1.1: --
//////////////////////////////////////////////////////////////////////

#include "DemoGL\dgl_dllstdafx.h"
#include <shellapi.h>
#include <commctrl.h>
#include "Misc\zlib.h"

///////////////////////////////////////////////////////////////////////
//
// Global variables for about dlg opengl canvas and rotating logo
//
///////////////////////////////////////////////////////////////////////

static	HDC					m_ghAboutDlgDC;
static	HWND				m_ghAboutLogoWnd;
static	WINDOWPLACEMENT		m_gwplLogo;
static	HGLRC				m_ghAboutDlgRC;
static	HBITMAP				m_ghAppLogoBitmapHandle;
static	int					m_giTimerID;
static	GLuint				m_garriTextureIDs[MAXDLGTEXTURES];
static	GLuint				m_giDGLDlists;
static	float				m_gfAlpha=0.0f;
static	float				m_gfBeta=0.0f;
static	float				m_gfGamma=0.0f;


////////////////////////////////////////////////////////////////////////
//                          
//						CODE
//
////////////////////////////////////////////////////////////////////////

// Purpose: The messagehandler of the startup dialog.
BOOL CALLBACK 
StartupDlgMsgHandler(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam) 
{ 
	int							iButton, iResolution, i;
	LRESULT						lrResult;
	HWND						hResCBox, hAppLogo, hGreyBar, hSoundDevices, hSliderVolume;
	SStartupDat					*pStartupDat;
	SSoundSystemOptionDat		*pSSODat;
	CStdString					sConvert;
	DWORD						dwVolumeSliderPos;

    switch (iMsg) 
    {
		case WM_INITDIALOG:
		{
			// Get pointer to startupdat structure
			pStartupDat=m_gpDemoDat->GetStartupDat();

			// get pointer to what's possible with the soundsystem.
			// it can be possible that this is not initialized by the caller. it is then containing
			// DemoGL startup defaults which state no sounddevices are found.
			pSSODat=m_gpDemoDat->GetSSODat();

			// Read startupdata from registry
			LoadStartupDatFromRegistry(pStartupDat);

			// Get handle of Resolution combobox
			hResCBox=GetDlgItem(hWnd,IDC_CMBRESOLUTION);
			// Get handle of App Logo picture holder
			hAppLogo=GetDlgItem(hWnd,IDC_APPLOGO);
			// Get handle of grey bar
			hGreyBar=GetDlgItem(hWnd,IDC_GREYBAR);
			// Get handle of Sounddevices dropdownbox
			hSoundDevices=GetDlgItem(hWnd, IDC_CMBSOUNDDEVICES);
			// Get handle of volume slider
			hSliderVolume=GetDlgItem(hWnd,IDC_SLVOLUME);

			// Load applogo from application's resources
			m_ghAppLogoBitmapHandle=LoadBitmap(m_gpDemoDat->GethInstance(),MAKEINTRESOURCE(IDB_APPLOGO));
			if(!m_ghAppLogoBitmapHandle)
			{
				// visualize rectangle
				ShowWindow(hGreyBar,SW_SHOW);
				// hide applogo picturebox
				ShowWindow(hAppLogo,SW_HIDE);
			}
			else
			{
				// visualize applogo picturebox,
				ShowWindow(hAppLogo,SW_SHOW);
				// hide rectangle
				ShowWindow(hGreyBar,SW_HIDE);
				// send message
				SendMessage(hAppLogo,STM_SETIMAGE,IMAGE_BITMAP,(LPARAM)m_ghAppLogoBitmapHandle);
			}

			// Appearance
			iButton = IDC_RDBAPPWIN;
			if(pStartupDat->m_bFullScreen)
			{
				iButton = IDC_RDBAPPFS;
			}
			CheckRadioButton(hWnd,IDC_RDBAPPFS,IDC_RDBAPPWIN,iButton);

			// Display quality
			iButton = IDC_RDBDQ16;
			if(pStartupDat->m_bDQ32bit)
			{
				iButton = IDC_RDBDQ32;
			}
			CheckRadioButton(hWnd,IDC_RDBDQ32,IDC_RDBDQ16,iButton);

			// Texture quality
			iButton = IDC_RDBTQ16;
			if(pStartupDat->m_bTQ32bit)
			{
				iButton = IDC_RDBTQ32;
			}
			CheckRadioButton(hWnd,IDC_RDBTQ32,IDC_RDBTQ16,iButton);

			// Sound
			if(m_gpDemoDat->GetbSoundCanBeEnabled() && pSSODat->m_bSound_Possible)
			{
				// BASS reported, there are devices, so sound should be possible
				iButton = IDC_RDBSNDNO;
				if(pStartupDat->m_bSound)
				{
					iButton = IDC_RDBSNDYES;
				}
				CheckRadioButton(hWnd,IDC_RDBSNDYES,IDC_RDBSNDNO,iButton);

				// Volume slider
				// Set range (0-100)
				SendMessage(hSliderVolume,TBM_SETRANGE,(WPARAM)FALSE,(LPARAM)MAKELONG(0,100));
				// Set current value, reported in startupdat in slider
				SendMessage(hSliderVolume,TBM_SETPOS,(WPARAM)TRUE,(LPARAM)pStartupDat->m_iSS_OverallVolume);
				// Set current value, reported in startupdat in tbx that displayes the decimal value
				sConvert.Format("%d",pStartupDat->m_iSS_OverallVolume);
				SetDlgItemText(hWnd,IDC_TBXVOLUME,&sConvert[0]);;

				// Sound Devices drop downbox
				SendMessage(hSoundDevices,LB_RESETCONTENT,0,0);
				// Fill soundsystem device dropdownbox
				for(i=0;i<pSSODat->m_iAmDevices;i++)
				{
					lrResult=SendMessage(hSoundDevices,CB_INSERTSTRING,i,(LPARAM)pSSODat->m_arrssdoDevices[i].m_sDescription);
				}
				// Select the device supplied with the StartupDat structure. if that device number is not available, select
				// first. Because sound is possible, there are devices, so there is always at least _ONE_
				// device in the dropdown box
				if(pStartupDat->m_iSS_SoundDevice>=pSSODat->m_iAmDevices)
				{
					// select first
					SendMessage(hSoundDevices,CB_SETCURSEL,0,0);
				}
				else
				{
					SendMessage(hSoundDevices,CB_SETCURSEL,pStartupDat->m_iSS_SoundDevice,0);
				}

				// 8/16 bit quality
				iButton=IDC_RDBSND8BIT;
				if(pStartupDat->m_bSS_16bit)
				{
					iButton=IDC_RDBSND16BIT;
				}
				CheckRadioButton(hWnd,IDC_RDBSND8BIT,IDC_RDBSND16BIT,iButton);
		
				// mono/stereo
				iButton=IDC_RDBSNDMONO;
				if(pStartupDat->m_bSS_Stereo)
				{
					iButton=IDC_RDBSNDSTEREO;
				}
				CheckRadioButton(hWnd,IDC_RDBSNDMONO,IDC_RDBSNDSTEREO,iButton);

				// low/high quality
				if(pStartupDat->m_bSS_LowQuality)
				{
					CheckDlgButton(hWnd,IDC_CHKSNDLOWQ,BST_CHECKED);
				}
				
				// A3D
				if(pStartupDat->m_bSS_A3D)
				{
					CheckDlgButton(hWnd,IDC_CHKA3D,BST_CHECKED);
				}

				// EAX
				if(pStartupDat->m_bSS_EAX)
				{
					CheckDlgButton(hWnd,IDC_CHKEAX,BST_CHECKED);
				}

				ControlSoundSystemDialogItems(hWnd,!pStartupDat->m_bSound);
			}
			else
			{
				// no sound possible
				CheckRadioButton(hWnd,IDC_RDBSNDYES,IDC_RDBSNDNO,IDC_RDBSNDNO);
				// disable the 'enabled' radiobutton.
				EnableWindow(GetDlgItem(hWnd,IDC_RDBSNDYES),false);
				ControlSoundSystemDialogItems(hWnd, true);
			}

			// Resolution dropdownbox.
			SendMessage(hResCBox,LB_RESETCONTENT,0,0);

			// Fill resolution dropdown box
			lrResult=SendMessage(hResCBox,CB_INSERTSTRING,INDX_RES320x240,(LPARAM)STR_RES320x240);
			lrResult=SendMessage(hResCBox,CB_INSERTSTRING,INDX_RES400x300,(LPARAM)STR_RES400x300);
			lrResult=SendMessage(hResCBox,CB_INSERTSTRING,INDX_RES512x384,(LPARAM)STR_RES512x384);
			lrResult=SendMessage(hResCBox,CB_INSERTSTRING,INDX_RES640x480,(LPARAM)STR_RES640x480);
			lrResult=SendMessage(hResCBox,CB_INSERTSTRING,INDX_RES800x600,(LPARAM)STR_RES800x600);
			lrResult=SendMessage(hResCBox,CB_INSERTSTRING,INDX_RES960x720,(LPARAM)STR_RES960x720);
			lrResult=SendMessage(hResCBox,CB_INSERTSTRING,INDX_RES1024x768,(LPARAM)STR_RES1024x768);
			lrResult=SendMessage(hResCBox,CB_INSERTSTRING,INDX_RES1152x864,(LPARAM)STR_RES1152x864);
			lrResult=SendMessage(hResCBox,CB_INSERTSTRING,INDX_RES1280x1024,(LPARAM)STR_RES1280x1024);
			lrResult=SendMessage(hResCBox,CB_INSERTSTRING,INDX_RES1600x1200,(LPARAM)STR_RES1600x1200);
			lrResult=SendMessage(hResCBox,CB_INSERTSTRING,INDX_RES2048x1536,(LPARAM)STR_RES2048x1536);
			// ADD MORE RESOLUTIONS HERE

			// select the default resolution
			switch(pStartupDat->m_iResolution)
			{
				case RES800x600:
				{
					SendMessage(hResCBox,CB_SETCURSEL,INDX_RES800x600,0);
				}; break;
				case RES640x480:
				{
					SendMessage(hResCBox,CB_SETCURSEL,INDX_RES640x480,0);
				}; break;
				case RES1024x768:
				{
					SendMessage(hResCBox,CB_SETCURSEL,INDX_RES1024x768,0);
				}; break;
				case RES320x240:
				{
					SendMessage(hResCBox,CB_SETCURSEL,INDX_RES320x240,0);
				}; break;
				case RES400x300:
				{
					SendMessage(hResCBox,CB_SETCURSEL,INDX_RES400x300,0);
				}; break;
				case RES512x384:
				{
					SendMessage(hResCBox,CB_SETCURSEL,INDX_RES512x384,0);
				}; break;
				case RES960x720:
				{
					SendMessage(hResCBox,CB_SETCURSEL,INDX_RES960x720,0);
				}; break;
				case RES1152x864:	
				{
					SendMessage(hResCBox,CB_SETCURSEL,INDX_RES1152x864,0);
				}; break;
				case RES1280x1024:
				{
					SendMessage(hResCBox,CB_SETCURSEL,INDX_RES1280x1024,0);
				}; break;
				case RES1600x1200:
				{
					SendMessage(hResCBox,CB_SETCURSEL,INDX_RES1600x1200,0);
				}; break;
				case RES2048x1536:
				{
					SendMessage(hResCBox,CB_SETCURSEL,INDX_RES2048x1536,0);
				}; break;
				// ADD MORE RESOLUTION CONSTANTS HERE
				default:
				{
					// unknown resolution, choose 800x600
					SendMessage(hResCBox,CB_SETCURSEL,INDX_RES800x600,0);
				}; break;
			}

			// set textboxes
			if(!pStartupDat->m_sDemoName)
			{
				SetDlgItemText(hWnd,IDC_TBXTITLE,"A Demo");
			}
			else
			{
				SetDlgItemText(hWnd,IDC_TBXTITLE,pStartupDat->m_sDemoName);
			}

			if(!pStartupDat->m_sReleasedBy)
			{
				SetDlgItemText(hWnd,IDC_TBXRELEASEDBY,"Well... you know.");
			}
			else
			{
				SetDlgItemText(hWnd,IDC_TBXRELEASEDBY,pStartupDat->m_sReleasedBy);
			}

			// Save in registry. Check always when config dialog for screensaver
			if((pStartupDat->m_bSaveInRegistry)||(m_gpDemoDat->GetRunType()==RUNTYPE_SAVER_CONFIG))
			{
				CheckDlgButton(hWnd,IDC_CHKREGSAVE,BST_CHECKED);
			}

			// Shrink textures if needed. Control is disabled by default.
			if(!m_gpDemoDat->GetbNeverRescaleTextures())
			{
				// it's allowed to rescale textures. Enable control.
				EnableWindow(GetDlgItem(hWnd,IDC_CHKRESCALETEX),true);
				// set control to value specified in startup dat.
				if(pStartupDat->m_bRescaleTextures)
				{
					CheckDlgButton(hWnd,IDC_CHKRESCALETEX,BST_CHECKED);
				}
			}

			// If we're running as RUNTYPE_SAVER_CONFIG, we have to modify the text on the start button
			// so it says "Save" instead of "Start!"
			if(m_gpDemoDat->GetRunType()==RUNTYPE_SAVER_CONFIG)
			{
				SetDlgItemText(hWnd,IDC_BTNSTART,"Save");
			}
			return TRUE;
		}; break;

		case WM_LBUTTONDOWN:
		{
			// trick windows to think we clicked the titlebar, so the dialog is moveable by click-drag in the complete
			// dialog :D
			PostMessage(hWnd,WM_NCLBUTTONDOWN,HTCAPTION,NULL);
			return FALSE;
		}; break;

		case WM_HSCROLL:
		{
			// Get pointer to startupdat structure
			pStartupDat=m_gpDemoDat->GetStartupDat();

			// Get handle of volume slider
			hSliderVolume=GetDlgItem(hWnd,IDC_SLVOLUME);
			switch(LOWORD(wParam))
			{
				case SB_THUMBTRACK:
				case SB_ENDSCROLL:
				case SB_LINELEFT:
				case SB_LINERIGHT:
				case SB_PAGELEFT:
				case SB_PAGERIGHT:
				{
					// Get position of slider.
					dwVolumeSliderPos=SendMessage(hSliderVolume, TBM_GETPOS, 0, 0); 
					// Set this value in the tbx that reflects the slider pos in decimal.
					sConvert.Format("%d",dwVolumeSliderPos);
					SetDlgItemText(hWnd,IDC_TBXVOLUME,&sConvert[0]);
				}; break;
			}
			return FALSE;
		}; break;

		case WM_COMMAND:
		{
			// Get pointer to startupdat structure
			pStartupDat=m_gpDemoDat->GetStartupDat();

			// Get handle of Resolution combobox
			hResCBox=GetDlgItem(hWnd,IDC_CMBRESOLUTION);
			// Get handle of Sounddevices dropdownbox
			hSoundDevices=GetDlgItem(hWnd, IDC_CMBSOUNDDEVICES);
			// Get handle of volume slider
			hSliderVolume=GetDlgItem(hWnd,IDC_SLVOLUME);

			switch(HIWORD(wParam))
			{
				case EN_SETFOCUS:
				{
					if(((HWND)lParam==GetDlgItem(hWnd,IDC_TBXTITLE))||((HWND)lParam==GetDlgItem(hWnd,IDC_TBXRELEASEDBY)))
					{
						// set focus to dropdownbox. So no-one can edit the boxes (not that that's useful, but
						// it's nicer
						SetFocus(hResCBox);
					}
				}; break;
				case CBN_SELCHANGE:
				{
					switch(LOWORD(wParam))
					{
						case IDC_CMBSOUNDDEVICES:
						{
							ControlSoundSystemDialogItems(hWnd,(IsDlgButtonChecked(hWnd,IDC_RDBSNDNO)==BST_CHECKED));
						}; break;
					}
				}; break;
				case BN_CLICKED:
				{ 
					switch(LOWORD(wParam)) 
					{ 
						case IDC_BTNSTART: 
						{
							// Get values from dialog.
							pStartupDat->m_bFullScreen=(IsDlgButtonChecked(hWnd,IDC_RDBAPPFS)==BST_CHECKED);
							pStartupDat->m_bDQ32bit=(IsDlgButtonChecked(hWnd,IDC_RDBDQ32)==BST_CHECKED);
							pStartupDat->m_bTQ32bit=(IsDlgButtonChecked(hWnd,IDC_RDBTQ32)==BST_CHECKED);
							pStartupDat->m_bSound = (IsDlgButtonChecked(hWnd,IDC_RDBSNDYES)==BST_CHECKED);
							pStartupDat->m_bSaveInRegistry = (IsDlgButtonChecked(hWnd,IDC_CHKREGSAVE)==BST_CHECKED);
							pStartupDat->m_bRescaleTextures=(IsDlgButtonChecked(hWnd,IDC_CHKRESCALETEX)==BST_CHECKED);
							pStartupDat->m_bSS_16bit=(IsDlgButtonChecked(hWnd,IDC_RDBSND16BIT)==BST_CHECKED);
							pStartupDat->m_bSS_A3D=(IsDlgButtonChecked(hWnd,IDC_CHKA3D)==BST_CHECKED);
							pStartupDat->m_bSS_EAX=(IsDlgButtonChecked(hWnd,IDC_CHKEAX)==BST_CHECKED);
							pStartupDat->m_bSS_Stereo=(IsDlgButtonChecked(hWnd,IDC_RDBSNDSTEREO)==BST_CHECKED);
							pStartupDat->m_bSS_LowQuality=(IsDlgButtonChecked(hWnd,IDC_CHKSNDLOWQ)==BST_CHECKED);
							pStartupDat->m_iSS_SoundDevice=SendMessage(hSoundDevices,CB_GETCURSEL,0,0);
							pStartupDat->m_iSS_OverallVolume=(int)SendMessage(hSliderVolume, TBM_GETPOS, 0, 0); 

							// Get resolution.
							iResolution=SendMessage(hResCBox,CB_GETCURSEL,0,0);

							// this could have been done smarter but we're stuck with backward compatibility
							switch(iResolution)
							{
								case INDX_RES800x600:
								{
									pStartupDat->m_iResolution=RES800x600;
								}; break;
								case INDX_RES640x480:
								{
									pStartupDat->m_iResolution=RES640x480;
								}; break;
								case INDX_RES1024x768:
								{
									pStartupDat->m_iResolution=RES1024x768;
								}; break;
								case INDX_RES320x240:
								{
									pStartupDat->m_iResolution=RES320x240;
								}; break;
								case INDX_RES400x300:
								{
									pStartupDat->m_iResolution=RES400x300;
								}; break;
								case INDX_RES512x384:
								{
									pStartupDat->m_iResolution=RES512x384;
								}; break;
								case INDX_RES960x720:
								{
									pStartupDat->m_iResolution=RES960x720;
								}; break;
								case INDX_RES1152x864:	
								{
									pStartupDat->m_iResolution=RES1152x864;
								}; break;
								case INDX_RES1280x1024:
								{
									pStartupDat->m_iResolution=RES1280x1024;
								}; break;
								case INDX_RES1600x1200:
								{
									pStartupDat->m_iResolution=RES1600x1200;
								}; break;
								case INDX_RES2048x1536:
								{
									pStartupDat->m_iResolution=RES2048x1536;
								}; break;
								// ADD MORE RESOLUTION CONSTANTS HERE
								default:
								{
									// nothing selected. select 800x600
									pStartupDat->m_iResolution=RES800x600;
								}; break;
							};
							// ADD MORE OPTIONS HERE

							DeleteObject(m_ghAppLogoBitmapHandle);

							// Check if the user wanted the settings to be stored in the registry
							if(pStartupDat->m_bSaveInRegistry)
							{
								SaveStartupDatInRegistry(pStartupDat);
							}

							// Done
							EndDialog(hWnd,SYS_SUD_OKCLICKED);
						}; break;
						case IDC_BTNCANCEL:
						case IDCANCEL:
						{
							// flag quit
							EndDialog(hWnd,SYS_SUD_CANCELCLICKED);
							DeleteObject(m_ghAppLogoBitmapHandle);
						}; break;
						case IDC_BTNABOUT:
						{
							// open about dialog.
							DialogBox(m_ghDLLInstance,MAKEINTRESOURCE(IDD_ABOUTFORM),hWnd,AboutDlgMsgHandler);
						}; break;
						case IDC_RDBSNDYES:
						case IDC_RDBSNDNO:
						{
							// enable/disable soundsystem options, based on the new value of the radiobutton group.
							ControlSoundSystemDialogItems(hWnd,(IsDlgButtonChecked(hWnd,IDC_RDBSNDNO)==BST_CHECKED));
						}; break;
					}
				};break;
			}
			return TRUE;
		}; break;
        default: 
		{
            return FALSE; 
		}
    } 
} 


// Purpose: sets the various soundsystem controls when the user selects a different device and/or the
// user selects Sound/No sound
void
ControlSoundSystemDialogItems(HWND hDlg, bool bAllOff)
{
	HWND						hSoundDevices, hMono, hStereo, h8Bit, h16Bit, hLowQ, hA3D, hEAX, hSliderVolume;
	HWND						hLblVolumeMin, hLblVolumeMax;
	SSoundSystemOptionDat		*pSSODat;
	int							iSelectedDevice;
		
	// Get handles
	hSoundDevices=GetDlgItem(hDlg, IDC_CMBSOUNDDEVICES);
	hMono=GetDlgItem(hDlg, IDC_RDBSNDMONO);
	hStereo=GetDlgItem(hDlg, IDC_RDBSNDSTEREO);
	h8Bit=GetDlgItem(hDlg, IDC_RDBSND8BIT);
	h16Bit=GetDlgItem(hDlg, IDC_RDBSND16BIT);
	hLowQ=GetDlgItem(hDlg, IDC_CHKSNDLOWQ);
	hA3D=GetDlgItem(hDlg, IDC_CHKA3D);
	hEAX=GetDlgItem(hDlg, IDC_CHKEAX);
	hSliderVolume = GetDlgItem(hDlg,IDC_SLVOLUME);
	hLblVolumeMin = GetDlgItem(hDlg,IDC_LBLVOLMIN);
	hLblVolumeMax = GetDlgItem(hDlg,IDC_LBLVOLMAX);

	// ADD MORE SOUNDCONTROLS HERE

	if(bAllOff)
	{
		EnableWindow(hSoundDevices,false);
		EnableWindow(hMono,false);
		EnableWindow(hStereo,false);
		EnableWindow(h8Bit,false);
		EnableWindow(h16Bit, false);
		EnableWindow(hLowQ,false);
		EnableWindow(hA3D,false);
		EnableWindow(hEAX,false);
		EnableWindow(hSliderVolume,false);
		EnableWindow(hLblVolumeMin,false);
		EnableWindow(hLblVolumeMax,false);

		// ADD MORE SOUNDCONTROLS HERE
		return;
	}
	else
	{
		pSSODat=m_gpDemoDat->GetSSODat();

		iSelectedDevice=SendMessage(hSoundDevices,CB_GETCURSEL,0,0);

		// enable everything and then switch off what's not available
		EnableWindow(hSoundDevices,true);
		EnableWindow(hMono,true);
		EnableWindow(hStereo,true);
		EnableWindow(h8Bit,true);
		EnableWindow(h16Bit, true);
		EnableWindow(hLowQ,true);
		EnableWindow(hSliderVolume,true);
		EnableWindow(hLblVolumeMin,true);
		EnableWindow(hLblVolumeMax,true);

		if(pSSODat->m_arrssdoDevices[iSelectedDevice].m_b3DSound_A3DPossible)
		{
			EnableWindow(hA3D,true);
		}
		else
		{
			EnableWindow(hA3D,false);
		}
		if(pSSODat->m_arrssdoDevices[iSelectedDevice].m_b3DSound_EAXPossible)
		{
			EnableWindow(hEAX,true);
		}
		else
		{
			EnableWindow(hEAX,false);
		}
	}
}


////////////////////////////////////////////////////////////////////////////////////////////////
//
//                             ABOUT DIALOG FUNCTIONS
//
////////////////////////////////////////////////////////////////////////////////////////////////

// Purpose: the messagehandler of the OpenGL powered rotating logo in the About box
long WINAPI
AboutLogoMsgHandler(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	int		iRCWidth, iRCHeight;

	switch(iMsg)
	{
		case WM_CREATE:
		{
			// Get the DC
			m_ghAboutDlgDC=GetDC(hWnd);

			// setup pixelformat
			if(SetupPixelFormat(m_ghAboutDlgDC)!=SYS_OK)
			{
				// not valid
				DestroyWindow(hWnd);
				return 0;
			}

			// create rendercontext
			m_ghAboutDlgRC = wglCreateContext(m_ghAboutDlgDC);
			if(!m_ghAboutDlgRC)
			{
				// not succeeded
				DestroyWindow(hWnd);
				return 0;
			}

			// make current
			wglMakeCurrent(m_ghAboutDlgDC, m_ghAboutDlgRC);

			// Init OpenGL specific aspects
			glClearColor(0.0f,0.0f,0.0f,1.0f);
			glShadeModel(GL_SMOOTH);							// set default shading.
			glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_NICEST);	// set perspective correcthint

			iRCWidth=m_gwplLogo.rcNormalPosition.right-m_gwplLogo.rcNormalPosition.left;
			iRCHeight=m_gwplLogo.rcNormalPosition.bottom-m_gwplLogo.rcNormalPosition.top;
			if(iRCHeight==0)
			{
				// won't happen but perhaps wacky drivers will cause this failure.
				iRCHeight=1;
			}

			glMatrixMode (GL_PROJECTION);    
			glLoadIdentity (); 

			// set up frustum. Compensate the frustum to still have correct proportions, allthough the frustum is wide
			glFrustum (-1.0, 1.0, -((double)iRCHeight/(double)iRCWidth), ((double)iRCHeight/(double)iRCWidth), 1.0, 20.0); 

			// set viewport at the same spot.
			glViewport(0,0,iRCWidth, iRCHeight);

			glMatrixMode (GL_MODELVIEW);  

			// generate textureID's
			glGenTextures(MAXDLGTEXTURES,m_garriTextureIDs);

			// generate displaylist ID's
			m_giDGLDlists=glGenLists(MAXDGLDLISTS);
			// first quad list
			glNewList(m_giDGLDlists + FRONTQUADDL, GL_COMPILE);
				glBegin(GL_QUADS);
					glNormal3f(0.0f,0.0f,1.0f);
					glTexCoord2f(1.0f,0.0f);glVertex3f(0.0f - XTRANS,0.0f - YTRANS,0.0f);
					glTexCoord2f(0.0f,0.0f);glVertex3f(LWIDTH - XTRANS,0.0f - YTRANS,0.0f);
					glTexCoord2f(0.0f,1.0f);glVertex3f(LWIDTH - XTRANS,LHEIGHT - YTRANS,0.0f);
					glTexCoord2f(1.0f,1.0f);glVertex3f(0.0f - XTRANS,LHEIGHT - YTRANS,0.0f);
				glEnd();
			glEndList();
			// second quad
			glNewList(m_giDGLDlists + BACKQUADDL, GL_COMPILE);
				glBegin(GL_QUADS);
					glNormal3f(0.0f,0.0f,-1.0f);
					glTexCoord2f(1.0f,0.0f);glVertex3f(LWIDTH - XTRANS,0.0f - YTRANS,-0.01f);
					glTexCoord2f(0.0f,0.0f);glVertex3f(0.0f - XTRANS,0.0f - YTRANS,-0.01f);
					glTexCoord2f(0.0f,1.0f);glVertex3f(0.0f - XTRANS,LHEIGHT - YTRANS,-0.01f);
					glTexCoord2f(1.0f,1.0f);glVertex3f(LWIDTH - XTRANS,LHEIGHT - YTRANS,-0.01f);
				glEnd();
			glEndList();

			// create the timer. 20 ms per frame, roughly 50fps. any faster will make this type of timer go bezerk
			// on some systems.
			m_giTimerID=SetTimer(hWnd,1,20,(TIMERPROC) AboutDlgTimerHandler);

			// extract textures and upload textures to opengl
			ExtractAndUploadADlgTextures();

			// set some opengl states we use every frame:
			glEnable(GL_TEXTURE_2D);
			glEnable(GL_CULL_FACE);
			glFrontFace(GL_CW);
			glCullFace(GL_BACK);
			glPolygonMode(GL_FRONT,GL_FILL);
			glEnable(GL_BLEND);
			glDisable(GL_LIGHTING);
		}; break;

		case WM_DESTROY:
		{
			KillTimer(hWnd,1);
			wglDeleteContext(m_ghAboutDlgRC);
			ReleaseDC(hWnd,m_ghAboutDlgDC);
			wglMakeCurrent(NULL,NULL);
		}; break;
	}
	return DefWindowProc(hWnd,iMsg,wParam,lParam);
}


// Purpose: The messagehandler of the about dialog.
BOOL CALLBACK 
AboutDlgMsgHandler(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam) 
{
	CStdString			sVersion;
	WNDCLASS			wcOglWnd;
	RECT				rcCanvas;
	int					iCanvasDx, iCanvasDy;
	HWND				hLogoWnd;

    switch (iMsg) 
    { 
		case WM_INITDIALOG:
		{
			sVersion.Format("%1.2f.%05d.%s",DEMOGL_VERSION,DEMOGL_BUILD,DEMOGL_VERSIONTYPE);
			SetDlgItemText(hWnd,IDC_VERSION,sVersion);

			wcOglWnd.style=CS_HREDRAW | CS_VREDRAW;
			wcOglWnd.lpfnWndProc=AboutLogoMsgHandler;
			wcOglWnd.cbClsExtra=0;
			wcOglWnd.cbWndExtra=0;
			wcOglWnd.hInstance=m_ghDLLInstance;
			wcOglWnd.hIcon=NULL;
			wcOglWnd.hCursor=NULL;
			wcOglWnd.hbrBackground=(HBRUSH)GetStockObject(BLACK_BRUSH);
			wcOglWnd.lpszMenuName=NULL;
			wcOglWnd.lpszClassName="AdlgLogoClass";
			RegisterClass(&wcOglWnd);
			
			// Get position of image.
			hLogoWnd=GetDlgItem(hWnd,IDC_DGLLOGOBMP);
			m_gwplLogo.length = sizeof(WINDOWPLACEMENT);
			GetWindowPlacement(hLogoWnd,&m_gwplLogo);
			GetWindowRect(hLogoWnd,&rcCanvas);
			iCanvasDx=rcCanvas.right - rcCanvas.left;
			iCanvasDy=rcCanvas.bottom - rcCanvas.top;  
			m_ghAboutLogoWnd=CreateWindowEx(0,"AdlgLogoClass","DGL Logo",WS_CHILD|WS_VISIBLE,
                              0,0,iCanvasDx,iCanvasDy,hLogoWnd,NULL,m_ghDLLInstance,NULL);
			return TRUE;
		}; break;
		case WM_COMMAND:
		{
			if (HIWORD(wParam) == BN_CLICKED) 
			{ 
				switch (LOWORD(wParam)) 
				{ 
					case IDC_OK:
					case IDCANCEL:
					{
						// flag OK
						EndDialog(hWnd,SYS_SUD_OKCLICKED);
					}; break;
					case IDC_BTNWEBSITE:
					{
						// open website
						ShellExecute(0, "open", DEMOGLWEBSITE, 0, 0, SW_SHOW);
					}
				}
			}
			return TRUE;
		}; break;
		default:
		{
			return FALSE;
		}
	}
}


// Purpose: extracts the textureresources from the dll and uploads these to OpenGL for usage
// in the AboutDlg. It is NOT using DemoGL texture management capabilities because those datastorages are
// not yet Initialized. 
// This routine assumes there is an active RenderContext. 
void
ExtractAndUploadADlgTextures()
{
	byte			*pRawRCData, *pTmpTexData, *pUnzippedData;
	HRSRC			hsResourceFH;
	HGLOBAL			hgResourceLH;
	int				iResult, iWidth, iHeight;
	unsigned long	lDstLength;
	bool			bHasAlpha;

	// Load dgllogo texture
	hsResourceFH = FindResource(m_ghDLLInstance,MAKEINTRESOURCE(IDR_DGLLOGOTGA),"ZIPPEDTGA");
	if(hsResourceFH)
	{
		// load the resource
		hgResourceLH = LoadResource(m_ghDLLInstance,hsResourceFH);
		if(hgResourceLH)
		{
			// lock it to get a pointer.
			pRawRCData = (byte *)LockResource(hgResourceLH);

			// create buffer for unzipped data
			pUnzippedData = (byte *)malloc(4 * DGLLOGOWIDTH * DGLLOGOHEIGHT);
			lDstLength=4 * DGLLOGOWIDTH * DGLLOGOHEIGHT;

			// unzip. the data is compressed with compress(), also a function of zlib
			iResult = uncompress(pUnzippedData, &lDstLength, pRawRCData, SizeofResource(m_ghDLLInstance, hsResourceFH));

			// create the buffer
			pTmpTexData = (byte *)malloc(4 * DGLLOGOWIDTH * DGLLOGOHEIGHT);

			// do conversion to RGBA
			iResult=ConvertTGAtoRGBA(pUnzippedData, pTmpTexData,DGLLOGOWIDTH, DGLLOGOHEIGHT, false, &bHasAlpha);

			// Upload the buffer
			glBindTexture( GL_TEXTURE_2D, m_garriTextureIDs[LOGOTEXIDIDX]);		
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
			glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA8,DGLLOGOWIDTH, DGLLOGOHEIGHT,0,GL_RGBA, GL_UNSIGNED_BYTE, pTmpTexData);

			// set additional texture params. These will be stored with the texture object in OpenGL.
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

			// Free the buffer
			free(pTmpTexData);
			free(pUnzippedData);
		}
	}

	// Load the envmap (it's a lensflare)
	hsResourceFH = FindResource(m_ghDLLInstance,MAKEINTRESOURCE(IDR_LENSFLARE),"JPG");
	if(hsResourceFH)
	{
		// load the resource
		hgResourceLH = LoadResource(m_ghDLLInstance,hsResourceFH);
		if(hgResourceLH)
		{
			// lock it to get a pointer.
			pRawRCData = (byte *)LockResource(hgResourceLH);

			// convert the JPG compressed buffer to RGBA
			pTmpTexData = ConvertJPGtoRGBA(pRawRCData,(long)SizeofResource(m_ghDLLInstance,hsResourceFH),&iWidth, &iHeight, false);

			// Upload the buffer
			glBindTexture( GL_TEXTURE_2D, m_garriTextureIDs[ENVMAPTEXIDIDX]);		
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
			glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA8,iWidth, iHeight,0,GL_RGBA, GL_UNSIGNED_BYTE, pTmpTexData);

			// set additional texture params. These will be stored with the texture object in OpenGL.
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

			// Free the buffer
			free(pTmpTexData);
		}
	}
}


// Purpose: draws a rotating DemoGL Logo in the dialogbox
void
DrawDGLLogoInDlg()
{
	// Adjust angles
	m_gfAlpha+=1.0f;
	m_gfBeta+=1.5f;
	m_gfGamma+=0.01f;

	// set matrixmode in Modelview.
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
		// load the identity matrix into the modelview matrix.
		glLoadIdentity();
		// position the camera.
		glTranslatef(0.0f,0.0f,-2.55f);
		glPushMatrix();
			// rotate the axis
			glRotatef(m_gfAlpha,1.0f,0.0f,0.0f);
			glRotatef(m_gfBeta,0.0f,1.0f,0.0f);
			glRotatef(m_gfGamma,0.0f,0.0f,1.0f);
			// Draw frontside
			DrawLogoOneSide(m_giDGLDlists + FRONTQUADDL);
			// draw backside
			DrawLogoOneSide(m_giDGLDlists + BACKQUADDL);
		glPopMatrix();
		// done
	glPopMatrix();
}


// Purpose: helperroutine that actually renders the logo plus envmapping. because
// we use 2 quads (one for each side, so it's always readable) we need this routine
// twice, and therefor it's a separate routine.
void
DrawLogoOneSide(GLuint iDLList)
{
	glColor4f(1.0f,1.0f,1.0f,1.0f);

	glBindTexture( GL_TEXTURE_2D, m_garriTextureIDs[LOGOTEXIDIDX]);		
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	glCallList(iDLList);

	glColor4f(1.0f,1.0f,1.0f,1.0f);
	glBindTexture(GL_TEXTURE_2D, m_garriTextureIDs[ENVMAPTEXIDIDX]);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
	glEnable(GL_TEXTURE_GEN_S); 
	glEnable(GL_TEXTURE_GEN_T);

	glCallList(iDLList);
	
	glDisable(GL_TEXTURE_GEN_S); 
	glDisable(GL_TEXTURE_GEN_T);
	
	glColor4f(1.0f,1.0f,1.0f,0.9f);
	glBindTexture( GL_TEXTURE_2D, m_garriTextureIDs[LOGOTEXIDIDX]);		
	glBlendFunc(GL_SRC_ALPHA,GL_SRC_ALPHA);
	glCallList(iDLList);
}


// Purpose: the timer handler proc that is called with the timer created in the AboutDlgMessagehandler
// when creating the dialog. It draws the rotating logo
VOID CALLBACK 
AboutDlgTimerHandler( HWND hWnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
	// Do the drawing of the rotating logo.
	// Clear the buffers
	glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);
	// Draw the logo
	DrawDGLLogoInDlg();
	// Swap the back/front buffer
	SwapBuffers(m_ghAboutDlgDC);
}
