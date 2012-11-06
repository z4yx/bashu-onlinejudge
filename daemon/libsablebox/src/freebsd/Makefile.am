SUBDIRS=
if X86_64
SUBDIRS+= amd64
endif # X86_64
SUBDIRS+= .

EXTRA_DIST= pink-syscallent.conf
CLEANFILES= syscalls.master pink-syscallent.h

BUILT_SOURCES= pink-syscallent.h
syscalls.master: $(SYSSRC)/kern/syscalls.master
	$(AM_V_GEN)
	$(AM_V_at)cat $< > $@
pink-syscallent.h: syscalls.master
	$(AM_V_GEN)
	$(AM_V_at)/bin/sh $(SYSSRC)/kern/makesyscalls.sh $< pink-syscallent.conf
