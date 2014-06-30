include ../config.mk

CLEAN_FILES += *.lo *.la *.o *.core
DISTCLEAN_FILES := *.$(LIB_EXTENSION)
ifneq "$(LINK_MODE)" "static"
DISTCLEAN_FILES += *.so
endif
SUBDIRS :=
OBJS :=

all: $(OBJS)

$(OBJS): $(SUBDIRS)
	for i in $(^); do \
		(cd $${i} && $(MAKE)); \
	done

$(SUBDIRS):
	for i in $(@); do \
		tar xzf $${i}.t*gz; \
		cp GNUmakefile.$${i} $${i}/GNUmakefile; \
	done
