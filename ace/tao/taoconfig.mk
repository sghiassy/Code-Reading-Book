#----------------------------------------------------------------------------
#	taoconfig.mk,v 1.5 2000/02/12 21:52:32 coryan Exp
#
#	Common makefile definitions for all applications
#
#----------------------------------------------------------------------------

ifndef TAO_ROOT
TAO_ROOT = $(ACE_ROOT)/TAO
endif

LDFLAGS := $(patsubst -L$(ACE_ROOT)/ace, -L$(TAO_ROOT)/tao -L$(ACE_ROOT)/ace, $(LDFLAGS))
CPPFLAGS += -I$(TAO_ROOT)
