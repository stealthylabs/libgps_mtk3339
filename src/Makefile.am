AUTOMAKE_OPTIONS = subdir-objects
ACLOCAL_AMFLAGS = $(ACLOCAL_FLAGS)

thirdparty_includedir=$(top_srcdir)/3rdParty/uthash/include/
lib_LTLIBRARIES=libgps_mtk3339.la
# install these headers
libgps_mtk3339_la_HEADERS=$(top_srcdir)/include/gpsdata.h \
						  $(top_srcdir)/include/gpsutils.h \
						  $(top_srcdir)/include/gpsconfig.h \
						  gps_utlist.h

libgps_mtk3339_ladir=$(includedir)
libgps_mtk3339_la_SOURCES=$(libgps_mtk3339_la_HEADERS) gpsdata.c gpsutils.c
nodist_libgps_mtk3339_la_SOURCES=gpsparser.c
libgps_mtk3339_la_LDFLAGS=-shared -version-info 0:1:0 -L$(top_builddir) -L$(builddir) -lm
libgps_mtk3339_la_LIBADD=-lm
BUILT_SOURCES=gpsparser.c gps_utlist.h
CLEANFILES=gpsparser.c gps_utlist.h

#custom rule for ragel
gpsparser.c: gpsparser.rl
	$(RAGEL) -C -I$(includedir) $(THIRDPARTY_INCLUDES) -o $@ $^

gps_utlist.h: $(thirdparty_includedir)/utlist.h
	/bin/cp -v $^ $@

noinst_PROGRAMS=
if HAVE_LIBUV
noinst_PROGRAMS+=libuv_uart_gps
libuv_uart_gps_SOURCES=libuv_uart.c
libuv_uart_gps_CFLAGS=$(LIBUV_CFLAGS)
libuv_uart_gps_LDADD=libgps_mtk3339.la $(LIBUV_LIBS)
endif

if HAVE_LIBEV
noinst_PROGRAMS+=libev_uart_gps
libev_uart_gps_SOURCES=libev_uart.c
libev_uart_gps_CFLAGS=$(LIBEV_CFLAGS)
libev_uart_gps_LDADD=libgps_mtk3339.la $(LIBEV_LIBS)
endif
