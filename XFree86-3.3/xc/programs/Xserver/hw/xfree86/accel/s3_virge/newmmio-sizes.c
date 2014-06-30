/* $XConsortium: newmmio-sizes.c /main/2 1996/10/25 11:33:52 kaleb $ */
/* compile with
   cc -I../../common -I../../../../../../exports/include/X11 newmmio-sizes.c
   */

/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/s3_virge/newmmio-sizes.c,v 3.2.2.2 1997/05/11 02:56:13 dawes Exp $ */

#include "regs3v.h"

#define PS(set)  printf(#set "_regs \t\t = %04x\n", &((mmtr)0)->set ## _regs)
#define P(set,reg)  printf(#set "_regs.regs." #reg " \t = %04x\n", &((mmtr)0)->set ## _regs.regs.reg)

main()
{
   PS(pci);
   PS(streams);
   PS(memport);
   PS(v3b);
   PS(v3c);
   PS(v3d);
   PS(subsys);
   PS(dma);
   PS(colpatt);
   PS(bltfill);
   PS(line);
   PS(polyfill);
   PS(line3d);
   PS(triangle3d);
   PS(lbp);

   printf("\n");
   printf("\n");

   P(bltfill,src_base);
   P(bltfill,dest_base);
   P(bltfill,clip_l_r);
   P(bltfill,clip_t_b);
   P(bltfill,dest_src_str);
   P(bltfill,mono_pat0);
   P(bltfill,mono_pat1);
   P(bltfill,pat_bg_clr);
   P(bltfill,pat_fg_clr);
   P(bltfill,src_bg_clr);
   P(bltfill,src_fg_clr);
   P(bltfill,cmd_set);
   P(bltfill,rwidth_height);
   P(bltfill,rsrc_xy);
   P(bltfill,rdest_xy);

   printf("\n");
   P(line,src_base);
   P(line,dest_base);
   P(line,clip_l_r);
   P(line,clip_t_b);
   P(line,dest_src_str);
   P(line,pat_fg_clr);
   P(line,cmd_set);
   P(line,lxend0_end1);
   P(line,ldx);
   P(line,lxstart);
   P(line,lystart);
   P(line,lycnt);

   printf("\n");
   P(polyfill,src_base);
   P(polyfill,dest_base);
   P(polyfill,clip_l_r);
   P(polyfill,clip_t_b);
   P(polyfill,dest_src_str);
   P(polyfill,mono_pat0);
   P(polyfill,mono_pat1);
   P(polyfill,pat_bg_clr);
   P(polyfill,pat_fg_clr);
   P(polyfill,cmd_set);
   P(polyfill,prdx);
   P(polyfill,prxstart);
   P(polyfill,pldx);
   P(polyfill,plxstart);
   P(polyfill,pystart);
   P(polyfill,pycnt);

   printf("\n");
   P(line3d,z_base);
   P(line3d,dest_base);
   P(line3d,clip_l_r);
   P(line3d,clip_t_b);
   P(line3d,dest_src_str);
   P(line3d,z_stride);
   P(line3d,fog_clr);
   P(line3d,cmd_set);
   P(line3d,dgdy_dbdy);
   P(line3d,dady_drdy);
   P(line3d,gs_bs);
   P(line3d,as_rs);
   P(line3d,dz);
   P(line3d,zstart);
   P(line3d,xend0_end1);
   P(line3d,dx);
   P(line3d,xstart);
   P(line3d,ystart);
   P(line3d,ycnt);

   printf("\n");
   P(triangle3d,z_base);
   P(triangle3d,dest_base);
   P(triangle3d,clip_l_r);
   P(triangle3d,clip_t_b);
   P(triangle3d,dest_src_str);
   P(triangle3d,z_stride);
   P(triangle3d,tex_base);
   P(triangle3d,tex_bdr_clr);
   P(triangle3d,fog_clr);
   P(triangle3d,color0);
   P(triangle3d,color1);
   P(triangle3d,cmd_set);
   P(triangle3d,bv);
   P(triangle3d,bu);
   P(triangle3d,dwdx);
   P(triangle3d,dwdy);
   P(triangle3d,ws);
   P(triangle3d,dddx);
   P(triangle3d,dvdx);
   P(triangle3d,dudx);
   P(triangle3d,dddy);
   P(triangle3d,dvdy);
   P(triangle3d,dudy);
   P(triangle3d,ds);
   P(triangle3d,vs);
   P(triangle3d,us);
   P(triangle3d,dgdx_dbdx);
   P(triangle3d,dadx_drdx);
   P(triangle3d,dgdy_dbdy);
   P(triangle3d,dady_drdy);
   P(triangle3d,gs_bs);
   P(triangle3d,as_rs);
   P(triangle3d,dzdx);
   P(triangle3d,dzdy);
   P(triangle3d,zs);
   P(triangle3d,dxdy12);
   P(triangle3d,xend12);
   P(triangle3d,dxdy01);
   P(triangle3d,xend01);
   P(triangle3d,dxdy02);
   P(triangle3d,xstart02);
   P(triangle3d,ystart);
   P(triangle3d,y01_y12);
   printf("%08x\n",CMD_RECT |
/*MIX_MONO_PATT |*/
                        INC_Y | INC_X | DRAW   /*s3alu[pGC->alu]*/ );
}
