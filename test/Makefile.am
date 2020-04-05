AUTOMAKE_OPTIONS = subdir-objects
ACLOCAL_AMFLAGS = $(ACLOCAL_FLAGS)

built_cflags=-I$(top_builddir)/src/
noinst_PROGRAMS=test_gpsparser test_gpsutils
TESTS=$(noinst_PROGRAMS)
test_gpsparser_SOURCES=gpsparser.c
test_gpsparser_CFLAGS=$(CUNIT_CFLAGS) $(built_cflags)
test_gpsparser_LDADD=$(top_builddir)/src/libgps_mtk3339.la $(CUNIT_LIBS)

test_gpsutils_SOURCES=gpsutils.c
test_gpsutils_CFLAGS=$(CUNIT_CFLAGS) $(built_cflags)
test_gpsutils_LDADD=$(top_builddir)/src/libgps_mtk3339.la $(CUNIT_LIBS)
