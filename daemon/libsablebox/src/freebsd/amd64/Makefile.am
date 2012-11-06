SUBDIRS= .
EXTRA_DIST= pink-fbsd32.conf
CLEANFILES= fbsd32-syscalls.master pink-syscallent32.h

BUILT_SOURCES= pink-syscallent32.h

fbsd32-syscalls.master: $(SYSSRC)/compat/freebsd32/syscalls.master
	$(AM_V_GEN)
	$(AM_V_at)cat $< > $@
pink-syscallent32.h: fbsd32-syscalls.master
	$(AM_V_GEN)
	$(AM_V_at)/bin/sh $(SYSSRC)/kern/makesyscalls.sh $< pink-fbsd32.conf
