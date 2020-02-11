/*
 * Copyright: 2015-2020. Stealthy Labs LLC. All Rights Reserved.
 * Original Date: 28th April 2015
 * Modified Date: 16th October 2019
 * Modified Software: libgps_mtk3339
 */
#include <gpsutils.h>
#ifdef TIME_WITH_SYS_TIME
# include <sys/time.h>
# include <time.h>
#else
# ifdef HAVE_SYS_TIME_H
#  include <sys/time.h>
# else
#  include <time.h>
# endif
#endif

volatile int gpsutils_loglevel = GPSUTILS_LOGLEVEL_INFO;

void gpsutils_timer_start(gpsutils_timer_t *tt)
{
    if (tt) {
        struct timeval tv = { 0 };
        gettimeofday(&tv, NULL);
        tt->start = ((double)tv.tv_sec + ((double)tv.tv_usec * 1e-6));
        tt->stop = NAN;
        tt->time_taken = NAN;
    }
}

void gpsutils_timer_stop(gpsutils_timer_t *tt)
{
    if (tt) {
        struct timeval tv = { 0 };
        gettimeofday(&tv, NULL);
        tt->stop = ((double)tv.tv_sec + ((double)tv.tv_usec * 1e-6));
        tt->time_taken = tt->stop - tt->start;
    }
}

void gpsutils_hex_dump(const uint8_t *in, size_t inlen, FILE *fp)
{
    if (!in || inlen == 0 || !fp)
        return;
    size_t rows = inlen / 16;
    if (inlen % 16 > 0)
        rows++;
    for (size_t i = 0; i < rows; ++i) {
        fprintf(fp, "%08zX ", i * 16);
        for (size_t j = 0; j < 16; ++j) {
            if ((i * 16 + j) < inlen)
                fprintf(fp, "%02X ", in[i * 16 + j]);
            else
                fprintf(fp, "   ");
        }
        fprintf(fp, " |");
        for (size_t j = 0; j < 16; ++j) {
            if ((i * 16 + j) < inlen) {
                uint8_t ch = in[i * 16 + j];
                if (ch < 0x20 || ch > 0x7E || !isprint(ch)) {
                    ch = '.';
                }
                fprintf(fp, "%c", ch);
            }
        }
        fprintf(fp, "|\n");
    }
}

static const uint8_t gpsutils_hex_parse_lut[256] = {
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
     0,  1,  2,  3,  4,  5,  6,  7,  8,  9, -1, -1, -1, -1, -1, -1,
    -1, 10, 11, 12, 13, 14, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, 10, 11, 12, 13, 14, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
};

uint8_t gpsutils_hex_parse(const char a)
{
    return gpsutils_hex_parse_lut[(uint8_t)a];
}

void gpsutils_string_toupper(char *s)
{
    if (s) {
        size_t len = strlen(s);
        for (size_t i = 0; i < len; ++i) {
            s[i] = toupper(s[i]);
        }
    }
}
