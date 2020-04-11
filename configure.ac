########################################
##COPYRIGHT: 2020. Stealthy Labs LLC
##DATE: 2020-01-28
##LICENSE: Refer LICENSE file.
########################################
# initially generated using autoscan and then modified manually
#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([libgps_mtk3339], [0.1], [github@stealthylabs.com])
AM_INIT_AUTOMAKE
AC_CONFIG_SRCDIR([src/gpsdata.c])
AM_CONFIG_HEADER(include/gpsconfig.h)
AC_CONFIG_MACRO_DIRS([m4])

# Checks for programs.
## store user given CFLAGS
USER_CFLAGS=$CFLAGS
AC_PROG_CC
AC_PROG_INSTALL
LT_INIT([disable-static pic-only])
CFLAGS=$USER_CFLAGS

# shared library generation stuff
AC_ENABLE_SHARED
AC_DISABLE_STATIC

# Checks for libraries.

# Checks for header files.
AC_HEADER_SYS_WAIT
AC_HEADER_TIME
AC_HEADER_STDC
AC_CHECK_HEADERS([ errno.h features.h fcntl.h inttypes.h limits.h])
AC_CHECK_HEADERS([unistd.h stdio.h ctype.h termios.h math.h libgen.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T
AC_TYPE_UINT64_T
AC_TYPE_INT64_T
AC_TYPE_UINT32_T
AC_TYPE_INT32_T
AC_TYPE_UINT16_T
AC_TYPE_INT16_T
AC_TYPE_UINT8_T
AC_TYPE_INT8_T
AC_TYPE_UINTPTR_T
AC_TYPE_INTPTR_T
AC_HEADER_STDBOOL
AC_C_INLINE

# Checks for library functions.
AC_FUNC_STRERROR_R
AC_CHECK_FUNCS([memset strdup memcpy calloc ioctl])
AC_CHECK_FUNCS_ONCE([timegm])

## debug test
AC_MSG_CHECKING([whether to build with debug information])
AC_ARG_ENABLE([debug],
              [AS_HELP_STRING([--enable-debug], [enable debug data generation (def=no)])],
              [debugit="$enableval"],
              [debugit=no])
AC_MSG_RESULT([$debugit])
AM_CFLAGS="-Wall -Wextra -Wno-uninitialized -fPIC -pedantic -posix -std=c11 -D_DEFAULT_SOURCE -D_REENTRANT -Wno-unused-const-variable"

if test x"$debugit" = x"yes"; then
    AC_DEFINE([_DEBUG],[],[Debug Mode])
    dnl Remove all optimization flags from CFLAGS
    changequote({,})
    CFLAGS=`echo "$CFLAGS" | sed -e 's/-O[0-9s]*//g' | sed -e 's/-g[0-9]*//g'`
    changequote([,])
    echo "CFLAGS=$CFLAGS"

    AM_CFLAGS="$AM_CFLAGS -g -Werror -O0"
else
    AC_DEFINE([NDEBUG],[],[No Debug Mode])
    AC_DEFINE([NDEBUG],[],[No Debug Mode])
    AM_CFLAGS="$AM_CFLAGS"
fi

AC_SUBST([AM_CFLAGS])
AC_SUBST([AM_LDFLAGS])

AC_PATH_PROG([RAGEL], [ragel])
AS_IF([test "x$RAGEL" = "x"], [AC_MSG_ERROR([Please install ragel 6.10])])

PKG_CHECK_MODULES([CUNIT], [cunit], [AC_DEFINE([HAVE_CUNIT], [1], [Use CUnit])])
AC_SUBST([CUNIT_CFLAGS])
AC_SUBST([CUNIT_LIBS])

AC_ARG_WITH([libev],
              [AS_HELP_STRING([--without-libev], [build examples without libev support @<:@default=yes@:>@])],
              [with_libev=no],
              [with_libev=yes])
AS_IF([test "x$with_libev" = "xyes"], [
    AC_MSG_CHECKING([if libev is installed])
    PKG_CHECK_MODULES([LIBEV], [libev], 
                      [
                       use_libev=true
                       AC_DEFINE([HAVE_EV_H], [1], [Have libev.h])
                      ],
                      [
                       AC_CHECK_HEADERS([ev.h], [use_libev=true], [use_libev=false])
                       AC_CHECK_LIB([ev], [ev_run], [use_libev=true], [use_libev=false])
                       LIBEV_LIBS="-lev"
                       echo "Setting LIBEV_LIBS to $LIBEV_LIBS"
                      ]
    )
])
AM_CONDITIONAL([HAVE_LIBEV], [test "x$use_libev" = "xtrue"])
AC_SUBST([LIBEV_CFLAGS])
AC_SUBST([LIBEV_LIBS])

AC_CONFIG_FILES([Makefile src/Makefile test/Makefile])
AC_OUTPUT
