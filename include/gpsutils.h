/*
 * Copyright: 2015-2020. Stealthy Labs LLC. All Rights Reserved.
 * Original Date: 28th April 2015
 * Modified Date: 16th Oct 2019
 * Modified Software: libgps_mtk3339
 */
#ifndef __GPSUTILS_UTILS_H__
#define __GPSUTILS_UTILS_H__

#include <gpsconfig.h>
#ifdef HAVE_FEATURES_H
    #include <features.h>
#endif 
#ifdef HAVE_ERRNO_H
    #include <errno.h>
#endif
#ifdef HAVE_INTTYPES_H
    #include <inttypes.h>
#endif
#ifdef HAVE_STDINT_H
    #include <stdint.h>
#endif
#ifdef HAVE_LIMITS_H
    #include <limits.h>
#endif
#ifdef HAVE_STDIO_H
    #include <stdio.h>
#endif
#ifdef HAVE_STDBOOL_H
#include <stdbool.h>
#endif
#ifdef HAVE_UNISTD_H
    #include <unistd.h>
#endif
#ifdef HAVE_CTYPE_H
    #include <ctype.h>
#endif
#ifdef HAVE_STRING_H
    #include <string.h>
#endif
#ifdef HAVE_STDLIB_H
    #include <stdlib.h>
#endif
#ifdef HAVE_MATH_H
    #include <math.h>
#endif

#undef EXTERN_C_BEGIN
#undef EXTERN_C_END
#ifdef __cplusplus
    #define EXTERN_C_BEGIN extern "C" {
    #define EXTERN_C_END  }
#else
    #define EXTERN_C_BEGIN
    #define EXTERN_C_END
#endif

EXTERN_C_BEGIN

enum {
    GPSUTILS_LOGLEVEL_ERROR = 0,
    GPSUTILS_LOGLEVEL_WARN = 1,
    GPSUTILS_LOGLEVEL_INFO = 2, /* default */
    GPSUTILS_LOGLEVEL_DEBUG = 3
};
extern volatile int gpsutils_loglevel; /* defined in utils.c */

#define GPSUTILS_LOG_PTR stderr
#define GPSUTILS_STRINGIFY(A) #A
#define GPSUTILS_STRINGIFY2(A,B) A ## B
#define GPSUTILS_LOGLEVEL_STRINGIFY(A) GPSUTILS_STRINGIFY2(GPSUTILS_LOGLEVEL_,A)
#define GPSUTILS_LOGLEVEL_SET(A) gpsutils_loglevel = GPSUTILS_LOGLEVEL_STRINGIFY(A)
#define GPSUTILS_LOGLEVEL_IS(A) (gpsutils_loglevel >= GPSUTILS_LOGLEVEL_STRINGIFY(A))
#ifdef NDEBUG
    #define GPSUTILS_LOG_THIS(A,...) \
    do { \
        if (GPSUTILS_LOGLEVEL_IS(A)) { \
            fprintf(GPSUTILS_LOG_PTR, "%s: ", GPSUTILS_STRINGIFY(A)); \
            fprintf(GPSUTILS_LOG_PTR, __VA_ARGS__); \
        } \
    } while (0)
#else  /* NDEBUG not defined */
    #define GPSUTILS_LOG_THIS(A,...) \
    do { \
        if (GPSUTILS_LOGLEVEL_IS(A)) { \
            fprintf(GPSUTILS_LOG_PTR, "%s: [%s:%d] ", GPSUTILS_STRINGIFY(A), __func__, __LINE__); \
            fprintf(GPSUTILS_LOG_PTR, __VA_ARGS__); \
        } \
    } while (0)
#endif
#define GPSUTILS_DEBUG(...) GPSUTILS_LOG_THIS(DEBUG,__VA_ARGS__)
#define GPSUTILS_INFO(...) GPSUTILS_LOG_THIS(INFO, __VA_ARGS__)
#define GPSUTILS_ERROR(...) GPSUTILS_LOG_THIS(ERROR, __VA_ARGS__)
#define GPSUTILS_WARN(...) GPSUTILS_LOG_THIS(WARN, __VA_ARGS__)
#define GPSUTILS_NONE(...) fprintf(GPSUTILS_LOG_PTR, __VA_ARGS__)
#define GPSUTILS_ERROR_NOMEM(A) \
    GPSUTILS_ERROR("Out of memory allocating %zu bytes\n", (size_t)(A))

#ifndef GPSUTILS_FREE
#define GPSUTILS_FREE(A) do { \
        if ((A)) { \
            free((A)); \
            (A) = NULL; \
        } \
    } while (0)
#endif

typedef struct {
    double start;
    double stop;
    double time_taken;
} gpsutils_timer_t;

void gpsutils_timer_start(gpsutils_timer_t *tt);
void gpsutils_timer_stop(gpsutils_timer_t *tt);
void gpsutils_hex_dump(const uint8_t *in, size_t inlen, FILE *fp);
uint8_t gpsutils_hex_parse(const char a);
void gpsutils_string_toupper(char *s);

EXTERN_C_END

#endif /* __GPSUTILS_UTILS_H__ */
