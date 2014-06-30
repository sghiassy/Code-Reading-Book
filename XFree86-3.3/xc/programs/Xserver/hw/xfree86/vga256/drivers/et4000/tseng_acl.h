
/* $XFree86: xc/programs/Xserver/hw/xfree86/vga256/drivers/et4000/tseng_acl.h,v 3.5.2.3 1997/05/24 08:36:15 dawes Exp $ */

#ifndef _TSENG_ACL_H
#define _TSENG_ACL_H

typedef volatile unsigned char *ByteP; 
typedef volatile unsigned short *WordP;
typedef volatile unsigned *LongP;

/*
 * Shortcuts to Tseng memory-mapped accelerator-control registers
 */

extern
ByteP MMU_CONTROL;


extern
ByteP ACL_SUSPEND_TERMINATE,
      ACL_OPERATION_STATE,
      ACL_SYNC_ENABLE,
      ACL_INTERRUPT_MASK,
      ACL_INTERRUPT_STATUS,
      ACL_ACCELERATOR_STATUS;

/* for ET6000: */
#define ACL_6K_CONFIG ACL_SYNC_ENABLE

extern
WordP ACL_X_POSITION,
      ACL_Y_POSITION;

extern
WordP ACL_NQ_X_POSITION,
      ACL_NQ_Y_POSITION;


extern
LongP ACL_PATTERN_ADDRESS,
      ACL_SOURCE_ADDRESS;


extern
WordP ACL_PATTERN_Y_OFFSET,
      ACL_SOURCE_Y_OFFSET,
      ACL_DESTINATION_Y_OFFSET;


extern
ByteP ACL_VIRTUAL_BUS_SIZE,     /* only for w32 and w32i */
      ACL_XY_DIRECTION,
      ACL_PIXEL_DEPTH;          /* only for w32p_rev_A and w32p_rev_B */


extern
ByteP ACL_PATTERN_WRAP,
      ACL_SOURCE_WRAP;


extern
WordP ACL_X_COUNT,
      ACL_Y_COUNT;


extern
ByteP ACL_ROUTING_CONTROL,
      ACL_RELOAD_CONTROL,
      ACL_BACKGROUND_RASTER_OPERATION,
      ACL_FOREGROUND_RASTER_OPERATION;

/* for ET6000: */
#define ACL_MIX_CONTROL ACL_ROUTING_CONTROL
#define ACL_STEPPING_INHIBIT ACL_RELOAD_CONTROL

extern
LongP ACL_DESTINATION_ADDRESS,

      /* only for w32p_rev_A and w32p_rev_B */
      ACL_MIX_ADDRESS;


extern
WordP ACL_MIX_Y_OFFSET,
      ACL_ERROR_TERM,
      ACL_DELTA_MINOR,
      ACL_DELTA_MAJOR;

/* for ET6000 only */
extern
ByteP ACL_POWER_CONTROL;
extern
ByteP ACL_SECONDARY_EDGE;
extern
WordP ACL_SECONDARY_ERROR_TERM,
      ACL_SECONDARY_DELTA_MINOR,
      ACL_SECONDARY_DELTA_MAJOR;


extern int W32OpTable[16];
extern int W32PatternOpTable[16];

extern int W32BresTable[8];

extern long W32ForegroundPing;
extern long W32ForegroundPong;
extern long W32BackgroundPing;
extern long W32BackgroundPong;
extern long W32PatternPing;
extern long W32PatternPong;
extern long W32Mix;

extern LongP MemW32ForegroundPing;
extern LongP MemW32ForegroundPong;
extern LongP MemW32BackgroundPing;
extern LongP MemW32BackgroundPong;
extern LongP MemW32PatternPing;
extern LongP MemW32PatternPong;
extern LongP MemW32Mix;    /* ping-ponging the MIX map is done by XAA */ 

extern LongP CPU2ACLBase;

extern long scratchVidBase;

extern Bool Use_Pci_Retry; /* Often checked value */

/******************************************************************************/

#define WAIT_QUEUE \
{while (*(volatile unsigned char *)ACL_ACCELERATOR_STATUS & 0x1);}

#define WAIT_QUEUE_VERBOSE \
{ int cnt=0; while (*(volatile unsigned char *)ACL_ACCELERATOR_STATUS & 0x1) cnt++; ErrorF("Q%d ",cnt);}

#define MAX_WAIT_CNT 500000

#define WAIT_ACL_VERBOSE DO_WAIT_ACL(ErrorF("W%d ",MAX_WAIT_CNT - cnt))

#define WAIT_ACL DO_WAIT_ACL( {} )
  
#define DO_WAIT_ACL(command) \
  { int cnt = MAX_WAIT_CNT; \
    while (*(volatile unsigned char *)ACL_ACCELERATOR_STATUS & 0x2) \
      if (--cnt < 0) \
      { \
        if (et4000_type < TYPE_ET6000) \
        { \
          *CPU2ACLBase = 0L; /* try unlocking the bus when CPU-to-accel gets stuck */ \
          FLUSH_ACL \
        } \
        ErrorF("WAIT_ACL: timeout.\n"); \
        break; \
      } \
    command; \
  }
  
#define WAIT_XY \
  {while (*(volatile unsigned char *)ACL_ACCELERATOR_STATUS & 0x4);}

#define FLUSH_ACL \
  if (et4000_type > TYPE_ET4000W32I) \
  { \
    *ACL_SUSPEND_TERMINATE = 0x00; \
    *ACL_SUSPEND_TERMINATE = 0x02; \
    *ACL_SUSPEND_TERMINATE = 0x00; \
  }


/***********************************************************************/

void tseng_init_acl();


#endif
