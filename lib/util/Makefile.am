AM_CPPFLAGS = -I$(top_srcdir)/lib

noinst_HEADERS = \
  exit.h \
  gv_fopen.h \
  overflow.h \
  prisize_t.h \
  strcasecmp.h \
  streq.h \
  unreachable.h \
  unused.h
noinst_LTLIBRARIES = libutil_C.la

libutil_C_la_SOURCES = gv_fopen.c
libutil_C_la_CPPFLAGS = $(AM_CPPFLAGS)

EXTRA_DIST = README
