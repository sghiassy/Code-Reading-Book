#
# tao_targets.GNU,v 1.1 1999/08/25 17:10:22 coryan Exp
#

TAO_DIRS=ace \
	apps/gperf/src \
	TAO/tao \
	TAO/TAO_IDL

tao:
ifeq (Windows,$(findstring Windows,$(OS)))
	@cmd /c "FOR /D %i IN ($(TAO_DIRS)) DO $(MAKE) ACE_COMPONENTS=FOR_TAO -C %i"
else  # ! Windows
	@for dir in $(TAO_DIRS); do $(MAKE) ACE_COMPONENTS=FOR_TAO -C $$dir $(ACE_NESTED_COMMAND); done
endif # ! Windows
