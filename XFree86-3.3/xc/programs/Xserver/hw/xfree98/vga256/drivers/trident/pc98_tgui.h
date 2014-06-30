/* $XConsortium: pc98_tgui.h /main/2 1996/10/25 10:35:23 kaleb $ */




/* $XFree86: xc/programs/Xserver/hw/xfree98/vga256/drivers/trident/pc98_tgui.h,v 3.3 1996/12/28 08:20:02 dawes Exp $ */

typedef enum { PC98Unknown , PC98PCIBus , PC98CBus } PC98BusType;
typedef enum { PC98PAGE , PC98LINEAR , PC98BOTH } PC98VramType;
typedef enum { PC98NoExist , PC98NEC96xx , PC98NEC9320
		 , PC98DRV96xx } PC98TGUiType;

typedef struct {
  char          info[80];
  PC98TGUiType  TGUiType;
  PC98BusType   BusType;
  PC98VramType  VramType;
  unsigned long pciBase;
  unsigned long vgaBase;
  unsigned long mmioBase;
  unsigned char DRAMspeed;
  int MCLK;
  int Bpp_Clocks[4];
  void (*crtsw)(short);
  Bool (*test)(void);
  Bool (*init)(void);
} PC98TGUiTable;
