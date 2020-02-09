AUTOMAKE_OPTIONS = subdir-objects
ACLOCAL_AMFLAGS = $(ACLOCAL_FLAGS)

lib_LTLIBRARIES=libgps_mt3339.la
libgps_mt3339_la_HEADERS=$(top_srcdir)/include/gpsdata.h $(top_srcdir)/include/gpsutils.h $(top_srcdir)/include/gpsconfig.h
libgps_mt3339_ladir=$(includedir)
libgps_mt3339_la_SOURCES=$(libgps_mt3339_la_HEADERS) gpsdata.c gpsutils.c
nodist_libgps_mt3339_la_SOURCES=gpsparser.c
libgps_mt3339_la_LDFLAGS=-shared -version-info 0:1:0 -L$(top_builddir) -L$(builddir) -lm
libgps_mt3339_la_LIBADD=-lm
CLEANFILES=gpsparser.c

#custom rule for ragel
gpsparser.c: gpsparser.rl 
	/usr/bin/ragel -C -I$(includedir) -o $@ $^

bin_PROGRAMS=
if HAVE_LIBUV
bin_PROGRAMS+=gps_mt3339_uart
endif
gps_mt3339_uart_SOURCES=gpsuart.c
gps_mt3339_uart_CFLAGS=$(LIBUV_CFLAGS)
gps_mt3339_uart_LDADD=libgps_mt3339.la $(LIBUV_LIBS)
