if WITH_CUTTER
noinst_LTLIBRARIES =				\
	test-snip.la				\
	test-string.la
endif

INCLUDES =			\
	-I$(top_srcdir)/include	\
	-I$(top_srcdir)/lib	\
	$(GROONGA_INCLUDEDIR)

AM_CFLAGS = $(GCUTTER_CFLAGS)
AM_LDFLAGS = -module -rpath $(libdir) -avoid-version

LIBS =								\
	$(top_builddir)/lib/libgroonga.la			\
	$(GCUTTER_LIBS)						\
	$(top_builddir)/test/unit/lib/libgrn-test-utils.la

test_snip_la_SOURCES = test-snip.c
test_string_la_SOURCES = test-string.c
