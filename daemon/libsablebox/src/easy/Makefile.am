SUBDIRS= .

CLEANFILES=
AM_CFLAGS= \
	   -I$(top_srcdir)/include \
	   @PINKTRACE_CFLAGS@

easy_SRCS= \
	   pink-easy-attach.c \
	   pink-easy-call.c \
	   pink-easy-context.c \
	   pink-easy-exec.c \
	   pink-easy-error.c \
	   pink-easy-internal.c \
	   pink-easy-loop.c \
	   pink-easy-process.c
EXTRA_DIST= $(easy_SRCS)

if WANT_EASY
lib_LTLIBRARIES= libpinktrace_easy_@PINKTRACE_PC_SLOT@.la
libpinktrace_easy_@PINKTRACE_PC_SLOT@_la_SOURCES= $(easy_SRCS)
endif # WANT_EASY
