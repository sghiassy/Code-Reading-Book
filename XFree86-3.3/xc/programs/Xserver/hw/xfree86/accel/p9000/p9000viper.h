/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/p9000/p9000viper.h,v 3.7.2.2 1997/05/11 01:54:44 dawes Exp $ */
/*
 * Written by Erik Nygren
 *
 * This code may be freely incorporated in any program without royalty, as
 * long as the copyright notice stays intact.
 *
 * ERIK NYGREN DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL ERIK NYGREN BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF
 * CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 */
/* $XConsortium: p9000viper.h /main/4 1996/02/21 17:33:25 kaleb $ */

/* Bits for the Output Control Register */

#define VPR_VLB_OCR_BASE_A0000000     0x01
#define VPR_VLB_OCR_BASE_20000000     0x02
#define VPR_VLB_OCR_BASE_80000000     0x03
#define VPR_VLB_OCR_RESERVED_MASK     0x0C
#define VPR_VLB_OCR_ENABLE_P9000      0x10
#define VPR_VLB_OCR_ENABLE_W5186      0x80

#define VPR_PCI_OCR_ENABLE_P9000      0x10
#define VPR_PCI_OCR_ENABLE_W5186      0x00
#define VPR_PCI_OCR_RESERVED_MASK     0xEF

/* The location and text of the Viper VLB signature for autodetection
 * Warning: chack for the Viper PCI first since this also exists for
 * it as well.... */
#define VPR_VLB_BIOS_OFFSET           0x37
#define VPR_VLB_BIOS_LENGTH           56
/* The version isn't relavant */
#define VPR_VLB_BIOS_SIGNATURE        "VIPER VLB  Vers."  

#if 0
/* The location and text of the Viper PCI signature for autodetection
 * Warning: the Viper PCI also the Viper VLB sig! */
#define VPR_PCI_BIOS_OFFSET           0x77
#define VPR_PCI_BIOS_LENGTH           56
/* The version isn't relavant */
#define VPR_PCI_BIOS_SIGNATURE        "VIPER PCI  Vers."  
#else
/* Viper PCI ids */
#define PCI_WEITEK_VENDOR_ID		0x100e
#define PCI_P9000_DEVICE_ID		0x9001
#endif

extern p9000VendorRec p9000ViperVlbVendor;
extern p9000VendorRec p9000ViperPciVendor;

/* Common (for now at least) */

extern void p9000LockVGAExtRegs(
#if NeedFunctionPrototypes
    void
#endif
);

extern void p9000UnlockVGAExtRegs(
#if NeedFunctionPrototypes
    void
#endif
);
