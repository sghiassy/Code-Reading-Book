#----------------------------------------------------------------------------
#       rules.nested.GNU,v 4.9 2000/03/03 17:55:14 levine Exp
#
#       Nested directory targets makefile definitions
#----------------------------------------------------------------------------

ifeq ($(MAKEFILE),)
  MAKEFILE = Makefile
endif # ! MAKEFILE

$(TARGETS_NESTED):
ifneq ($(DIRS),)
  ifeq (Windows,$(findstring Windows,$(OS)))
	@cmd /c "FOR /D %i IN ($(DIRS)) DO $(MAKE) -C %i $(@:.nested=)"
  else  # ! Windows
	@for dir in $(DIRS); do \
        $(MAKE) -f $(MAKEFILE) -C $$dir $(@:.nested=) $(ACE_NESTED_COMMAND); \
        done
  endif # ! Windows
endif # DIRS
