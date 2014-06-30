/* $XConsortium: svgaColor.c,v 1.4 94/03/19 17:26:54 rws Exp $ */
/*
 * Copyright 1990,91,92,93 by Thomas Roell, Germany.
 * Copyright 1991,92,93    by SGCS (Snitily Graphics Consulting Services), USA.
 *
 * Permission to use, copy, modify, distribute, and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and
 * that both that copyright notice and this  permission notice appear
 * in supporting documentation, and that the name of Thomas Roell nor
 * SGCS be used in advertising or publicity pertaining to distribution
 * of the software without specific, written prior permission.
 * Thomas Roell nor SGCS makes no representations about the suitability
 * of this software for any purpose. It is provided "as is" without
 * express or implied warranty.
 *
 * THOMAS ROELL AND SGCS DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS
 * SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS, IN NO EVENT SHALL THOMAS ROELL OR SGCS BE LIABLE FOR ANY
 * SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
 * RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF
 * CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include "svga.h"
#include <stdio.h>
#include "os.h"
#include "opaque.h"

/*
 * Sorry, but I can't stand it anymore !!! Why using a libdbm thing, indstead
 * of reading the plain rgb.txt and create the database online while starting
 * the server ?? As result we would have fewer disk accesses ...
 */

#define HASHSIZE 511

typedef struct _dbEntry * dbEntryPtr;
typedef struct _dbEntry {
  dbEntryPtr     link;
  unsigned short red;
  unsigned short green;
  unsigned short blue;
  char           *name;
} dbEntry;


static dbEntryPtr hashTab[HASHSIZE];

static dbEntryPtr
lookup(
    char *name,
    int  len,
    Bool create
)
{
  ulong      hashkey = 0;
  dbEntryPtr entry, *prev;
  char       *str = name;

  if (!(name = (char*)ALLOCATE_LOCAL(len +1))) return NULL;
  CopyISOLatin1Lowered((unsigned char *)name, (unsigned char *)str, len);
  name[len] = '\0';

  for(str = name; *str; str++) hashkey = *str + (hashkey << 1);
  hashkey %= HASHSIZE;

  if ( (entry = hashTab[hashkey]) != NULL )
    {
      for( ; entry; prev = (dbEntryPtr*)entry, entry = entry->link )
	if (! strcmp(name, entry->name) ) break;
    }
  else
    prev = &(hashTab[hashkey]);

  if (!entry && create && (entry = (dbEntryPtr)Xalloc(sizeof(dbEntry) +len+1)))
    {
      *prev = entry;
      entry->link = NULL;
      entry->name = (char*)(entry +1);
      strcpy( entry->name, name );
    }

  DEALLOCATE_LOCAL(name);

  return entry;
}

Bool
OsInitColors()
{
  FILE       *rgb;
  char       *path;
  char       line[BUFSIZ];
  char       name[BUFSIZ];
  int        red, green, blue, lineno = 0;
  dbEntryPtr entry;

  static Bool was_here = FALSE;

  if (!was_here)
    {
      path = (char*)ALLOCATE_LOCAL(strlen(rgbPath) +5);
      strcpy(path, rgbPath);

      if (!(rgb = fopen(path, "r")))
	{
	  strcat(path, ".txt");

	  if (!(rgb = fopen(path, "r")))
	    {
	      ErrorF( "Couldn't open RGB_DB '%s'\n", rgbPath );
	      DEALLOCATE_LOCAL(path);
	      return FALSE;
	    }
	}

      while(fgets(line, sizeof(line), rgb))
	{
	  lineno++;
	  if (line[0] == '!')
	      continue;
	  if (sscanf(line,"%d %d %d %[^\n]\n", &red, &green, &blue, name) == 4)
	    {
	      if (red >= 0   && red <= 0xff &&
		  green >= 0 && green <= 0xff &&
		  blue >= 0  && blue <= 0xff)
		{
		  if ((entry = lookup(name, strlen(name), TRUE)) != NULL)
		    {
		      entry->red   = (red * 65535)   / 255;
		      entry->green = (green * 65535) / 255;
		      entry->blue  = (blue  * 65535) / 255;
		    }
		}
	      else
		ErrorF("Value for \"%s\" out of range: %s:%d\n",
		       name, path, lineno);
	    }
	  else if (*line && *line != '#')
	    ErrorF("Syntax Error: %s:%d\n", path, lineno);
	}
      
      fclose(rgb);
      DEALLOCATE_LOCAL(path);

      was_here = TRUE;
    }

  return TRUE;
}

/* ARGSUSED */

Bool
OsLookupColor(
    int	     screen,
    char     *name,
    unsigned len,
    ushort   *pred,
    ushort   *pgreen,
    ushort   *pblue
)
{
  dbEntryPtr entry;

  if ((entry = lookup(name, len, FALSE)) != NULL)
    {
      *pred   = entry->red;
      *pgreen = entry->green;
      *pblue  = entry->blue;
      return TRUE;
    }

  return FALSE;
}
