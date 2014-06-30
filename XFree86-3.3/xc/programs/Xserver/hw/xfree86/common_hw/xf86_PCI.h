/* $XFree86: xc/programs/Xserver/hw/xfree86/common_hw/xf86_PCI.h,v 3.12 1996/12/23 06:44:27 dawes Exp $ */
/*
 * Copyright 1995 by Robin Cutshaw <robin@XFree86.Org>
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the names of the above listed copyright holder(s)
 * not be used in advertising or publicity pertaining to distribution of 
 * the software without specific, written prior permission.  The above listed
 * copyright holder(s) make(s) no representations about the suitability of this 
 * software for any purpose.  It is provided "as is" without express or 
 * implied warranty.
 *
 * THE ABOVE LISTED COPYRIGHT HOLDER(S) DISCLAIM(S) ALL WARRANTIES WITH REGARD 
 * TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY 
 * AND FITNESS, IN NO EVENT SHALL THE ABOVE LISTED COPYRIGHT HOLDER(S) BE 
 * LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY 
 * DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER 
 * IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING 
 * OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 */
/* $XConsortium: xf86_PCI.h /main/13 1996/10/25 10:25:59 kaleb $ */

#ifndef _XF86_PCI_H
#define _XF86_PCI_H 1

#if 0
#define USE_OLD_PCI_CODE
#endif

typedef struct pci_config_reg {
    /* start of official PCI config space header */
    union {
	CARD32 device_vendor;
	struct {
	    CARD16 vendor;
	    CARD16 device;
	} dv;
    } dv_id;
#define _device_vendor dv_id.device_vendor
#define _vendor dv_id.dv.vendor
#define _device dv_id.dv.device
    union {
        CARD32 status_command;
	struct {
	    CARD16 command;
	    CARD16 status;
	} sc;
    } stat_cmd;
#define _status_command stat_cmd.status_command
#define _command stat_cmd.sc.command
#define _status  stat_cmd.sc.status
    union {
        CARD32 class_revision;
	struct {
	    CARD8 rev_id;
	    CARD8 prog_if;
	    CARD8 sub_class;
	    CARD8 base_class;
	} cr;
    } class_rev;
#define _class_revision class_rev.class_revision
#define _rev_id     class_rev.cr.rev_id
#define _prog_if    class_rev.cr.prog_if
#define _sub_class  class_rev.cr.sub_class
#define _base_class class_rev.cr.base_class
    union {
        CARD32 bist_header_latency_cache;
	struct {
	    CARD8 cache_line_size;
	    CARD8 latency_timer;
	    CARD8 header_type;
	    CARD8 bist;
	} bhlc;
    } bhlc;
#define _bist_header_latency_cache bhlc.bist_header_latency_cache
#define _cache_line_size bhlc.bhlc.cache_line_size
#define _latency_timer   bhlc.bhlc.latency_timer
#define _header_type     bhlc.bhlc.header_type
#define _bist            bhlc.bhlc.bist
    union {
	struct {
	    CARD32 dv_base0;
	    CARD32 dv_base1;
	    CARD32 dv_base2;
	    CARD32 dv_base3;
	    CARD32 dv_base4;
	    CARD32 dv_base5;
	} dv;
	struct {
	    CARD32 bg_rsrvd[2];
	    CARD8 primary_bus_number;
	    CARD8 secondary_bus_number;
	    CARD8 subordinate_bus_number;
	    CARD8 secondary_latency_timer;
	    CARD8 io_base;
	    CARD8 io_limit;
	    CARD16 secondary_status;
	    CARD16 mem_base;
	    CARD16 mem_limit;
	    CARD16 prefetch_mem_base;
	    CARD16 prefetch_mem_limit;
	} bg;
    } bc;
#define	_base0				bc.dv.dv_base0
#define	_base1				bc.dv.dv_base1
#define	_base2				bc.dv.dv_base2
#define	_base3				bc.dv.dv_base3
#define	_base4				bc.dv.dv_base4
#define	_base5				bc.dv.dv_base5
#define	_primary_bus_number		bc.bg.primary_bus_number
#define	_secondary_bus_number		bc.bg.secondary_bus_number
#define	_subordinate_bus_number		bc.bg.subordinate_bus_number
#define	_secondary_latency_timer	bc.bg.secondary_latency_timer
#define _io_base			bc.bg.io_base
#define _io_limit			bc.bg.io_limit
#define _secondary_status		bc.bg.secondary_status
#define _mem_base			bc.bg.mem_base
#define _mem_limit			bc.bg.mem_limit
#define _prefetch_mem_base		bc.bg.prefetch_mem_base
#define _prefetch_mem_limit		bc.bg.prefetch_mem_limit
    CARD32 rsvd1;
    CARD32 rsvd2;
    CARD32 _baserom;
    CARD32 rsvd3;
    CARD32 rsvd4;
    union {
        CARD32 max_min_ipin_iline;
	struct {
	    CARD8 int_line;
	    CARD8 int_pin;
	    CARD8 min_gnt;
	    CARD8 max_lat;
	} mmii;
    } mmii;
#define _max_min_ipin_iline mmii.max_min_ipin_iline
#define _int_line mmii.mmii.int_line
#define _int_pin  mmii.mmii.int_pin
#define _min_gnt  mmii.mmii.min_gnt
#define _max_lat  mmii.mmii.max_lat
    /* I don't know how accurate or standard this is (DHD) */
    union {
	CARD32 user_config;
	struct {
	    CARD8 user_config_0;
	    CARD8 user_config_1;
	    CARD8 user_config_2;
	    CARD8 user_config_3;
	} uc;
    } uc;
#define _user_config uc.user_config
#define _user_config_0 uc.uc.user_config_0
#define _user_config_1 uc.uc.user_config_1
#define _user_config_2 uc.uc.user_config_2
#define _user_config_3 uc.uc.user_config_3
    /* end of official PCI config space header */
#ifdef USE_OLD_PCI_CODE
    CARD32 _pcibusidx;
    CARD32 _pcinumbus;
    CARD32 _pcibuses[16];
    CARD16 _configtype;   /* config type found                   */
    CARD16 _ioaddr;       /* config type 1 - private I/O addr    */
#endif
    CARD32 _bus;
    CARD32 _cardnum;      /* config type 2 - private card number */
    CARD32 _func;         /* function number */
} pciConfigRec, *pciConfigPtr;

typedef union {
    CARD32 cfg1;
    struct {
	CARD8  enable;
	CARD8  forward;
	CARD16 port;
    } cfg2;
    CARD32 tag;
} pciTagRec, *pciTagPtr;

#define PCI_EN 0x80000000

#define MAX_PCI_DEVICES 64

#define PCI_CONFIG1_MAXDEV	32
#define PCI_CONFIG2_MAXDEV	16

/* Registers */
#define PCI_REG_USERCONFIG 		0x40
#define PCI_OPTION_REG	 		0x40

/* PCI Configuration address */
#define	PCI_MODE1_ADDRESS_REG		0xCF8
#define	PCI_MODE1_DATA_REG		0xCFC

#define	PCI_MODE2_ENABLE_REG		0xCF8
#ifdef PC98
#define	PCI_MODE2_FORWARD_REG		0xCF9
#else
#define	PCI_MODE2_FORWARD_REG		0xCFA
#endif

/* Device identification register */
#define PCI_ID_REG			0x00

/* Command and status register */
#define	PCI_CMD_STAT_REG		0x04
#define	PCI_CMD_MASK			0xffff
#define	PCI_CMD_IO_ENABLE		0x01
#define	PCI_CMD_MEM_ENABLE		0x02
#define	PCI_CMD_MASTER_ENABLE		0x04
#define PCI_CMD_SPECIAL_ENABLE		0x08
#define PCI_CMD_INVALIDATE_ENABLE 	0x10
#define PCI_CMD_PALETTE_ENABLE 		0x20
#define	PCI_CMD_PARITY_ENABLE		0x40
#define	PCI_CMD_STEPPING_ENABLE		0x80
#define	PCI_CMD_SERR_ENABLE		0x100
#define	PCI_CMD_BACKTOBACK_ENABLE	0x200

/* base classes */
#define PCI_CLASS_REG          		0x08
#define PCI_CLASS_MASK          	0xff000000

/* base classes as in _base_class */
#define PCI_CLASS_PREHISTORIC           0x00
#define PCI_CLASS_MASS_STORAGE          0x01
#define PCI_CLASS_NETWORK               0x02
#define PCI_CLASS_DISPLAY               0x03
#define PCI_CLASS_MULTIMEDIA            0x04
#define PCI_CLASS_MEMORY                0x05
#define PCI_CLASS_BRIDGE                0x06
#define PCI_CLASS_UNDEFINED             0xff

/* Sub class */

#define PCI_SUBCLASS_MASK       0x00ff0000

/* Sub classes as in _sub_class */

/* 0x00 prehistoric subclasses */
#define PCI_SUBCLASS_PREHISTORIC_MISC   0x00
#define PCI_SUBCLASS_PREHISTORIC_VGA    0x01

/* 0x01 mass storage subclasses */
#define PCI_SUBCLASS_MASS_STORAGE_SCSI  	0x00
#define PCI_SUBCLASS_MASS_STORAGE_IDE   	0x01
#define PCI_SUBCLASS_MASS_STORAGE_FLOPPY        0x02
#define PCI_SUBCLASS_MASS_STORAGE_IPI   	0x03
#define PCI_SUBCLASS_MASS_STORAGE_MISC  	0x80

/* 0x02 network subclasses */
#define PCI_SUBCLASS_NETWORK_ETHERNET   	0x00
#define PCI_SUBCLASS_NETWORK_TOKENRING  	0x01
#define PCI_SUBCLASS_NETWORK_FDDI       	0x02
#define PCI_SUBCLASS_NETWORK_MISC       	0x80

/* 0x03 display subclasses */
#define PCI_SUBCLASS_DISPLAY_VGA        0x00
#define PCI_SUBCLASS_DISPLAY_XGA        0x01
#define PCI_SUBCLASS_DISPLAY_MISC       0x80

/* 0x04 multimedia subclasses */
#define PCI_SUBCLASS_MULTIMEDIA_VIDEO   0x00
#define PCI_SUBCLASS_MULTIMEDIA_AUDIO   0x01
#define PCI_SUBCLASS_MULTIMEDIA_MISC    0x80

/* 0x05 memory subclasses */
#define PCI_SUBCLASS_MEMORY_RAM         0x00
#define PCI_SUBCLASS_MEMORY_FLASH       0x01
#define PCI_SUBCLASS_MEMORY_MISC        0x80

/* 0x06 bridge subclasses */
#define PCI_SUBCLASS_BRIDGE_HOST        0x00
#define PCI_SUBCLASS_BRIDGE_ISA         0x01
#define PCI_SUBCLASS_BRIDGE_EISA        0x02
#define PCI_SUBCLASS_BRIDGE_MC          0x03
#define PCI_SUBCLASS_BRIDGE_PCI         0x04
#define PCI_SUBCLASS_BRIDGE_PCMCIA      0x05
#define PCI_SUBCLASS_BRIDGE_MISC        0x80

/* Header */
#define PCI_HEADER_MISC			0x0c
#define PCI_HEADER_MULTIFUNCTION	0x00800000

/* Interrupt configration register */
#define PCI_INTERRUPT_REG		0x3c
#define PCI_INTERRUPT_PIN_MASK          0x0000ff00
#define PCI_INTERRUPT_PIN_EXTRACT(x)    ((((x) & PCI_INTERRUPT_PIN_MASK) >> 8) & 0xff)
#define PCI_INTERRUPT_PIN_NONE          0x00
#define PCI_INTERRUPT_PIN_A             0x01
#define PCI_INTERRUPT_PIN_B             0x02
#define PCI_INTERRUPT_PIN_C             0x03
#define PCI_INTERRUPT_PIN_D             0x04

#define PCI_INTERRUPT_LINE_MASK         0x000000ff
#define PCI_INTERRUPT_LINE_EXTRACT(x)   ((((x) & PCI_INTERRUPT_LINE_MASK) >> 0) & 0xff) 
#define PCI_INTERRUPT_LINE_INSERT(x,v)  (((x) & ~PCI_INTERRUPT_LINE_MASK) | ((v) << 0))
/* Mapping registers */
#define PCI_MAP_REG_START               0x10
#define PCI_MAP_REG_END                 0x28
#define PCI_MAP_ROM_REG			0x30

#define PCI_MAP_MEMORY                  0x00000000
#define PCI_MAP_IO                      0x00000001  

#define PCI_MAP_MEMORY_TYPE_32BIT       0x00000000
#define PCI_MAP_MEMORY_TYPE_32BIT_1M    0x00000002
#define PCI_MAP_MEMORY_TYPE_64BIT       0x00000004
#define PCI_MAP_MEMORY_TYPE_MASK        0x00000006
#define PCI_MAP_MEMORY_CACHABLE         0x00000008
#define PCI_MAP_MEMORY_ADDRESS_MASK     0xfffffff0

#define PCI_MAP_IO_ADDRESS_MASK         0xfffffffc


/* PCI-PCI bridge mapping registers */
#define PCI_PCI_BRIDGE_BUS_REG          0x18
#define PCI_PCI_BRIDGE_IO_REG           0x1c
#define PCI_PCI_BRIDGE_MEM_REG          0x20
#define PCI_PCI_BRIDGE_PMEM_REG         0x24

#define PCI_SUBORDINATE_BUS_MASK        0x00ff0000
#define PCI_SECONDARY_BUS_MASK          0x0000ff00
#define PCI_PRIMARY_BUS_MASK            0x000000ff

#define PCI_SUBORDINATE_BUS_EXTRACT(x)  (((x) >> 16) & 0xff)
#define PCI_SECONDARY_BUS_EXTRACT(x)    (((x) >>  8) & 0xff)
#define PCI_PRIMARY_BUS_EXTRACT(x)      (((x)      ) & 0xff)

#define PCI_PRIMARY_BUS_INSERT(x, y)    (((x) & ~PCI_PRIMARY_BUS_MASK) | ((y) << 0))
#define PCI_SECONDARY_BUS_INSERT(x, y)  (((x) & ~PCI_SECONDARY_BUS_MASK) | ((y) <<  8)) 
#define PCI_SUBORDINATE_BUS_INSERT(x, y) (((x) & ~PCI_SUBORDINATE_BUS_MASK) | (( y) << 16))

#define PCI_PPB_IOBASE_EXTRACT(x)       (((x) << 8) & 0xFF00)
#define PCI_PPB_IOLIMIT_EXTRACT(x)      (((x) << 0) & 0xFF00)

#define PCI_PPB_MEMBASE_EXTRACT(x)      (((x) << 16) & 0xFFFF0000)
#define PCI_PPB_MEMLIMIT_EXTRACT(x)     (((x) <<  0) & 0xFFFF0000)


pciConfigPtr *xf86scanpci(
#if NeedFunctionPrototypes
	int
#endif
);

#ifdef USE_OLD_PCI_CODE
void xf86writepci(
#if NeedFunctionPrototypes
	int,
	int,
	int,
	int,
	int,
	unsigned long,
	unsigned long
#endif
);
#else
void xf86writepci(
#if NeedFunctionPrototypes
	int,
	int,
	int,	
	int,
	int,
	CARD32,
	CARD32
#endif
);
#endif

void xf86cleanpci(
#if NeedFunctionPrototypes
	void
#endif
);

pciTagRec pcibusTag(
#if NeedFunctionPrototypes
	CARD8,
	CARD8,
	CARD8
#endif
);

CARD32 pcibusRead(
#if NeedFunctionPrototypes
	pciTagRec,
	CARD32
#endif
);

#define pciReadLong(tag, addr) pcibusRead(tag, addr)

CARD16 pciReadWord(
#if NeedFunctionPrototypes
	pciTagRec,
	CARD32
#endif
);

CARD8 pciReadByte(
#if NeedFunctionPrototypes
	pciTagRec,
	CARD32
#endif
);

void pcibusWrite(
#if NeedFunctionPrototypes
	pciTagRec,
	CARD32,
	CARD32
#endif
);

#define pciWriteLong(tag, addr, data) pcibusWrite(tag, addr, data)

void pciWriteWord(
#if NeedFunctionPrototypes
	pciTagRec,
	CARD32,
	CARD16
#endif
);

void pciWriteByte(
#if NeedFunctionPrototypes
	pciTagRec,
	CARD32,
	CARD8
#endif
);

#endif /* _XF86_PCI_H */
