/*
 * Copyright: 2015-2020. Stealthy Labs LLC. All Rights Reserved.
 * Original Date: 28th April 2015
 * Modified Date: 16th October 2019
 * Modified Software: libgps_mtk3339
 */
#include <gpsconfig.h>
#include <gpsutils.h>
#ifdef HAVE_ERRNO_H
    #include <errno.h>
#endif
#ifdef HAVE_STDIO_H
    #include <stdio.h>
#endif
#ifdef HAVE_FCNTL_H
    #include <fcntl.h>
#endif
#ifdef HAVE_TERMIOS_H
    #include <termios.h>
#endif
#ifdef HAVE_DECL_STRERROR_R
// do nothing
#else
#warning "strerror_r is reentrant. strerror is not, so removing usage of strerror_r"
#define strerror_r(A,B,C) do { snprintf(B, C "undefined"); } while (0)
#endif

volatile int gpsutils_loglevel = GPSUTILS_LOGLEVEL_INFO;

static time_t gpsutils_timegm(struct tm *_tm)
{
#ifdef HAVE_TIMEGM
    return timegm(_tm);
#else
#warning "Using mktime() instead of timegm() and it is non-reentrant"
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

int gpsutils_set_baudrate(int fd, uint32_t baud_rate)
{
    if (fd < 0)
        return -1;
    speed_t baudspeed = B9600;
    switch (baud_rate) {
    case 1200: baudspeed = B1200; break;
    case 2400: baudspeed = B2400; break;
    case 4800: baudspeed = B4800; break;
    case 9600: baudspeed = B9600; break;
    case 19200: baudspeed = B19200; break;
    case 38400: baudspeed = B38400; break;
    case 57600: baudspeed = B57600; break;
    case 115200: baudspeed = B115200; break;
    default:
        GPSUTILS_WARN("Invalid baud rate %u bps given. Using 9600 bps\n", baud_rate);
        baudspeed = B9600;
        baud_rate = 9600;
        break;
    }
    int rc = 0;
    do {
        char serrbuf[256];
        struct termios opts;
        // get the terminal options on the fd
        rc = tcgetattr(fd, &opts);
        if (rc < 0) {
            int err = errno;
            memset(serrbuf, 0, sizeof(serrbuf));
            strerror_r(err, serrbuf, sizeof(serrbuf) - 1);
            GPSUTILS_ERROR("tcgetattr(%d) error: %s(%d)\n", fd, serrbuf, err);
            break;
        }
        //IGNPAR: ignore characters with parity errors
        opts.c_iflag = IGNPAR;
        //CS8 : 8-bit byte character size mask
        //CLOCAL: ignore modem control lines
        //CREAD: enable receiver
        opts.c_cflag = baudspeed | CS8 | CLOCAL | CREAD;
        opts.c_lflag = 0;
        opts.c_oflag = 0;
        rc = tcflush(fd, TCIOFLUSH);
        if (rc < 0) {
            int err = errno;
            memset(serrbuf, 0, sizeof(serrbuf));
            strerror_r(err, serrbuf, sizeof(serrbuf) - 1);
            GPSUTILS_WARN("tcflush(%d, TCIOFLUSH) error: %s(%d). Ignoring\n", fd, serrbuf, err);
        }
        rc = tcsetattr(fd, TCSANOW, &opts);
        if (rc < 0) {
            int err = errno;
            memset(serrbuf, 0, sizeof(serrbuf));
            strerror_r(err, serrbuf, sizeof(serrbuf) - 1);
            GPSUTILS_ERROR("tcsetattr(%d, TCSANOW) error: %s(%d). Ignoring\n", fd, serrbuf, err);
            break;
        }
        GPSUTILS_DEBUG("Baud rate set to %d on fd %d\n", baud_rate, fd);
    } while (0);
    return rc;
}

int gpsutils_open_device(const char *device, bool non_blocking)
{
    if (!device) {
        GPSUTILS_ERROR("no device address given\n");
        return -1;
    }
    int flags = O_RDONLY | O_NOCTTY | O_CLOEXEC;
    if (non_blocking)
        flags |= O_NDELAY;
    int fd = open(device, flags);
    if (fd < 0) {
        int err = errno;
        char serrbuf[256];
        memset(serrbuf, 0, sizeof(serrbuf));
        strerror_r(err, serrbuf, sizeof(serrbuf) - 1);
        GPSUTILS_ERROR("Failed to open %s: %s(%d)\n", device, serrbuf, err);
        return -1;
    }
    if (gpsutils_set_baudrate(fd, 9600) < 0) {
        GPSUTILS_ERROR("File %s was opened but we were unable to set baud rate of %d. Closing file\n", 
                device, 9600);
        close(fd);
        return -1;
    }
    return fd;
}
