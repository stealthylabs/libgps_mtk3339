/*
 * Copyright: 2015-2020. Stealthy Labs LLC. All Rights Reserved.
 * Date: 5th July 2019
 * Software: libgps_mtk3339
 */
#ifndef __GPSDATA_H__
#define __GPSDATA_H__

#include <gpsconfig.h>
#include <gpsutils.h>
#include <gps_utlist.h>

EXTERN_C_BEGIN

typedef enum {
    GPSDATA_MSGID_UNSET,
    GPSDATA_MSGID_GPGGA,
    GPSDATA_MSGID_GPGSA,
    GPSDATA_MSGID_GPGSV,
    GPSDATA_MSGID_GPRMC,
    GPSDATA_MSGID_GPVTG,
    GPSDATA_MSGID_PGTOP,
    GPSDATA_MSGID_PMTK
} gpsdata_msgid_t;

const char *gpsdata_msgid_tostring(gpsdata_msgid_t);

typedef enum {
    GPSDATA_DIRECTION_UNSET = 0, // b0000 (binary)
    GPSDATA_DIRECTION_NORTH = 1, // b0001
    GPSDATA_DIRECTION_SOUTH = 2, // b0010
    GPSDATA_DIRECTION_EAST  = 3, // b0011
    GPSDATA_DIRECTION_WEST  = 4  // b0100
} gpsdata_direction_t;

const char *gpsdata_direction_tostring(gpsdata_direction_t);

typedef enum {
    GPSDATA_POSFIX_NOFIX,
    GPSDATA_POSFIX_GPSFIX,
    GPSDATA_POSFIX_DGPSFIX
} gpsdata_posfix_t;

const char *gpsdata_posfix_tostring(gpsdata_posfix_t);

typedef enum {
    GPSDATA_MODE_UNSET,
    GPSDATA_MODE1_MANUAL,
    GPSDATA_MODE1_AUTOMATIC,
    GPSDATA_MODE2_NOFIX,
    GPSDATA_MODE2_2DFIX,
    GPSDATA_MODE2_3DFIX,
    GPSDATA_MODE_AUTONOMOUS,
    GPSDATA_MODE_DIFFERENTIAL,
    GPSDATA_MODE_ESTIMATED
} gpsdata_mode_t;

const char *gpsdata_mode_tostring(gpsdata_mode_t);

typedef enum {
    GPSDATA_ANTENNA_UNSET = 0,
    GPSDATA_ANTENNA_SHORTED = 1,
    GPSDATA_ANTENNA_INTERNAL = 2,
    GPSDATA_ANTENNA_ACTIVE = 3
} gpsdata_antenna_t;

const char *gpsdata_antenna_tostring(gpsdata_antenna_t);

typedef struct {
    // direction of UNSET means invalid/uninitialized values
    gpsdata_direction_t direction;
    short degrees; // degrees cannot be more than 360
    float minutes;
} gpsdata_latlon_t;

typedef struct gpsdata_data {
    gpsdata_msgid_t msgid;      // the message ID this update is from
    gpsdata_latlon_t latitude;
    gpsdata_latlon_t longitude;
    // if a full timestamp is available this bool will be set to true
    bool is_valid_timestamp;
    struct timeval timestamp;
    gpsdata_mode_t mode; // set for GPRMC/GPVTG
    gpsdata_posfix_t posfix;
    uint32_t num_satellites;
    // if not available all float values will be NAN
    float altitude_meters;
    float speed_kmph;
    float speed_knots;
    float course_degrees;
    float heading_degrees;
    // antenna status
    gpsdata_antenna_t antenna_status;
    /* make this a linked list using utlist.h or gps_utlist.h in
     * our case to make sure we get expected behavior */
    struct gpsdata_data *next;    
} gpsdata_data_t;

void gpsdata_initialize(gpsdata_data_t *);
void gpsdata_dump(const gpsdata_data_t *, FILE *);
void gpsdata_list_free(gpsdata_data_t **listp);
ssize_t gpsdata_list_count(const gpsdata_data_t *listp);
void gpsdata_list_dump(const gpsdata_data_t *listp, FILE *fp);

typedef struct gpsdata_parser_t gpsdata_parser_t;

gpsdata_parser_t *gpsdata_parser_create();
void gpsdata_parser_free(gpsdata_parser_t *);
void gpsdata_parser_reset(gpsdata_parser_t *);
void gpsdata_parser_dump_state(const gpsdata_parser_t *, FILE *);

int gpsdata_parser_parse(gpsdata_parser_t *ptr,
            const char *buf, size_t buflen,
            gpsdata_data_t **listp, // the link list pointer to which to append the results to
            size_t *outnum // the number of elements added to the list in this call
            );

/* this is necessary if you're reading the chip using this library.
 * you can call open() on the device and get a filedescriptor and then call this
 * function on it to set the BAUD Rate to 9600, which is the default. You may
 * try 115200 or other rates. supported values are 1200, 2400, 4800, 9600,
 * 19200, 38400, 57600, 115200. Anything greater than that is rejected and 9600
 * is used. -1 is returned if fd is invalid or setting the baud rate has failed
 */
int gpsdevice_set_baudrate(int fd, uint32_t baud_rate);
/* a wrapper function for opening device in Read-only mode and setting baud rate
 * to 9600 bps. this function calls gpsutils_set_baudrate() with 9600 bps
 * internally.
 * Returns the file descriptor if successful or -1 on error. The user must call
 * the close() function on the file descriptor when closing the device
 * by default the device is opened in blocking mode
 */
int gpsdevice_open(const char *device, bool non_blocking);
void gpsdevice_close(int fd);

int gpsdevice_set_restart(int fd, bool is_warm);
int gpsdevice_set_standby(int fd);
/* the position fix interval is set here. values can range between 100 and
 * 10000 milliseconds. any other value defaults to 1000.
 */
int gpsdevice_set_fix_interval(int fd, int milliseconds);

/* navspeed threshold in m/s. valid values are 0, 0.2, 0.4, 0.6, 0.8, 1.0, 1.5,
 * 2.0 m/s. Any other value defaults to 0.2 m/s. 0 m/s disables the speed
 * threshold.
 * if the speed is slower than the threshold the position is frozen. the user
 * may want to adjust this based on the application.
 */
int gpsdevice_set_speed_threshold(int fd, float speed);
int gpsdevice_request_firmware_info(int fd);
const char *gpsdevice_get_firmware_info(int fd);

EXTERN_C_END
#endif /* __GPSDATA_H__ */
