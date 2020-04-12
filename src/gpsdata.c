/*
 * Copyright: 2015-2019. Stealthy Labs LLC. All Rights Reserved.
 * Date: 5th July 2019
 * Software: libgps_mtk3339
 */
#include <gpsdata.h>
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

/** NOTE: anything that is not fsm related goes here **/

const char *gpsdata_msgid_tostring(gpsdata_msgid_t m)
{
    switch (m) {
    case GPSDATA_MSGID_UNSET: return "UNSET";
    case GPSDATA_MSGID_GPGGA: return "GPGGA";
    case GPSDATA_MSGID_GPGSA: return "GPGSA";
    case GPSDATA_MSGID_GPGSV: return "GPGSV";
    case GPSDATA_MSGID_GPRMC: return "GPRMC";
    case GPSDATA_MSGID_GPVTG: return "GPVTG";
    case GPSDATA_MSGID_PGTOP: return "PGTOP";
    case GPSDATA_MSGID_PMTK:  return "PMTK";
    default: break;
    }
    return "INVALID";
}

const char *gpsdata_direction_tostring(gpsdata_direction_t d)
{
    switch (d) {
    case GPSDATA_DIRECTION_NORTH: return "NORTH";
    case GPSDATA_DIRECTION_EAST:  return "EAST";
    case GPSDATA_DIRECTION_WEST:  return "WEST";
    case GPSDATA_DIRECTION_SOUTH: return "SOUTH";
    case GPSDATA_DIRECTION_UNSET:
    default: break;
    }
    return "UNSET";
}

const char *gpsdata_posfix_tostring(gpsdata_posfix_t p)
{
    switch (p) {
    case GPSDATA_POSFIX_NOFIX: return "NOFIX";
    case GPSDATA_POSFIX_GPSFIX: return "GPSFIX";
    case GPSDATA_POSFIX_DGPSFIX: return "DGPSFIX";
    default: break;
    }
    return "UNSET";
}

const char *gpsdata_mode_tostring(gpsdata_mode_t m)
{
    switch (m) {
    case GPSDATA_MODE1_MANUAL: return "MANUAL";
    case GPSDATA_MODE1_AUTOMATIC: return "AUTOMATIC";
    case GPSDATA_MODE2_NOFIX: return "NOFIX";
    case GPSDATA_MODE2_2DFIX: return "2DFIX";
    case GPSDATA_MODE2_3DFIX: return "3DFIX";
    case GPSDATA_MODE_AUTONOMOUS: return "AUTONOMOUS";
    case GPSDATA_MODE_DIFFERENTIAL: return "DIFFERENTIAL";
    case GPSDATA_MODE_ESTIMATED: return "ESTIMATED";
    case GPSDATA_MODE_UNSET:
    default: break;
    }
    return "UNSET";
}

const char *gpsdata_antenna_tostring(gpsdata_antenna_t a)
{
    switch (a) {
    case GPSDATA_ANTENNA_UNSET: return "UNSET";
    case GPSDATA_ANTENNA_SHORTED: return "SHORTED";
    case GPSDATA_ANTENNA_INTERNAL: return "INTERNAL";
    case GPSDATA_ANTENNA_ACTIVE: return "ACTIVE";
    default: break;
    }
    return "UNSET";
}

void gpsdata_list_free(gpsdata_data_t **listp)
{
    if (listp) {
        gpsdata_data_t *item = NULL;
        gpsdata_data_t *tmp = NULL;
        LL_FOREACH_SAFE(*listp, item, tmp) {
            LL_DELETE(*listp, item);
            GPSUTILS_FREE(item);
        }
    }
}

ssize_t gpsdata_list_count(const gpsdata_data_t *listp)
{
    if (listp) {
        ssize_t count = 0;
        const gpsdata_data_t *item = NULL;
        LL_COUNT(listp, item, count);
        return count;
    }
    return -1;
}

void gpsdata_list_dump(const gpsdata_data_t *listp, FILE *fp)
{
    if (listp) {
        const gpsdata_data_t *item = NULL;
        LL_FOREACH(listp, item) {
            gpsdata_dump(item, fp);
        }
    }
}

void gpsdata_initialize(gpsdata_data_t *o)
{
    if (o) {
        o->msgid = GPSDATA_MSGID_UNSET;
        o->latitude.direction = GPSDATA_DIRECTION_UNSET;
        o->latitude.degrees = 0;
        o->latitude.minutes = NAN;
        o->longitude.direction = GPSDATA_DIRECTION_UNSET;
        o->longitude.degrees = 0;
        o->longitude.minutes = NAN;
        o->is_valid_timestamp = false;
        o->timestamp.tv_sec = o->timestamp.tv_usec = 0;
        o->mode = GPSDATA_MODE_UNSET;
        o->posfix = GPSDATA_POSFIX_NOFIX;
        o->num_satellites = 0;
        o->altitude_meters = NAN;
        o->speed_kmph = NAN;
        o->speed_knots = NAN;
        o->course_degrees = NAN;
        o->heading_degrees = NAN;
        o->antenna_status = GPSDATA_ANTENNA_UNSET;
        o->next = NULL;
    }
}

void gpsdata_dump(const gpsdata_data_t *o, FILE *fp)
{
    if (o && fp) {
        const char *msgid_str = gpsdata_msgid_tostring(o->msgid);
        fprintf(fp, "msgid: %s\n", msgid_str);
        if (o->latitude.direction != GPSDATA_DIRECTION_UNSET) {
            fprintf(fp, "latitude: %d° %0.04f\' %s\n",
                   o->latitude.degrees, o->latitude.minutes,
                  gpsdata_direction_tostring(o->latitude.direction));
        }
        if (o->longitude.direction != GPSDATA_DIRECTION_UNSET) {
            fprintf(fp, "longitude: %d° %0.04f\' %s\n",
                   o->longitude.degrees, o->longitude.minutes,
                  gpsdata_direction_tostring(o->longitude.direction));
        }
        if (o->timestamp.tv_sec != 0 || o->timestamp.tv_usec != 0) {
            fprintf(fp, "timestamp(%s): %ld.%06ld\n",
                o->is_valid_timestamp ? "valid" : "incomplete",
                o->timestamp.tv_sec, o->timestamp.tv_usec);
        }
        if (o->mode != GPSDATA_MODE_UNSET) {
            fprintf(fp, "%s mode: %s\n", msgid_str, gpsdata_mode_tostring(o->mode));
        }
        if (o->msgid == GPSDATA_MSGID_GPGGA) {
            fprintf(fp, "posfix: %s num_satellites: %d antenna altitude(m): %0.04f\n",
                    gpsdata_posfix_tostring(o->posfix), o->num_satellites,
                    o->altitude_meters);
        }
        if (!isnan(o->speed_kmph)) {
            fprintf(fp, "speed (km/hr): %0.04f\n", o->speed_kmph);
        }
        if (!isnan(o->speed_knots)) {
            fprintf(fp, "speed (knots): %0.04f\n", o->speed_knots);
        }
        if (!isnan(o->course_degrees)) {
            fprintf(fp, "course (degrees): %0.04f\n", o->course_degrees);
        }
        if (!isnan(o->heading_degrees)) {
            fprintf(fp, "magnetic heading(degrees): %0.04f\n",
                    o->heading_degrees);
        }
        if (o->antenna_status != GPSDATA_ANTENNA_UNSET) {
            fprintf(fp, "antenna status: %s\n",
                    gpsdata_antenna_tostring(o->antenna_status));
        }
        if (o->next) {
            fprintf(fp, "next pointer in linkedin list: %p\n", (const void *)o->next);
        }
        fflush(fp);
    }
}

int gpsdevice_set_baudrate(int fd, uint32_t baud_rate)
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

int gpsdevice_open(const char *device, bool non_blocking)
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
    if (gpsdevice_set_baudrate(fd, 9600) < 0) {
        GPSUTILS_ERROR("File %s was opened but we were unable to set baud rate of %d. Closing file\n", 
                device, 9600);
        close(fd);
        return -1;
    }
    return fd;
}

void gpsdevice_close(int fd)
{
    if (fd < 0)
        return;
    close(fd);
}
