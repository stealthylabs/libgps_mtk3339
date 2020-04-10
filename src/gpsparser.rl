/*
 * Copyright: 2015-2019. Stealthy Labs LLC. All Rights Reserved.
 * Date: 5th July 2019
 * Software: libgps_mtk3339
 */
#ifndef _DEFAULT_SOURCE
    #define _DEFAULT_SOURCE
#endif
#include <gpsconfig.h>
#include <gpsutils.h>
#include <gpsdata.h>
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
struct gpsdata_parser_t {
    // variables used by Ragel Section 5.1
    int cs; // current state variable
    const char *p; // data begin pointer, must be initialized to 0
    const char *pe; // data end pointer, must be initialized to p + length
    const char *eof; // end of file/stream pointer
    void (*init)(struct gpsdata_parser_t *);
    void (*reset)(struct gpsdata_parser_t *);
    void (*fini)(struct gpsdata_parser_t *);
    int (*execute)(struct gpsdata_parser_t *, const char *, size_t);
    void (*clean_state)(struct gpsdata_parser_t *);
    void (*dump_state)(const struct gpsdata_parser_t *, FILE *);
    int (*save)(struct gpsdata_parser_t *);

    /* a linked-list of items that can be loaded from a single buffer
    * highly unlikely that the code will hold more than 3-4 messages in this
    * list but better to use a list than an array
    * the items in this list are added in by the save function
    */
    gpsdata_data_t *items;
    // used to store the most recent date from the GPRMC message
    // since it is like a delta feed and we may not have that info
    struct tm rmc_tm;

    /* internal portion of the FSM to track each message */
    struct tm _tm;
    uint32_t _tm_msec;
    gpsdata_latlon_t _lll;
    uint32_t _tmp_real[4];
    float _tmp_float;
    uint32_t _calc_checksum;
    uint32_t _checksum;

    gpsdata_msgid_t _msgid;
    gpsdata_latlon_t _lat;
    gpsdata_latlon_t _lon;
    gpsdata_posfix_t _posfix;
    gpsdata_mode_t mode1;
    gpsdata_mode_t mode2;
    gpsdata_mode_t mode_common;
    uint8_t _num_sats;//0 - 14
    uint8_t _num_msg_max;//values 1-3 as per datasheet, but sometimes 4 show up so 1-9
    uint8_t _num_msg_idx;//values 1-9 as it is 1-based indexing
    bool _rmc_valid;
    uint8_t _pgtop_fntype;// is this always 11 ?
    uint8_t _pgtop_value;// values 1-3

    uint16_t _satellites_used[12];
    uint8_t  _satellites_used_idx;
    float _hdop;
    float _altitude;
    float _geoidal;
    float _pdop;
    float _vdop;
    float _speed_knots;
    float _speed_kmph;
    float _course_degrees; // true heading
    float _heading_degrees; // magnetic heading
    float _magvar_degrees; // magnetic variation
    gpsdata_direction_t _magvar_direction; // magnetic variation direction

#define FSM_GSV_SAT_MAX 9
    struct gpsdata_parser_gsv_t {
        uint8_t satellite_id; // values 1-32. sometimes can be > 48
        uint8_t elevation; // values 0-90
        bool is_elevation_null;
        uint16_t azimuth; // values 0-359
        bool is_azimuth_null;
        uint8_t snr_cno; // null, 0-99.
        bool is_snr_cno_null; // true if null
    } _gsv_sats[FSM_GSV_SAT_MAX]; // can handle a total of 9 messages at max
    uint8_t _gsv_sat_idx;
};

%%{
    machine gpsdata_parser_fsm;
    alphtype char;
    access fsm->;
    variable p fsm->p;
    variable pe fsm->pe;
    variable eof fsm->eof;

    action xn_clean_state { if (fsm->clean_state) fsm->clean_state(fsm); }
    action xn_msgid_gpgga { fsm->_msgid = GPSDATA_MSGID_GPGGA; }
    action xn_msgid_gpgsa { fsm->_msgid = GPSDATA_MSGID_GPGSA; }
    action xn_msgid_gpgsv { fsm->_msgid = GPSDATA_MSGID_GPGSV; }
    action xn_msgid_gprmc { fsm->_msgid = GPSDATA_MSGID_GPRMC; }
    action xn_msgid_gpvtg { fsm->_msgid = GPSDATA_MSGID_GPVTG; }
    action xn_msgid_pgtop { fsm->_msgid = GPSDATA_MSGID_PGTOP; }
    action xn_msgid_pmtk  { fsm->_msgid = GPSDATA_MSGID_PMTK;  }

    action xn_tm_Z_dd { fsm->_tm.tm_mday  = 0; }
    action xn_tm_0_dd { fsm->_tm.tm_mday += (fc - '0'); }
    action xn_tm_1_dd { fsm->_tm.tm_mday += 10 * (fc - '0'); }
    action xn_tm_Z_mm { fsm->_tm.tm_mon   = 0; }
    action xn_tm_0_mm { fsm->_tm.tm_mon  += (fc - '0'); }
    action xn_tm_1_mm { fsm->_tm.tm_mon  += 10 * (fc - '0'); }
    action xn_tm_E_mm { fsm->_tm.tm_mon--; } #month is 0-11
    action xn_tm_Z_yy { fsm->_tm.tm_year  = 0; }
    action xn_tm_0_yy { fsm->_tm.tm_year += (fc - '0'); }
    action xn_tm_1_yy { fsm->_tm.tm_year += 10 * (fc - '0'); }
    action xn_tm_E_yy { fsm->_tm.tm_year += 100; } # we are in 2000+ years now

    action xn_tm_Z_HH { fsm->_tm.tm_hour = 0; }
    action xn_tm_0_HH { fsm->_tm.tm_hour += (fc - '0'); }
    action xn_tm_1_HH { fsm->_tm.tm_hour += 10 * (fc - '0'); }
    action xn_tm_Z_MM { fsm->_tm.tm_min = 0; }
    action xn_tm_0_MM { fsm->_tm.tm_min += (fc - '0'); }
    action xn_tm_1_MM { fsm->_tm.tm_min += 10 * (fc - '0'); }
    action xn_tm_Z_SS { fsm->_tm.tm_sec = 0; }
    action xn_tm_0_SS { fsm->_tm.tm_sec += (fc - '0'); }
    action xn_tm_1_SS { fsm->_tm.tm_sec += 10 * (fc - '0'); }
    action xn_tm_Z_MS { fsm->_tm_msec = 0; }
    action xn_tm_0_MS { fsm->_tm_msec += (fc - '0'); }
    action xn_tm_1_MS { fsm->_tm_msec += 10 * (fc - '0'); }
    action xn_tm_2_MS { fsm->_tm_msec += 100 * (fc - '0'); }

    action xn_lat_Z {
        fsm->_lll.degrees = 0;
        fsm->_lll.minutes = 0;
        fsm->_lll.direction = GPSDATA_DIRECTION_UNSET;
    }
    action xn_lat_0_dd { fsm->_lll.degrees += (fc - '0'); }
    action xn_lat_1_dd { fsm->_lll.degrees += 10 * (fc - '0'); }
    action xn_lat_2_dd { fsm->_lll.degrees += 100 * (fc - '0'); }
    action xn_lat_0_mm { fsm->_lll.minutes += (fc - '0'); }
    action xn_lat_1_mm { fsm->_lll.minutes += 10 * (fc - '0'); }
    action xn_lat_0_ss { fsm->_lll.minutes += 0.0001 * (fc - '0'); }
    action xn_lat_1_ss { fsm->_lll.minutes += 0.001 * (fc - '0'); }
    action xn_lat_2_ss { fsm->_lll.minutes += 0.01 * (fc - '0'); }
    action xn_lat_3_ss { fsm->_lll.minutes += 0.1 * (fc - '0'); }
    action xn_latitude {
        fsm->_lat.direction = fsm->_lll.direction;
        fsm->_lat.degrees = fsm->_lll.degrees;
        fsm->_lat.minutes = fsm->_lll.minutes;
    }
    action xn_longitude {
        fsm->_lon.direction = fsm->_lll.direction;
        fsm->_lon.degrees = fsm->_lll.degrees;
        fsm->_lon.minutes = fsm->_lll.minutes;
    }
    action xn_magvariation {
        fsm->_magvar_direction = GPSDATA_DIRECTION_UNSET;
        if (!isnanf(fsm->_tmp_float)) {
            fsm->_magvar_degrees = fsm->_tmp_float;
        } else {
            GPSUTILS_DEBUG("GPVTG magnetic variation is empty/nan\n");
            fsm->_magvar_degrees = NAN;
        }
        fsm->_tmp_float = NAN;
    }
    action xn_latitude_ns {
        fsm->_lat.direction = (fc == 'N') ? GPSDATA_DIRECTION_NORTH : GPSDATA_DIRECTION_SOUTH;
    }
    action xn_longitude_ew {
        fsm->_lon.direction = (fc == 'E') ? GPSDATA_DIRECTION_EAST : GPSDATA_DIRECTION_WEST;
    }
    action xn_magvariation_ew {
        if (fc == 'E') {
            fsm->_magvar_direction = GPSDATA_DIRECTION_EAST;
        } else if (fc == 'W') {
            fsm->_magvar_direction = GPSDATA_DIRECTION_WEST;
        } else {
            GPSUTILS_DEBUG("GPRMC magnetic variation direction is empty/null\n");
            fsm->_magvar_direction = GPSDATA_DIRECTION_UNSET;
        }
    }
    action xn_posfix {
        if (fc == '0') {
            fsm->_posfix = GPSDATA_POSFIX_NOFIX;
        } else if (fc == '1') {
            fsm->_posfix = GPSDATA_POSFIX_GPSFIX;
        } else if (fc == '2') {
            fsm->_posfix = GPSDATA_POSFIX_DGPSFIX;
        }
    }
    action xn_mode1 {
        if (fc == 'M') {
            fsm->mode1 = GPSDATA_MODE1_MANUAL;
        } else if (fc == 'A') {
            fsm->mode1 = GPSDATA_MODE1_AUTOMATIC;
        }
    }
    action xn_mode2 {
        if (fc == '1') {
            fsm->mode2 = GPSDATA_MODE2_NOFIX;
        } else if (fc == '2') {
            fsm->mode2 = GPSDATA_MODE2_2DFIX;
        } else if (fc == '3') {
            fsm->mode2 = GPSDATA_MODE2_3DFIX;
        }
    }

    action xn_mode_common {
        if (fc == 'A') {
            fsm->mode_common = GPSDATA_MODE_AUTONOMOUS;
        } else if (fc == 'D') {
            fsm->mode_common = GPSDATA_MODE_DIFFERENTIAL;
        } else if (fc == 'E') {
            fsm->mode_common = GPSDATA_MODE_ESTIMATED;
        }
    }

    action xn_status_valid {
        if (fc == 'A') {
            fsm->_rmc_valid = true;
        } else if (fc == 'V') {
            fsm->_rmc_valid = true;
        } else {
            GPSUTILS_ERROR("GPRMC Status can be either 'A' or 'V', not '%c'\n", fc);
            fsm->_rmc_valid = false;
        }
    }

    action xn_pgtop_fntype {
        fsm->_pgtop_fntype = fsm->_pgtop_fntype * 10 + (fc - '0');
    }

    action xn_pgtop_value {
        fsm->_pgtop_value = (fc - '0');
        if (!(fsm->_pgtop_value >= 1 && fsm->_pgtop_value <= 3)) {
            GPSUTILS_ERROR("PGTOP value should be in [1,3]. We have: %d", fsm->_pgtop_value);
        }
    }

    action xn_num_sats {
        fsm->_num_sats = fsm->_num_sats * 10 + (fc - '0');
    }
    action xn_real_start {
        fsm->_tmp_real[0] = 0;
        fsm->_tmp_real[1] = 0;
        fsm->_tmp_real[2] = 1;
        fsm->_tmp_real[3] = 1;
        fsm->_tmp_float = NAN;
    }
    action xn_real_front {
        fsm->_tmp_real[0]  = fsm->_tmp_real[0] * 10 + (fc - '0');
    }
    action xn_real_back {
        fsm->_tmp_real[1]  = fsm->_tmp_real[1] * 10 + (fc - '0');
        fsm->_tmp_real[2] *= 10;
    }
    action xn_real_sign {
        fsm->_tmp_real[3] = -1;
    }
    action xn_real_end {
        // if we do not use float here the compiler does the RHS in ints and
        // then converts to float which is not what we want
        fsm->_tmp_float = ((float)fsm->_tmp_real[0] +
            ((float)fsm->_tmp_real[1] / fsm->_tmp_real[2])) * (float)fsm->_tmp_real[3];
        /*GPSUTILS_DEBUG("tmp_real: %d %d %d %d %f\n",
            fsm->_tmp_real[0], fsm->_tmp_real[1],
            fsm->_tmp_real[2], fsm->_tmp_real[3],
            fsm->_tmp_float);*/
    }

    action xn_hdop {
        fsm->_hdop = fsm->_tmp_float;
        fsm->_tmp_float = NAN;
    }
    action xn_pdop {
        fsm->_pdop = fsm->_tmp_float;
        fsm->_tmp_float = NAN;
    }
    action xn_vdop {
        fsm->_vdop = fsm->_tmp_float;
        fsm->_tmp_float = NAN;
    }
    action xn_altitude {
        fsm->_altitude = fsm->_tmp_float;
        fsm->_tmp_float = NAN;
    }
    action xn_geoidal {
        fsm->_geoidal = fsm->_tmp_float;
        fsm->_tmp_float = NAN;
    }
    action xn_speed_knots {
        fsm->_speed_knots = fsm->_tmp_float;
        fsm->_tmp_float = NAN;
    }
    action xn_speed_kmph {
        fsm->_speed_kmph = fsm->_tmp_float;
        fsm->_tmp_float = NAN;
    }
    action xn_ground_course {
        fsm->_course_degrees = fsm->_tmp_float;
        fsm->_tmp_float = NAN;
    }
    action xn_magnetic_heading {
        if (!isnanf(fsm->_tmp_float)) {
            fsm->_heading_degrees = fsm->_tmp_float;
        } else {
            GPSUTILS_DEBUG("GPVTG magnetic heading is empty/nan\n");
            fsm->_heading_degrees = NAN;
        }
        fsm->_tmp_float = NAN;
    }

    action xn_satellites_used {
        if (!isnanf(fsm->_tmp_float)) {
            uint32_t sat = (uint32_t)(fsm->_tmp_float);
            fsm->_tmp_float = NAN;
            GPSUTILS_DEBUG("Satellite used: %d\n", sat);
            if (sat < 64 && fsm->_satellites_used_idx < 12) {
                fsm->_satellites_used[fsm->_satellites_used_idx++] = sat;
            } else {
                GPSUTILS_ERROR("GPGSA satellites_used is > 64: %d", sat);
            }
        } else {
            GPSUTILS_DEBUG("GPGSA satellites_used is empty/nan\n");
            fsm->_satellites_used[fsm->_satellites_used_idx++] = 0;
        }
    }
    action xn_gpgsv_msgcount {
        fsm->_num_msg_max = (fc - '0');
        if (fsm->_gsv_sat_idx > fsm->_num_msg_max || fsm->_gsv_sat_idx > FSM_GSV_SAT_MAX) {
            GPSUTILS_ERROR("_gsv_sat_idx %d > _num_msg_max %d || %d. Parser has an error\n",
                    fsm->_gsv_sat_idx, fsm->_num_msg_max, FSM_GSV_SAT_MAX);
            fbreak;
        }
    }
    action xn_gpgsv_msgindex {
        fsm->_num_msg_idx = (fc - '0'); // index is 1-based
        if ((fsm->_num_msg_idx - 1) != fsm->_gsv_sat_idx) {
            GPSUTILS_WARN("GPGSV message index %d does not match parser state internal message index %d\n",
                fsm->_num_msg_idx, fsm->_gsv_sat_idx + 1);
        }
    }
    action xn_satellite_id {
        if (!isnanf(fsm->_tmp_float)) {
            uint8_t num = ((uint32_t)(fsm->_tmp_float)) & 0xFF;
            fsm->_tmp_float = NAN;
            if (num >= 1 && num <= 99) {
                GPSUTILS_DEBUG("Satellite ID: %d\n", num);
                fsm->_gsv_sats[fsm->_gsv_sat_idx].satellite_id = num;
            } else {
                GPSUTILS_WARN("GPGSV satellite_id is not in [1,99]: %d\n", num);
                fsm->_gsv_sats[fsm->_gsv_sat_idx].satellite_id = num;
            }
        } else {
            GPSUTILS_DEBUG("GPGSV satelliteid is empty/nan\n");
            fsm->_gsv_sats[fsm->_gsv_sat_idx].satellite_id = 0;
        }
    }
    action xn_elevation {
        if (!isnanf(fsm->_tmp_float)) {
            uint8_t num = ((uint32_t)(fsm->_tmp_float)) & 0xFF;
            fsm->_tmp_float = NAN;
            if (num <= 90) {
                GPSUTILS_DEBUG("Elevation: %d\n", num);
                fsm->_gsv_sats[fsm->_gsv_sat_idx].elevation = num;
                fsm->_gsv_sats[fsm->_gsv_sat_idx].is_elevation_null = false;
            } else {
                GPSUTILS_ERROR("GPGSV elevation is not in [0,90]: %d", num);
                fsm->_gsv_sats[fsm->_gsv_sat_idx].elevation = 0;
                fsm->_gsv_sats[fsm->_gsv_sat_idx].is_elevation_null = true;
            }
        } else {
            GPSUTILS_DEBUG("GPGSV elevation is empty/nan\n");
            fsm->_gsv_sats[fsm->_gsv_sat_idx].elevation = 0;
            fsm->_gsv_sats[fsm->_gsv_sat_idx].is_elevation_null = true;
        }
    }
    action xn_azimuth {
        if (!isnanf(fsm->_tmp_float)) {
            uint16_t num = ((uint32_t)(fsm->_tmp_float)) & 0x0000FFFF;
            fsm->_tmp_float = NAN;
            if (num < 360) {
                GPSUTILS_DEBUG("Azimuth: %d\n", num);
                fsm->_gsv_sats[fsm->_gsv_sat_idx].azimuth = num;
                fsm->_gsv_sats[fsm->_gsv_sat_idx].is_azimuth_null = false;
            } else {
                GPSUTILS_ERROR("GPGSV azimuth is not in [0,360): %d", num);
                fsm->_gsv_sats[fsm->_gsv_sat_idx].azimuth = 0;
                fsm->_gsv_sats[fsm->_gsv_sat_idx].is_azimuth_null = true;
            }
        } else {
            GPSUTILS_DEBUG("GPGSV azimuth is empty/nan\n");
            fsm->_gsv_sats[fsm->_gsv_sat_idx].azimuth = 0;
            fsm->_gsv_sats[fsm->_gsv_sat_idx].is_azimuth_null = true;
        }
    }
    action xn_snr_cno {
        if (!isnanf(fsm->_tmp_float)) {
            uint8_t num = ((uint32_t)(fsm->_tmp_float)) & 0xFF;
            fsm->_tmp_float = NAN;
            if (num <= 99) {
                GPSUTILS_DEBUG("SNR C/No: %d\n", num);
                fsm->_gsv_sats[fsm->_gsv_sat_idx].snr_cno = num;
                fsm->_gsv_sats[fsm->_gsv_sat_idx].is_snr_cno_null = false;
            } else {
                GPSUTILS_ERROR("GPGSV snr_cno is not in [0,99]: %d", num);
                fsm->_gsv_sats[fsm->_gsv_sat_idx].snr_cno = 0;
                fsm->_gsv_sats[fsm->_gsv_sat_idx].is_snr_cno_null = true;
            }
        } else {
            GPSUTILS_DEBUG("GPGSV snr_cno is empty/nan\n");
            fsm->_gsv_sats[fsm->_gsv_sat_idx].snr_cno = 0;
            fsm->_gsv_sats[fsm->_gsv_sat_idx].is_snr_cno_null = true;
        }
        fsm->_gsv_sat_idx++;// increment
        GPSUTILS_DEBUG("GPGSV gsv_sat_idx incremented to %d\n", fsm->_gsv_sat_idx);
    }

    action xn_checksum_xdigit {
        if (fc <= '9') {
            fsm->_checksum = fsm->_checksum * 16 + (fc - '0');
        } else {
            fsm->_checksum = fsm->_checksum * 16 + ((toupper(fc) - 'A') + 10);
        }
    }
    action xn_checksum_reset { fsm->_calc_checksum = 0; }
    action xn_checksum_calculate { fsm->_calc_checksum ^= fc; }
    action xn_checksum_verify {
        if (fsm->_calc_checksum != fsm->_checksum) {
            GPSUTILS_ERROR("Checksum does not match. Expected: %x Calculated: %x\n",
                    fsm->_checksum, fsm->_calc_checksum);
        } else {
            GPSUTILS_DEBUG("checksum: %x verified\n", fsm->_checksum);
        }
    }
    action xn_message_save {
        GPSUTILS_DEBUG("message save called for msgid: %s\n", gpsdata_msgid_tostring(fsm->_msgid));
        if (fsm->save) {
            fsm->save(fsm);
        }
    }

    COMMA = ',';
    DOT = '.';
    integer = '-' ? @xn_real_sign digit+ >xn_real_start $xn_real_front %xn_real_end;
    real = '-' ? @xn_real_sign digit* >xn_real_start $xn_real_front DOT digit+ $xn_real_back %xn_real_end;
    number = real | integer;
    optional_integer = integer | zlen;
    optional_number = number | zlen;

    ## we could do this better by using the fpc pointer and subtracting the
    ## pointer locations, using strtol and converting but maybe it is cheaper to
    ## be dumb and have many actions, might be faster since it is just a table
    ##ddmmyy
    UTCDate = digit >xn_tm_Z_dd @xn_tm_1_dd digit @xn_tm_0_dd
              digit >xn_tm_Z_mm @xn_tm_1_mm digit @xn_tm_0_mm %xn_tm_E_mm
              digit >xn_tm_Z_yy @xn_tm_1_yy digit @xn_tm_0_yy %xn_tm_E_yy
            ;

    ## hhmmss.sss
    UTCTime = digit >xn_tm_Z_HH @xn_tm_1_HH digit @xn_tm_0_HH
              digit >xn_tm_Z_MM @xn_tm_1_MM digit @xn_tm_0_MM
              digit >xn_tm_Z_SS @xn_tm_1_SS digit @xn_tm_0_SS
              DOT digit >xn_tm_Z_MS @xn_tm_2_MS digit @xn_tm_1_MS digit @xn_tm_0_MS
            ;
    ## ddmm.mmmm
    Latitude = digit >xn_lat_Z @xn_lat_1_dd digit @xn_lat_0_dd
               digit @xn_lat_1_mm digit @xn_lat_0_mm DOT
               digit @xn_lat_3_ss digit @xn_lat_2_ss
               digit @xn_lat_1_ss digit @xn_lat_0_ss;
    ## dddmm.mmmm
    Longitude = digit>xn_lat_Z @xn_lat_2_dd digit @xn_lat_1_dd digit @xn_lat_0_dd
               digit @xn_lat_1_mm digit @xn_lat_0_mm DOT
               digit @xn_lat_3_ss digit @xn_lat_2_ss
               digit @xn_lat_1_ss digit @xn_lat_0_ss;

    range1_32 = ([0]?[1-9]) | ([1-2][0-9]) | ([3][0-2]);
    range0_90 = ([0-8]?[0-9]) | '90' ;
    range0_99 = ([0-9]{1,2});
    range0_359 = ([0-2]?[0-9]{2}) | ([3][0-5][0-9]);

    ## time, position and fix type data
    gpgga = 'GPGGA' @xn_msgid_gpgga COMMA .
        UTCTime COMMA .
        Latitude %xn_latitude COMMA .
        [NS] @xn_latitude_ns COMMA .
        Longitude %xn_longitude COMMA .
        [EW] @xn_longitude_ew COMMA .
        [012] @xn_posfix COMMA .
        (digit{1,2} $xn_num_sats) COMMA .
        number %xn_hdop COMMA .
        number %xn_altitude COMMA 'M' COMMA .
        number %xn_geoidal COMMA 'M' COMMA number? COMMA;

    ## GPS receiver operating mode, active satellites used in the position
    ## solution and DOP values
    gpgsa = 'GPGSA' @xn_msgid_gpgsa COMMA .
        [MA] @xn_mode1 COMMA [1-3] @xn_mode2 COMMA .
        (optional_integer %xn_satellites_used COMMA){12} .
        number %xn_pdop COMMA .
        number %xn_hdop COMMA .
        number %xn_vdop COMMA?; #no comma here as per datasheet, but let's handle it

    ## the number of GPS satellites in view satellite ID numbers, elevation,
    ## azimuth, and SNR values
    gpgsv = 'GPGSV' @xn_msgid_gpgsv COMMA .
        [1-9] @xn_gpgsv_msgcount COMMA .
        [1-9] @xn_gpgsv_msgindex COMMA .
        (digit{1,2} $xn_num_sats) .
        (COMMA optional_integer %xn_satellite_id .
         COMMA optional_integer %xn_elevation .
         COMMA optional_integer %xn_azimuth .
         COMMA optional_integer %xn_snr_cno ) {1,4} .
        COMMA ?; #optional comma in case needed

    ## time, date, position, course and speed data. recommended minimum navigation information
    gprmc = 'GPRMC' @xn_msgid_gprmc COMMA .
        UTCTime COMMA .
        [AV] @xn_status_valid COMMA .
        Latitude %xn_latitude COMMA .
        [NS] @xn_latitude_ns COMMA .
        Longitude %xn_longitude COMMA .
        [EW] @xn_longitude_ew COMMA .
        number %xn_speed_knots COMMA .
        number %xn_ground_course COMMA .
        UTCDate COMMA .
        optional_number %xn_magvariation COMMA .
        ([EW] | zlen) @xn_magvariation_ew COMMA . # this is for magnetic variation
        [ADE] @xn_mode_common COMMA ?; #optional comma in case needed

    ## course and speed information relative to the ground
    gpvtg = 'GPVTG' @xn_msgid_gpvtg COMMA .
        number %xn_ground_course COMMA 'T' COMMA .
        optional_number %xn_magnetic_heading COMMA 'M' COMMA .
        number %xn_speed_knots COMMA 'N' COMMA .
        number %xn_speed_kmph COMMA 'K' COMMA .
        [ADE] @xn_mode_common COMMA ?; #optional comma in case needed

    ## status of antenna
    pgtop = 'PGTOP' @xn_msgid_pgtop COMMA .
        integer @xn_pgtop_fntype COMMA .
        [1-3] @xn_pgtop_value COMMA ?; #optional comma in case needed

    ## cold start: don't use time, position, almanacs & ephemeris data at re-start
    pmtk = 'PMTK' @xn_msgid_pmtk '103'; #for cold start

    action xn_fake_msg {
        GPSUTILS_WARN("fake message 0x00 0xFF received, ignoring\n");
    }
    message = '$' >xn_clean_state .
        (gpgga | gpgsa | gpgsv | gprmc | gpvtg | pgtop | pmtk) >xn_checksum_reset $xn_checksum_calculate .
        '*' xdigit{2} $xn_checksum_xdigit %xn_checksum_verify;
    fake_message = 0x00 0xFF %xn_fake_msg;
    main := (message %xn_message_save | fake_message | space | empty | 0x00 )* ;# allow nulls

}%%

%% write data;

static void gpsdata_parser_internal_clean_state(gpsdata_parser_t *fsm)
{
    memset(&(fsm->_tm), 0, sizeof(fsm->_tm));
    fsm->_tm_msec = 0;
    fsm->_lll.direction = GPSDATA_DIRECTION_UNSET;
    fsm->_lll.degrees = SHRT_MIN;
    fsm->_lll.minutes = NAN;
    memset(fsm->_tmp_real, 0, sizeof(fsm->_tmp_real));
    fsm->_tmp_float = NAN;
    fsm->_calc_checksum = 0;
    fsm->_checksum = 0;
    fsm->_msgid = GPSDATA_MSGID_UNSET;
    fsm->_lat.direction = GPSDATA_DIRECTION_UNSET;
    fsm->_lat.degrees = SHRT_MIN;
    fsm->_lat.minutes = NAN;
    fsm->_lon.direction = GPSDATA_DIRECTION_UNSET;
    fsm->_lon.degrees = SHRT_MIN;
    fsm->_lon.minutes = NAN;
    fsm->mode1 = GPSDATA_MODE_UNSET;
    fsm->mode2 = GPSDATA_MODE_UNSET;
    fsm->mode_common = GPSDATA_MODE_UNSET;
    fsm->_posfix = GPSDATA_POSFIX_NOFIX;
    fsm->_rmc_valid = false;
    fsm->_num_sats = 0;
    fsm->_num_msg_max = 0;
    fsm->_num_msg_idx = 0;
    fsm->_pgtop_fntype = 0;
    fsm->_pgtop_value = 0;
    memset(fsm->_satellites_used, 0, sizeof(fsm->_satellites_used));
    fsm->_satellites_used_idx = 0;
    fsm->_hdop = NAN;
    fsm->_altitude = NAN;
    fsm->_geoidal = NAN;
    fsm->_pdop = NAN;
    fsm->_vdop = NAN;
    fsm->_speed_knots = NAN;
    fsm->_speed_kmph = NAN;
    fsm->_course_degrees = NAN;
    fsm->_heading_degrees = NAN;
    fsm->_magvar_degrees = NAN;
    fsm->_magvar_direction = GPSDATA_DIRECTION_UNSET;
    memset(fsm->_gsv_sats, 0, sizeof(fsm->_gsv_sats));
    fsm->_gsv_sat_idx = 0;
}

static void gpsdata_parser_internal_dump_state(const gpsdata_parser_t *fsm, FILE *fp)
{
    if (fsm && fp) {
        fprintf(fp, "Ragel FSM cs: %d\n", fsm->cs);
        fprintf(fp, "Ragel FSM p: %p\n", fsm->p);
        fprintf(fp, "Ragel FSM pe: %p\n", fsm->pe);
        fprintf(fp, "Ragel FSM eof: %p\n", fsm->eof);
        fprintf(fp, "_tm.tm_year: %d _tm.tm_mon: %d _tm.tm_mday: %d\n",
            fsm->_tm.tm_year, fsm->_tm.tm_mon, fsm->_tm.tm_mday);
        fprintf(fp, "_tm.tm_hour: %d _tm.tm_min: %d _tm.tm_sec: %d _tm_msec: %d\n",
            fsm->_tm.tm_hour, fsm->_tm.tm_min, fsm->_tm.tm_sec, fsm->_tm_msec);
        fprintf(fp, "msgid: %s\n", gpsdata_msgid_tostring(fsm->_msgid));
        fprintf(fp, "_lat: %d' %0.04f\" %s\n", fsm->_lat.degrees, fsm->_lat.minutes,
            gpsdata_direction_tostring(fsm->_lat.direction));
        fprintf(fp, "_lon: %d' %0.04f\" %s\n", fsm->_lon.degrees, fsm->_lon.minutes,
            gpsdata_direction_tostring(fsm->_lon.direction));
        fprintf(fp, "_posfix: %s _num_sats: %d\n",
            gpsdata_posfix_tostring(fsm->_posfix), fsm->_num_sats);
        fprintf(fp, "_hdop: %f _altitude: %f _geoidal: %f\n", fsm->_hdop,
            fsm->_altitude, fsm->_geoidal);
        fprintf(fp, "_pdop: %f _vdop: %f\n", fsm->_pdop, fsm->_vdop);
        fprintf(fp, "no. of satellites used: %d Satellites: ",
            (int)fsm->_satellites_used_idx);
        for (uint8_t i = 0; i < fsm->_satellites_used_idx; ++i) {
            fprintf(fp, " %d%s", fsm->_satellites_used[i],
            ((i + 1) == fsm->_satellites_used_idx) ? "\n" : ",");
        }
        fprintf(fp, "mode-1: %s mode-2: %s mode-common: %s\n",
            gpsdata_mode_tostring(fsm->mode1), gpsdata_mode_tostring(fsm->mode2),
            gpsdata_mode_tostring(fsm->mode_common));
        fprintf(fp, "GSV number of messages: %d 1-based index of this message: %d\n",
                fsm->_num_msg_max, fsm->_num_msg_idx);
        fprintf(fp, "GSV Satellites: found: %d\n", fsm->_gsv_sat_idx);
        for (uint8_t i = 0; i < fsm->_gsv_sat_idx && i < FSM_GSV_SAT_MAX; ++i) {
            fprintf(fp, "[%d]: ID: %d ", i, fsm->_gsv_sats[i].satellite_id);
            if (fsm->_gsv_sats[i].is_elevation_null) {
                fprintf(fp, "Elevation: (null) ");
            } else {
                fprintf(fp, "Elevation: %d ", fsm->_gsv_sats[i].elevation);
            }
            if (fsm->_gsv_sats[i].is_azimuth_null) {
                fprintf(fp, "Azimuth: (null) ");
            } else {
                fprintf(fp, "Azimuth: %d ", fsm->_gsv_sats[i].azimuth);
            }
            if (fsm->_gsv_sats[i].is_snr_cno_null) {
                fprintf(fp, "SNR C/No: (null)\n");
            } else {
                fprintf(fp, "SNR C/No: %d\n", fsm->_gsv_sats[i].snr_cno);
            }
        }
        fprintf(fp, "RMC is-valid: %s Speed(knots): %0.04f Course(deg): %0.04f ",
            (fsm->_rmc_valid ? "true" : "false"), fsm->_speed_knots,
            fsm->_course_degrees);
        fprintf(fp, "_magvariation: %0.04f degrees %s\n", fsm->_magvar_degrees,
            gpsdata_direction_tostring(fsm->_magvar_direction));
        fprintf(fp, "VTG Speed(kmph): %0.04f Magnetic Heading: %0.04f \n",
            fsm->_speed_kmph, fsm->_heading_degrees);
        fprintf(fp, "PGTOP Fntype: %d Value: %d\n",
            fsm->_pgtop_fntype, fsm->_pgtop_value);
        fprintf(fp, "_checksum: %02x (%d)\n", fsm->_checksum, fsm->_checksum);
    }
}

static int gpsdata_parser_internal_save(gpsdata_parser_t *fsm)
{
    int rc = 0;
    if (!fsm) {
        GPSUTILS_ERROR("FSM's save function called but fsm is NULL. Inconsistent state\n");
        return -1;
    }
    gpsdata_data_t *item = NULL;
    do {
        GPSUTILS_DEBUG("Trying to save current message to list of items\n");
        // saves a single message to the items list
        item = calloc(1, sizeof(*item));
        if (!item) {
            GPSUTILS_ERROR_NOMEM(sizeof(*item));
            rc = -1;
            break;
        }
        gpsdata_initialize(item);
        // different messages have different handling
        const char *msgid_str = gpsdata_msgid_tostring(fsm->_msgid);
        item->msgid = fsm->_msgid;
        switch (fsm->_msgid) {
        case GPSDATA_MSGID_GPGGA:
            memcpy(&(item->latitude), &(fsm->_lat), sizeof(fsm->_lat));
            memcpy(&(item->longitude), &(fsm->_lon), sizeof(fsm->_lon));
            // a valid rmc_tm
            if (fsm->rmc_tm.tm_year > 0) {
                fsm->_tm.tm_year = fsm->rmc_tm.tm_year;
                fsm->_tm.tm_mon = fsm->rmc_tm.tm_mon;
                fsm->_tm.tm_mday = fsm->rmc_tm.tm_mday;
                if (gpsutils_get_timeval(&(fsm->_tm), fsm->_tm_msec,
                    &(item->timestamp)) < 0) {
                    GPSUTILS_WARN("Message %s: invalid timestamp conversion\n",
                        msgid_str);
                    item->is_valid_timestamp = false;
                } else {
                    item->is_valid_timestamp = true;
                }
            } else {
                // we still convert just in case we have knowledge of the date
                // external to this library
                gpsutils_get_timeval(&(fsm->_tm), fsm->_tm_msec,
                    &(item->timestamp));
                item->is_valid_timestamp = false;
                GPSUTILS_WARN("RMC date is unavailable, so timestamp is considered invalid for message %s\n", msgid_str);
            }
            item->posfix = fsm->_posfix;
            item->num_satellites = fsm->_num_sats;
            item->altitude_meters = fsm->_altitude;
            break;
        case GPSDATA_MSGID_GPGSA:
            GPSUTILS_DEBUG("Message %s: ignoring until needed in the future\n",
                msgid_str);
            rc = 1; //ignore
            break;
        case GPSDATA_MSGID_GPGSV:
            GPSUTILS_DEBUG("Message %s: ignoring until needed in the future\n",
                msgid_str);
            rc = 1; //ignore
            break;
        case GPSDATA_MSGID_GPRMC:
            memcpy(&(item->latitude), &(fsm->_lat), sizeof(fsm->_lat));
            memcpy(&(item->longitude), &(fsm->_lon), sizeof(fsm->_lon));
            item->mode = fsm->mode_common;
            if (fsm->_rmc_valid) {
                item->speed_knots = fsm->_speed_knots;
                item->course_degrees = fsm->_course_degrees;
                if (gpsutils_get_timeval(&(fsm->_tm), fsm->_tm_msec,
                    &(item->timestamp)) < 0) {
                    GPSUTILS_WARN("Message %s: invalid timestamp conversion\n",
                        msgid_str);
                    item->is_valid_timestamp = false;
                } else {
                    item->is_valid_timestamp = true;
                    // update this delta timestamp storage
                    memcpy(&(fsm->rmc_tm), &(fsm->_tm), sizeof(fsm->_tm));
                }
            } else {
                GPSUTILS_WARN("%s message is not valid. Ignoring\n", msgid_str);
                rc = 1;//ignore
            }
            break;
        case GPSDATA_MSGID_GPVTG:
            item->mode = fsm->mode_common;
            item->course_degrees = fsm->_course_degrees;
            item->heading_degrees = fsm->_heading_degrees;
            item->speed_knots = fsm->_speed_knots;
            item->speed_kmph = fsm->_speed_kmph;
            break;
        case GPSDATA_MSGID_PGTOP:
            GPSUTILS_DEBUG("Received message ID %s. Antenna state: %d CommandID: %d\n",
                msgid_str, fsm->_pgtop_value, fsm->_pgtop_fntype);
            if (fsm->_pgtop_fntype == 11) {
                item->msgid = fsm->_msgid;
                switch (fsm->_pgtop_value) {
                case 1: item->antenna_status = GPSDATA_ANTENNA_SHORTED; break;
                case 2: item->antenna_status = GPSDATA_ANTENNA_INTERNAL; break;
                case 3: item->antenna_status = GPSDATA_ANTENNA_ACTIVE; break;
                default:
                    item->antenna_status = GPSDATA_ANTENNA_UNSET;
                    GPSUTILS_WARN("Message %s: Antenna status cannot be %d. Ignoring\n",
                                msgid_str, fsm->_pgtop_value);
                    rc = 1; //ignore
                    break;
                }
            } else {
                GPSUTILS_WARN("Message %s: ignoring since %d is unsupported command ID\n",
                    msgid_str, fsm->_pgtop_fntype);
                rc = 1;//ignore
            }
            break;
        case GPSDATA_MSGID_PMTK:
            GPSUTILS_DEBUG("Received message ID %s. Ignoring\n", msgid_str);
            rc = 1;//ignore
            break;
        default:
            item->msgid = GPSDATA_MSGID_UNSET;
            GPSUTILS_WARN("Unsupported message ID parsed: %d(%s)\n",
                    fsm->_msgid, msgid_str);
            rc = -1;
            break;
        }
    } while (0);
    // on failure or ignore message free the item
    if (rc < 0 || rc > 0) {
        GPSUTILS_FREE(item);
    } else {
        // add to items list
        LL_APPEND(fsm->items, item);
    }
    return rc;
}

static void gpsdata_parser_internal_init(gpsdata_parser_t *fsm)
{
    //initialize
    if (fsm) {
        fsm->p = fsm->pe = fsm->eof = NULL;
        fsm->cs = 0;
        fsm->items = NULL;
        memset(&(fsm->rmc_tm), 0, sizeof(fsm->rmc_tm));
        if (fsm->clean_state)
            fsm->clean_state(fsm);
        %% write init;
    }
}

static void gpsdata_parser_internal_fini(gpsdata_parser_t *fsm)
{
    if (fsm && fsm->items) {
        gpsdata_list_free(&(fsm->items));
    }
}

static void gpsdata_parser_internal_reset(gpsdata_parser_t *fsm)
{
    if (fsm) {
        if (fsm->cs == %%{ write error; }%%) {
            fsm->cs = %%{write start; }%%;
        }
        if (fsm->items) {
            gpsdata_list_free(&(fsm->items));
            fsm->items = NULL;
        }
        memset(&(fsm->rmc_tm), 0, sizeof(fsm->rmc_tm));
    }
}

static int gpsdata_parser_internal_execute(gpsdata_parser_t *fsm, const char *bytes, size_t len)
{
    if (!fsm || !bytes || len == 0) {
        return -1;
    }
    if (fsm->cs == %%{ write first_final; }%%) {
        // parsing has not begun yet
        // find the first $ sign
        bool found = false;
        GPSUTILS_DEBUG("Finding the first message in the stream. ");
        for (size_t i = 0; i < len; ++i) {
            if (bytes[i] == '$') {
                fsm->p = &bytes[i];
                fsm->pe = bytes + len;
                GPSUTILS_NONE("found message at index %zu\n", i);
                found = true;
                break;
            }
        }
        if (!found) {
            GPSUTILS_WARN("First '$' sign message not found, trying to parse anyway\n");
            fsm->p = bytes;
            fsm->pe = bytes + len;
        }
    } else {
        fsm->p = bytes;
        fsm->pe = bytes + len;
    }
    %% write exec;
    if (fsm->cs == %%{ write error; }%%) {
        size_t errlen = fsm->pe - fsm->p;
        GPSUTILS_ERROR("Error in parsing. fsm->cs: %d\t Len: %zu Buffer: \n",
                       fsm->cs, errlen);
        gpsutils_hex_dump((const uint8_t *)fsm->p, errlen, GPSUTILS_LOG_PTR);
        return -1;
    }
    return 0;
}

gpsdata_parser_t *gpsdata_parser_create()
{
    gpsdata_parser_t *fsm = NULL;
    fsm = calloc(1, sizeof(gpsdata_parser_t));
    if (fsm != NULL) {
        fsm->init = gpsdata_parser_internal_init;
        fsm->reset = gpsdata_parser_internal_reset;
        fsm->fini = gpsdata_parser_internal_fini;
        fsm->execute = gpsdata_parser_internal_execute;
        fsm->clean_state = gpsdata_parser_internal_clean_state;
        fsm->dump_state = gpsdata_parser_internal_dump_state;
        fsm->save = gpsdata_parser_internal_save;
        if (fsm->init) {
            fsm->init(fsm);
        }
    } else {
        GPSUTILS_ERROR_NOMEM(sizeof(gpsdata_parser_t));
    }
    return fsm;
}

void gpsdata_parser_dump_state(const gpsdata_parser_t *fsm, FILE *fp)
{
    if (fsm && fsm->dump_state)
        fsm->dump_state(fsm, fp);
}

void gpsdata_parser_free(gpsdata_parser_t *fsm)
{
    if (fsm) {
        if (fsm->fini)
            fsm->fini(fsm);
        GPSUTILS_FREE(fsm);
    }
}

void gpsdata_parser_reset(gpsdata_parser_t *fsm)
{
    if (fsm) {
        if (fsm->reset)
            fsm->reset(fsm);
        if (fsm->clean_state)
            fsm->clean_state(fsm);
    }
}

int gpsdata_parser_parse(gpsdata_parser_t *fsm,
            const char *data, size_t len,
            gpsdata_data_t **outp, size_t *onum)
{
    if (!fsm || !data || len == 0) {
        GPSUTILS_DEBUG("Invalid input arguments\n");
        return -1;
    }
    if (!fsm->execute) {
        GPSUTILS_ERROR("Invalid function setup for parsing\n");
        return -1;
    }
    int rc = fsm->execute(fsm, data, len);
    if (rc < 0) {
        GPSUTILS_ERROR("Failed to parse data\n");
        if (fsm->dump_state)
            fsm->dump_state(fsm, GPSUTILS_LOG_PTR);
        return rc;
    }
    if (outp) {
        if (fsm->items) {
            ssize_t count = gpsdata_list_count(fsm->items);
            if (onum && count >= 0)
                *onum = (size_t)count;
            GPSUTILS_DEBUG("adding %zd message items to the output list\n", count);
            LL_CONCAT(*outp, fsm->items);
            fsm->items = NULL;
        }
    } else {
        GPSUTILS_DEBUG("No list outp given, cleaning up parsed message items\n");
        gpsdata_list_free(&(fsm->items));
        fsm->items = NULL;
        if (onum)
            *onum = 0;
    }
    return rc;
}

