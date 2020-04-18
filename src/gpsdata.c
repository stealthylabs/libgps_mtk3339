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
    case GPSDATA_MSGID_GPGLL: return "GPGLL";
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
            if (item) {
                GPSUTILS_FREE(item->fwinfo.firmware);
                GPSUTILS_FREE(item->fwinfo.build_id);
                GPSUTILS_FREE(item->fwinfo.chip_name);
                GPSUTILS_FREE(item->fwinfo.chip_version);
            }
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
        o->fwinfo.firmware = NULL;
        o->fwinfo.build_id = NULL;
        o->fwinfo.chip_name = NULL;
        o->fwinfo.chip_version = NULL;
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
            fprintf(fp, "posfix: %s num_satellites: %d antenna altitude: %0.04fm\n",
                    gpsdata_posfix_tostring(o->posfix), o->num_satellites,
                    o->altitude_meters);
        }
        if (!isnan(o->speed_kmph)) {
            fprintf(fp, "speed: %0.04f km/hr\n", o->speed_kmph);
        }
        if (!isnan(o->speed_knots)) {
            fprintf(fp, "speed: %0.04f knots\n", o->speed_knots);
        }
        if (!isnan(o->course_degrees)) {
            fprintf(fp, "course: %0.04f°\n", o->course_degrees);
        }
        if (!isnan(o->heading_degrees)) {
            fprintf(fp, "magnetic heading: %0.04f°\n",
                    o->heading_degrees);
        }
        if (o->antenna_status != GPSDATA_ANTENNA_UNSET) {
            fprintf(fp, "antenna status: %s\n",
                    gpsdata_antenna_tostring(o->antenna_status));
        }
        if (o->msgid == GPSDATA_MSGID_PMTK && o->fwinfo.firmware != NULL) {
            fprintf(fp, "Firmware: %s Build ID: %s Chip Name: %s Version: %s\n",
                    o->fwinfo.firmware, o->fwinfo.build_id, o->fwinfo.chip_name,
                    o->fwinfo.chip_version);
        }
        if (o->next) {
            fprintf(fp, "next pointer in linkedin list: %p\n", (const void *)o->next);
        }
        fflush(fp);
    }
}

int gpsdevice_open(const char *device, bool non_blocking)
{
    if (!device) {
        GPSUTILS_ERROR("no device address given\n");
        return -1;
    }
    int flags = O_RDWR | O_NOCTTY | O_CLOEXEC;
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
        gpsdevice_close(fd);
        return -1;
    }
    if (gpsdevice_set_fix_interval(fd, 1000) < 0) {
        GPSUTILS_WARN("File %s was opened but we were unable to set fix interval to 1000 milliseconds\n", device);
    }
    if (gpsdevice_set_enabled(fd, false, false, false) < 0) {
        GPSUTILS_WARN("File %s was opened but we were unable to set default messages to GPRMC and GPGGA\n", device);
    }
    return fd;
}

void gpsdevice_close(int fd)
{
    if (fd < 0)
        return;
    if (close(fd) < 0) {
        int err = errno;
        char serrbuf[256];
        memset(serrbuf, 0, sizeof(serrbuf));
        strerror_r(err, serrbuf, sizeof(serrbuf) - 1);
        GPSUTILS_ERROR("Failed to close %d: %s(%d)\n", fd, serrbuf, err);
    }
}

//NULL terminated messages are sent
int gpsdevice_send_message(int fd, const char *msg)
{
    if (fd < 0 || !msg)
        return -1;
    size_t len = strlen(msg);
    if (len == 0)
            return -1;
    ssize_t nb = write(fd, msg, len);
    if (nb > 0) {
        if ((size_t)nb == len) {
            GPSUTILS_DEBUG("Wrote a message of %zu bytes: %s\n", nb, msg);
        } else {
            GPSUTILS_WARN("Wrote a message of %zu bytes, expected to write %zu bytes. Message: %s\n", nb, len, msg);
        }
        return 0;
    } else {
        int err = errno;
        char serrbuf[256];
        memset(serrbuf, 0, sizeof(serrbuf));
        strerror_r(err, serrbuf, sizeof(serrbuf) - 1);
        GPSUTILS_ERROR("Failed to write message to fd: %d. Error: %s(%d)\n",
                fd, serrbuf, err);
        return -1;
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
        char buf1[32];
        char buf2[64];
        memset(buf1, 0, sizeof(buf1));
        memset(buf2, 0, sizeof(buf2));
        snprintf(buf1, sizeof(buf1) - 1, "PMTK251,%d", baud_rate);
        // no error check here because buf1 is not null and strlen(buf1) is > 0
        snprintf(buf2, sizeof(buf2) - 1, "$%s*%02X\r\n", buf1,
                gpsutils_checksum(buf1, -1));
        rc = gpsdevice_send_message(fd, buf2);
    } while (0);
    return rc;
}

int gpsdevice_set_standby(int fd)
{
    const char *msg = "$PMTK161,0*28\r\n";
    return gpsdevice_send_message(fd, msg);
}

int gpsdevice_set_restart(int fd, gpsdevice_restart_type_t rt)
{
    const char *msg;
    switch (rt) {
    case GPSDEVICE_RESTART_COLD: msg = "$PMTK103*30\r\n"; break;
    case GPSDEVICE_RESTART_WARM: msg = "$PMTK102*31\r\n"; break;
    case GPSDEVICE_RESTART_HOT: msg = "$PMTK101*32\r\n"; break;
    case GPSDEVICE_RESTART_FACTORY: msg = "$PMTK104*37\r\n"; break;
    /* default cold start */
    default: msg = "$PMTK103*30\r\n"; break;
    }
    return gpsdevice_send_message(fd, msg);
}

int gpsdevice_request_firmware_info(int fd)
{
    const char *msg = "$PMTK605*31\r\n";
    return gpsdevice_send_message(fd, msg);
}

int gpsdevice_request_antenna_status(int fd, bool once, bool periodic_enable)
{
    // as per https://forum.trenz-electronic.de/index.php?topic=273.0
    // we can send this below message to query the antenna status.
    // this message is also mentioned in the datasheet
    //"$PGTOP,11,3*6F\r\n";
    //Adafruit however uses $PGCMD,33,1*6C to enable and $PGCMD,33,0*6D to
    //disable periodic messages. Both work!
    if (once) {
        int rc = gpsdevice_send_message(fd, "$PGTOP,11,3*6F\r\n");
        if (rc < 0)
            return rc;
    }
    return gpsdevice_send_message(fd, periodic_enable ? "$PGCMD,33,1*6C\r\n" : "$PGCMD,33,0*6D\r\n");
}

int gpsdevice_set_speed_threshold(int fd, float speed)
{
    if (fd < 0)
        return -1;
    int val = truncf(speed * 10);
    const char *buf1 = NULL;
    // multiply by 10 to avoid floating point comparisons in jump table
    switch (val) {
    case 0: // 0 m/s
        buf1 = "PMTK397,0";
        break;
    case 2: // 0.2 m/s
        buf1 = "PMTK397,0.2";
        break;
    case 4: // 0.4 m/s
        buf1 = "PMTK397,0.4";
        break;
    case 6: // 0.6 m/s
        buf1 = "PMTK397,0.6";
        break;
    case 8: // 0.8 m/s
        buf1 = "PMTK397,0.8";
        break;
    case 10: // 1.0 m/s
        buf1 = "PMTK397,1.0";
        break;
    case 15: // 1.5 m/s
        buf1 = "PMTK397,1.5";
        break;
    case 20: // 2.0 m/s
        buf1 = "PMTK397,2.0";
        break;
    default:
        // use the default
        GPSUTILS_WARN("Unsupported speed threshold %lf m/s. Using 0.2m/s\n",
                speed);
        buf1 = "PMTK397,0.2";
        break;
    }
    char buf2[64];
    memset(buf2, 0, sizeof(buf2));
    snprintf(buf2, sizeof(buf2) - 1, "$%s*%02X\r\n",
            buf1, gpsutils_checksum(buf1, -1));
    return gpsdevice_send_message(fd, buf2);
}

int gpsdevice_set_fix_interval(int fd, uint16_t milliseconds)
{
    if (fd < 0)
       return -1;
    if (milliseconds < 100)
        milliseconds = 100;
    if (milliseconds > 10000)
        milliseconds = 10000;
    char buf1[32];
    char buf2[64];
    memset(buf1, 0, sizeof(buf1));
    memset(buf2, 0, sizeof(buf2));
    snprintf(buf1, sizeof(buf1) - 1, "PMTK220,%d", milliseconds);
    snprintf(buf2, sizeof(buf2) - 1, "$%s*%02X\r\n", buf1,
            gpsutils_checksum(buf1, -1));
    return gpsdevice_send_message(fd, buf2);
}

int gpsdevice_set_enabled(int fd, bool is_gpvtg, bool is_gpgsa, bool is_gpgsv)
{
    char buf1[64];
    memset(buf1, 0, sizeof(buf1));
    snprintf(buf1, sizeof(buf1) - 1,
            "PMTK314,0,1,%d,1,%d,%d,0,0,0,0,0,0,0,0,0,0,0,0,0",
            is_gpvtg ? 1 : 0,
            is_gpgsa ? 1 : 0,
            is_gpgsv ? 5 : 0);
    char buf2[128];
    memset(buf2, 0, sizeof(buf2));
    snprintf(buf2, sizeof(buf2) - 1, "$%s*%02X\r\n", buf1,
            gpsutils_checksum(buf1, -1));
    return gpsdevice_send_message(fd, buf2);
}

