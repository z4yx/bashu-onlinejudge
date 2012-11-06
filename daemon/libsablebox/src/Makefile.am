SUBDIRS=
if FREEBSD
SUBDIRS+= freebsd
endif # FREEBSD

if LINUX
SUBDIRS+= linux
endif # LINUX
SUBDIRS+= . easy

CLEANFILES=
AM_CFLAGS= \
	   -I$(top_srcdir)/include \
	   @PINKTRACE_CFLAGS@
SPARSE=sparse
SPARSE_CPPFLAGS= $(DEFAULT_INCLUDES) \
		 -Wbitwise -Wcast-to-as -Wdefault-bitfield-sign \
		 -Wparen-string -Wptr-subtraction-blows \
		 -Wreturn-void -Wshadow -Wtypesign -Wundef \
		 -I$(shell $(CC) -print-file-name=include) \
		 -I$(shell $(CC) -print-file-name=include-fixed)

lib_LTLIBRARIES = libpinktrace_@PINKTRACE_PC_SLOT@.la

libpinktrace_@PINKTRACE_PC_SLOT@_la_SOURCES= \
					     pink-bitness.c \
					     pink-decode-array.c \
					     pink-trace-internal.c
libpinktrace_@PINKTRACE_PC_SLOT@_la_LDFLAGS= -version-info @VERSION_LIB_CURRENT@:@VERSION_LIB_REVISION@:0

# Operating system specific sources
if FREEBSD
libpinktrace_@PINKTRACE_PC_SLOT@_la_SOURCES+= \
					      pink-freebsd-event.c \
					      pink-freebsd-trace.c \
					      pink-freebsd-util.c

# Platform specific sources
if I386
SPARSE_CPPFLAGS+= -D__i386__=1
libpinktrace_@PINKTRACE_PC_SLOT@_la_SOURCES+= \
					      pink-freebsd-name-i386.c \
					      pink-freebsd-trace-i386.c
endif # I386
if X86_64
SPARSE_CPPFLAGS+= -D__x86_64__=1
libpinktrace_@PINKTRACE_PC_SLOT@_la_SOURCES+= \
					      pink-freebsd-name-amd64.c \
					      pink-freebsd-trace-amd64.c
endif # X86_64
endif # FREEBSD

if LINUX
libpinktrace_@PINKTRACE_PC_SLOT@_la_SOURCES+= \
					      pink-linux-event.c \
					      pink-linux-socket.c \
					      pink-linux-trace.c \
					      pink-linux-util.c

# Platform specific sources
if I386
SPARSE_CPPFLAGS+= -D__i386__=1
libpinktrace_@PINKTRACE_PC_SLOT@_la_SOURCES+= \
					      pink-linux-name-x86.c \
					      pink-linux-trace-x86.c
endif # I386

if X86_64
SPARSE_CPPFLAGS+= -D__x86_64__=1
libpinktrace_@PINKTRACE_PC_SLOT@_la_SOURCES+= \
					      pink-linux-name-x86_64.c \
					      pink-linux-trace-x86_64.c
endif # X86_64

if IA64
SPARSE_CPPFLAGS+= -D__ia64__=1
libpinktrace_@PINKTRACE_PC_SLOT@_la_SOURCES+= \
					      pink-linux-name-ia64.c \
					      pink-linux-trace-ia64.c
endif # IA64

if POWERPC
SPARSE_CPPFLAGS+= -D__powerpc__=1
libpinktrace_@PINKTRACE_PC_SLOT@_la_SOURCES+= \
					      pink-linux-name-powerpc.c \
					      pink-linux-trace-powerpc.c
endif # POWERPC

if POWERPC64
SPARSE_CPPFLAGS+= -D__powerpc64__=1
libpinktrace_@PINKTRACE_PC_SLOT@_la_SOURCES+= \
					      pink-linux-name-powerpc.c \
					      pink-linux-trace-powerpc.c
endif # POWERPC64

if ARM
SPARSE_CPPFLAGS+= -D__arm__=1
libpinktrace_@PINKTRACE_PC_SLOT@_la_SOURCES+= \
					      pink-linux-name-arm.c \
					      pink-linux-trace-arm.c
endif # ARM

endif # LINUX

sparse-check:
	for src in $(libpinktrace_@PINKTRACE_PC_SLOT@_la_SOURCES); \
	do \
		$(SPARSE) $(DEFS) $(SPARSE_CPPFLAGS) $(AM_CFLAGS) $$src || exit 1; \
	done

.PHONY: sparse-check
