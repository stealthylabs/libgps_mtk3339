/*
 * Copyright: 2015-2020. Stealthy Labs LLC. All Rights Reserved.
 * Original Date: 28th April 2015
 * Modified Date: 16th October 2019
 * Modified Software: libgps_mtk3339
 */
#include <gpsutils.h>

volatile int gpsutils_loglevel = GPSUTILS_LOGLEVEL_INFO;

static time_t gpsutils_timegm(struct tm *_tm)
{
#ifdef HAVE_TIMEGM
    return timegm(_tm);
#else
    char *old_tz = getenv("TZ");
    setenv("TZ", "UTC", 1);
    time_t val = mktime(_tm);
    if (old_tz) {
        setenv("TZ", old_tz, 1);
    } else {
        unsetenv("TZ");
    }
    return val;
#endif
}

int gpsutils_get_timeval(const struct tm *tm1, uint32_t millisecs, struct timeval *tv)
{
    if (tm1 && tv) {
        // the mktime/timegm functions may modify the tm structure so we copy it
        struct tm tm2 = { 0 };
        memcpy(&tm2, tm1, sizeof(struct tm));
        tv->tv_sec = gpsutils_timegm(&tm2);
        if (millisecs < 1000) {
            tv->tv_usec = millisecs * 1000;
        } else {
            tv->tv_sec += millisecs / 1000;
            tv->tv_usec = (millisecs % 1000) * 1000;
        }
        return 0;
    }
    return -1;
}

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

void gpsutils_hex_dump(const uint8_t *inp, size_t inlen, FILE *fp)
{
    if (!inp || inlen == 0 || !fp)
        return;
    size_t rows = inlen / 16;
    if ((inlen % 16) > 0)
        rows++;
    for (size_t i = 0; i < rows; ++i) {
        fprintf(fp, "%08zX ", i * 16);
        for (size_t j = 0; j < 16; ++j) {
            if ((i * 16 + j) < inlen) {
                uint8_t ch = inp[i * 16 + j];
                fprintf(fp, "%02X ", ch);
            } else {
                fprintf(fp, "   ");
            }
        }
        fprintf(fp, " |");
        for (size_t j = 0; j < 16; ++j) {
            if ((i * 16 + j) < inlen) {
                uint8_t ch = inp[i * 16 + j];
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
