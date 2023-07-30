## Process this file with automake to produce Makefile.in

SUBDIRS = Sfio_f

AM_CPPFLAGS = -I$(top_srcdir)/lib

noinst_HEADERS = sfhdr.h sfio.h sfio_t.h
noinst_LTLIBRARIES = libsfio_C.la

libsfio_C_la_SOURCES = sfclose.c sfcvt.c \
	sfexcept.c sfextern.c \
	sfflsbuf.c \
	sfmode.c sfnew.c \
	sfnputc.c sfopen.c sfpkrd.c \
	sfraise.c sfrd.c \
	sfscanf.c sfsetbuf.c \
	sfsk.c sfstack.c sfswap.c sfsync.c \
	sftable.c sfprint.c \
	sfvscanf.c sfwr.c sfwrite.c

libsfio_C_la_DEPENDENCIES = \
	Sfio_f/libsfiof_C.la

EXTRA_DIST = README sfio.vcxproj*
