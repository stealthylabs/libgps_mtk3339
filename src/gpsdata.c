/*
 * Copyright: 2015-2019. Stealthy Labs LLC. All Rights Reserved.
 * Date: 5th July 2019
 * Software: libgps_mtk3339
 */
#include <gpsdata.h>

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
            fprintf(fp, "latitude: %d' %0.04f\" %s\n",
                   o->latitude.degrees, o->latitude.minutes,
                  gpsdata_direction_tostring(o->latitude.direction));
        }
        if (o->longitude.direction != GPSDATA_DIRECTION_UNSET) {
            fprintf(fp, "longitude: %d' %0.04f\" %s\n",
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
    }
}
