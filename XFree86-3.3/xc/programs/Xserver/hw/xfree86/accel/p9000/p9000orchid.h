/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/p9000/p9000orchid.h,v 3.4 1996/12/23 06:40:49 dawes Exp $ */
/*
 * Copyright 1994, Erik Nygren (nygren@mit.edu)
 *
 * This code may be freely incorporated in any program without royalty, as
 * long as the copyright notice stays intact.
 *
 * Additions by Harry Langenbacher (harry@brain.jpl.nasa.gov)
 *
 * ERIK NYGREN AND HARRY LANGENBACHER
 * DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL ERIK NYGREN
 * OR HARRY LANGENBACHER BE LIABLE
 * FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
 * RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF
 * CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 */
/* $XConsortium: p9000orchid.h /main/4 1996/02/21 17:32:56 kaleb $ */

#define ORCHID_OCR_RESERVED_MASK     0xCF
#define ORCHID_OCR_ENABLE_P9000      0x10
#define ORCHID_OCR_ENABLE_W5186      0x00
#define ORCHID_OCR_SYNC_POSITIVE     0x20
#define ORCHID_OCR_SYNC_NEGATIVE     0x00
#define ORCHID_CLKSELBITS_MASK       0xF3 /* mask for clock slect bits in misc reg */
#define ORCHID_CLKSELBITS_P9000      0x0C /* value to use in clock select */
                                          /* bits when in p9000 mode */
/* The location and text of the Orchid P9000 signature for autodetection */
#define ORCHID_BIOS_OFFSET           0x37
#define ORCHID_BIOS_LENGTH           54
/* The version isn't relavant */
#define ORCHID_BIOS_SIGNATURE        "I have no idea what goes here"  

extern p9000VendorRec p9000OrchidVendor;

