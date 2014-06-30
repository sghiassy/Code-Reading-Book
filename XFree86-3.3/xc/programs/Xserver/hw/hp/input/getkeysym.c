/* $XConsortium: getkeysym.c,v 1.7 95/01/24 02:45:23 dpw Exp $ */
/*

Copyright (c) 1986, 1987 by Hewlett-Packard Company

Permission to use, copy, modify, and distribute this
software and its documentation for any purpose and without
fee is hereby granted, provided that the above copyright
notice appear in all copies and that both that copyright
notice and this permission notice appear in supporting
documentation, and that the name of Hewlett Packard not be used in
advertising or publicity pertaining to distribution of the
software without specific, written prior permission.

HEWLETT-PACKARD MAKES NO WARRANTY OF ANY KIND WITH REGARD
TO THIS SOFWARE, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
PURPOSE.  Hewlett-Packard shall not be liable for errors
contained herein or direct, indirect, special, incidental or
consequential damages in connection with the furnishing,
performance, or use of this material.

This software is not subject to any license of the American
Telephone and Telegraph Company or of the Regents of the
University of California.

*/

#ifndef LIBDIR
#if OSMAJORVERSION >= 10
#define LIBDIR "/etc/X11"
#else
#define LIBDIR "/usr/lib/X11"
#endif
#endif

#include <stdio.h>
#include <X11/XHPlib.h>

/*#include <X11/keysym.h>*/	/* gets sucked in by XHPlib.h */

#define SUCCESS 		0

extern char *malloc();

static FILE *keysym_file = NULL;
static int num_headers = 0;
static struct XHP_keymap_header *header_data = NULL;
static int read_kd_name_table();
static int read_modmap_table();

#if 0		/* Not used any more */

/* GetKeySyms() gets a key sym table from a file.
 * This routine was copied and modified from XHPSetKeyboardMapping() in
 * lib/Xhp/XHPSetKbd.c.

 * The routine accepts two arguments:  a keyboard id (specifying which
 * keymap is to be loaded) and a pointer to a keysym array to be filled in.
 * If the keyboard id is in the file, that keysym table is copied into the
 * keysym array.  Otherwise, an error is returned and nothing is copied.

 * The routine  returns  zero if it  succeeds.  Otherwise,  it returns a
 * non-zero  value  specifying  the type of  error  that  occurred.  The
 * keyboard  ID value (-2) is a special  value used to  specify a non-HP
 * keyboard.  The keyboard ID value (-1) is used to identify HP keyboards
 * that are not supported by X (i.e. Arabic, Hebrew, Turkish, ...).

 * The  first  time  it  is   called,   the   routine   opens  the  file
 * "/usr/lib/X11/Xkeymaps",  reading a header of the  keysym  tables  it
 * contains.  The header contains a magic value for identification,  and
 * the ID, size, and offset of each keysym table in the file.
 */

Status GetKeySyms(kbd_id, keysyms)
  KEYBOARD_ID 	kbd_id;
  KeySym	*keysyms;
{
    char            magic[MAGIC_SIZE];
    int 	    header_size;
    int		    index;
    int             n, return_code = SUCCESS;
    char *name;
    char filename[512];

    /* If this is the first time this routine has been called, then open
     * the keysym file, read the header, and verify the magic and read in
     * the keysym table.
     */

    /* get the correct keymapfile name -- use environment variable if it is
     * set else use default.  If name starts with '/' it describes full path
     * name, else it is relative to the default directory.
     */

    if ( ( name = (char *)getenv("XHPKEYMAPFILE") ) == NULL )
	name = DEF_FILENAME;

    if (name[0] != '/')
    {
	sprintf(filename, "%s/%s",LIBDIR,name);
    }
    else
	strcpy(filename, name);

    if ((keysym_file = fopen(filename,"r")) == NULL)
	return(XHPKB_NOKEYFILE);

    /* Read and verify the magic number */

    fread(magic,MAGIC_SIZE,1,keysym_file);
    if ((strcmp(magic,VERIFY_MAGIC)) != 0)
	{ return_code = XHPKB_BADMAGIC; goto done; }

    /* Next read in the header size - it is in the next 4 bytes.
     * Then read in the header table, which is found in the next
     * header-size bytes of the file.
     */

    fread(&header_size,4,1,keysym_file);
    if (!(header_data = (struct XHP_keymap_header *)malloc(header_size)))
	{ return_code = XHPKB_NOMEM; goto done; }
    fread(header_data,header_size,1,keysym_file);

    num_headers = header_size / sizeof(struct XHP_keymap_header);

    /* Verify that a valid keyboard ID has been passed in. While you're
     * at it, position the header_data pointer to the correct record.
     */

    for (index = 0; index < num_headers; index++)
    {
	if (kbd_id == header_data[index].kbd)
	    break;
    }

    if (index >= num_headers)            /* It's an illegal keyboard ID */
	{ return_code = XHPKB_BADKBID; goto done; }

    /* Copy the keysym table from the file into the keysyms array */

    fseek(keysym_file, header_data[index].offset, 0);
    fread((char *)keysyms, header_data[index].size, 1, keysym_file);

done:
    free((char *)header_data);
    fclose(keysym_file);
    return(return_code);
}
#endif


    /* This next stuff supersedes the above stuff.  It provides support for
     *   keydevice names and modmap names.
     * Most of this code was taken from lib/Xhp/tools/cdkeymap.c (probably
     *   not called that anymore).
     */

static char *kd_name_heap = NULL, *modmap_name_heap = NULL;
static int kd_names = 0, num_modmaps = 0, 
	num_keydevice_names = 0;
static HPKKeyDeviceInfo *keydevice_name_table = NULL;
static HPKModMap *modmap_table = NULL;


    /* 
     * input:
     *   file_name:  If not NULL, try and open "/usr/lib/X11/<file_name>".
     * Notes:
     *   Sequence:
     *     if (!HPKsetup()) error_occured
     *     piddle with keymaps (other routines in here)
     *     HPKclean_up();
     *       You MUST call HPKclean_up() after calling HPKsetup() (no matter
     *       what HPKclean_up() returns).  It resets state variables.
     */
int HPKsetup(file_name) char *file_name;
{
    char            magic[MAGIC_SIZE];
    int 	    header_size;
    char *name;
    char filename[512];

    /* If this is the first time this routine has been called, then open
     * the keysym file, read the header, and verify the magic and read in
     * the keysym table.
     */

    /* get the correct keymapfile name -- use environment variable if it is
     * set else use default.  If name starts with '/' it describes full path
     * name, else it is relative to the default directory.
     */
    if (file_name) name = file_name;
    else
      if (NULL == (name = (char *)getenv("XHPKEYMAPFILE")))
	name = DEF_FILENAME;

    if (name[0] != '/')
    {
      sprintf(filename, "%s/%s",LIBDIR,name);
    }
    else strcpy(filename, name);

    if (NULL == (keysym_file = fopen(filename,"r")))
	return False;

    /* Read and verify the magic number */

    fread(magic,MAGIC_SIZE,1,keysym_file);
    if ((strcmp(magic,VERIFY_MAGIC)) != 0) return False;

    /* Next read in the header size - it is in the next 4 bytes.
     * Then read in the header table, which is found in the next
     * header-size bytes of the file.
     */

    fread(&header_size,4,1,keysym_file);
    if (!(header_data = (struct XHP_keymap_header *)malloc(header_size)))
	return False;
    fread(header_data,header_size,1,keysym_file);

    num_headers = header_size / sizeof(struct XHP_keymap_header);

    read_kd_name_table();
    read_modmap_table();

    return True;
}


/* ******************************************************************** */
/* ************************* Lookup Routines ************************** */
/* ******************************************************************** */


    /* Returns:
     *   NULL : id not found
     *    ptr : pointer into the header table
     */
static struct XHP_keymap_header *HPKlookup_kd_id(kd_id)
{
  int n;
  struct XHP_keymap_header *ptr;

  for (n = num_headers, ptr = header_data; n--; ptr++)
    if (kd_id == ptr->kbd) return ptr;

  return NULL;
}

    /* Returns:
     *   NULL : id not found
     *    ptr : pointer into name table
     */
HPKKeyDeviceInfo *HPKlookup_kd_by_id(kd_id)
{
  int n;
  HPKKeyDeviceInfo *ptr;

  for (n = num_keydevice_names, ptr = keydevice_name_table; n--; ptr++)
    if (kd_id == ptr->keydevice_id) return ptr;

  return NULL;
}

    /* Returns:
     *   NULL : modmap not found
     *    ptr : pointer into name table
     */
HPKModMap *HPKlookup_modmap(modmap_name) char *modmap_name;
{
  int n;
  HPKModMap *ptr;

  for (n = num_modmaps, ptr = modmap_table; n--; ptr++)
    if (0 == strcmp(modmap_name, ptr->modmap_name)) return ptr;

  return NULL;
}

    /* Returns:
     *   NULL : id not found
     *    ptr : pointer into name table
     */
HPKKeyDeviceInfo *HPKlookup_kd_by_name(kd_name) char *kd_name;
{
  int n;
  HPKKeyDeviceInfo *ptr;

  for (n = num_keydevice_names, ptr = keydevice_name_table; n--; ptr++)
    if (0 == strcmp(kd_name, ptr->name)) return ptr;

  return NULL;
}


/* ******************************************************************** */
/* *************************** Read Tables **************************** */
/* ******************************************************************** */

static int read_kd_name_table()
{
  int a,b,c,d,e,f, n, z, kd_id;
  HPKKeyDeviceInfo *kptr;
  struct XHP_keymap_header *ptr;

  if (!(ptr = HPKlookup_kd_id(HPK_KEYDEVICE_NAME_TABLE_ID))) return False;

  fseek(keysym_file, ptr->offset, 0);

  fread((char *)&num_keydevice_names, sizeof(int), 1, keysym_file);
  n = num_keydevice_names;

  fread((char *)&z,  sizeof(int), 1, keysym_file);   /* size of names table */

  if (!(kd_name_heap = malloc(z))		||
      !(keydevice_name_table =
		(HPKKeyDeviceInfo *)malloc(n*sizeof(HPKKeyDeviceInfo))))
  {
    num_keydevice_names = 0;
    return False;
  }

  fread(kd_name_heap, z, 1, keysym_file);		/* read the names */

  for (kptr = keydevice_name_table; n--; kptr++)	/* read the name table */
  {
    fread((char *)&a, sizeof(int), 1, keysym_file);	/* kd name offset */
    fread((char *)&b, sizeof(int), 1, keysym_file);	/* key device id */
    fread((char *)&c, sizeof(int), 1, keysym_file);	/* modmap name offset */
    fread((char *)&d, sizeof(int), 1, keysym_file);	/* min keycode */
    fread((char *)&e, sizeof(int), 1, keysym_file);	/* max keycode */
    fread((char *)&f, sizeof(int), 1, keysym_file);	/* columns */

    kptr->name         = kd_name_heap + a;
    kptr->keydevice_id = b;
    kptr->modmap_name  = kd_name_heap + c;
    kptr->min_keycode  = d;
    kptr->max_keycode  = e;
    kptr->columns      = f;
  }

  return True;
}

static int read_modmap_table()
{
  int n, a, z;
  HPKModMap *mptr;
  struct XHP_keymap_header *ptr;

  if (!(ptr = HPKlookup_kd_id(HPK_MODMAP_TABLE_ID))) return False;

  fseek(keysym_file, ptr->offset, 0);
  fread((char *)&num_modmaps, sizeof(int), 1, keysym_file);

  fread((char *)&z, sizeof(int), 1, keysym_file);	/* name table size */

  if (!(modmap_name_heap  =           malloc(z))	||
      !(modmap_table = (HPKModMap *)malloc(num_modmaps*sizeof(HPKModMap))))

  {
    num_modmaps = 0;
    return False;
  }

  fread(modmap_name_heap, z,1, keysym_file);	/* read the name table */

  n = num_modmaps;
  for (mptr = modmap_table; n--; mptr++)	/* read & build the table */
  {
    fread((char *)&a, sizeof(int), 1, keysym_file);	/* modmap name offset */
    fread((char *)mptr->modmap, MODMAP_SIZE,1, keysym_file);	/* modmap */

    mptr->modmap_name = modmap_name_heap + a;
  }

  return True;
}


    /* Read a keytable.
     * Input:
     *   kd_id:  id of the keytable to read.
     *   keysyms:  pointer to an area big enough to hold the keytable.
     * Returns:
     *   TRUE: everything went as expected.
     *   FALSE: couldn't find id.
     */
int HPKread_keymap(kd_id, keysyms) KeySym *keysyms;
{
  struct XHP_keymap_header *ptr;

  if (!(ptr = HPKlookup_kd_id(kd_id))) return False;

  fseek(keysym_file, ptr->offset, 0);

  fread((char *)keysyms, ptr->size, 1, keysym_file);

  return True;
}


/* ******************************************************************** */
/* ***************************** Clean Up ***************************** */
/* ******************************************************************** */

void HPKclean_up()
{
  if (keysym_file) fclose(keysym_file);
	keysym_file = NULL;

  if (header_data) free((char *)header_data);
	header_data = NULL;

  if (kd_name_heap)    free((char *)kd_name_heap);
	kd_name_heap = NULL;
  if (keydevice_name_table) free((char *)keydevice_name_table);
	keydevice_name_table = NULL;

  if (modmap_name_heap) free((char *)modmap_name_heap);
	modmap_name_heap = NULL;
  if (modmap_table) free((char *)modmap_table);
	modmap_table = NULL;


  num_headers = kd_names = num_modmaps = num_keydevice_names = 0;
}
