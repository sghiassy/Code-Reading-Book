/* $XFree86: xc/programs/Xserver/hw/xfree86/xf86config/xf86config.c,v 3.37.2.3 1997/05/25 05:06:54 dawes Exp $ */

/*
 * This is a configuration program that will create a base XF86Config
 * file based on menu choices. Its main feature is that clueless users
 * may be less inclined to select crazy sync rates way over monitor spec,
 * by presenting a menu with standard monitor types. Also some people
 * don't read docs unless an executable that they can run tells them to.
 *
 * It assumes a 24-line or bigger text console.
 *
 * Revision history:
 * 25Sep94 Initial version.
 * 27Sep94 Fix hsync range of monitor types to match with best possible mode.
 *         Remove 'const'.
 *         Tweak descriptions.
 * 28Sep94 Fixes from J"org Wunsch:
 *           Don't use gets().
 *           Add mouse device prompt.
 *           Fix lines overrun for 24-line console.
 *         Increase buffer size for probeonly output.
 * 29Sep94 Fix bad bug with old XF86Config preserving during probeonly run.
 *         Add note about vertical refresh in interlaced modes.
 *         Name gets() replacement getstring().
 *         Add warning about binary paths.
 *         Fixes from David Dawes:
 *           Don't use 'ln -sf'.
 *           Omit man path reference in comment.
 *           Generate only a generic 320x200 SVGA section for accel cards.
 *	     Only allow writing to /usr/X11R6/lib/X11 if root, and use
 *	       -xf86config for the -probeonly phase (root only).
 *         Fix bug that forces screen type to accel in some cases.
 * 30Sep94 Continue after clocks probe fails.
 *         Note about programmable clocks.
 *         Rename to 'xf86config'. Not to be confused with XF86Config
 *           or the -xf86config option.
 * 07Oct94 Correct hsync in standard mode timings comments, and include
 *           the proper +/-h/vsync flags.
 * 11Oct94 Skip 'numclocks:' and 'pixel clocks:' lines when probing for
 * 	     clocks.
 * 18Oct94 Add check for existence of /usr/X11R6.
 *	   Add note about ctrl-alt-backspace.
 * 06Nov94 Add comment above standard mode timings in XF86Config.
 * 24Dec94 Add low-resolution modes using doublescan.
 * 29Dec94 Add note in horizontal sync range selection.
 *	   Ask about ClearDTR/RTS option for Mouse Systems mice.
 *	   Ask about writing to /etc/XF86Config.
 *	   Allow link to be set in /var/X11R6/bin.
 *	   Note about X -probeonly crashing.
 *	   Add keyboard Alt binding option for non-ASCII characters.
 *	   Add card database selection.
 *	   Write temporary XF86Config for clock probing in /tmp instead
 *	     of /usr/X11R6/lib/X11.
 *	   Add RAMDAC and Clockchip menu.
 *
 * Possible enhancements:
 * - Add more standard mode timings (also applies to README.Config). Missing
 *   are 1024x768 @ 72 Hz, 1152x900 modes, and 1280x1024 @ ~70 Hz.
 *   I suspect there is a VESA standard for 1024x768 @ 72 Hz with 77 MHz dot
 *   clock, and 1024x768 @ 75 Hz with 78.7 MHz dot clock. New types of
 *   monitors probably work better with VESA 75 Hz timings.
 * - Add option for creation of clear, minimal XF86Config.
 * - The card database doesn't include most of the entries in previous
 *   databases.
 *
 * Send comments to H.Hanemaayer@inter.nl.net.
 *
 * Things to keep up-to-date:
 * - Accelerated server names.
 * - Ramdac and Clockchip settings.
 * - The card database.
 *
 */
/* $XConsortium: xf86config.c /main/21 1996/10/28 05:43:57 kaleb $ */

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "cards.h"


/*
 * Define the following to 310 to remove references to XFree86 features that
 * have been added since XFree86 3.1 (e.g. DoubleScan modes).
 */
#define XFREE86_VERSION 313

/*
 * This is the filename of the temporary XF86Config file that is written
 * when the program is told to probe clocks (which can only happen for
 * root).
 */
#ifndef __EMX__
#define TEMPORARY_XF86CONFIG_FILENAME "/tmp/XF86Config.tmp"
#else
/* put in root dir, would have to find TMP dir first else */
#define TEMPORARY_XF86CONFIG_FILENAME "\\XConfig.tmp"
#endif

/*
 * Define this to have /etc/XF86Config prompted for as the default
 * location to write the XF86Config file to.
 */
#define PREFER_XF86CONFIG_IN_ETC

/*
 * Define this to force the user to go through XKB configuration section.
 *
#define FORCE_XKB_DIALOG
 */

/*
 * Configuration variables.
 */

#define MAX_CLOCKS_LINES 16

/* (hv) make a number of filenames defines, because I want OS/2 to need just
 * 8.3 names here
 */
#ifdef __EMX__
#define DUMBCONFIG2 "\\dconfig.2"
#define DUMBCONFIG3 "\\dconfig.3"
#else
#define DUMBCONFIG2 "/tmp/dumbconfig.2"
#define DUMBCONFIG3 "/tmp/dumbconfig.3"
#endif

/* some more vars to make path names in texts more flexible. OS/2 users
 * may be more irritated than Unix users
 */
#ifndef __EMX__
#define TREEROOT "/usr/X11R6"
#define TREEROOTLX "/usr/X11R6/lib/X11"
#define MODULEPATH "/usr/X11R6/lib/modules"
#define CONFIGNAME "XF86Config"
#else
#define TREEROOT "/XFree86"
#define TREEROOTLX "/XFree86/lib/X11"
#define MODULEPATH "/XFree86/lib/modules"
#define CONFIGNAME "XConfig"
#endif

int config_mousetype;		/* Mouse. */
int config_emulate3buttons;
int config_chordmiddle;
int config_cleardtrrts;
char *config_pointerdevice;
int config_altmeta;		/* Keyboard. */
int config_monitortype;		/* Monitor. */
char *config_hsyncrange;
char *config_vsyncrange;
char *config_monitoridentifier;
char *config_monitorvendorname;
char *config_monitormodelname;
int config_videomemory;		/* Video card. */
int config_screentype;		/* mono, vga16, svga, accel */
char *config_deviceidentifier;
char *config_devicevendorname;
char *config_deviceboardname;
int config_numberofclockslines;
char *config_clocksline[MAX_CLOCKS_LINES];
char *config_modesline8bpp;
char *config_modesline16bpp;
char *config_modesline24bpp;
char *config_modesline32bpp;
int config_virtual;		/* 1 (yes) or 0 (no) */
int config_virtualx8bpp, config_virtualy8bpp;
int config_virtualx16bpp, config_virtualy16bpp;
int config_virtualx24bpp, config_virtualy24bpp;
int config_virtualx32bpp, config_virtualy32bpp;
char *config_ramdac;
char *config_dacspeed;
char *config_clockchip;
int config_xkbdisable;
char *config_xkbkeymap;
char *config_xkbkeycodes;
char *config_xkbtypes;
char *config_xkbcompat;
char *config_xkbsymbols;
char *config_xkbgeometry;




/*
 * These are from the selected card definition. Parameters from the
 * definition are offered during the questioning about the video card.
 */

int card_selected;	/* Card selected from database. */
int card_screentype;
int card_accelserver;


void write_XF86Config();


/*
 * This is the initial intro text that appears when the program is started.
 */

static char *intro_text =
"\n"
"This program will create a basic " CONFIGNAME " file, based on menu selections you\n"
"make.\n"
"\n"
"The " CONFIGNAME " file usually resides in " TREEROOTLX " or /etc. A sample\n"
CONFIGNAME " file is supplied with XFree86; it is configured for a standard\n"
"VGA card and monitor with 640x480 resolution. This program will ask for a\n"
"pathname when it is ready to write the file.\n"
"\n"
"You can either take the sample " CONFIGNAME " as a base and edit it for your\n"
"configuration, or let this program produce a base " CONFIGNAME " file for your\n"
"configuration and fine-tune it. Refer to " TREEROOTLX "/doc/README.Config\n"
"for a detailed overview of the configuration process.\n"
"\n"
"For accelerated servers (including accelerated drivers in the SVGA server),\n"
"there are many chipset and card-specific options and settings. This program\n"
"does not know about these. On some configurations some of these settings must\n"
"be specified. Refer to the server man pages and chipset-specific READMEs.\n"
"\n"
"Before continuing with this program, make sure you know the chipset and\n"
"amount of video memory on your video card. SuperProbe can help with this.\n"
"It is also helpful if you know what server you want to run.\n"
"\n"
;

static char *finalcomment_text =
"File has been written. Take a look at it before running 'startx'. Note that\n"
"the " CONFIGNAME " file must be in one of the directories searched by the server\n"
"(e.g. " TREEROOTLX ") in order to be used. Within the server press\n"
"ctrl, alt and '+' simultaneously to cycle video resolutions. Pressing ctrl,\n"
"alt and backspace simultaneously immediately exits the server (use if\n"
"the monitor doesn't sync for a particular mode).\n"
"\n"
"For further configuration, refer to " TREEROOTLX "/doc/README.Config.\n"
"\n";


void keypress() {
	printf("Press enter to continue, or ctrl-c to abort.");
	getchar();
	printf("\n");
}

void emptylines() {
	int i;
	for (i = 0; i < 50; i++)
		printf("\n");
}

int answerisyes(s)
	char *s;
{
	if (s[0] == '\'')	/* For fools that type the ' literally. */
		return tolower(s[1]) == 'y';
	return tolower(s[0]) == 'y';
}

/*
 * This is a replacement for gets(). Limit is 80 chars.
 * The 386BSD descendants scream about using gets(), for good reason.
 */

void getstring(s)
	char *s;
{
	char *cp;
	fgets(s, 80, stdin);
	cp = strchr(s, '\n');
	if (cp)
		*cp=0;
}

/*
 * Mouse configuration.
 *
 * (hv) OS/2 (__EMX__) only has an OS supported mouse, so user has no options
 * the server will enable a third button automatically if there is one
 */

static char *mousetype_identifier[10] = {
	"Microsoft",
	"MouseSystems",
	"Busmouse",
	"PS/2",
	"Logitech",
	"MouseMan",
	"MMSeries",
	"MMHitTab",
	"IntelliMouse",
#ifdef __EMX__
	"OSMOUSE"
#endif
};

#ifndef __EMX__
static char *mouseintro_text =
"First specify a mouse protocol type. Choose one from the following list:\n"
"\n";

static char *mousetype_name[10] = {
	"Microsoft compatible (2-button protocol)",
	"Mouse Systems (3-button protocol)",
	"Bus Mouse",
	"PS/2 Mouse",
	"Logitech Mouse (serial, old type, Logitech protocol)",
	"Logitech MouseMan (Microsoft compatible)",
	"MM Series",	/* XXXX These descriptions should be improved. */
	"MM HitTablet",
	"Microsoft IntelliMouse"
};

static char *mousedev_text =
"Now give the full device name that the mouse is connected to, for example\n"
"/dev/tty00. Just pressing enter will use the default, /dev/mouse.\n"
"\n";

static char *mousecomment_text =
"If you have a two-button mouse, it is most likely of type 1, and if you have\n"
"a three-button mouse, it can probably support both protocol 1 and 2. There are\n"
"two main varieties of the latter type: mice with a switch to select the\n"
"protocol, and mice that default to 1 and require a button to be held at\n"
"boot-time to select protocol 2. Some mice can be convinced to do 2 by sending\n"
"a special sequence to the serial port (see the ClearDTR/ClearRTS options).\n"
"\n";

static char *twobuttonmousecomment_text =
"You have selected a two-button mouse protocol. It is recommended that you\n"
"enable Emulate3Buttons.\n";

static char *threebuttonmousecomment_text =
"You have selected a three-button mouse protocol. It is recommended that you\n"
"do not enable Emulate3Buttons, unless the third button doesn't work.\n";

static char *unknownbuttonsmousecomment_text =
"If your mouse has only two buttons, it is recommended that you enable\n"
"Emulate3Buttons.\n";

static char *microsoftmousecomment_text =
"You have selected a Microsoft protocol mouse. If your mouse was made by\n"
"Logitech, you might want to enable ChordMiddle which could cause the\n"
"third button to work.\n";

static char *mousesystemscomment_text =
"You have selected a Mouse Systems protocol mouse. If your mouse is normally\n"
"in Microsoft-compatible mode, enabling the ClearDTR and ClearRTS options\n"
"may cause it to switch to Mouse Systems mode when the server starts.\n";

static char *logitechmousecomment_text =
"You have selected a Logitech protocol mouse. This is only valid for old\n"
"Logitech mice.\n";

static char *mousemancomment_text =
"You have selected a Logitech MouseMan type mouse. You might want to enable\n"
"ChordMiddle which could cause the third button to work.\n";

#endif /* !__EMX__ */

void mouse_configuration() {

#ifndef __EMX__
	int i;
	char s[80];
	printf("%s", mouseintro_text);
	
	for (i = 0; i < 9; i++)
		printf("%2d.  %s\n", i + 1, mousetype_name[i]);

	printf("\n");

	printf("%s", mousecomment_text);
	
	printf("Enter a protocol number: ");
	getstring(s);
	config_mousetype = atoi(s) - 1;

	printf("\n");

	if (config_mousetype == 4) {
		/* Logitech. */
		printf("%s", logitechmousecomment_text);
		printf("\n");
		printf("Please answer the following question with either 'y' or 'n'.\n");
		printf("Are you sure it's really not a Microsoft compatible one? ");
		getstring(s);
		if (!answerisyes(s))
			config_mousetype = 0;
		printf("\n");
	}

	config_chordmiddle = 0;
	if (config_mousetype == 0 || config_mousetype == 5) {
		/* Microsoft or MouseMan. */
		if (config_mousetype == 0)
			printf("%s", microsoftmousecomment_text);
		else
			printf("%s", mousemancomment_text);
		printf("\n");
		printf("Please answer the following question with either 'y' or 'n'.\n");
		printf("Do you want to enable ChordMiddle? ");
		getstring(s);
		if (answerisyes(s))
			config_chordmiddle = 1;
		printf("\n");
	}

	config_cleardtrrts = 0;
	if (config_mousetype == 1) {
		/* Mouse Systems. */
		printf("%s", mousesystemscomment_text);
		printf("\n");
		printf("Please answer the following question with either 'y' or 'n'.\n");
		printf("Do you want to enable ClearDTR and ClearRTS? ");
		getstring(s);
		if (answerisyes(s))
			config_cleardtrrts = 1;
		printf("\n");
	}

	switch (config_mousetype) {
	case 0 : /* Microsoft compatible */
		if (config_chordmiddle)
			printf("%s", threebuttonmousecomment_text);
		else
			printf("%s", twobuttonmousecomment_text);
		break;
	case 1 : /* Mouse Systems. */
	case 8 : /* IntelliMouse */
		printf("%s", threebuttonmousecomment_text);
		break;
	default :
		printf("%s", unknownbuttonsmousecomment_text);
		break;
	}

	printf("\n");

	printf("Please answer the following question with either 'y' or 'n'.\n");
	printf("Do you want to enable Emulate3Buttons? ");
	getstring(s);
	if (answerisyes(s))
		config_emulate3buttons = 1;
	else
		config_emulate3buttons = 0;
	printf("\n");

	printf("%s", mousedev_text);
	printf("Mouse device: ");
	getstring(s);
	if (strlen(s) == 0)
		config_pointerdevice = "/dev/mouse";
	else {
		config_pointerdevice = malloc(strlen(s) + 1);
		strcpy(config_pointerdevice, s);
       }
       printf("\n");

#else /* __EMX__ */
       	/* set some reasonable defaults for OS/2 */
       	config_mousetype = 9;
	config_chordmiddle = 0;       
	config_cleardtrrts = 0;
	config_emulate3buttons = 0;
	config_pointerdevice = "OS2MOUSE";
#endif /* __EMX__ */
}


/*
 * Keyboard configuration.
 */

/* XXXX Does this make sense? */

static char *keyboardalt_text =
"If you want your keyboard to generate non-ASCII characters in X, because\n"
"you want to be able to enter language-specific characters, you can\n"
"set the left Alt key to Meta, and the right Alt key to ModeShift.\n"
"\n";

void keyboard_configuration() {
	char s[80];

	printf("%s", keyboardalt_text);

	printf("Please answer the following question with either 'y' or 'n'.\n");
	printf("Do you want to enable these bindings for the Alt keys? ");
	getstring(s);
	config_altmeta = 0;
	if (answerisyes(s))
		config_altmeta = 1;
	printf("\n");
}

/*
 * Configuration of XKB 
 */

static char* xkbcompose1 =
"You did not select one of the preconfigured keymaps. We will now try to\n"
"compose a suitable XKB setting. This setting is untested.\n"
"Please select one of the following standard keyboards. Use DEFAULT if\n"
"nothing really fits (101-key, tune manually)\n\n";

static char* xkbcompose2 =
"Please choose one of the following countries. Use DEFAULT if nothing\n"
"really fits (US encoding, tune manually)\n";

static char* noncompatible_text = 
"You have chosen the combination:\n"
"         keyboard %s\n"
"         country %s\n"
"This configuration might be incompatible and not supported yet. The\n"
"requested entry will be made, but the server might fail to map the\n"
"keyboard properly.\n";

/* 
 * Add here combinations of symbols and geometry for keyboards
 * This will be annotated with "en_US" and "+..." later
 */

static struct symlist1 {
	int usonly;	/* 0=us or us_EN, 1=us */
	char *symname;
	char *geoname;
	char *desc;
} sympart1[] = {
{0,	"pc101",		"pc",	"Standard 101-key keyboard"},
{0,	"pc102",		"pc",	"Standard 102-key keyboard"},
{1,	"pc101euro",		"pc",	"101-key with ALT_R = Multi_key"},
{1,	"pc102euro",		"pc",	"102-key with ALT_R = Multi_key"},
{1,	"microsoft",		"microsoft",   "Microsoft Natural keyboard"},
{1,	"pc101",		"keytronic(FlexPro)", "KeyTronic FlexPro keyboard"},
{0,	"pc101",		"pc",	"DEFAULT"}
};

static int nsym1 = sizeof(sympart1)/sizeof(struct symlist1);

static struct symlist2 {
	char *prefix;	/* us or us_en */
	char *extend;	/* +de, +fr, ... */
	char *desc;	/* description */
} sympart2[] = {
{	"en_US",	"+be",	"Belgium"},
{	"en_US",	"+bg",	"Bulgaria"},
{	"en_US",	"+ca",	"Canada"},
{	"en_US",	"+cs",	"Czechoslovakia"},
{	"en_US",	"+dk",	"Denmark"},
{	"en_US",	"+fi",	"Finland"},
{	"en_US",	"+fr",	"France"},
{	"en_US",	"+de",	"Germany"},
{	"en_US",	"+it",	"Italy"},
{	"en_US",	"+no",	"Norway"},
{	"en_US",	"+pl",	"Poland"},
{	"en_US",	"+pt",	"Portugal"},
{	"en_US",	"+ru",	"Russia"},
{	"en_US",	"+es",	"Spain"},
{	"en_US",	"+se",	"Sweden"},
{	"en_US",	"+th",	"Thailand"},
{	"en_US",	"+fr_CH",	"Switzerland/French layout"},
{	"en_US",	"+de_CH",	"Switzerland/German layout"},
{	"en_US",	"+gb",	"United Kingdom"},
{	"us",		"",	"USA"},
{	"en_US",	"",	"DEFAULT"}
};

static int nsym2 = sizeof(sympart2)/sizeof(struct symlist2);

void xkb_composekeymaps()
{
	int i;
	char s[80];
	int xkbsym1,xkbsym2;

	/* this is entered when the user did not find a preconfigured 
	 * XkbKeymap 
	 */
	emptylines();
	printf(xkbcompose1);
	for (i=0; i<nsym1; i++) {
		printf("%3d  %-50s\n",i+1,sympart1[i].desc);
	}

	printf("\nEnter a number to choose the keyboard.\n\n");
	getstring(s);
	if (strlen(s) == 0)
		xkbsym1 = 0;
	else {
		i = atoi(s)-1;
		xkbsym1 = (i < 0 || i > nsym1) ? 0 : i;
	}
	emptylines();
	printf(xkbcompose2);
	keypress();

	xkbsym2 = -1;
	for (i=0;;) {
		int j;
		emptylines();
		for (j = i; j < i + 18 && j < nsym2; j++)
			printf("%3d  %-50s\n", j+1,
				sympart2[j].desc);
		printf("\n");
		printf("Enter a number to choose the country.\n");
		if (nsym2 >= 18)
			printf("Press enter for the next page\n");
		printf("\n");
		getstring(s);
		if (strlen(s) == 0) {
			i += 18;
			if (i > nsym2)
				i = 0;
			continue;
		}
		xkbsym2 = atoi(s) - 1;
		if (xkbsym2 >= 0 && xkbsym2 < nsym2)
			break;
	}

	/* XXX check consistency: currently en_US supports only pc101 and 102,
	 * which is IMHO wrong, because I have seen German MS-Keyboards at
	 * least (hv)
	 */
	if (sympart1[xkbsym1].usonly &&
	    strcmp(sympart2[xkbsym2].prefix,"us") != 0) {
		emptylines();
		printf(noncompatible_text,
			sympart1[xkbsym1].desc,
			sympart2[xkbsym2].desc);
		keypress();
	}		

	/* compose the lines */
	config_xkbdisable = 0;
	config_xkbkeymap = 0;
#ifdef XFREE98_XKB
	config_xkbkeycodes = "xfree98";	/* static */
#else
	config_xkbkeycodes = "xfree86";	/* static */
#endif
	config_xkbtypes = "default";		/* static */
	config_xkbcompat = "default";		/* static */

	i = strlen(sympart2[xkbsym2].prefix)
	      + strlen(sympart1[xkbsym1].symname)
	      + strlen(sympart2[xkbsym2].extend)
	      + 1;
	config_xkbsymbols = malloc(i);
	sprintf(config_xkbsymbols,"%s(%s)%s",
		sympart2[xkbsym2].prefix,
		sympart1[xkbsym1].symname,
		sympart2[xkbsym2].extend);

	config_xkbgeometry = malloc(strlen(sympart1[xkbsym1].geoname)+1);
	sprintf(config_xkbgeometry,"%s",sympart1[xkbsym1].geoname);

	return;
}

#ifndef FORCE_XKB_DIALOG
static char *xkb_intro1 =
"Beginning with XFree86 3.1.2D, you can use the new X11R6.1 XKEYBOARD\n"
"extension to manage the keyboard layout. If you answer 'n' to the following\n"
"question, the server will use the old method, and you have to adjust\n"
"your keyboard layout with xmodmap.\n\n";
#endif

static char *xkb_intro2 =
#ifdef FORCE_XKB_DIALOG
"Beginning with XFree86 3.1.2D, the X server uses the new X11R6.1 XKEYBOARD\n"
"extension to manage the keyboard layout. This requires you to answer a\n"
"number of questions to add certain entries to the " CONFIGNAME " file.\n\n"
#endif
"The following dialogue will allow you to select from a list of already\n"
"preconfigured keymaps. If you don't find a suitable keymap in the list,\n"
"the program will try to combine a keymap from additional information you\n"
"are asked then. Such a keymap is by default untested and may require\n"
"manual tuning. Please report success or required changes for such a\n"
"keymap to XFREE86@XFREE86.ORG for addition to the list of preconfigured\n"
"keymaps in the future.\n\n";

/*
 *  Add new standard keymaps here
 */
static struct kmlist {
	char *name;	/* the string to be added to XkbKeymap */
	char *desc;	/* the string that is shown to the user */
} keymaps[] = {
{"xfree86(us)",		"Standard 101-key, US encoding"},
{"xfree86(us_microsoft)",	"Microsoft Natural, US encoding"},
{"xfree86(us_flexpro)",	"KeyTronic FlexPro, US encoding"},
{"xfree86(en_US)",	"Standard 101-key, US encoding with ISO9995-3 extensions"},
{"xfree86(de)",		"Standard 101-key, German encoding"},
{"xfree86(fr)",		"Standard 101-key, French encoding"},
{"xfree86(th)",		"Standard 101-key, Thai encoding"},
{"xfree86(de_CH)",	"Standard 101-key, Swiss/German encoding"},
{"xfree86(fr_CH)",	"Standard 101-key, Swiss/French encoding"},
#ifdef XFREE98_XKB
{"xfree98(jp)",		"NEC PC98, Japanese encoding"},
#endif
{0,			"None of the above"},	/* must be last one */
};

static int nkeymaps = sizeof(keymaps)/sizeof(struct kmlist);

static char *keymap_default = "xfree86(us)";

void xkb_keyboard_configuration()
{
	char s[80];
	int i;
	int map_selected;

	/* preset the config variables */
	config_xkbdisable = 0;
	config_xkbkeymap = keymap_default;

#ifndef FORCE_XKB_DIALOG
	printf("%s", xkb_intro1);

	printf("Please answer the following question with either 'y' or 'n'.\n");
	printf("Do you want to use XKB? ");
	getstring(s);
	printf("\n");
	config_xkbdisable = 0;
	if (!answerisyes(s)) {
		config_xkbdisable = 1;
		return;
	}
#endif

	emptylines();
	printf("%s", xkb_intro2);
	keypress();

	i = 0;
	map_selected = -1;
	for (;;) {
		int j;
		emptylines();
		printf("List of preconfigured keymaps:\n\n");
		for (j = i; j < i + 18 && j < nkeymaps; j++)
			printf("%3d  %-50s\n", j+1,
				keymaps[j].desc);
		printf("\n");
		printf("Enter a number to choose the keymap.\n");
		if (nkeymaps >= 18)
			printf("Press enter for the next page\n");
		printf("\n");
		getstring(s);
		if (strlen(s) == 0) {
			i += 18;
			if (i > nkeymaps)
				i = 0;
			continue;
		}
		map_selected = atoi(s);
		if (map_selected >= 0 && map_selected <= nkeymaps)
			break;
	}

	if (map_selected==nkeymaps) {	/* none of the above */
		xkb_composekeymaps();
	} else {
		config_xkbkeymap = keymaps[map_selected-1].name;
	}
	return;
}

/*
 * Monitor configuration.
 */

static char *monitorintro_text =
"Now we want to set the specifications of the monitor. The two critical\n"
"parameters are the vertical refresh rate, which is the rate at which the\n"
"the whole screen is refreshed, and most importantly the horizontal sync rate,\n"
"which is the rate at which scanlines are displayed.\n"
"\n"
"The valid range for horizontal sync and vertical sync should be documented\n"
"in the manual of your monitor. If in doubt, check the monitor database\n"
TREEROOTLX "/doc/Monitors to see if your monitor is there.\n"
"\n";

static char *hsyncintro_text =
"You must indicate the horizontal sync range of your monitor. You can either\n"
"select one of the predefined ranges below that correspond to industry-\n"
"standard monitor types, or give a specific range.\n"
"\n"
"It is VERY IMPORTANT that you do not specify a monitor type with a horizontal\n"
"sync range that is beyond the capabilities of your monitor. If in doubt,\n"
"choose a conservative setting.\n"
"\n";

static char *customhsync_text =
"Please enter the horizontal sync range of your monitor, in the format used\n"
"in the table of monitor types above. You can either specify one or more\n"
"continuous ranges (e.g. 15-25, 30-50), or one or more fixed sync frequencies.\n"
"\n";

static char *vsyncintro_text =
"You must indicate the vertical sync range of your monitor. You can either\n"
"select one of the predefined ranges below that correspond to industry-\n"
"standard monitor types, or give a specific range. For interlaced modes,\n"
"the number that counts is the high one (e.g. 87 Hz rather than 43 Hz).\n"
"\n"
" 1  50-70\n"
" 2  50-90\n"
" 3  50-100\n"
" 4  40-150\n"
" 5  Enter your own vertical sync range\n";

static char *monitordescintro_text =
"You must now enter a few identification/description strings, namely an\n"
"identifier, a vendor name, and a model name. Just pressing enter will fill\n"
"in default names.\n"
"\n";

#define NU_MONITORTYPES 10

static char *monitortype_range[NU_MONITORTYPES] = {
	"31.5",
	"31.5 - 35.1",
	"31.5, 35.5",
	"31.5, 35.15, 35.5",
	"31.5 - 37.9",
	"31.5 - 48.5",
	"31.5 - 57.0",
	"31.5 - 64.3",
	"31.5 - 79.0",
	"31.5 - 82.0"
};

static char *monitortype_name[NU_MONITORTYPES] = {
	"Standard VGA, 640x480 @ 60 Hz",
	"Super VGA, 800x600 @ 56 Hz",
	"8514 Compatible, 1024x768 @ 87 Hz interlaced (no 800x600)",
	"Super VGA, 1024x768 @ 87 Hz interlaced, 800x600 @ 56 Hz",
	"Extended Super VGA, 800x600 @ 60 Hz, 640x480 @ 72 Hz",
	"Non-Interlaced SVGA, 1024x768 @ 60 Hz, 800x600 @ 72 Hz",
	"High Frequency SVGA, 1024x768 @ 70 Hz",
	"Monitor that can do 1280x1024 @ 60 Hz",
	"Monitor that can do 1280x1024 @ 74 Hz",
	"Monitor that can do 1280x1024 @ 76 Hz"
};

void monitor_configuration() {
	int i;
	char s[80];
	printf("%s", monitorintro_text);

	keypress();
	emptylines();

	printf("%s", hsyncintro_text);

	printf("    hsync in kHz; monitor type with characteristic modes\n");
	for (i = 0; i < NU_MONITORTYPES; i++)
		printf("%2d  %s; %s\n", i + 1, monitortype_range[i],
			monitortype_name[i]);

	printf("%2d  Enter your own horizontal sync range\n",
		NU_MONITORTYPES + 1);
	printf("\n");

	printf("Enter your choice (1-%d): ", NU_MONITORTYPES + 1);
	getstring(s);
	config_monitortype = atoi(s) - 1;

	printf("\n");

	if (config_monitortype < NU_MONITORTYPES)
		config_hsyncrange = monitortype_range[config_monitortype];
	else {
		/* Custom hsync range option selected. */
		printf("%s", customhsync_text);
		printf("Horizontal sync range: ");
		getstring(s);
		config_hsyncrange = malloc(strlen(s) + 1);
		strcpy(config_hsyncrange, s);
		printf("\n");
	}

	printf("%s", vsyncintro_text);
	printf("\n");

	printf("Enter your choice: ");
	getstring(s);
	printf("\n");
	switch (atoi(s)) {
	case 1 :
		config_vsyncrange = "50-70";
		break;
	case 2 :
		config_vsyncrange = "50-90";
		break;
	case 3 :
		config_vsyncrange = "50-100";
		break;
	case 4 :
		config_vsyncrange = "40-150";
		break;
	case 5 :
		/* Custom vsync range option selected. */
		printf("Vertical sync range: ");
		getstring(s);
		config_vsyncrange = malloc(strlen(s) + 1);
		strcpy(config_vsyncrange, s);
		printf("\n");
		break;
	}
	printf("%s", monitordescintro_text);
	printf("The strings are free-form, spaces are allowed.\n");
	printf("Enter an identifier for your monitor definition: ");
	getstring(s);
	if (strlen(s) == 0)
		config_monitoridentifier = "My Monitor";
	else {
		config_monitoridentifier = malloc(strlen(s) + 1);
		strcpy(config_monitoridentifier, s);
	}
	printf("Enter the vendor name of your monitor: ");
	getstring(s);
	if (strlen(s) == 0)
		config_monitorvendorname = "Unknown";
	else {
		config_monitorvendorname = malloc(strlen(s) + 1);
		strcpy(config_monitorvendorname, s);
	}
	printf("Enter the model name of your monitor: ");
	getstring(s);
	if (strlen(s) == 0)
		config_monitormodelname = "Unknown";
	else {
		config_monitormodelname = malloc(strlen(s) + 1);
		strcpy(config_monitormodelname, s);
	}
}


/*
 * Card database.
 */

static char *cardintro_text =
"Now we must configure video card specific settings. At this point you can\n"
"choose to make a selection out of a database of video card definitions.\n"
"Because there can be variation in Ramdacs and clock generators even\n"
"between cards of the same model, it is not sensible to blindly copy\n"
"the settings (e.g. a Device section). For this reason, after you make a\n"
"selection, you will still be asked about the components of the card, with\n"
"the settings from the chosen database entry presented as a strong hint.\n"
"\n"
"The database entries include information about the chipset, what server to\n"
"run, the Ramdac and ClockChip, and comments that will be included in the\n"
"Device section. However, a lot of definitions only hint about what server\n"
"to run (based on the chipset the card uses) and are untested.\n"
"\n"
"If you can't find your card in the database, there's nothing to worry about.\n"
"You should only choose a database entry that is exactly the same model as\n"
"your card; choosing one that looks similar is just a bad idea (e.g. a\n"
"GemStone Snail 64 may be as different from a GemStone Snail 64+ in terms of\n"
"hardware as can be).\n"
"\n";

static char *cardunsupported_text =
"This card is basically UNSUPPORTED. It may only work as a generic\n"
"VGA-compatible card. If you have an XFree86 version more recent than what\n"
"this card definition was based on, there's a chance that it is now\n"
"supported.\n";

#define NU_ACCELSERVER_IDS 10

static char *accelserver_id[NU_ACCELSERVER_IDS] = {
	"S3", "Mach32", "Mach8", "8514", "P9000", "AGX", "W32", "Mach64",
	"I128", "S3V"
};

void carddb_configuration() {
	int i;
	char s[80];
	card_selected = -1;
	card_screentype = -1;
	printf("%s", cardintro_text);
	printf("Do you want to look at the card database? ");
	getstring(s);
	printf("\n");
	if (!answerisyes(s))
		return;

	/*
	 * Choose a database entry.
	 */
	if (parse_database()) {
		printf("Couldn't read card database file %s.\n",
			CARD_DATABASE_FILE);
		keypress();
		return;
	}

	i = 0;
	for (;;) {
		int j;
		emptylines();
		for (j = i; j < i + 18 && j <= lastcard; j++)
			printf("%3d  %-50s%s\n", j,
				card[j].name,
				card[j].chipset);
		printf("\n");
		printf("Enter a number to choose the corresponding card definition.\n");
		printf("Press enter for the next page, q to continue configuration.\n");
		printf("\n");
		getstring(s);
		if (s[0] == 'q')
			break;
		if (strlen(s) == 0) {
			i += 18;
			if (i > lastcard)
				i = 0;
			continue;
		}
		card_selected = atoi(s);
		if (card_selected >= 0 && card_selected <= lastcard)
			break;
	}

	/*
	 * Look at the selected card.
	 */
	if (card_selected != -1) {
		if (strcmp(card[card_selected].server, "Mono") == 0)
			card_screentype = 1;
		else
		if (strcmp(card[card_selected].server, "VGA16") == 0)
			card_screentype = 2;
		if (strcmp(card[card_selected].server, "SVGA") == 0)
			card_screentype = 3;
		for (i = 0; i < NU_ACCELSERVER_IDS; i++)
			if (strcmp(card[card_selected].server,
			accelserver_id[i]) == 0) {
				card_screentype = 4;
				card_accelserver = i;
				break;
			}

		printf("\nYour selected card definition:\n\n");
		printf("Identifier: %s\n", card[card_selected].name);
		printf("Chipset:    %s\n", card[card_selected].chipset);
		printf("Server:     XF86_%s\n", card[card_selected].server);
		if (card[card_selected].ramdac != NULL)
			printf("Ramdac:     %s\n", card[card_selected].ramdac);
		if (card[card_selected].dacspeed != NULL)
			printf("DacSpeed:     %s\n", card[card_selected].dacspeed);
		if (card[card_selected].clockchip != NULL)
			printf("Clockchip:  %s\n", card[card_selected].clockchip);
		if (card[card_selected].flags & NOCLOCKPROBE)
			printf("Do NOT probe clocks or use any Clocks line.\n");
		if (card[card_selected].flags & UNSUPPORTED)
			printf("%s", cardunsupported_text);
#if 0	/* Might be confusing. */
		if (strlen(card[card_selected].lines) > 0)
			printf("Device section text:\n%s",
				card[card_selected].lines);
#endif
		printf("\n");
		keypress();
	}
}


/*
 * Screen/video card configuration.
 */

static char *screenintro_text =
"Now you must determine which server to run. Refer to the manpages and other\n"
"documentation. The following servers are available (they may not all be\n"
"installed on your system):\n"
"\n"
" 1  The XF86_Mono server. This a monochrome server that should work on any\n"
"    VGA-compatible card, in 640x480 (more on some SVGA chipsets).\n"
" 2  The XF86_VGA16 server. This is a 16-color VGA server that should work on\n"
"    any VGA-compatible card.\n"
" 3  The XF86_SVGA server. This is a 256 color SVGA server that supports\n"
"    a number of SVGA chipsets. On some chipsets it is accelerated or\n"
"    supports higher color depths.\n"
" 4  The accelerated servers. These include XF86_S3, XF86_Mach32, XF86_Mach8,\n"
"    XF86_8514, XF86_P9000, XF86_AGX, XF86_W32, XF86_Mach64, XF86_I128 and\n"
"    XF86_S3V.\n"
"\n"
"These four server types correspond to the four different \"Screen\" sections in\n"
CONFIGNAME " (vga2, vga16, svga, accel).\n"
"\n";

#ifndef __EMX__
static char *screenlink_text =
"The server to run is selected by changing the symbolic link 'X'. For example,\n"
"'rm /usr/X11R6/bin/X; ln -s /usr/X11R6/bin/XF86_SVGA /usr/X11R6/bin/X' selects\n"
"the SVGA server.\n"
"\n";

static char *varlink_text =
"The directory /var/X11R6/bin exists. On many Linux systems this is the\n"
"preferred location of the symbolic link 'X'. You can select this location\n"
"when setting the symbolic link.\n"
"\n";
#endif /* !__EMX__ */

static char *deviceintro_text =
"Now you must give information about your video card. This will be used for\n"
"the \"Device\" section of your video card in " CONFIGNAME ".\n"
"\n";

static char *videomemoryintro_text =
"You must indicate how much video memory you have. It is probably a good\n"
"idea to use the same approximate amount as that detected by the server you\n"
"intend to use. If you encounter problems that are due to the used server\n"
"not supporting the amount memory you have (e.g. ATI Mach64 is limited to\n"
"1024K with the SVGA server), specify the maximum amount supported by the\n"
"server.\n"
"\n"
"How much video memory do you have on your video card:\n"
"\n";

static char *screenaccelservers_text =
"Select an accel server:\n"
"\n";

static char *carddescintro_text =
"You must now enter a few identification/description strings, namely an\n"
"identifier, a vendor name, and a model name. Just pressing enter will fill\n"
"in default names (possibly from a card definition).\n"
"\n";

static char *devicevendornamecomment_text =
"You can simply press enter here if you have a generic card, or want to\n"
"describe your card with one string.\n";

static char *devicesettingscomment_text =
"Especially for accelerated servers, Ramdac, Dacspeed and ClockChip settings\n"
"or special options may be required in the Device section.\n"
"\n";

static char *ramdaccomment_text =
"The RAMDAC setting only applies to the S3, AGX, W32 servers, and some \n"
"drivers in the SVGA servers. Some RAMDAC's are auto-detected by the server.\n"
"The detection of a RAMDAC is forced by using a Ramdac \"identifier\" line in\n"
"the Device section. The identifiers are shown at the right of the following\n"
"table of RAMDAC types:\n"
"\n";

#define NU_RAMDACS 24

static char *ramdac_name[NU_RAMDACS] = {
	"AT&T 20C490 (S3 and AGX servers, ARK driver)",
	"AT&T 20C498/21C498/22C498 (S3, autodetected)",
	"AT&T 20C409/20C499 (S3, autodetected)",
	"AT&T 20C505 (S3)",
	"BrookTree BT481 (AGX)",
	"BrookTree BT482 (AGX)",
	"BrookTree BT485/9485 (S3)",
	"Sierra SC15025 (S3, AGX)",
#if XFREE86_VERSION >= 311	
	"S3 GenDAC (86C708) (autodetected)",
	"S3 SDAC (86C716) (autodetected)",
#else
	"S3 GenDAC (86C708)",
	"S3 SDAC (86C716)",
#endif
	"STG-1700 (S3, autodetected)",
	"STG-1703 (S3, autodetected)",
	"TI 3020 (S3, autodetected)",
	"TI 3025 (S3, autodetected)",
	"TI 3026 (S3, autodetected)",
	"IBM RGB 514 (S3, autodetected)",
	"IBM RGB 524 (S3, autodetected)",
	"IBM RGB 525 (S3, autodetected)",
	"IBM RGB 526 (S3)",
	"IBM RGB 528 (S3, autodetected)",
	"ICS5342 (S3, ARK)",
	"ICS5341 (W32)",
	"IC Works w30C516 ZoomDac (ARK)",
	"Normal DAC"
};

static char *ramdac_id[NU_RAMDACS] = {
	"att20c490", "att20c498", "att20c409", "att20c505", "bt481", "bt482",
	"bt485", "sc15025", "s3gendac", "s3_sdac", "stg1700","stg1703",
	"ti3020", "ti3025", "ti3026", "ibm_rgb514", "ibm_rgb524",
	"ibm_rgb525", "ibm_rgb526", "ibm_rgb528", "ics5342", "ics5341",
	"zoomdac", "normal"
};

static char *clockchipcomment_text =
"A Clockchip line in the Device section forces the detection of a\n"
"programmable clock device. With a clockchip enabled, any required\n"
"clock can be programmed without requiring probing of clocks or a\n"
"Clocks line. Most cards don't have a programmable clock chip.\n"
"Choose from the following list:\n"
"\n";

#define NU_CLOCKCHIPS 12

static char *clockchip_name[] = {
	"Chrontel 8391",
	"ICD2061A and compatibles (ICS9161A, DCS2824)",
	"ICS2595",
	"ICS5342 (similar to SDAC, but not completely compatible)",
	"ICS5341",
	"S3 GenDAC (86C708) and ICS5300 (autodetected)",
	"S3 SDAC (86C716)",
	"STG 1703 (autodetected)",
	"Sierra SC11412",
	"TI 3025 (autodetected)",
	"TI 3026 (autodetected)",
	"IBM RGB 51x/52x (autodetected)",
};

static char *clockchip_id[] = {
	"ch8391", "icd2061a", "ics2595", "ics5342", "ics5341",
	"s3gendac", "s3_sdac",
	"stg1703", "sc11412", "ti3025", "ti3026", "ibm_rgb5xx",
};

static char *deviceclockscomment_text =
"For most configurations, a Clocks line is useful since it prevents the slow\n"
"and nasty sounding clock probing at server start-up. Probed clocks are\n"
"displayed at server startup, along with other server and hardware\n"
"configuration info. You can save this information in a file by running\n"
"'X -probeonly 2>output_file'. Be warned that clock probing is inherently\n"
"imprecise; some clocks may be slightly too high (varies per run).\n"
"\n";

static char *deviceclocksquestion_text =
"At this point I can run X -probeonly, and try to extract the clock information\n"
"from the output. It is recommended that you do this yourself and add a clocks\n"
"line (note that the list of clocks may be split over multiple Clocks lines) to\n"
"your Device section afterwards. Be aware that a clocks line is not\n"
"appropriate for drivers that have a fixed set of clocks and don't probe by\n"
"default (e.g. Cirrus). Also, for the P9000 server you must simply specify\n"
"clocks line that matches the modes you want to use.  For the S3 server with\n"
"a programmable clock chip you need a 'ClockChip' line and no Clocks line.\n"
"\n"
"You must be root to be able to run X -probeonly now.\n"
"\n";

static char *probeonlywarning_text =
"It is possible that the hardware detection routines in the server will somehow\n"
"cause the system to crash and the screen to remain blank. If this is the\n"
"case, do not choose this option the next time. The server may need a\n"
"Ramdac, ClockChip or special option (e.g. \"nolinear\" for S3) to probe\n"
"and start-up correctly.\n"
"\n";

static char *modesorderintro_text =
"For each depth, a list of modes (resolutions) is defined. The default\n"
"resolution that the server will start-up with will be the first listed\n"
"mode that can be supported by the monitor and card.\n"
"Currently it is set to:\n"
"\n";

static char *modesorder_text2 =
"Note that 16, 24 and 32bpp are only supported on a few configurations.\n"
"Modes that cannot be supported due to monitor or clock constraints will\n"
"be automatically skipped by the server.\n"
"\n"
" 1  Change the modes for 8pp (256 colors)\n"
" 2  Change the modes for 16bpp (32K/64K colors)\n"
" 3  Change the modes for 24bpp (24-bit color, packed pixel)\n"
" 4  Change the modes for 32bpp (24-bit color)\n"
" 5  The modes are OK, continue.\n"
"\n";

static char *modeslist_text =
"Please type the digits corresponding to the modes that you want to select.\n"
"For example, 432 selects \"1024x768\" \"800x600\" \"640x480\", with a\n"
"default mode of 1024x768.\n"
"\n";

static char *virtual_text =
"You can have a virtual screen (desktop), which is screen area that is larger\n"
"than the physical screen and which is panned by moving the mouse to the edge\n"
"of the screen. If you don't want virtual desktop at a certain resolution,\n"
"you cannot have modes listed that are larger. Each color depth can have a\n"
"differently-sized virtual screen\n"
"\n";

#define NU_ACCEL_SERVERS 10

static char *accelserver_name[NU_ACCEL_SERVERS] = {
	"XF86_S3", "XF86_Mach32", "XF86_Mach8", "XF86_8514", "XF86_P9000",
	"XF86_AGX", "XF86_W32" ,"XF86_Mach64", "XF86_I128", "XF86_S3V"
};

static int videomemory[5] = {
	256, 512, 1024, 2048, 4096
};

#if XFREE86_VERSION >= 311
#define NU_MODESTRINGS 8
#else
#define NU_MODESTRINGS 5
#endif

static char *modestring[NU_MODESTRINGS] = {
	"\"640x400\"",
	"\"640x480\"",
	"\"800x600\"",
	"\"1024x768\"",
	"\"1280x1024\"",
#if XFREE86_VERSION >= 311
	"\"320x200\"",
	"\"320x240\"",
	"\"400x300\""
#endif
};

#ifdef __EMX__
/* yet another instance of this code, sigh! */
char *__XOS2RedirRoot(char *path, char sep)
{
	static char pn[300];
	char *root;
	int i,l;
	if ((isalpha(path[0]) && path[1]==':') || path[0] != '/')
		return path;

	root = getenv("X11ROOT");
	if (!root) root = "";
	sprintf(pn,"%s%s",root,path);
	if (sep=='\\') {
		l = strlen(pn);
		for (i=0; i<l; i++) 
			if (pn[i]=='/') pn[i]='\\';
	}
	return pn;
}
#endif

/* (hv) to avoid the UNIXISM to try to open a dir to check for existance */
static int exists_dir(char *name) {
	struct stat sbuf;

#ifdef __EMX__
	name = __XOS2RedirRoot(name,'/');
#endif
	/* is it there ? */
	if (stat(name,&sbuf) == -1)
		return 0;

	/* is there, but is it a dir? */
	return ((sbuf.st_mode & S_IFMT)==S_IFDIR) ? 1 : 0;
}

void screen_configuration() {
	int i, c, varlink, np;
	int usecardscreentype;
	char s[80];

	printf("%s", screenintro_text);

	if (card_screentype != -1)
		printf(" 5  Choose the server from the card definition, XF86_%s.\n\n",
			card[card_selected].server);

	printf("Which one of these screen types do you intend to run by default (1-%d)? ",
		4 + (card_screentype != -1 ? 1 : 0));
	getstring(s);
	config_screentype = atoi(s);
	if (config_screentype == 0)
		config_screentype = 4 + (card_screentype != -1 ? 1 : 0);
	usecardscreentype = 0;
	if (config_screentype == 5) {
		config_screentype = card_screentype;	/* From definition. */
		usecardscreentype = 1;
	}
	printf("\n");

#ifndef __EMX__
	printf("%s", screenlink_text);

	varlink = exists_dir("/var/X11R6/bin");
	if (varlink) {
		printf("%s", varlink_text);
	}

	printf("Please answer the following question with either 'y' or 'n'.\n");
	printf("Do you want me to set the symbolic link? ");
	getstring(s);
	printf("\n");
	if (answerisyes(s)) {
		char *servername;
		if (varlink) {
			printf("Do you want to set it in /var/X11R6/bin? ");
			getstring(s);
			printf("\n");
			if (!answerisyes(s))
				varlink = 0;
		}
		if (config_screentype == 4 && usecardscreentype)
			/* Use screen type from card definition. */
			servername = accelserver_name[card_accelserver];
		else
		if (config_screentype == 4) {
			/* Accel server. */
			printf("%s", screenaccelservers_text);
			for (i = 0; i < NU_ACCEL_SERVERS; i++)
				printf("%2d  %s\n", i + 1,
					accelserver_name[i]);
			printf("\n");
			printf("Which accel server: ");
			getstring(s);
			servername = accelserver_name[atoi(s) - 1];
			printf("\n");
		}
		else
			switch (config_screentype) {
			case 1 : servername = "XF86_Mono"; break;
			case 2 : servername = "XF86_VGA16"; break;
			case 3 : servername = "XF86_SVGA"; break;
			}
		if (varlink) {
			system("rm -f /var/X11R6/bin/X");
			sprintf(s, "ln -s /usr/X11R6/bin/%s /var/X11R6/bin/X",
				servername);
		}
		else {
			system("rm -f /usr/X11R6/bin/X");
			sprintf(s, "ln -s /usr/X11R6/bin/%s /usr/X11R6/bin/X",
				servername);
		}
		system(s);
	}
#endif /* !__EMX__ */

	emptylines();

	/*
	 * Configure the "Device" section for the video card.
	 */

	printf("%s", deviceintro_text);

	printf("%s", videomemoryintro_text);

	for (i = 0; i < 5; i++)
		printf("%2d  %dK\n", i + 1, videomemory[i]);
	printf(" 6  Other\n\n");

	printf("Enter your choice: ");
	getstring(s);
	printf("\n");

	c = atoi(s) - 1;
	if (c < 5)
		config_videomemory = videomemory[c];
	else {
		printf("Amount of video memory in Kbytes: ");
		getstring(s);
		config_videomemory = atoi(s);
		printf("\n");
	}

	printf("%s", carddescintro_text);
	if (card_selected != -1)
		printf("Your card definition is %s.\n\n",
			card[card_selected].name);
	printf("The strings are free-form, spaces are allowed.\n");
	printf("Enter an identifier for your video card definition: ");
	getstring(s);
	if (strlen(s) == 0)
		if (card_selected != -1)
			config_deviceidentifier = card[card_selected].name;
		else
			config_deviceidentifier = "My Video Card";
	else {
		config_deviceidentifier = malloc(strlen(s) + 1);
		strcpy(config_deviceidentifier, s);
	}
	printf("%s", devicevendornamecomment_text);
	
	printf("Enter the vendor name of your video card: ");
	getstring(s);
	if (strlen(s) == 0)
		config_devicevendorname = "Unknown";
	else {
		config_devicevendorname = malloc(strlen(s) + 1);
		strcpy(config_devicevendorname, s);
	}
	printf("Enter the model (board) name of your video card: ");
	getstring(s);
	if (strlen(s) == 0)
		config_deviceboardname = "Unknown";
	else {
		config_deviceboardname = malloc(strlen(s) + 1);
                strcpy(config_deviceboardname, s);
	}
	printf("\n");

	emptylines();

	/*
	 * Initialize screen mode variables for svga and accel
	 * to default values.
	 * XXXX Doesn't leave room for off-screen caching in 16/32bpp modes
	 *      for the accelerated servers in some situations.
	 */
	config_modesline8bpp =
	config_modesline16bpp =
	config_modesline24bpp =
	config_modesline32bpp = "\"640x480\"";
	config_virtualx8bpp = config_virtualx16bpp = config_virtualx24bpp =
	config_virtualx32bpp =
	config_virtualy8bpp = config_virtualy16bpp = config_virtualy24bpp =
	config_virtualy32bpp = 0;
	if (config_videomemory >= 4096) {
		config_virtualx8bpp = 1600;
		config_virtualy8bpp = 1280;
		if (config_screentype == 4) {
			/*
			 * Allow room for font/pixmap cache for accel
			 * servers.
			 */
			config_virtualx16bpp = 1280;
			config_virtualy16bpp = 1024;
		}
		else {
			config_virtualx16bpp = 1600;
			config_virtualy16bpp = 1280;
		}
		if (config_screentype == 4) {
			config_virtualx24bpp = 1152;
			config_virtualy24bpp = 900;
			config_virtualx32bpp = 1024;
			config_virtualy32bpp = 768;
		}
		else {
			config_virtualx24bpp = 1280;
			config_virtualy24bpp = 1024;
			config_virtualx32bpp = 1152;
			config_virtualy32bpp = 900;
		}
		/* Add 1600x1280 */
		config_modesline8bpp = "\"640x480\" \"800x600\" \"1024x768\" \"1280x1024\"";
		config_modesline16bpp = "\"640x480\" \"800x600\" \"1024x768\" \"1280x1024\"";
		config_modesline24bpp = "\"640x480\" \"800x600\" \"1024x768\" \"1280x1024\"";
		config_modesline32bpp = "\"640x480\" \"800x600\" \"1024x768\"";
	}
	else
	if (config_videomemory >= 2048) {
		if (config_screentype == 4) {
			/*
			 * Allow room for font/pixmap cache for accel
			 * servers.
			 * Also the mach32 is has a limited width.
			 */
			config_virtualx8bpp = 1280;
			config_virtualy8bpp = 1024;
		}
		else {
			config_virtualx8bpp = 1600;
			config_virtualy8bpp = 1200;
		}
		if (config_screentype == 4) {
			config_virtualx16bpp = 1024;
			config_virtualy16bpp = 768;
		}
		else {
			config_virtualx16bpp = 1152;
			config_virtualy16bpp = 900;
		}
		config_virtualx24bpp = 800;
		config_virtualy24bpp = 600;
		if (config_videomemory >= 2048 + 256) {
			config_virtualx24bpp = 1024;
			config_virtualy24bpp = 768;
		}
		config_virtualx32bpp = 800;
		config_virtualy32bpp = 600;
		config_modesline8bpp = "\"640x480\" \"800x600\" \"1024x768\" \"1280x1024\"";
		config_modesline16bpp = "\"640x480\" \"800x600\" \"1024x768\"";
		if (config_videomemory >= 2048 + 256)
			config_modesline24bpp = "\"640x480\" \"800x600\" \"1024x768\"";
		else
			config_modesline24bpp = "\"640x480\" \"800x600\"";
		config_modesline32bpp = "\"640x480\" \"800x600\"";
	}
	else
	if (config_videomemory >= 1024) {
		if (config_screentype == 4) {
			/*
			 * Allow room for font/pixmap cache for accel
			 * servers.
			 */
			config_virtualx8bpp = 1024;
			config_virtualy8bpp = 768;
		}
		else {
			config_virtualx8bpp = 1152;
			config_virtualy8bpp = 900;
		}
		config_virtualx16bpp = 800; /* Forget about cache space;  */
		config_virtualy16bpp = 600; /* it's small enough as it is. */
		config_virtualx24bpp = 640;
		config_virtualy24bpp = 480;
		config_virtualx32bpp = 640;
		config_virtualy32bpp = 400;
		config_modesline8bpp = "\"640x480\" \"800x600\" \"1024x768\"";
		config_modesline16bpp = "\"640x480\" \"800x600\"";
		config_modesline24bpp = "\"640x480\"";
		config_modesline32bpp = "\"640x400\"";
	}
	else
	if (config_videomemory >= 512) {
		config_virtualx8bpp = 800;
		config_virtualy8bpp = 600;
		config_modesline8bpp = "\"640x480\" \"800x600\"";
		config_modesline16bpp = "\"640x400\"";
	}
	else
	if (config_videomemory >= 256) {
		config_modesline8bpp = "640x400";
		config_virtualx8bpp = 640;
		config_virtualy8bpp = 400;
	}
	else {
		printf("Fatal error: Invalid amount of video memory.\n");
		exit(-1);
	}

	/*
	 * Handle the Ramdac/Clockchip setting.
	 */

	printf("%s", devicesettingscomment_text);

	if (config_screentype < 3)
		goto skipramdacselection;

	printf("%s", ramdaccomment_text);

	/* meanwhile there are so many RAMDACs that they do no longer fit on
	 * on page
	 */
	for (np=12, i=0 ;;) {
		int j;
		for (j = i; j < i + np && j < NU_RAMDACS; j++)
			printf("%3d  %-60s%s\n", j+1,
				ramdac_name[j],
				ramdac_id[j]);

		printf("\n");
		if (card_selected != -1)
			if (card[card_selected].ramdac != NULL)
				printf("The card definition has Ramdac \"%s\".\n\n",
					card[card_selected].ramdac);
		printf("\n");
		printf("Enter a number to choose the corresponding RAMDAC.\n");
		printf("Press enter for the next page, q to quit without selection of a RAMDAC.\n");
		printf("\n");
		getstring(s);

		config_ramdac = NULL;
		if (s[0] == 'q')
			break;

		if (strlen(s) > 0) {
			c = atoi(s)-1;
			if (c >= 0 && c < NU_RAMDACS) {
				config_ramdac = ramdac_id[atoi(s)-1];
				break;
			}
		}
		
		i += np;
		if (np==12) np = 18; /* account intro lines only displayed 1st time */
		if (i >= NU_RAMDACS)
			i = 0;
		emptylines();
	}

skipramdacselection:
	emptylines();
	printf("%s", clockchipcomment_text);

	for (i = 0; i < NU_CLOCKCHIPS; i++)
		printf("%2d  %-60s%s\n",
			i + 1, clockchip_name[i], clockchip_id[i]);

	printf("\n");

	if (card_selected != -1)
		if (card[card_selected].clockchip != NULL)
			printf("The card definition has Clockchip \"%s\"\n\n",
				card[card_selected].clockchip);

	printf("Just press enter if you don't want a Clockchip setting.\n");
	printf("What Clockchip setting do you want (1-%d)? ", NU_CLOCKCHIPS);

	getstring(s);
	config_clockchip = NULL;
	if (strlen(s) > 0)
		config_clockchip = clockchip_id[atoi(s) - 1];

	emptylines();

	/*
	 * Optionally run X -probeonly to figure out the clocks.
	 */

	config_numberofclockslines = 0;

	printf("%s", deviceclockscomment_text);

	printf("%s", deviceclocksquestion_text);

	if (card_selected != -1)
		if (card[card_selected].flags & NOCLOCKPROBE)
			printf("The card definition says to NOT probe clocks.\n");

	if (config_clockchip != NULL) {
		printf("Because you have enabled a Clockchip line, there's no need for clock\n"
			"probing.\n");
		keypress();
		goto skipclockprobing;
	}

	printf("Do you want me to run 'X -probeonly' now? ");
	getstring(s);
	printf("\n");
	if (answerisyes(s)) {
		/*
		 * Write temporary XF86Config and run X -probeonly.
		 * Only allow when root.
		 */
		FILE *f;
		char *buf;
		char syscmdline[2*256+100]; /* enough */

		if (getuid() != 0) {
			printf("Sorry, you must be root to do this.\n\n");
			goto endofprobeonly;
		}
		printf("%s", probeonlywarning_text);
		keypress();
		printf("Running X -probeonly -pn -xf86config "
			TEMPORARY_XF86CONFIG_FILENAME ".\n");
		write_XF86Config(TEMPORARY_XF86CONFIG_FILENAME);
#ifndef __EMX__
		sync();
#endif
		/* compose a line with the real path */
#ifndef __EMX__
		sprintf(syscmdline,
		       "X -probeonly -pn -xf86config "
		       TEMPORARY_XF86CONFIG_FILENAME " 2>" DUMBCONFIG2);
#else
		/* OS/2 does not have symlinks, so "X" does not exist,
		 * call the real X server
		 */
		sprintf(syscmdline,"%s/XF86_%s -probeonly -pn -xf86config "
		       TEMPORARY_XF86CONFIG_FILENAME " 2>" DUMBCONFIG2,
		       __XOS2RedirRoot("/XFree86/bin",'\\'),
		       card[card_selected].server);
#endif

		if (system(syscmdline)) {
			printf("X -probeonly call failed.\n");
			printf("No Clocks line inserted.\n");
			goto clocksprobefailed;
		}
		/* Look for 'clocks:' (case sensitive). */		
		if (system("grep clocks\\: " DUMBCONFIG2 " >" DUMBCONFIG3)) {
			printf("grep failed.\n");
			printf("Cannot find clocks in server output.\n");
			goto clocksprobefailed;
		}
		f = fopen(DUMBCONFIG3, "r");
		buf = malloc(8192);
		/* Parse lines. */
		while (fgets(buf, 8192, f) != NULL) {
			char *clks;
			clks = strstr(buf, "clocks: ") + 8;
			if (clks >= buf + 3 && strcmp(clks - 11, "num") == 0)
				/* Reject lines with 'numclocks:'. */
				continue;
			if (clks >= buf + 8 && strcpy(clks - 14, "pixel ") == 0)
				/* Reject lines with 'pixel clocks:'. */
				continue;
			clks[strlen(clks) - 1] = '\0';	/* Remove '\n'. */
			config_clocksline[config_numberofclockslines] =
				malloc(strlen(clks) + 1);
			strcpy(config_clocksline[config_numberofclockslines],
				clks);
			printf("Clocks %s\n", clks);
			config_numberofclockslines++;
		}
		fclose(f);
clocksprobefailed:
		unlink(DUMBCONFIG3);
		unlink(DUMBCONFIG2);
		unlink(TEMPORARY_XF86CONFIG_FILENAME);
		printf("\n");

endofprobeonly:
		keypress();
	}
skipclockprobing:

	/*
	 * For the mono and vga16 server, no further configuration is
	 * required.
	 */
	if (config_screentype == 1 || config_screentype == 2)
		return;
	
	/*
	 * Configure the modes order.
	 */
	config_virtual = 0;
	for (;;) {
	 	char modes[128];

		emptylines();

		printf("%s", modesorderintro_text);
		printf("%s for 8bpp\n", config_modesline8bpp);
		printf("%s for 16bpp\n", config_modesline16bpp);
		printf("%s for 24bpp\n", config_modesline24bpp);
		printf("%s for 32bpp\n", config_modesline32bpp);
		printf("\n");
		printf("%s", modesorder_text2);

		printf("Enter your choice: ");
		getstring(s);
		printf("\n");

		c = atoi(s) - 1;
		if (c < 0 || c >= 4)
			break;

		printf("Select modes from the following list:\n\n");

		for (i = 0; i < NU_MODESTRINGS; i++)
			printf("%2d  %s\n", i + 1, modestring[i]);
		printf("\n");

		printf("%s", modeslist_text);

		printf("Which modes? ");
		getstring(s);
		printf("\n");

		modes[0] = '\0';
		for (i = 0; i < strlen(s); i++) {
			if (s[i] < '1' || s[i] > '0' + NU_MODESTRINGS) {
				printf("Invalid mode skipped.\n");
				continue;
			}
			if (i > 0)
				strcat(modes, " ");
			strcat(modes, modestring[s[i] - '1']);
		}
		switch (c) {
		case 0 :
			config_modesline8bpp = malloc(strlen(modes) + 1);
			strcpy(config_modesline8bpp, modes);
			break;
		case 1 :
			config_modesline16bpp = malloc(strlen(modes) + 1);
			strcpy(config_modesline16bpp, modes);
			break;
		case 2 :
			config_modesline24bpp = malloc(strlen(modes) + 1);
			strcpy(config_modesline24bpp, modes);
			break;
		case 3 :
			config_modesline32bpp = malloc(strlen(modes) + 1);
			strcpy(config_modesline32bpp, modes);
			break;
		}

		printf("%s", virtual_text);

		printf("Please answer the following question with either 'y' or 'n'.\n");
		printf("Do you want a virtual screen that is larger than the physical screen?");
		getstring(s);
		if (answerisyes(s))
			config_virtual = 1;
	}
}


/*
 * Create the XF86Config file.
 */

static char *XF86Config_firstchunk_text =
"# File generated by xf86config.\n"
"\n"
"#\n"
"# Copyright (c) 1995 by The XFree86 Project, Inc.\n"
"#\n"
"# Permission is hereby granted, free of charge, to any person obtaining a\n"
"# copy of this software and associated documentation files (the \"Software\"),\n"
"# to deal in the Software without restriction, including without limitation\n"
"# the rights to use, copy, modify, merge, publish, distribute, sublicense,\n"
"# and/or sell copies of the Software, and to permit persons to whom the\n"
"# Software is furnished to do so, subject to the following conditions:\n"
"# \n"
"# The above copyright notice and this permission notice shall be included in\n"
"# all copies or substantial portions of the Software.\n"
"# \n"
"# THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR\n"
"# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,\n"
"# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL\n"
"# THE XFREE86 PROJECT BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,\n"
"# WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF\n"
"# OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE\n"
"# SOFTWARE.\n"
"# \n"
"# Except as contained in this notice, the name of the XFree86 Project shall\n"
"# not be used in advertising or otherwise to promote the sale, use or other\n"
"# dealings in this Software without prior written authorization from the\n"
"# XFree86 Project.\n"
"#\n"
"\n"
"# **********************************************************************\n"
"# Refer to the XF86Config(4/5) man page for details about the format of \n"
"# this file.\n"
"# **********************************************************************\n"
"\n"
"# **********************************************************************\n"
"# Files section.  This allows default font and rgb paths to be set\n"
"# **********************************************************************\n"
"\n"
"Section \"Files\"\n"
"\n"
"# The location of the RGB database.  Note, this is the name of the\n"
"# file minus the extension (like \".txt\" or \".db\").  There is normally\n"
"# no need to change the default.\n"
"\n"
"    RgbPath	\"" TREEROOTLX "/rgb\"\n"
"\n"
"# Multiple FontPath entries are allowed (which are concatenated together),\n"
"# as well as specifying multiple comma-separated entries in one FontPath\n"
"# command (or a combination of both methods)\n"
"# \n"
"# If you don't have a floating point coprocessor and emacs, Mosaic or other\n"
"# programs take long to start up, try moving the Type1 and Speedo directory\n"
"# to the end of this list (or comment them out).\n"
"# \n"
"\n";

static char *XF86Config_fontpaths[] = 
{
/*	"    FontPath	\"" TREEROOTLX "/fonts/75dpi/\"\n"*/
	"/fonts/misc/",
	"/fonts/75dpi/:unscaled",
	"/fonts/100dpi/:unscaled",
	"/fonts/Type1/",
	"/fonts/Speedo/",
	"/fonts/75dpi/",
	"/fonts/100dpi/",
	0 /* end of fontpaths */
};

static char *XF86Config_fontpathchunk_text =

"\n"
"# For OSs that support Dynamically loaded modules, ModulePath can be\n"
"# used to set a search path for the modules.  This is currently supported\n"
"# for Linux ELF, FreeBSD 2.x and NetBSD 1.x.  The default path is shown\n"
"# here.\n"
"\n"
"#    ModulePath \"" MODULEPATH "\"\n"
"\n"
"EndSection\n"
"\n"
"# **********************************************************************\n"
"# Module section -- this is an optional section which is used to specify\n"
"# which dynamically loadable modules to load.  Dynamically loadable\n"
"# modules are currently supported only for Linux ELF, FreeBSD 2.x\n"
"# and NetBSD 1.x.  Currently, dynamically loadable modules are used\n"
"# only for some extended input (XInput) device drivers.\n"
"# **********************************************************************\n"
"#\n"
"# Section \"Module\"\n"
"#\n"
"# This loads the module for the Joystick driver\n"
"#\n"
"# Load \"xf86Jstk.so\"\n"
"# \n"
"# EndSection\n"
"\n"
"# **********************************************************************\n"
"# Server flags section.\n"
"# **********************************************************************\n"
"\n"
"Section \"ServerFlags\"\n"
"\n"
"# Uncomment this to cause a core dump at the spot where a signal is \n"
"# received.  This may leave the console in an unusable state, but may\n"
"# provide a better stack trace in the core dump to aid in debugging\n"
"\n"
"#    NoTrapSignals\n"
"\n"
"# Uncomment this to disable the <Crtl><Alt><BS> server abort sequence\n"
"# This allows clients to receive this key event.\n"
"\n"
"#    DontZap\n"
"\n"
"# Uncomment this to disable the <Crtl><Alt><KP_+>/<KP_-> mode switching\n"
"# sequences.  This allows clients to receive these key events.\n"
"\n"
"#    DontZoom\n"
"\n"
"# Uncomment this to disable tuning with the xvidtune client. With\n"
"# it the client can still run and fetch card and monitor attributes,\n"
"# but it will not be allowed to change them. If it tries it will\n"
"# receive a protocol error.\n"
"\n"
"#       DisableVidModeExtension\n"
"\n"
"# Uncomment this to enable the use of a non-local xvidtune client. \n"
"\n"
"#       AllowNonLocalXvidtune\n"
"\n"
"# Uncomment this to disable dynamically modifying the input device\n"
"# (mouse and keyboard) settings. \n"
"\n"
"#       DisableModInDev\n"
"\n"
"# Uncomment this to enable the use of a non-local client to\n"
"# change the keyboard or mouse settings (currently only xset).\n"
"\n"
"#       AllowNonLocalModInDev\n"
"\n"
"EndSection\n"
"\n"
"# **********************************************************************\n"
"# Input devices\n"
"# **********************************************************************\n"
"\n"
"# **********************************************************************\n"
"# Keyboard section\n"
"# **********************************************************************\n"
"\n"
"Section \"Keyboard\"\n"
"\n"
"    Protocol	\"Standard\"\n"
"\n"
"# when using XQUEUE, comment out the above line, and uncomment the\n"
"# following line\n"
"\n"
"#    Protocol	\"Xqueue\"\n"
"\n"
"    AutoRepeat	500 5\n"
"# Let the server do the NumLock processing.  This should only be required\n"
"# when using pre-R6 clients\n"
"#    ServerNumLock\n"
"\n"
"# Specify which keyboard LEDs can be user-controlled (eg, with xset(1))\n"
"#    Xleds      1 2 3\n"
"\n"
"# To set the LeftAlt to Meta, RightAlt key to ModeShift, \n"
"# RightCtl key to Compose, and ScrollLock key to ModeLock:\n"
"\n";

static char *keyboardchunk2_text =
"#    RightCtl    Compose\n"
"#    ScrollLock  ModeLock\n"
"\n"
"# To disable the XKEYBOARD extension, uncomment XkbDisable.\n"
"\n";

static char *keyboardchunk3_text =
"# To customise the XKB settings to suit your keyboard, modify the\n"
"# lines below (which are the defaults).  For example, for a non-U.S.\n"
"# keyboard, you will probably want to use:\n"
"#    XkbModel    \"pc102\"\n"
"# If you have a US Microsoft Natural keyboard, you can use:\n"
"#    XkbModel    \"microsoft\"\n"
"#\n"
"# Then to change the language, change the Layout setting.\n"
"# For example, a german layout can be obtained with:\n"
"#    XkbLayout   \"de\"\n"
"# or:\n"
"#    XkbLayout   \"de\"\n"
"#    XkbVariant  \"nodeadkeys\"\n"
"#\n"
"# If you'd like to switch the positions of your capslock and\n"
"# control keys, use:\n"
"#    XkbOptions  \"ctrl:swapcaps\"\n"
"\n"
"# These are the default XKB settings for XFree86\n"
"#    XkbRules    \"xfree86\"\n"
"#    XkbModel    \"pc101\"\n"
"#    XkbLayout   \"us\"\n"
"#    XkbVariant  \"\"\n"
"#    XkbOptions  \"\"\n"
"\n";

static char *keyboardchunk4_text =
"\n";

static char *keyboardlastchunk_text =
"\n"
"EndSection\n"
"\n"
"\n";

static char *pointersection_text1 = 
"# **********************************************************************\n"
"# Pointer section\n"
"# **********************************************************************\n"
"\n"
"Section \"Pointer\"\n";

static char *pointersection_text2 =
"\n"
"# When using XQUEUE, comment out the above two lines, and uncomment\n"
"# the following line.\n"
"\n"
"#    Protocol	\"Xqueue\"\n"
"\n"
"# Baudrate and SampleRate are only for some Logitech mice\n"
"\n"
"#    BaudRate	9600\n"
"#    SampleRate	150\n"
"\n"
"# Emulate3Buttons is an option for 2-button Microsoft mice\n"
"# Emulate3Timeout is the timeout in milliseconds (default is 50ms)\n"
"\n";

static char *xinputsection_text =
"# **********************************************************************\n"
"# Xinput section -- this is optional and is required only if you\n"
"# are using extended input devices.  This is for example only.  Refer\n"
"# to the XF86Config man page for a description of the options.\n"
"# **********************************************************************\n"
"#\n"
"# Section \"Xinput\" \n"
"#    SubSection \"WacomStylus\"\n"
"#        Port \"/dev/ttyS1\"\n"
"#        DeviceName \"Wacom\"\n"
"#    EndSubSection\n"
"#    SubSection \"WacomCursor\"\n"
"#        Port \"/dev/ttyS1\"\n"
"#    EndSubSection\n"
"#    SubSection \"WacomEraser\"\n"
"#        Port \"/dev/ttyS1\"\n"
"#    EndSubSection\n"
"#\n"
"#    SubSection \"Elographics\"\n"
"#        Port \"/dev/ttyS1\"\n"
"#        DeviceName \"Elo\"\n"
"#        MinimumXPosition 300\n"
"#        MaximumXPosition 3500\n"
"#        MinimumYPosition 300\n"
"#        MaximumYPosition 3500\n"
"#        Screen 0\n"
"#        UntouchDelay 10\n"
"#        ReportDelay 10\n"
"#    EndSubSection\n"
"#\n"
"#    SubSection \"Joystick\"\n"
"#        Port \"/dev/joy0\"\n"
"#        DeviceName \"Joystick\"\n"
"#        TimeOut 10\n"
"#        MinimumXPosition 100\n"
"#        MaximumXPosition 1300\n"
"#        MinimumYPosition 100\n"
"#        MaximumYPosition 1100\n"
"#        # CenterX 700\n"
"#        # CenterY 600\n"
"#        Delta 20\n"
"#    EndSubSection\n"
"#\n"
"# The Mouse Subsection contains the same type of entries as the\n"
"# standard Pointer Section (see above), with the addition of the\n"
"# DeviceName entry.\n"
"#\n"
"#    SubSection \"Mouse\" \n"
"#        Port \"/dev/mouse2\"\n"
"#        DeviceName \"Second Mouse\"\n"
"#        Protocol \"Logitech\"\n"
"#    EndSubSection\n"
"# EndSection\n"
"\n";

static char *monitorsection_text1 =
"# **********************************************************************\n"
"# Monitor section\n"
"# **********************************************************************\n"
"\n"
"# Any number of monitor sections may be present\n"
"\n"
"Section \"Monitor\"\n"
"\n";

static char *monitorsection_text2 =
"# HorizSync is in kHz unless units are specified.\n"
"# HorizSync may be a comma separated list of discrete values, or a\n"
"# comma separated list of ranges of values.\n"
"# NOTE: THE VALUES HERE ARE EXAMPLES ONLY.  REFER TO YOUR MONITOR\'S\n"
"# USER MANUAL FOR THE CORRECT NUMBERS.\n"
"\n";

static char *monitorsection_text3 =
"#    HorizSync	30-64         # multisync\n"
"#    HorizSync	31.5, 35.2    # multiple fixed sync frequencies\n"
"#    HorizSync	15-25, 30-50  # multiple ranges of sync frequencies\n"
"\n"
"# VertRefresh is in Hz unless units are specified.\n"
"# VertRefresh may be a comma separated list of discrete values, or a\n"
"# comma separated list of ranges of values.\n"
"# NOTE: THE VALUES HERE ARE EXAMPLES ONLY.  REFER TO YOUR MONITOR\'S\n"
"# USER MANUAL FOR THE CORRECT NUMBERS.\n"
"\n";

static char *monitorsection_text4 =
"# Modes can be specified in two formats.  A compact one-line format, or\n"
"# a multi-line format.\n"
"\n"
"# These two are equivalent\n"
"\n"
"#    ModeLine \"1024x768i\" 45 1024 1048 1208 1264 768 776 784 817 Interlace\n"
"\n"
"#    Mode \"1024x768i\"\n"
"#        DotClock	45\n"
"#        HTimings	1024 1048 1208 1264\n"
"#        VTimings	768 776 784 817\n"
"#        Flags		\"Interlace\"\n"
"#    EndMode\n"
"\n";

static char *modelines_text =
"# This is a set of standard mode timings. Modes that are out of monitor spec\n"
"# are automatically deleted by the server (provided the HorizSync and\n"
"# VertRefresh lines are correct), so there's no immediate need to\n"
"# delete mode timings (unless particular mode timings don't work on your\n"
"# monitor). With these modes, the best standard mode that your monitor\n"
"# and video card can support for a given resolution is automatically\n"
"# used.\n"
"\n"
"# 640x400 @ 70 Hz, 31.5 kHz hsync\n"
"Modeline \"640x400\"     25.175 640  664  760  800   400  409  411  450\n"
"# 640x480 @ 60 Hz, 31.5 kHz hsync\n"
"Modeline \"640x480\"     25.175 640  664  760  800   480  491  493  525\n"
"# 800x600 @ 56 Hz, 35.15 kHz hsync\n"
"ModeLine \"800x600\"     36     800  824  896 1024   600  601  603  625\n"
"# 1024x768 @ 87 Hz interlaced, 35.5 kHz hsync\n"
"Modeline \"1024x768\"    44.9  1024 1048 1208 1264   768  776  784  817 Interlace\n"
"\n"
"# 640x400 @ 85 Hz, 37.86 kHz hsync\n"
"Modeline \"640x400\"     31.5   640  672 736   832   400  401  404  445 -HSync +VSync\n"
"# 640x480 @ 72 Hz, 36.5 kHz hsync\n"
"Modeline \"640x480\"     31.5   640  680  720  864   480  488  491  521\n"
"# 640x480 @ 75 Hz, 37.50 kHz hsync\n"
"ModeLine  \"640x480\"    31.5   640  656  720  840   480  481  484  500 -HSync -VSync\n"
"# 800x600 @ 60 Hz, 37.8 kHz hsync\n"
"Modeline \"800x600\"     40     800  840  968 1056   600  601  605  628 +hsync +vsync\n"
"\n"
"# 640x480 @ 85 Hz, 43.27 kHz hsync\n"
"Modeline \"640x400\"     36     640  696  752  832   480  481  484  509 -HSync -VSync\n"
"# 1152x864 @ 89 Hz interlaced, 44 kHz hsync\n"
"ModeLine \"1152x864\"    65    1152 1168 1384 1480   864  865  875  985 Interlace\n"
"\n"
"# 800x600 @ 72 Hz, 48.0 kHz hsync\n"
"Modeline \"800x600\"     50     800  856  976 1040   600  637  643  666 +hsync +vsync\n"
"# 1024x768 @ 60 Hz, 48.4 kHz hsync\n"
"Modeline \"1024x768\"    65    1024 1032 1176 1344   768  771  777  806 -hsync -vsync\n"
"\n"
"# 640x480 @ 100 Hz, 53.01 kHz hsync\n"
"Modeline \"640x480\"     45.8   640  672  768  864   480  488  494  530 -HSync -VSync\n"
"# 1152x864 @ 60 Hz, 53.5 kHz hsync\n"
"Modeline  \"1152x864\"   89.9  1152 1216 1472 1680   864  868  876  892 -HSync -VSync\n"
"# 800x600 @ 85 Hz, 55.84 kHz hsync\n"
"Modeline  \"800x600\"    60.75  800  864  928 1088   600  616  621  657 -HSync -VSync\n"
"\n"
"# 1024x768 @ 70 Hz, 56.5 kHz hsync\n"
"Modeline \"1024x768\"    75    1024 1048 1184 1328   768  771  777  806 -hsync -vsync\n"
"# 1280x1024 @ 87 Hz interlaced, 51 kHz hsync\n"
"Modeline \"1280x1024\"   80    1280 1296 1512 1568  1024 1025 1037 1165 Interlace\n"
"\n"
"# 800x600 @ 100 Hz, 64.02 kHz hsync\n"
"Modeline  \"800x600\"    69.65  800  864  928 1088   600  604  610  640 -HSync -VSync\n"
"# 1024x768 @ 76 Hz, 62.5 kHz hsync\n"
"Modeline \"1024x768\"    85    1024 1032 1152 1360   768  784  787  823\n"
"# 1152x864 @ 70 Hz, 62.4 kHz hsync\n"
"Modeline  \"1152x864\"   92    1152 1208 1368 1474   864  865  875  895\n"
"# 1280x1024 @ 61 Hz, 64.2 kHz hsync\n"
"Modeline \"1280x1024\"  110    1280 1328 1512 1712  1024 1025 1028 1054\n"
"\n"
"# 1024x768 @ 85 Hz, 70.24 kHz hsync\n"
"Modeline \"1024x768\"   98.9  1024 1056 1216 1408   768 782 788 822 -HSync -VSync\n"
"# 1152x864 @ 78 Hz, 70.8 kHz hsync\n"
"Modeline \"1152x864\"   110   1152 1240 1324 1552   864  864  876  908\n"
"\n"
"# 1280x1024 @ 70 Hz, 74.59 kHz hsync\n"
"Modeline \"1280x1024\"  126.5 1280 1312 1472 1696  1024 1032 1040 1068 -HSync -VSync\n"
"# 1600x1200 @ 60Hz, 75.00 kHz hsync\n"
"Modeline \"1600x1200\"  162   1600 1664 1856 2160  1200 1201 1204 1250 +HSync +VSync\n"
"# 1152x864 @ 84 Hz, 76.0 kHz hsync\n"
"Modeline \"1152x864\"   135    1152 1464 1592 1776   864  864  876  908\n"
"\n"
"# 1280x1024 @ 74 Hz, 78.85 kHz hsync\n"
"Modeline \"1280x1024\"  135    1280 1312 1456 1712  1024 1027 1030 1064\n"
"\n"
"# 1024x768 @ 100Hz, 80.21 kHz hsync\n"
"Modeline \"1024x768\"   115.5  1024 1056 1248 1440  768  771  781  802 -HSync -VSync\n"
"# 1280x1024 @ 76 Hz, 81.13 kHz hsync\n"
"Modeline \"1280x1024\"  135    1280 1312 1416 1664  1024 1027 1030 1064\n"
"\n"
"# 1600x1200 @ 70 Hz, 87.50 kHz hsync\n"
"Modeline \"1600x1200\"  189    1600 1664 1856 2160  1200 1201 1204 1250 -HSync -VSync\n"
"# 1152x864 @ 100 Hz, 89.62 kHz hsync\n"
"Modeline \"1152x864\"   137.65 1152 1184 1312 1536   864  866  885  902 -HSync -VSync\n"
"# 1280x1024 @ 85 Hz, 91.15 kHz hsync\n"
"Modeline \"1280x1024\"  157.5  1280 1344 1504 1728  1024 1025 1028 1072 +HSync +VSync\n"
"# 1600x1200 @ 75 Hz, 93.75 kHz hsync\n"
"Modeline \"1600x1200\"  202.5  1600 1664 1856 2160  1200 1201 1204 1250 +HSync +VSync\n"
"# 1600x1200 @ 85 Hz, 105.77 kHz hsync\n"
"Modeline \"1600x1200\"  220    1600 1616 1808 2080  1200 1204 1207 1244 +HSync +VSync\n"
"# 1280x1024 @ 100 Hz, 107.16 kHz hsync\n"
"Modeline \"1280x1024\"  181.75 1280 1312 1440 1696  1024 1031 1046 1072 -HSync -VSync\n"
"\n"
"# 1800x1440 @ 64Hz, 96.15 kHz hsync \n"
"ModeLine \"1800X1440\"  230    1800 1896 2088 2392 1440 1441 1444 1490 +HSync +VSync\n"
"# 1800x1440 @ 70Hz, 104.52 kHz hsync \n"
"ModeLine \"1800X1440\"  250    1800 1896 2088 2392 1440 1441 1444 1490 +HSync +VSync\n"
"\n"
"# 512x384 @ 78 Hz, 31.50 kHz hsync\n"
"Modeline \"512x384\"    20.160 512  528  592  640   384  385  388  404 -HSync -VSync\n"
"# 512x384 @ 85 Hz, 34.38 kHz hsync\n"
"Modeline \"512x384\"    22     512  528  592  640   384  385  388  404 -HSync -VSync\n"
"\n"
#if XFREE86_VERSION >= 311
"# Low-res Doublescan modes\n"
"# If your chipset does not support doublescan, you get a 'squashed'\n"
"# resolution like 320x400.\n"
"\n"
"# 320x200 @ 70 Hz, 31.5 kHz hsync, 8:5 aspect ratio\n"
"Modeline \"320x200\"     12.588 320  336  384  400   200  204  205  225 Doublescan\n"
"# 320x240 @ 60 Hz, 31.5 kHz hsync, 4:3 aspect ratio\n"
"Modeline \"320x240\"     12.588 320  336  384  400   240  245  246  262 Doublescan\n"
"# 320x240 @ 72 Hz, 36.5 kHz hsync\n"
"Modeline \"320x240\"     15.750 320  336  384  400   240  244  246  262 Doublescan\n"
"# 400x300 @ 56 Hz, 35.2 kHz hsync, 4:3 aspect ratio\n"
"ModeLine \"400x300\"     18     400  416  448  512   300  301  302  312 Doublescan\n"
"# 400x300 @ 60 Hz, 37.8 kHz hsync\n"
"Modeline \"400x300\"     20     400  416  480  528   300  301  303  314 Doublescan\n"
"# 400x300 @ 72 Hz, 48.0 kHz hsync\n"
"Modeline \"400x300\"     25     400  424  488  520   300  319  322  333 Doublescan\n"
"# 480x300 @ 56 Hz, 35.2 kHz hsync, 8:5 aspect ratio\n"
"ModeLine \"480x300\"     21.656 480  496  536  616   300  301  302  312 Doublescan\n"
"# 480x300 @ 60 Hz, 37.8 kHz hsync\n"
"Modeline \"480x300\"     23.890 480  496  576  632   300  301  303  314 Doublescan\n"
"# 480x300 @ 63 Hz, 39.6 kHz hsync\n"
"Modeline \"480x300\"     25     480  496  576  632   300  301  303  314 Doublescan\n"
"# 480x300 @ 72 Hz, 48.0 kHz hsync\n"
"Modeline \"480x300\"     29.952 480  504  584  624   300  319  322  333 Doublescan\n"
"\n"
#endif
;

static char *devicesection_text =
"# **********************************************************************\n"
"# Graphics device section\n"
"# **********************************************************************\n"
"\n"
"# Any number of graphics device sections may be present\n"
"\n"
"# Standard VGA Device:\n"
"\n"
"Section \"Device\"\n"
"    Identifier	\"Generic VGA\"\n"
"    VendorName	\"Unknown\"\n"
"    BoardName	\"Unknown\"\n"
"    Chipset	\"generic\"\n"
"\n"
"#    VideoRam	256\n"
"\n"
"#    Clocks	25.2 28.3\n"
"\n"
"EndSection\n"
"\n"
"# Sample Device for accelerated server:\n"
"\n"
"# Section \"Device\"\n"
"#    Identifier	\"Actix GE32+ 2MB\"\n"
"#    VendorName	\"Actix\"\n"
"#    BoardName	\"GE32+\"\n"
"#    Ramdac	\"ATT20C490\"\n"
"#    Dacspeed	110\n"
"#    Option	\"dac_8_bit\"\n"
"#    Clocks	 25.0  28.0  40.0   0.0  50.0  77.0  36.0  45.0\n"
"#    Clocks	130.0 120.0  80.0  31.0 110.0  65.0  75.0  94.0\n"
"# EndSection\n"
"\n"
"# Sample Device for Hercules mono card:\n"
"\n"
"# Section \"Device\"\n"
"#    Identifier \"Hercules mono\"\n"
"# EndSection\n"
"\n"
"# Device configured by xf86config:\n"
"\n";

static char *screensection_text1 =
"# **********************************************************************\n"
"# Screen sections\n"
"# **********************************************************************\n"
"\n";

void write_fontpath_section(f)
	FILE *f;
{
	/* this will create the Fontpath lines, but only after checking,
	 * that the corresponding dir exists (was THE absolute problem
	 * users had with XFree86/OS2 3.1.2D !)
	 */
	int i;
	char cur[256+20],*colon, *hash;

	for (i=0; XF86Config_fontpaths[i]; i++) {
		strcpy(cur,TREEROOTLX);
		strcat(cur,XF86Config_fontpaths[i]);
		/* remove a ':' */
		colon = strchr(cur+2,':'); /* OS/2: C:/...:scaled */
		if (colon) *colon = 0;
		hash = exists_dir(cur) ? "" : "#";
		fprintf(f,"%s    FontPath   \"%s%s\"\n",
			hash,
			TREEROOTLX,
			XF86Config_fontpaths[i]);
	}
}

void write_XF86Config(filename)
	char *filename;
{
	FILE *f;

	/*
	 * Write the file.
	 */

	f = fopen(filename, "w");
	if (f == NULL) {
		printf("Failed to open filename for writing.\n");
#ifndef __EMX__
		if (getuid() != 0)
			printf("Maybe you need to be root to write to the specified directory?\n");
#endif
		exit(-1);
	}

	fprintf(f, "%s", XF86Config_firstchunk_text);
	write_fontpath_section(f);
	fprintf(f, "%s", XF86Config_fontpathchunk_text);

	/*
	 * Write keyboard section.
	 */
	if (config_altmeta) {
		fprintf(f, "    LeftAlt     Meta\n");
		fprintf(f, "    RightAlt    ModeShift\n");
	}
	else {
		fprintf(f, "#    LeftAlt     Meta\n");
		fprintf(f, "#    RightAlt    ModeShift\n");
	}
	fprintf(f, "%s", keyboardchunk2_text);

	if (config_xkbdisable) {
		fprintf(f, "    XkbDisable\n\n");
	} else {
		fprintf(f, "#    XkbDisable\n\n");
	}
	fprintf(f, "%s", keyboardchunk3_text);
	if (config_xkbkeymap) {
		fprintf(f, "    XkbKeymap   \"%s\"\n",config_xkbkeymap);
		fprintf(f, "%s", keyboardchunk4_text);
	} else {
		fprintf(f, "    Xkbkeycodes \"%s\"\n",config_xkbkeycodes);
		fprintf(f, "    XkbTypes    \"%s\"\n",config_xkbtypes);
		fprintf(f, "    XkbCompat   \"%s\"\n",config_xkbcompat);
		fprintf(f, "    XkbSymbols  \"%s\"\n",config_xkbsymbols);
		fprintf(f, "    XkbGeometry \"%s\"\n",config_xkbgeometry);
	}
	fprintf(f, "%s",keyboardlastchunk_text);

	/*
	 * Write pointer section.
	 */
	fprintf(f, "%s", pointersection_text1);
	fprintf(f, "    Protocol    \"%s\"\n",
		mousetype_identifier[config_mousetype]);
#ifndef __EMX__
	fprintf(f, "    Device      \"%s\"\n", config_pointerdevice);
#endif
	fprintf(f, "%s", pointersection_text2);
	if (!config_emulate3buttons)
		fprintf(f, "#");
	fprintf(f, "    Emulate3Buttons\n");
	if (!config_emulate3buttons)
		fprintf(f, "#");
	fprintf(f, "    Emulate3Timeout    50\n\n");
	fprintf(f, "# ChordMiddle is an option for some 3-button Logitech mice\n\n");
	if (!config_chordmiddle)
		fprintf(f, "#");
	fprintf(f, "    ChordMiddle\n\n");
	if (config_cleardtrrts) {
		fprintf(f, "    ClearDTR\n");
		fprintf(f, "    ClearRTS\n\n");
	}
	fprintf(f, "EndSection\n\n\n");

	/*
	 * Write XInput sample section
	 */
	fprintf(f, "%s", xinputsection_text);

	/*
	 * Write monitor section.
	 */
	fprintf(f, "%s", monitorsection_text1);
	fprintf(f, "    Identifier  \"%s\"\n", config_monitoridentifier);
	fprintf(f, "    VendorName  \"%s\"\n", config_monitorvendorname);
	fprintf(f, "    ModelName   \"%s\"\n", config_monitormodelname);
	fprintf(f, "\n");
	fprintf(f, "%s", monitorsection_text2);
	fprintf(f, "    HorizSync   %s\n", config_hsyncrange);
	fprintf(f, "\n");
	fprintf(f, "%s", monitorsection_text3);
	fprintf(f, "    VertRefresh %s\n", config_vsyncrange);
	fprintf(f, "\n");
	fprintf(f, "%s", monitorsection_text4);
	fprintf(f, "%s", modelines_text);
	fprintf(f, "EndSection\n\n\n");

	/*
	 * Write Device section.
	 */

	fprintf(f, "%s", devicesection_text);
	fprintf(f, "Section \"Device\"\n");
	fprintf(f, "    Identifier  \"%s\"\n", config_deviceidentifier);
	fprintf(f, "    VendorName  \"%s\"\n", config_devicevendorname);
	fprintf(f, "    BoardName   \"%s\"\n", config_deviceboardname);
	/* Rely on server to detect video memory. */
	fprintf(f, "    #VideoRam    %d\n", config_videomemory);
	if (card_selected != -1)
		/* Add comment lines from card definition. */
		fprintf(f, card[card_selected].lines);
	if (config_ramdac != NULL)
		fprintf(f, "    Ramdac      \"%s\"\n", config_ramdac);
	if (card_selected != -1)
		if (card[card_selected].dacspeed != NULL)
			fprintf(f, "    Dacspeed    %s\n",
				card[card_selected].dacspeed);
	if (config_clockchip != NULL)
		fprintf(f, "    Clockchip   \"%s\"\n", config_clockchip);
	else
	if (config_numberofclockslines == 0)
		fprintf(f, "    # Insert Clocks lines here if appropriate\n");
	else {
		int i;
		for (i = 0; i < config_numberofclockslines; i++)
			fprintf(f, "    Clocks %s\n", config_clocksline[i]);
	}
	fprintf(f, "EndSection\n\n\n");	

	/*
	 * Write Screen sections.
	 */

	fprintf(f, "%s", screensection_text1);

	/*
	 * SVGA screen section.
	 */
	if (config_screentype == 3) {
		fprintf(f, 
			"# The Colour SVGA server\n"
			"\n"
			"Section \"Screen\"\n"
			"    Driver      \"svga\"\n"
			"    # Use Device \"Generic VGA\" for Standard VGA 320x200x256\n"
			"    #Device      \"Generic VGA\"\n"
			"    Device      \"%s\"\n"
			"    Monitor     \"%s\"\n"
			"    Subsection \"Display\"\n"
			"        Depth       8\n"
			"        # Omit the Modes line for the \"Generic VGA\" device\n"
			"        Modes       %s\n"
			"        ViewPort    0 0\n"
			"        # Use Virtual 320 200 for Generic VGA\n",
			config_deviceidentifier,
			config_monitoridentifier,
			config_modesline8bpp);
		if (config_virtual)
			fprintf(f, "        Virtual     %d %d\n",
				config_virtualx8bpp, config_virtualy8bpp);
		fprintf(f, 
			"    EndSubsection\n"
			"    Subsection \"Display\"\n"
			"        Depth       16\n"
			"        Modes       %s\n"
			"        ViewPort    0 0\n",
			config_modesline16bpp);
		if (config_virtual)
			fprintf(f, "        Virtual     %d %d\n",
				config_virtualx16bpp, config_virtualy16bpp);
		fprintf(f, 
			"    EndSubsection\n"
			"    Subsection \"Display\"\n"
			"        Depth       24\n"
			"        Modes       %s\n"
			"        ViewPort    0 0\n",
			config_modesline24bpp);
		if (config_virtual)
			fprintf(f, "        Virtual     %d %d\n",
				config_virtualx24bpp, config_virtualy24bpp);
		fprintf(f, 
			"    EndSubsection\n"
			"    Subsection \"Display\"\n"
			"        Depth       32\n"
			"        Modes       %s\n"
			"        ViewPort    0 0\n",
			config_modesline32bpp);
		if (config_virtual)
			fprintf(f, "        Virtual     %d %d\n",
				config_virtualx32bpp, config_virtualy32bpp);
		fprintf(f, 
			"    EndSubsection\n"
			"EndSection\n"
			"\n");
	}
	else
		/*
		 * If the default server is not the SVGA server, generate
		 * an SVGA server screen for just generic 320x200.
		 */
		fprintf(f, 
			"# The Colour SVGA server\n"
			"\n"
			"Section \"Screen\"\n"
			"    Driver      \"svga\"\n"
			"    Device      \"Generic VGA\"\n"
			"    #Device      \"%s\"\n"
			"    Monitor     \"%s\"\n"
			"    Subsection \"Display\"\n"
			"        Depth       8\n"
			"        #Modes       %s\n"
			"        ViewPort    0 0\n"
			"        Virtual     320 200\n"
			"        #Virtual     %d %d\n"
			"    EndSubsection\n"
			"EndSection\n"
			"\n",
			config_deviceidentifier,
			config_monitoridentifier,
			config_modesline8bpp,
			config_virtualx8bpp, config_virtualy8bpp
		);

	/*
	 * VGA16 screen section.
	 */
	fprintf(f, 
		"# The 16-color VGA server\n"
		"\n"
		"Section \"Screen\"\n"
		"    Driver      \"vga16\"\n"
		"    Device      \"%s\"\n"
		"    Monitor     \"%s\"\n"
		"    Subsection \"Display\"\n"
		/*
		 * Depend on 800x600 to be deleted if not available due to
		 * dot clock or monitor constraints.
		 */
		"        Modes       \"640x480\" \"800x600\"\n"
		"        ViewPort    0 0\n"
		"        Virtual     800 600\n"
		"    EndSubsection\n"
		"EndSection\n"
		"\n",
		/*
		 * If mono or vga16 is configured as the default server,
		 * use the configured video card device instead of the
		 * generic VGA device.
		 */
		(config_screentype == 1 || config_screentype == 2) ?
			config_deviceidentifier :
			"Generic VGA",
		config_monitoridentifier
	);

	/*
	 * VGA2 screen section.
	 * This is almost identical to the VGA16 section.
	 */
	fprintf(f, 
		"# The Mono server\n"
		"\n"
		"Section \"Screen\"\n"
		"    Driver      \"vga2\"\n"
		"    Device      \"%s\"\n"
		"    Monitor     \"%s\"\n"
		"    Subsection \"Display\"\n"
		/*
		 * Depend on 800x600 to be deleted if not available due to
		 * dot clock or monitor constraints.
		 */
		"        Modes       \"640x480\" \"800x600\"\n"
		"        ViewPort    0 0\n"
		"        Virtual     800 600\n"
		"    EndSubsection\n"
		"EndSection\n"
		"\n",
		/*
		 * If mono or vga16 is configured as the default server,
		 * use the configured video card device instead of the
		 * generic VGA device.
		 */
		(config_screentype == 1 || config_screentype == 2) ?
			config_deviceidentifier :
			"Generic VGA",
		config_monitoridentifier
	);

	/*
	 * The Accel section.
	 */
	fprintf(f, 
#if XFREE86_VERSION >= 311
		"# The accelerated servers (S3, Mach32, Mach8, 8514, P9000, AGX, W32, Mach64)\n"
#else
		"# The accelerated servers (S3, Mach32, Mach8, 8514, P9000, AGX, W32)\n"
#endif
		"\n"
		"Section \"Screen\"\n"
		"    Driver      \"accel\"\n"
		"    Device      \"%s\"\n"
		"    Monitor     \"%s\"\n"
		"    Subsection \"Display\"\n"
		"        Depth       8\n"
		"        Modes       %s\n"
		"        ViewPort    0 0\n",
		config_deviceidentifier,
		config_monitoridentifier,
		config_modesline8bpp);
	if (config_virtual)
		fprintf(f, "        Virtual     %d %d\n",
			config_virtualx8bpp, config_virtualy8bpp);
	fprintf(f, 
		"    EndSubsection\n"
		"    Subsection \"Display\"\n"
		"        Depth       16\n"
		"        Modes       %s\n"
		"        ViewPort    0 0\n",
		config_modesline16bpp);
	if (config_virtual)
		fprintf(f, "        Virtual     %d %d\n",
			config_virtualx16bpp, config_virtualy16bpp);
	fprintf(f, 
		"    EndSubsection\n"
		"    Subsection \"Display\"\n"
		"        Depth       24\n"
		"        Modes       %s\n"
		"        ViewPort    0 0\n",
		config_modesline24bpp);
	if (config_virtual)
		fprintf(f, "        Virtual     %d %d\n",
			config_virtualx24bpp, config_virtualy24bpp);
	fprintf(f, 
		"    EndSubsection\n"
		"    Subsection \"Display\"\n"
		"        Depth       32\n"
		"        Modes       %s\n"
		"        ViewPort    0 0\n",
		config_modesline32bpp);
	if (config_virtual)
		fprintf(f, "        Virtual     %d %d\n",
			config_virtualx32bpp, config_virtualy32bpp);
	fprintf(f, 
		"    EndSubsection\n"
		"EndSection\n"
		"\n");

	fclose(f);
}


/*
 * Ask where to write XF86Config to. Returns filename.
 */

char *ask_XF86Config_location() {
	char s[80];
	char *filename;

	printf(
"I am going to write the XF86Config file now. Make sure you don't accidently\n"
"overwrite a previously configured one.\n\n");

#ifndef __EMX__
	if (getuid() == 0) {
#ifdef PREFER_XF86CONFIG_IN_ETC
		printf("Shall I write it to /etc/XF86Config? ");
		getstring(s);
		printf("\n");
		if (answerisyes(s))
			return "/etc/XF86Config";
#endif

		printf("Please answer the following question with either 'y' or 'n'.\n");
		printf("Shall I write it to the default location, /usr/X11R6/lib/X11/XF86Config? ");
		getstring(s);
		printf("\n");
		if (answerisyes(s))
			return "/usr/X11R6/lib/X11/XF86Config";

#ifndef PREFER_XF86CONFIG_IN_ETC
		printf("Shall I write it to /etc/XF86Config? ");
		getstring(s);
		printf("\n");
		if (answerisyes(s))
			return "/etc/XF86Config";
#endif
#else /* __EMX__ */
	{
		printf("Please answer the following question with either 'y' or 'n'.\n");
		printf("Shall I write it to the default location, drive:/XFree86/lib/X11/XConfig? ");
		getstring(s);
		printf("\n");
		if (answerisyes(s)) {
			return __XOS2RedirRoot("/XFree86/lib/X11/XConfig",'/');
		}
#endif /* __EMX__ */
	}

	printf("Do you want it written to the current directory as 'XF86Config'? ");
	getstring(s);
	printf("\n");
	if (answerisyes(s))
#ifndef __EMX__
		return "XF86Config";
#else
		return "XConfig";
#endif

	printf("Please give a filename to write to: ");
	getstring(s);
	printf("\n");
	filename = malloc(strlen(s) + 1);
	strcpy(filename, s);
	return filename;
}


/*
 * Check if an earlier version of XFree86 is installed; warn about proper
 * search path order in that case.
 */

static char *notinstalled_text =
"The directory " TREEROOT " does not exist. This probably means that you have\n"
"not yet installed an X11R6-based version of XFree86. Please install\n"
"XFree86 3.1+ before running this program, following the instructions in\n"
"the INSTALL or README that comes with the XFree86 distribution for your OS.\n"
"For a minimal installation it is sufficient to only install base binaries,\n"
"libraries, configuration files and a server that you want to use.\n"
"\n";

#ifndef __EMX__
static char *oldxfree86_text =
"The directory '/usr/X386/bin' exists. You probably have an old version of\n"
"XFree86 installed (XFree86 3.1 installs in '" TREEROOT "' instead of\n"
"'/usr/X386').\n"
"\n"
"It is important that the directory '" TREEROOT "' is present in your\n"
"search path, *before* any occurrence of '/usr/X386/bin'. If you have installed\n"
"X program binaries that are not in the base XFree86 distribution in\n"
"'/usr/X386/bin', you can keep the directory in your path as long as it is\n"
"after '" TREEROOT "'.\n"
"\n";

static char *pathnote_text =	
"Note that the X binary directory in your path may be a symbolic link.\n"
"In that case you could modify the symbolic link to point to the new binaries.\n"
"Example: 'rm -f /usr/bin/X11; ln -s /usr/X11R6/bin /usr/bin/X11', if the\n"
"link is '/usr/bin/X11'.\n"
"\n"
"Make sure the path is OK before continuing.\n";
#endif

void path_check() {
	char s[80];
	int ok;

	ok = exists_dir(TREEROOT);
	if (!ok) {
		printf("%s", notinstalled_text);
		printf("Do you want to continue? ");
		getstring(s);
		if (!answerisyes(s))
			exit(-1);
		printf("\n");
	}

#ifndef __EMX__
	ok = exists_dir("/usr/X386/bin");
	if (!ok)
		return;

	printf("%s", oldxfree86_text);
	printf("Your PATH is currently set as follows:\n%s\n\n",
		getenv("PATH"));
	printf("%s", pathnote_text);
	keypress();
#endif
}


/*
 * Program entry point.
 */

void main() {
	emptylines();

	printf("%s", intro_text);

	keypress();
	emptylines();

	path_check();

	emptylines();

	mouse_configuration();

	emptylines();

	xkb_keyboard_configuration();

	emptylines();

	/* XXX hv: if you have XKB, you might not need to make these
	 * settings; they are left in the config file though
	 */
	if (!config_xkbdisable) {
		config_altmeta = 0;
	} else {
		keyboard_configuration();
		emptylines();
	}

	monitor_configuration();

	emptylines();

	carddb_configuration();

	emptylines();

 	screen_configuration();

	emptylines();

	write_XF86Config(ask_XF86Config_location());

	printf("%s", finalcomment_text);

	exit(0);
}
