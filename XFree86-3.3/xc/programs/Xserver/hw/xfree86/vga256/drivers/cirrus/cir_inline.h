/* $XFree86: xc/programs/Xserver/hw/xfree86/vga256/drivers/cirrus/cir_inline.h,v 3.1 1996/12/23 06:56:50 dawes Exp $ */





/* $XConsortium: cir_inline.h /main/3 1996/02/21 18:03:55 kaleb $ */

/* Some inline assembler functions. */

#if __GNUC__ > 1 && defined(GCCUSESGAS)

static __inline__ unsigned long rotateleft( int n, unsigned long bits ) {
	__asm__ __volatile__("roll %%cl,%0\n\t"
	: "=r" (bits)		/* outputs */
	: "0" (bits), "c" (n)	/* inputs */
	);
	return bits;
}

/* Define __inline__ string operations. */

static __inline__ void __memsetlong( unsigned long *s, unsigned long c,
				     int count ) {
	__asm__ __volatile__("rep ; stosl\n\t"
	:
	: "a" (c), "D" (s), "c" (count)
	: "cx", "di");
} 

static __inline__ void __memset( void * s, char c, int count ) {
	__asm__ __volatile__(
	"cld\n\t"
	"cmpl $12,%%edx\n\t"
	"jl 1f\n\t"			/* if (count >= 12) */

	"movzbl %%al,%%ax\n\t"
	"movl %%eax,%%ecx\n\t"
	"shll $8,%%ecx\n\t"		/* c |= c << 8 */
	"orl %%ecx,%%eax\n\t"
	"movl %%eax,%%ecx\n\t"
	"shll $16,%%ecx\n\t"		/* c |= c << 16 */
	"orl %%ecx,%%eax\n\t" 
	
	"movl %%edx,%%ecx\n\t"
	"negl %%ecx\n\t"
	"andl $3,%%ecx\n\t"		/* (-s % 4) */
	"subl %%ecx,%%edx\n\t"		/* count -= (-s % 4) */
	"rep ; stosb\n\t"		/* align to longword boundary */
	
	"movl %%edx,%%ecx\n\t"
	"shrl $2,%%ecx\n\t"
	"rep ; stosl\n\t"		/* fill longwords */
	
	"andl $3,%%edx\n"		/* fill last few bytes */
	"1:\tmovl %%edx,%%ecx\n\t"	/* <= 12 entry point */
	"rep ; stosb\n\t"
	: :"a" (c),"D" (s),"d" (count)
	:"ax","cx","dx","di");
}

static __inline__ void __memcpy( void *to, void *from, int n ) {
__asm__ __volatile__("cld\n\t"
	"movl %%edi,%%ecx\n\t"
	"andl $1,%%ecx\n\t"
	"subl %%ecx,%%edx\n\t"
	"rep ; movsb\n\t"		/* 16-bit align destination */
	"movl %%edx,%%ecx\n\t"
	"shrl $2,%%ecx\n\t"
	"rep ; movsl\n\t"
	"testb $1,%%dl\n\t"
	"je 1f\n\t"
	"movsb\n"
	"1:\ttestb $2,%%dl\n\t"
	"je 2f\n\t"
	"movsw\n"
	"2:\n"
	::"d" (n),"D" ((long) to),"S" ((long) from)
	: "cx","dx","di","si");
}

static __inline__ void __memcpyb( unsigned char *to, unsigned char *from,
				  int n ) {
	__asm__ __volatile__("cld\n\t"
	"rep ; movsb\n\t"
	: :"c" (n),"D" ((long) to),"S" ((long) from)
	: "cx","di","si");
}

#else

#define __memset memset

#define __memcpy MemToBus

#define rotateleft(n, bits) (((bits) << (n)) | ((bits) >> (32 - (n))))

#ifdef __STDC__
static void __memsetlong( unsigned long *s, unsigned long c, int count )
#else
static void __memsetlong( s, c, count )
unsigned long *s, c;
int count;
#endif
{
	while (count > 4) {
		*s = c;
		*(s + 1) = c;
		*(s + 2) = c;
		*(s + 3) = c;
		s += 4;
		count -= 4;
	}
	while (count > 0) {
		*s = c;
		s++;
		count --;
	}
}

#ifdef __STDC__
static void __memcpyb( unsigned char *destp, unsigned char *srcp, int count )
#else
static void __memcpyb( destp, srcp, count )
unsigned char *destp, *srcp;
int count;
#endif
{
	while (count >= 8) {
		*destp = *srcp;
		*(destp + 1) = *(srcp + 1);
		*(destp + 2) = *(srcp + 2);
		*(destp + 3) = *(srcp + 3);
		*(destp + 4) = *(srcp + 4);
		*(destp + 5) = *(srcp + 5);
		*(destp + 6) = *(srcp + 6);
		*(destp + 7) = *(srcp + 7);
		destp += 8;
		srcp += 8;
		count -= 8;
	}
	while (count > 0) {
		*destp = *srcp;
		destp++;
		srcp++;
		count--;
	}
}
#endif
