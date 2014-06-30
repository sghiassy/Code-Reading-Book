#----------------------------------------------------------------------------
#	Makefile,v 4.7 1999/07/27 16:11:37 levine Exp
#
#	Makefile for the System V IPC directory
#----------------------------------------------------------------------------

#----------------------------------------------------------------------------
#	Local macros
#----------------------------------------------------------------------------

INFO  = README

DIRS  = SV_Message_Queues \
	SV_Semaphores \
	SV_Shared_Memory

#----------------------------------------------------------------------------
#	Include macros and targets
#----------------------------------------------------------------------------

include	$(ACE_ROOT)/include/makeinclude/wrapper_macros.GNU
include	$(ACE_ROOT)/include/makeinclude/macros.GNU

ifdef VXWORKS
  all clean realclean:
	@echo The ACE_wrappers/examples/System_V_IPC/ examples won\'t \
              run or build for VxWorks.
else
  include	$(ACE_ROOT)/include/makeinclude/rules.common.GNU
  include	$(ACE_ROOT)/include/makeinclude/rules.nested.GNU
  include	$(ACE_ROOT)/include/makeinclude/rules.nolocal.GNU
endif # VXWORKS
