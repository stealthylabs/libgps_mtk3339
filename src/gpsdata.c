/*
 * Copyright: 2015-2019. Stealthy Labs LLC. All Rights Reserved.
 * Date: 5th July 2019
 * Software: libgps_mt3339
 */
#include <gpsconfig.h>
#include <gpsdata.h>

const char *gpsdata_msgid_tostring(gpsdata_msgid_t m)
{
    switch (m) {
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
    case GPSDATA_ANTENNA_SHORTED: return "SHORTED";
    case GPSDATA_ANTENNA_INTERNAL: return "INTERNAL";
    case GPSDATA_ANTENNA_ACTIVE: return "ACTIVE";
    default: break;
    }
    return "UNSET";
}
