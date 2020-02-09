AUTOMAKE_OPTIONS = subdir-objects
ACLOCAL_AMFLAGS = $(ACLOCAL_FLAGS)

lib_LTLIBRARIES=libgps_mtk3339.la
libgps_mtk3339_la_HEADERS=$(top_srcdir)/include/gpsdata.h $(top_srcdir)/include/gpsutils.h $(top_srcdir)/include/gpsconfig.h
libgps_mtk3339_ladir=$(includedir)
libgps_mtk3339_la_SOURCES=$(libgps_mtk3339_la_HEADERS) gpsdata.c gpsutils.c
nodist_libgps_mtk3339_la_SOURCES=gpsparser.c
libgps_mtk3339_la_LDFLAGS=-shared -version-info 0:1:0 -L$(top_builddir) -L$(builddir) -lm
libgps_mtk3339_la_LIBADD=-lm
CLEANFILES=gpsparser.c

#custom rule for ragel
gpsparser.c: gpsparser.rl 
	$(RAGEL) -C -I$(includedir) -o $@ $^

bin_PROGRAMS=
if HAVE_LIBUV
bin_PROGRAMS+=gps_mtk3339_uart
endif
gps_mtk3339_uart_SOURCES=gpsuart.c
gps_mtk3339_uart_CFLAGS=$(LIBUV_CFLAGS)
gps_mtk3339_uart_LDADD=libgps_mtk3339.la $(LIBUV_LIBS)
