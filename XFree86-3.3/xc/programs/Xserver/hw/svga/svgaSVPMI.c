/* $XConsortium: svgaSVPMI.c,v 1.4 94/03/27 12:20:10 rws Exp $ */
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
#include <fcntl.h>
#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>
#include <sys/mman.h>


struct _Context {
  char          *pos;           /* current position                     */
  int           lineno;         /* current line number                  */
  int           num_keys;       /* number of keyelements in context     */
  char          *keys[8];       /* key elements                         */
  char          token[BUFSIZ];  /* buffer of last token                 */
};

typedef struct _Context* VESAContext;

static int memFd = -1;
static FILE *vesafp = NULL;
static char *fileBottom, *fileTop;

/*
 * Parser for the whole grammar
 */

#define sI 0
#define sE 1
#define sS 2
#define sQ 3
#define sG 4
#define sC 5
#define sT 6

static char const scan_tab[256] = {
    sE,  sI,  sI,  sI,  sI,  sI,  sI,  sI,	/* 0x00			*/
    sI,  sS,  sE,  sI,  sI,  sS,  sI,  sI,	/* 0x08			*/
    sI,  sI,  sI,  sI,  sI,  sI,  sI,  sI,	/* 0x10			*/
    sI,  sI,  sS,  sI,  sI,  sI,  sI,  sI,	/* 0x18			*/
    sS,  sI,  sQ,  sI,  sI,  sI,  sI,  sI,	/*   ! " # $ % & '	*/
    sS,  sS,  sI,  sI,  sS,  sG,  sT,  sC,	/* ( ) * + , - . /	*/
    sT,  sT,  sT,  sT,  sT,  sT,  sT,  sT,	/* 0 1 2 3 4 5 6 7	*/
    sT,  sT,  sI,  sG,  sI,  sG,  sI,  sI,	/* 8 9 : ; < = > ?	*/
    sI,  sT,  sT,  sT,  sT,  sT,  sT,  sT,	/* @ A B C D E F G	*/
    sT,  sT,  sT,  sT,  sT,  sT,  sT,  sT,	/* H I J K L M N O	*/
    sT,  sT,  sT,  sT,  sT,  sT,  sT,  sT,	/* P Q R S T U V W	*/
    sT,  sT,  sT,  sT,  sI,  sT,  sI,  sT,	/* X Y Z [ \ ] ^ _	*/
    sI,  sT,  sT,  sT,  sT,  sT,  sT,  sT,	/* ` a b c d e f g	*/
    sT,  sT,  sT,  sT,  sT,  sT,  sT,  sT,	/* h i j k l m n o	*/
    sT,  sT,  sT,  sT,  sT,  sT,  sT,  sT,	/* p q r s t u v w	*/
    sT,  sT,  sT,  sS,  sI,  sS,  sI,  sI,	/* x y z { | } ~  	*/
    sI,  sI,  sI,  sI,  sI,  sI,  sI,  sI,	/* 0x80			*/
    sI,  sI,  sI,  sI,  sI,  sI,  sI,  sI,	/* 0x88			*/
    sI,  sI,  sI,  sI,  sI,  sI,  sI,  sI,	/* 0x90			*/
    sI,  sI,  sI,  sI,  sI,  sI,  sI,  sI,	/* 0x98			*/
    sI,  sI,  sI,  sI,  sI,  sI,  sI,  sI,	/* 0xA0			*/
    sI,  sI,  sI,  sI,  sI,  sI,  sI,  sI,	/* 0xA8			*/
    sI,  sI,  sI,  sI,  sI,  sI,  sI,  sI,	/* 0xB0			*/
    sI,  sI,  sI,  sI,  sI,  sI,  sI,  sI,	/* 0xB8			*/
    sI,  sI,  sI,  sI,  sI,  sI,  sI,  sI,	/* 0xC0			*/
    sI,  sI,  sI,  sI,  sI,  sI,  sI,  sI,	/* 0xC8			*/
    sI,  sI,  sI,  sI,  sI,  sI,  sI,  sI,	/* 0xD0			*/
    sI,  sI,  sI,  sI,  sI,  sI,  sI,  sI,	/* 0xD8			*/
    sI,  sI,  sI,  sI,  sI,  sI,  sI,  sI,	/* 0xE0			*/
    sI,  sI,  sI,  sI,  sI,  sI,  sI,  sI,	/* 0xE8			*/
    sI,  sI,  sI,  sI,  sI,  sI,  sI,  sI,	/* 0xF0			*/
    sI,  sI,  sI,  sI,  sI,  sI,  sI,  sI,	/* 0xF8			*/
};

#define ADD_OPCODE(opcode,operand)					\
   *(pos)++ = (((opcode) << 24) | ((operand) & 0xffffff))

#define ADD_OPCODE_LONG(opcode,operand)					\
   *(pos)++ = ((opcode) << 24),						\
   *(pos)++ = (operand)

#define PSC_END			0
#define PSC_LOAD_CONST_0	1
#define PSC_LOAD_CONST_1	2
#define PSC_LOAD_CONST_2	3
#define PSC_LOAD_CONST_3	4
#define PSC_LOAD_CONST_LONG_0	5
#define PSC_LOAD_CONST_LONG_1	6
#define PSC_LOAD_CONST_LONG_2	7
#define PSC_LOAD_CONST_LONG_3	8
#define PSC_LOAD_REG_0		9
#define PSC_LOAD_REG_1		10
#define PSC_LOAD_REG_2		11
#define PSC_LOAD_REG_3		12
#define PSC_LOAD_PTR_0		13
#define PSC_LOAD_PTROFFSET_0	14
#define PSC_ASSIGN_REG		15
#define PSC_ASSIGN_PTR		16
#define PSC_NOT			17
#define PSC_AND			18
#define PSC_OR			19
#define PSC_XOR			20
#define PSC_SHL			21
#define PSC_SHR			22
#define PSC_ADD			23
#define PSC_SUB			24
#define PSC_MUL			25
#define PSC_DIV			26
#define PSC_REM			27
#define PSC_OUTB		28
#define PSC_OUTW		29
#define PSC_OUTDW		30
#define PSC_INB			31
#define PSC_INW			32
#define PSC_INDW		33
#define PSC_READB		34
#define PSC_READW		35
#define PSC_READDW		36
#define PSC_WRITEB		37
#define PSC_WRITEW		38
#define PSC_WRITEDW		39
#define PSC_BOUTB		40
#define PSC_BUFOUTB		41
#define PSC_BUFINB		42
#define PSC_BUFINDEXOUTB	43
#define PSC_BUFINDEXINB		44


struct _svpmiOp {
  char		*name;
  unchar        opcode;
  char          nargs;			/* readonly args		*/
  char          dstreg;			/* has destination		*/
  char          ptroffset;		/* first arg must be a offset   */
};

static const struct _svpmiOp svpmiOp[] = {
  { "NOT",          PSC_NOT,          0,  TRUE,  FALSE },
  { "AND",          PSC_AND,          1,  TRUE,  FALSE },
  { "OR",           PSC_OR,           1,  TRUE,  FALSE },
  { "XOR",          PSC_XOR,          1,  TRUE,  FALSE },
  { "SHR",          PSC_SHR,          1,  TRUE,  FALSE },
  { "SHL",          PSC_SHL,          1,  TRUE,  FALSE },
  { "MUL",          PSC_MUL,          1,  TRUE,  FALSE },
  { "DIV",          PSC_DIV,          1,  TRUE,  FALSE },
  { "REM",          PSC_REM,          1,  TRUE,  FALSE },
  { "OUTDW",        PSC_OUTDW,        2,  FALSE, FALSE },
  { "OUTB",         PSC_OUTB,         2,  FALSE, FALSE },
  { "OUTW",         PSC_OUTW,         2,  FALSE, FALSE },
  { "OUTDW",        PSC_OUTDW,        2,  FALSE, FALSE },
  { "INB",          PSC_INB,          1,  TRUE,  FALSE },
  { "INW",          PSC_INW,          1,  TRUE,  FALSE },
  { "INDW",         PSC_INDW,         1,  TRUE,  FALSE },
  { "READB",        PSC_READB,        1,  TRUE,  FALSE },
  { "READW",        PSC_READW,        1,  TRUE,  FALSE },
  { "READDW",       PSC_READDW,       1,  TRUE,  FALSE },
  { "WRITEB",       PSC_WRITEB,       2,  FALSE, FALSE },
  { "WRITEW",       PSC_WRITEW,       2,  FALSE, FALSE },
  { "WRITEDW",      PSC_WRITEDW,      2,  FALSE, FALSE },
  { "BOUTB",        PSC_BOUTB,        3,  FALSE, FALSE },
  { "BUFOUTB",      PSC_BUFOUTB,      3,  FALSE, TRUE  },
  { "BUFINB",       PSC_BUFINB,       3,  FALSE, TRUE  },
  { "BUFINDEXOUTB", PSC_BUFINDEXOUTB, 4,  FALSE, TRUE  },
  { "BUFINDEXINB",  PSC_BUFINDEXINB,  4,  FALSE, TRUE  },
  { NULL,           PSC_END,          0,  FALSE, FALSE },
};

static VESAContext
VESAOpen(
    char *path
)
{
  long        len;
  VESAContext ctxt = NULL;

  if (!(vesafp = fopen(path, "r"))) return NULL;

  fseek(vesafp, 0, SEEK_END); len = ftell(vesafp);

  fileBottom = (char*)mmap((caddr_t)0, len,
			   PROT_READ, MAP_PRIVATE, fileno(vesafp), 0);

  if (fileBottom == NULL) return NULL;

  fileTop = fileBottom + len;

  if (!(ctxt = (VESAContext)Xalloc(sizeof(struct _Context)))) {

    Xfree(ctxt);
    return NULL;
  }

  ctxt->pos      = fileBottom;
  ctxt->lineno   = 1;
  ctxt->num_keys = 0;
  ctxt->token[0] = '\0';

  return ctxt;
}

static void
VESAClose(
    void
)
{
  fclose(vesafp);
}

static char*
VESAToken(
    VESAContext ctxt
)
{
  char *last, *token = ctxt->token;
  int  c, length, index;

  for (;;) {

    if (ctxt->pos >= fileTop) return NULL;

    switch(scan_tab[c = *(last = ctxt->pos++)]) {
      
    case sI: /* Illegal */
      ErrorF("%d: illegal character %d\n", ctxt->lineno, c& 0xff);
      return NULL;

    case sE: /* End of line */
      ctxt->lineno++;
      break;
	
    case sS: /* White space */
      break;

    case sQ: /* Quoted string */
      for (length = 0; ctxt->pos < fileTop; ctxt->pos++, length++)
	if ((scan_tab[(int)*(ctxt->pos)] == sQ) &&
	    (*(ctxt->pos -1) != '\\'))
	  break;

      memcpy(token, last +1, length);
      token[length] = '\0';
      sprintf(token, "%s", token);                      /* subsitute escapes */
      ctxt->pos++;
      return token;

    case sG: /* Single char token */
      token[0] = c;
      token[1] = '\0';
      return token;

    case sC: /* Comment */
      if (scan_tab[(int)*(ctxt->pos)] == sC) {
	for (; ctxt->pos < fileTop; ctxt->pos++)
	  if (scan_tab[(int)*(ctxt->pos)] == sE) break;

	continue;
      }
      /* FALLTHRU */

    case sT: /* Normal token */
      token[0] = islower(c) ? toupper(c) : c;

      for (index = 1; ctxt->pos < fileTop; ctxt->pos++, index++) {
	c = (c = *(ctxt->pos), islower(c) ? toupper(c) : c);

	if (scan_tab[c] == sT || 
	    (scan_tab[c] == sC && scan_tab[(int)*(ctxt->pos +1)] != sC))

	  token[index] = c;
	else {
	  token[index] = '\0';
	  break;
	}
      }

      return token;
    }
  }
  /* NOTREACHED */
}

static void
VESADestroy(
    VESAContext ctxt
)
{
  int i;
  if (ctxt == NULL) return;

  for (i = 0; i < ctxt->num_keys; i++) Xfree(ctxt->keys[i]);
  Xfree(ctxt);
}

static VESAContext
VESASection(
       VESAContext ctxt,
       VESAContext *dst,
       char *name
)
{
  int         i, level, maxlevels;
  char        *token;
  VESAContext ret;

  if (!dst) {
    if (!(ret = (VESAContext)Xcalloc(sizeof(struct _Context)))) return NULL;
  }
  else if ((ret = *dst) != ctxt) {
    for (i = 0; i < ret->num_keys; i++) Xfree(ret->keys[i]);
  }

  if (ret != ctxt) {
    ret->pos      = ctxt->pos;
    ret->lineno   = ctxt->lineno;
    ret->num_keys = ctxt->num_keys;

    for (i = 0; i < ret->num_keys; i++)
      ret->keys[i] = (char *)strdup(ctxt->keys[i]);
  }

  if (name) {
    /*
     * get next matching section that matches name
     */
    maxlevels = ret->num_keys;
    ret->keys[ret->num_keys++] = (char *)strdup(name);
  }
  else {
    /*
     * just get next matching context
     */
    name = ret->keys[ret->num_keys -1];
    maxlevels = ret->num_keys -1;
  }

  while ((token = VESAToken(ret)) != NULL)
    {
      if (!strcmp(name, token)) {
	for (level = 0; level < maxlevels; level++)
	  if ((*token == '[') && !strcmp(ret->keys[level], token)) {
	    VESADestroy(ret);
	    return NULL;
	  }
	break;
      }
    }

  if (token)
    return ret;
  else {
    VESADestroy(ret);
    return NULL;
  }
}

static char*
VESAGetTag(
    VESAContext ctxt
)
{
  char c0, c1, *token;

  if (!(token = VESAToken(ctxt))) return NULL;

  c0 = token[0];
  c1 = token[1];

  if (!(token = VESAToken(ctxt)) || *token != ';') return NULL;

  token[0] = c0;
  token[1] = c1;
  return token;
}

static char*
VESAGetString(
    VESAContext ctxt,
    char        *name,
    char        *dval
)
{
  char *pos, *token;
  int  lineno;

  pos    = ctxt->pos;
  lineno = ctxt->lineno;

  while (((token = VESAToken(ctxt)) != NULL) && (*token != '['))
    if (strcmp(token, name))
      {
	/* skip assignment */
	if (((token = VESAToken(ctxt)) != NULL) && (*token != '[') && /* =   */
	    ((token = VESAToken(ctxt)) != NULL) && (*token != '[') && /* val */
	    ((token = VESAToken(ctxt)) != NULL) && (*token != '['))   /* ;   */
	  continue;
	else {
	  ctxt->pos    = pos;
	  ctxt->lineno = lineno;
	  return NULL; /* syntax error ... */
	}
      }
    else
      {
	if (((token = VESAToken(ctxt)) != NULL) && (*token == '=') &&
	    ((token = VESAToken(ctxt)) != NULL) && (*token != '['))
	  break;

	else {
	  ErrorF("%d: synatx error\n", ctxt->lineno);
	  ctxt->pos    = pos;
	  ctxt->lineno = lineno;
	  return NULL; /* syntax error ... */
	}
      }

  ctxt->pos    = pos;
  ctxt->lineno = lineno;

  if (!token || *token == '[') {
    if (dval) {
      strcpy(ctxt->token, dval);
      return ctxt->token;
    }
    else {
      return NULL;
    }
  }

  return token;
}

static int
VESAGetInt(
    VESAContext ctxt,
    char        *name,
    char        *dval
)
{
  char *token;
  int  ret;

  token = VESAGetString(ctxt, name, dval);

  if (token) {
    if (sscanf(token, "%i", &ret) != 1)
      ErrorF("%d: integer number expected\n", ctxt->lineno);
  }

  return ret;
}

static ulong*
VESAParseValue(
    VESAContext ctxt,
    ulong       *pos,
    int         reg,
    Bool        ptrAllowed
)
{
  ulong temp;
  char *token;

  if ((token = VESAToken(ctxt)) == NULL) return NULL;
  if (*token == '[') return NULL;

  if (sscanf(token, "R%lu", &temp) == 1) {

    ADD_OPCODE( (PSC_LOAD_REG_0 + reg), (temp & 0x3f) );
    return pos;
  }
  else if (ptrAllowed && (sscanf(token, "P%lu", &temp) == 1)) {

    ADD_OPCODE( (PSC_LOAD_PTR_0 + reg), temp );
    return pos;
  }
  else if (sscanf(token, "%li", &temp) == 1) {

    if (temp <= 0xffffff) {
      ADD_OPCODE( (PSC_LOAD_CONST_0 + reg), temp );
    }
    else {
      ADD_OPCODE_LONG( (PSC_LOAD_CONST_LONG_0 + reg), temp );
    }
    return pos;
  }

  ErrorF("%d: syntax error\n", ctxt->lineno);
  return NULL;
}

static ulong*
VESAGetCommand(
    VESAContext ctxt,
    Bool        ptrAllowed
)
{
  ulong                code[4096], *pos, *ret;
  char                 *token;
  int                   reg;
  ulong                 offset;
  const struct _svpmiOp *pOP, *p;

  pos = &code[0];

  while ((token = VESAToken(ctxt)) != NULL)
    {
      if (*token == '[') break;

      for (pOP = NULL, p = svpmiOp; p->name != NULL; p++) {
	if (!strcmp(p->name, token)) { pOP = p; break; }
      }

      if (pOP != NULL) {

	/*
	 * If we have a store operator, parse the destination register.
	 */
	if (pOP->dstreg &&
	    (((token = VESAToken(ctxt)) == NULL) ||
	     (*token == '[') ||
	     (sscanf(token, "R%d", &reg) != 1)))
	  {
	    ErrorF("%d: bad register identifier \"%s\"\n",
		   ctxt->lineno, token);
	    return NULL;
	  }

	if (pOP->ptroffset) {
	  if (!ptrAllowed) {
	    ErrorF("%d: no pointer variables allowed\n", ctxt->lineno);
	    return NULL;
	  }

	  if (((token = VESAToken(ctxt)) == NULL) ||
	      (*token == '[') ||
	      (sscanf(token, "P%lu", &offset) != 1))
	    {
	      ErrorF("%d: bad pointer identifier \"%s\"\n",
		     ctxt->lineno, token);
	      return NULL;
	    }

	  ADD_OPCODE( PSC_LOAD_PTROFFSET_0, offset );
	}
	else if ((pOP->nargs >= 1) &&
		 ((pos = VESAParseValue(ctxt, pos, 0, FALSE)) == NULL))
	  return NULL;

	if ((pOP->nargs >= 2) &&
	    ((pos = VESAParseValue(ctxt, pos, 1, FALSE)) == NULL))
	  return NULL;

	if ((pOP->nargs >= 3) &&
	    ((pos = VESAParseValue(ctxt, pos, 2, FALSE)) == NULL))
	  return NULL;

	if ((pOP->nargs >= 4) &&
	    ((pos = VESAParseValue(ctxt, pos, 3, FALSE)) == NULL))
	  return NULL;


	if (pOP->dstreg) {
	  ADD_OPCODE( pOP->opcode, reg & 0x3f );
	}
	else {
	  ADD_OPCODE( pOP->opcode, 0 );
	}
      }

      else if (sscanf(token, "R%d", &reg) == 1) {

	/*
	 * Parse Assignment:  Rxx = Rxx ; or Rxx = Pxx; or Rxx = const;
	 */
	if (((token = VESAToken(ctxt)) == NULL) || (*token != '=')) {
	  ErrorF("%d: syntax error\n", ctxt->lineno);
	  return NULL;
	}
	
	if (!(pos = VESAParseValue(ctxt, pos, 0, ptrAllowed))) return NULL;
	
	ADD_OPCODE( PSC_ASSIGN_REG, reg & 0x3f );
      }

      else if (ptrAllowed && (sscanf(token, "P%lu", &offset) == 1)) {

	/*
	 * Parse Assignment:  Pxx = Rxx ; or Pxx = Pxx; or Pxx = const;
	 */
	if (((token = VESAToken(ctxt)) == NULL) || (*token != '=')) {
	  ErrorF("%d: syntax error\n", ctxt->lineno);
	  return NULL;
	}
	
	if (!(pos = VESAParseValue(ctxt, pos, 0, TRUE))) return NULL;
	
	ADD_OPCODE( PSC_ASSIGN_PTR, offset );
      }

      else {
	ErrorF("%d: illegal command \"%s\"\n", ctxt->lineno, token);
	return NULL;
      }

      if (!(token = VESAToken(ctxt)) || *token != ';') return NULL;
    }

  ADD_OPCODE( PSC_END, 0 );

  ret = (ulong*)Xalloc(sizeof(ulong) * (pos - code));

  memcpy(ret, code, sizeof(ulong) * (pos - code));

  return ret;
}

static void
VESAExecCommand(
    ulong           *pcode,
    register ulong  R[64],
    unchar          *P
)
{
  register ulong  operand, i;
  ulong           opcode, a0, a1, a2, a3;
  unchar          *src, *dst;

  while (*pcode != (PSC_END << 24)) {

    opcode = *pcode >> 24;
    operand = *pcode++ & 0xffffff;

    switch (opcode) {

    case PSC_LOAD_CONST_0:        a0 = operand;                   break;
    case PSC_LOAD_CONST_1:        a1 = operand;                   break;
    case PSC_LOAD_CONST_2:        a2 = operand;                   break;
    case PSC_LOAD_CONST_3:        a3 = operand;                   break;
    case PSC_LOAD_CONST_LONG_0:   a0 = *pcode++;                  break;
    case PSC_LOAD_CONST_LONG_1:   a1 = *pcode++;                  break;
    case PSC_LOAD_CONST_LONG_2:   a2 = *pcode++;                  break;
    case PSC_LOAD_CONST_LONG_3:   a3 = *pcode++;                  break;
    case PSC_LOAD_REG_0:          a0 = R[operand];                break;
    case PSC_LOAD_REG_1:          a1 = R[operand];                break;
    case PSC_LOAD_REG_2:          a2 = R[operand];                break;
    case PSC_LOAD_REG_3:          a3 = R[operand];                break;
    case PSC_LOAD_PTR_0:          a0 = P[operand];                break;
    case PSC_LOAD_PTROFFSET_0:    a0 = (ulong)(P+ operand);       break;
    case PSC_ASSIGN_REG:          R[operand] = a0;                break;
    case PSC_ASSIGN_PTR:          *((unchar*)(P + operand)) = a0; break;
    case PSC_NOT:                 R[operand] = ~R[operand];       break;
    case PSC_AND:                 R[operand] &= a0;               break;
    case PSC_OR:                  R[operand] |= a0;               break;
    case PSC_XOR:                 R[operand] ^= a0;               break;
    case PSC_SHR:                 R[operand] >>= a0;              break;
    case PSC_SHL:                 R[operand] <<= a0;              break;
    case PSC_ADD:                 R[operand] += a0;               break;
    case PSC_SUB:                 R[operand] -= a0;               break;
    case PSC_MUL:                 R[operand] *= a0;               break;
    case PSC_DIV:                 R[operand] /= a0;               break;
    case PSC_REM:                 R[operand] %= a0;               break;
    case PSC_OUTB:                outb(a0, a1);                   break;
    case PSC_OUTW:                outw(a0, a1);                   break;
    case PSC_OUTDW:               outl(a0, a1);                   break;
    case PSC_INB:                 R[operand] = inb(a0);           break;
    case PSC_INW:                 R[operand] = inw(a0);           break;
    case PSC_INDW:                R[operand] = inl(a0);           break;

    case PSC_READB:
      (void)lseek(memFd, a0, SEEK_SET);
      (void)read(memFd, &(R[operand]), 1);
      break;

    case PSC_READW:
      (void)lseek(memFd, a0, SEEK_SET);
      (void)read(memFd, &(R[operand]), 2);
      break;

    case PSC_READDW:
      (void)lseek(memFd, a0, SEEK_SET);
      (void)read(memFd, &(R[operand]), 4);
      break;

    case PSC_WRITEB:
      (void)lseek(memFd, a0, SEEK_SET);
      (void)write(memFd, &a1, 1);
      break;

    case PSC_WRITEW:
      (void)lseek(memFd, a0, SEEK_SET);
      (void)write(memFd, &a1, 2);
      break;

    case PSC_WRITEDW:
      (void)lseek(memFd, a0, SEEK_SET);
      (void)write(memFd, &a1, 4);
      break;

    case PSC_BOUTB:
      for (i = 0; i < a0; i++) { outb(a1, i); outb(a2, R[i]); }
      break;

    case PSC_BUFOUTB:
      src = (unchar*)a0;
      while (a2--) outb(a1, *src--);
      break;

    case PSC_BUFINB:
      dst   = (unchar*)a0;
      while (a2--) *dst++ = inb(a1);
      break;

    case PSC_BUFINDEXOUTB:
      src = (unchar*)a0;
      for (i = 0; i < a3; i++) { outb(a1, i); outb(a2, *src++); }
      break;

    case PSC_BUFINDEXINB:
      dst = (unchar*)a0;
      for (i = 0; i < a3; i++) { outb(a1, i); *dst++ = inb(a2); }
      break;
    }
  }
}

static ulong *setText = NULL;
static ulong *setGraphics = NULL;
static ulong *setWindow = NULL;

extern char *svgaconfig;

/* ARGSUSED */
Bool
svgaSVPMIOpen(
    ScreenPtr pScreen,
    int        argc,
    char       **argv,
    ulong      *winAAttributes,		/* RETURN */
    ulong      *winBAttributes,		/* RETURN */
    int        *winGranularity,		/* RETURN */
    int        *winSize,		/* RETURN */
    ulong      *winABase,		/* RETURN */
    ulong      *winBBase,		/* RETURN */
    int        *bytesPerScanLine,	/* RETURN */
    int        *XResolution,		/* RETURN */
    int        *YResolution,		/* RETURN */
    int        *bitsRGB			/* RETURN */
)
{
  char          pmifile[BUFSIZ], *tag;
  int           width, height, textmode, tagval, w, h, d;
  ulong         modeAttributes;
  VESAContext   file, ctxt, ptxt;

  if (setText) Xfree(setText);
  if (setGraphics) Xfree(setGraphics);
  if (setWindow) Xfree(setWindow);

  if (sscanf(svgaconfig, "%[0-9a-zA-Z./]:%dx%d", pmifile, &width, &height)
      != 3) {
    ErrorF("bad -config: %s\n", svgaconfig);
    return FALSE;
  }

  if ((memFd == -1) && (memFd = open("/dev/mem", O_RDONLY)) == -1) {
    Error("open /dev/mem");
    return FALSE;
  }

  if ((file = VESAOpen(pmifile)) == NULL) {
    ErrorF("couldn't open \"%s\"\n", pmifile);
    return FALSE;
  }

  if (!(ctxt = VESASection(file, NULL, "[TEXT_MODE]")) ||
      !(tag = VESAGetTag(ctxt)) ||
      (sscanf(tag, "%i", &textmode) != 1))
    {
      ErrorF("missing/invalid [TEXT_MODE] section\n");
      return FALSE;
    }

  for (ctxt = VESASection(file, &ctxt, "[MODE]");
       ctxt;
       ctxt = VESASection(ctxt, &ctxt, NULL))
    {
      if (!(tag = VESAGetTag(ctxt)) ||
	  (sscanf(tag, "%i", &tagval) != 1))
	{
	  ErrorF("%d: invalid [MODE] section\n", ctxt->lineno);
	  return FALSE;
	}

      if (tagval == textmode) {

	if (!(ptxt = VESASection(ctxt, NULL, "[SETMODE]")) ||
	    !(setText = VESAGetCommand(ptxt, FALSE)))
	  {
	    ErrorF("%d: invalid [SETMODE] section\n", ptxt->lineno);
	    return FALSE;
	  }

	VESADestroy(ptxt);
      }
      else {
	if (!(ptxt = VESASection(ctxt, NULL, "[MODEINFO]")))
	  {
	    ErrorF("%d: missing [MODEINFO] section\n", ptxt->lineno);
	    return FALSE;
	  }

	modeAttributes = VESAGetInt(ptxt, "MODEATTRIBUTES", "0");
	w              = VESAGetInt(ptxt, "XRESOLUTION", "0");
	h              = VESAGetInt(ptxt, "YRESOLUTION", "0");
	d              = VESAGetInt(ptxt, "BITSPERPIXEL", "0");

	if (!(modeAttributes & 0x10) || 
	    (d != 8) || (width != w) || (height != h)) {
	  
	  VESADestroy(ptxt);
	  continue;
	}

	*winAAttributes =   VESAGetInt(ptxt, "WINAATTRIBUTES", "0");
	*winBAttributes =   VESAGetInt(ptxt, "WINBATTRIBUTES", "0");
	*winGranularity =   VESAGetInt(ptxt, "WINAGRANULARITY", "0");
	*winSize =          VESAGetInt(ptxt, "WINASIZE", "0");
	*winABase =         VESAGetInt(ptxt, "WINABASE", "0");
	*winBBase =         VESAGetInt(ptxt, "WINBBASE", "0");
	*bytesPerScanLine = VESAGetInt(ptxt, "BYTESPERSCANLINE", "0");
	*bitsRGB =          VESAGetInt(ptxt, "BITSRGB", "0");
	*XResolution = width;
	*YResolution = height;

	if (!(ptxt = VESASection(ctxt, &ptxt, "[SETMODE]")) ||
	    !(setGraphics = VESAGetCommand(ptxt, FALSE)))
	  {
	    ErrorF("%d: invalid [SETMODE] section\n", ptxt->lineno);
	    return FALSE;
	  }

	if (!(ptxt = VESASection(ctxt, &ptxt, "[SETWINDOW]")) ||
	    !(setWindow = VESAGetCommand(ptxt, FALSE)))
	  {
	    ErrorF("%d: invalid [SETWINDOW] section\n", ptxt->lineno);
	    return FALSE;
	  }

	VESADestroy(ptxt);
      }
    }

  if (!setText || !setGraphics || !setWindow) 
    {
      ErrorF("missing routines ...\n");
      return FALSE;
    }

  VESADestroy(ctxt);
  VESADestroy(file);
  VESAClose();

  return TRUE;
}

/* ARGSUSED */
void
svgaSVPMISetText(
    ScreenPtr pScreen
)
{
  ulong R[64];

  if (setText)
    VESAExecCommand(setText, R, NULL);
}

/* ARGSUSED */
void
svgaSVPMISetGraphics(
    ScreenPtr pScreen
)
{
  ulong R[64];

  VESAExecCommand(setGraphics, R, NULL);
}

/* ARGSUSED */
void
svgaSVPMISetWindow(
    ScreenPtr pScreen,
    int       position,
    int       windowNo
)
{
  ulong R[64];

  R[0] = position;
  R[1] = windowNo;

  VESAExecCommand(setWindow, R, NULL);
}

/* ARGSUSED */
void
svgaSVPMISetColor(
    ScreenPtr pScreen,
    int       index,
    ushort    red,
    ushort    green,
    ushort    blue
)
{
  outb(0x3c8, index );
  outb(0x3c9, red );
  outb(0x3c9, green );
  outb(0x3c9, blue );
}
