
#line 1 "gpsparser.rl"
/*
 * Copyright: 2015-2019. Stealthy Labs LLC. All Rights Reserved.
 * Date: 5th July 2019
 * Software: libgps_mt3339
 */
#ifndef _DEFAULT_SOURCE
    #define _DEFAULT_SOURCE
#endif
#include <gpsconfig.h>
#include <gpsutils.h>
#include <gpsdata.h>
#ifdef HAVE_TIME_H
    #include <time.h>
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

    struct tm _tm;
    uint32_t _tm_msec;
    gpsdata_latlon_t _lll;
    uint32_t _tmp_real[4];
    float _tmp_float;
    uint32_t _calc_checksum;
    uint32_t _checksum;

    gpsdata_msgid_t msgid;
    gpsdata_latlon_t _lat;
    gpsdata_latlon_t _lon;
    gpsdata_posfix_t _posfix;
    gpsdata_mode_t mode1;
    gpsdata_mode_t mode2;
    gpsdata_mode_t mode_common;
    uint8_t _num_sats;//0 - 14
    uint8_t _num_msg_max;//values 1-3
    uint8_t _num_msg_idx;//values 0-2 as we convert it to 0-based indexing
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
    
    
    struct gpsdata_parser_gsv_t {
        uint8_t satellite_id; // values 1-32
        uint8_t elevation; // values 0-90
        bool is_elevation_null;
        uint16_t azimuth; // values 0-359
        bool is_azimuth_null;
        uint8_t snr_cno; // null, 0-99.
        bool is_snr_cno_null; // true if null
    } _gsv_sats[4];
    uint8_t _gsv_sat_idx;
};


#line 515 "gpsparser.rl"



#line 87 "gpsparser.c"
static const char _gpsdata_parser_fsm_actions[] = {
	0, 1, 0, 1, 75, 1, 76, 1, 
	78, 1, 79, 2, 1, 78, 2, 2, 
	78, 2, 3, 78, 2, 4, 78, 2, 
	5, 78, 2, 6, 78, 2, 7, 78, 
	2, 9, 78, 2, 12, 78, 2, 14, 
	78, 2, 16, 78, 2, 19, 78, 2, 
	22, 78, 2, 25, 78, 2, 28, 78, 
	2, 29, 78, 2, 32, 78, 2, 33, 
	78, 2, 35, 78, 2, 36, 78, 2, 
	37, 78, 2, 38, 78, 2, 39, 78, 
	2, 40, 78, 2, 41, 78, 2, 42, 
	78, 2, 43, 78, 2, 44, 78, 2, 
	45, 78, 2, 46, 78, 2, 47, 78, 
	2, 48, 78, 2, 49, 78, 2, 50, 
	78, 2, 51, 78, 2, 53, 78, 2, 
	54, 78, 2, 55, 78, 2, 56, 78, 
	2, 57, 78, 2, 58, 78, 2, 59, 
	62, 2, 59, 75, 2, 59, 78, 2, 
	68, 78, 2, 69, 78, 2, 70, 78, 
	2, 71, 78, 2, 72, 78, 2, 73, 
	78, 2, 74, 78, 2, 75, 78, 2, 
	77, 78, 2, 79, 0, 3, 8, 10, 
	78, 3, 11, 13, 78, 3, 18, 20, 
	78, 3, 21, 23, 78, 3, 24, 26, 
	78, 3, 27, 30, 78, 3, 31, 33, 
	78, 3, 31, 34, 78, 3, 55, 56, 
	78, 3, 56, 52, 78, 3, 59, 43, 
	78, 3, 59, 60, 78, 3, 59, 61, 
	78, 3, 59, 62, 78, 3, 59, 63, 
	78, 3, 59, 64, 78, 3, 59, 65, 
	78, 3, 59, 66, 78, 3, 59, 67, 
	78, 3, 59, 68, 78, 3, 59, 69, 
	78, 3, 59, 72, 78, 3, 59, 73, 
	78, 3, 59, 74, 78, 3, 59, 75, 
	78, 4, 14, 15, 17, 78, 4, 55, 
	56, 52, 78
};

static const short _gpsdata_parser_fsm_key_offsets[] = {
	0, 0, 2, 3, 6, 8, 9, 10, 
	12, 14, 16, 18, 20, 22, 23, 25, 
	27, 29, 30, 32, 34, 36, 38, 39, 
	41, 43, 45, 47, 48, 50, 51, 53, 
	55, 57, 59, 61, 62, 64, 66, 68, 
	70, 71, 73, 74, 76, 77, 79, 82, 
	86, 89, 91, 94, 98, 101, 103, 106, 
	107, 108, 112, 115, 117, 120, 121, 122, 
	127, 128, 134, 140, 143, 145, 148, 152, 
	156, 160, 164, 165, 167, 168, 170, 171, 
	173, 174, 178, 182, 186, 190, 194, 198, 
	202, 206, 210, 214, 218, 222, 226, 229, 
	231, 234, 238, 241, 243, 246, 250, 253, 
	255, 259, 264, 268, 272, 274, 277, 279, 
	282, 284, 287, 289, 292, 294, 297, 299, 
	302, 304, 307, 309, 312, 314, 317, 319, 
	322, 324, 327, 329, 332, 333, 335, 336, 
	338, 339, 341, 344, 348, 352, 356, 361, 
	366, 370, 374, 379, 384, 388, 392, 397, 
	402, 406, 410, 415, 417, 421, 423, 426, 
	428, 431, 433, 436, 438, 442, 444, 447, 
	449, 452, 454, 457, 459, 463, 465, 468, 
	470, 473, 475, 478, 480, 484, 486, 489, 
	491, 494, 496, 499, 500, 501, 502, 503, 
	505, 507, 509, 511, 513, 515, 516, 518, 
	520, 522, 523, 525, 526, 528, 530, 532, 
	534, 535, 537, 539, 541, 543, 544, 546, 
	547, 549, 551, 553, 555, 557, 558, 560, 
	562, 564, 566, 567, 569, 570, 574, 577, 
	579, 582, 586, 589, 591, 594, 596, 598, 
	600, 602, 604, 606, 607, 612, 615, 618, 
	620, 621, 624, 626, 629, 633, 637, 641, 
	642, 643, 644, 648, 651, 653, 656, 657, 
	658, 663, 664, 665, 669, 672, 674, 677, 
	678, 679, 683, 686, 688, 691, 692, 696, 
	700, 703, 705, 708, 712, 716, 718, 719, 
	720, 721, 722, 725, 727, 730, 732, 733, 
	734, 735, 736, 737, 742
};

static const char _gpsdata_parser_fsm_trans_keys[] = {
	71, 80, 80, 71, 82, 86, 71, 83, 
	65, 44, 48, 57, 48, 57, 48, 57, 
	48, 57, 48, 57, 48, 57, 46, 48, 
	57, 48, 57, 48, 57, 44, 48, 57, 
	48, 57, 48, 57, 48, 57, 46, 48, 
	57, 48, 57, 48, 57, 48, 57, 44, 
	78, 83, 44, 48, 57, 48, 57, 48, 
	57, 48, 57, 48, 57, 46, 48, 57, 
	48, 57, 48, 57, 48, 57, 44, 69, 
	87, 44, 48, 50, 44, 48, 57, 44, 
	48, 57, 45, 46, 48, 57, 46, 48, 
	57, 48, 57, 44, 48, 57, 45, 46, 
	48, 57, 46, 48, 57, 48, 57, 44, 
	48, 57, 77, 44, 45, 46, 48, 57, 
	46, 48, 57, 48, 57, 44, 48, 57, 
	77, 44, 44, 45, 46, 48, 57, 42, 
	48, 57, 65, 70, 97, 102, 48, 57, 
	65, 70, 97, 102, 46, 48, 57, 48, 
	57, 44, 48, 57, 44, 46, 48, 57, 
	44, 46, 48, 57, 44, 46, 48, 57, 
	44, 46, 48, 57, 44, 65, 86, 44, 
	65, 77, 44, 49, 51, 44, 44, 45, 
	48, 57, 44, 45, 48, 57, 44, 45, 
	48, 57, 44, 45, 48, 57, 44, 45, 
	48, 57, 44, 45, 48, 57, 44, 45, 
	48, 57, 44, 45, 48, 57, 44, 45, 
	48, 57, 44, 45, 48, 57, 44, 45, 
	48, 57, 44, 45, 48, 57, 45, 46, 
	48, 57, 46, 48, 57, 48, 57, 44, 
	48, 57, 45, 46, 48, 57, 46, 48, 
	57, 48, 57, 44, 48, 57, 45, 46, 
	48, 57, 46, 48, 57, 48, 57, 42, 
	44, 48, 57, 42, 44, 46, 48, 57, 
	44, 46, 48, 57, 44, 46, 48, 57, 
	48, 57, 44, 48, 57, 48, 57, 44, 
	48, 57, 48, 57, 44, 48, 57, 48, 
	57, 44, 48, 57, 48, 57, 44, 48, 
	57, 48, 57, 44, 48, 57, 48, 57, 
	44, 48, 57, 48, 57, 44, 48, 57, 
	48, 57, 44, 48, 57, 48, 57, 44, 
	48, 57, 48, 57, 44, 48, 57, 48, 
	57, 44, 48, 57, 44, 49, 51, 44, 
	49, 51, 44, 48, 57, 44, 48, 57, 
	44, 45, 48, 57, 44, 45, 48, 57, 
	44, 45, 48, 57, 42, 44, 45, 48, 
	57, 42, 44, 45, 48, 57, 44, 45, 
	48, 57, 44, 45, 48, 57, 42, 44, 
	45, 48, 57, 42, 44, 45, 48, 57, 
	44, 45, 48, 57, 44, 45, 48, 57, 
	42, 44, 45, 48, 57, 42, 44, 45, 
	48, 57, 44, 45, 48, 57, 44, 45, 
	48, 57, 42, 44, 45, 48, 57, 48, 
	57, 42, 44, 48, 57, 48, 57, 44, 
	48, 57, 48, 57, 44, 48, 57, 48, 
	57, 44, 48, 57, 48, 57, 42, 44, 
	48, 57, 48, 57, 44, 48, 57, 48, 
	57, 44, 48, 57, 48, 57, 44, 48, 
	57, 48, 57, 42, 44, 48, 57, 48, 
	57, 44, 48, 57, 48, 57, 44, 48, 
	57, 48, 57, 44, 48, 57, 48, 57, 
	42, 44, 48, 57, 48, 57, 44, 48, 
	57, 48, 57, 44, 48, 57, 48, 57, 
	44, 48, 57, 44, 77, 67, 44, 48, 
	57, 48, 57, 48, 57, 48, 57, 48, 
	57, 48, 57, 46, 48, 57, 48, 57, 
	48, 57, 44, 65, 86, 44, 48, 57, 
	48, 57, 48, 57, 48, 57, 46, 48, 
	57, 48, 57, 48, 57, 48, 57, 44, 
	78, 83, 44, 48, 57, 48, 57, 48, 
	57, 48, 57, 48, 57, 46, 48, 57, 
	48, 57, 48, 57, 48, 57, 44, 69, 
	87, 44, 45, 46, 48, 57, 46, 48, 
	57, 48, 57, 44, 48, 57, 45, 46, 
	48, 57, 46, 48, 57, 48, 57, 44, 
	48, 57, 48, 57, 48, 57, 48, 57, 
	48, 57, 48, 57, 48, 57, 44, 44, 
	45, 46, 48, 57, 44, 69, 87, 65, 
	68, 69, 42, 44, 44, 46, 48, 57, 
	48, 57, 44, 48, 57, 44, 46, 48, 
	57, 44, 46, 48, 57, 44, 46, 48, 
	57, 84, 71, 44, 45, 46, 48, 57, 
	46, 48, 57, 48, 57, 44, 48, 57, 
	84, 44, 44, 45, 46, 48, 57, 77, 
	44, 45, 46, 48, 57, 46, 48, 57, 
	48, 57, 44, 48, 57, 78, 44, 45, 
	46, 48, 57, 46, 48, 57, 48, 57, 
	44, 48, 57, 75, 44, 46, 48, 57, 
	44, 46, 48, 57, 46, 48, 57, 48, 
	57, 44, 48, 57, 44, 46, 48, 57, 
	44, 46, 48, 57, 71, 77, 84, 79, 
	80, 44, 45, 48, 57, 48, 57, 44, 
	48, 57, 49, 51, 84, 75, 49, 48, 
	51, 0, 32, 36, 9, 13, 0, 32, 
	36, 9, 13, 0
};

static const char _gpsdata_parser_fsm_single_lengths[] = {
	0, 2, 1, 3, 2, 1, 1, 0, 
	0, 0, 0, 0, 0, 1, 0, 0, 
	0, 1, 0, 0, 0, 0, 1, 0, 
	0, 0, 0, 1, 2, 1, 0, 0, 
	0, 0, 0, 1, 0, 0, 0, 0, 
	1, 2, 1, 0, 1, 0, 1, 2, 
	1, 0, 1, 2, 1, 0, 1, 1, 
	1, 2, 1, 0, 1, 1, 1, 3, 
	1, 0, 0, 1, 0, 1, 2, 2, 
	2, 2, 1, 2, 1, 2, 1, 0, 
	1, 2, 2, 2, 2, 2, 2, 2, 
	2, 2, 2, 2, 2, 2, 1, 0, 
	1, 2, 1, 0, 1, 2, 1, 0, 
	2, 3, 2, 2, 0, 1, 0, 1, 
	0, 1, 0, 1, 0, 1, 0, 1, 
	0, 1, 0, 1, 0, 1, 0, 1, 
	0, 1, 0, 1, 1, 0, 1, 0, 
	1, 0, 1, 2, 2, 2, 3, 3, 
	2, 2, 3, 3, 2, 2, 3, 3, 
	2, 2, 3, 0, 2, 0, 1, 0, 
	1, 0, 1, 0, 2, 0, 1, 0, 
	1, 0, 1, 0, 2, 0, 1, 0, 
	1, 0, 1, 0, 2, 0, 1, 0, 
	1, 0, 1, 1, 1, 1, 1, 0, 
	0, 0, 0, 0, 0, 1, 0, 0, 
	0, 1, 2, 1, 0, 0, 0, 0, 
	1, 0, 0, 0, 0, 1, 2, 1, 
	0, 0, 0, 0, 0, 1, 0, 0, 
	0, 0, 1, 2, 1, 2, 1, 0, 
	1, 2, 1, 0, 1, 0, 0, 0, 
	0, 0, 0, 1, 3, 3, 1, 2, 
	1, 1, 0, 1, 2, 2, 2, 1, 
	1, 1, 2, 1, 0, 1, 1, 1, 
	3, 1, 1, 2, 1, 0, 1, 1, 
	1, 2, 1, 0, 1, 1, 2, 2, 
	1, 0, 1, 2, 2, 2, 1, 1, 
	1, 1, 1, 0, 1, 0, 1, 1, 
	1, 1, 1, 3, 3
};

static const char _gpsdata_parser_fsm_range_lengths[] = {
	0, 0, 0, 0, 0, 0, 0, 1, 
	1, 1, 1, 1, 1, 0, 1, 1, 
	1, 0, 1, 1, 1, 1, 0, 1, 
	1, 1, 1, 0, 0, 0, 1, 1, 
	1, 1, 1, 0, 1, 1, 1, 1, 
	0, 0, 0, 1, 0, 1, 1, 1, 
	1, 1, 1, 1, 1, 1, 1, 0, 
	0, 1, 1, 1, 1, 0, 0, 1, 
	0, 3, 3, 1, 1, 1, 1, 1, 
	1, 1, 0, 0, 0, 0, 0, 1, 
	0, 1, 1, 1, 1, 1, 1, 1, 
	1, 1, 1, 1, 1, 1, 1, 1, 
	1, 1, 1, 1, 1, 1, 1, 1, 
	1, 1, 1, 1, 1, 1, 1, 1, 
	1, 1, 1, 1, 1, 1, 1, 1, 
	1, 1, 1, 1, 1, 1, 1, 1, 
	1, 1, 1, 1, 0, 1, 0, 1, 
	0, 1, 1, 1, 1, 1, 1, 1, 
	1, 1, 1, 1, 1, 1, 1, 1, 
	1, 1, 1, 1, 1, 1, 1, 1, 
	1, 1, 1, 1, 1, 1, 1, 1, 
	1, 1, 1, 1, 1, 1, 1, 1, 
	1, 1, 1, 1, 1, 1, 1, 1, 
	1, 1, 1, 0, 0, 0, 0, 1, 
	1, 1, 1, 1, 1, 0, 1, 1, 
	1, 0, 0, 0, 1, 1, 1, 1, 
	0, 1, 1, 1, 1, 0, 0, 0, 
	1, 1, 1, 1, 1, 0, 1, 1, 
	1, 1, 0, 0, 0, 1, 1, 1, 
	1, 1, 1, 1, 1, 1, 1, 1, 
	1, 1, 1, 0, 1, 0, 1, 0, 
	0, 1, 1, 1, 1, 1, 1, 0, 
	0, 0, 1, 1, 1, 1, 0, 0, 
	1, 0, 0, 1, 1, 1, 1, 0, 
	0, 1, 1, 1, 1, 0, 1, 1, 
	1, 1, 1, 1, 1, 0, 0, 0, 
	0, 0, 1, 1, 1, 1, 0, 0, 
	0, 0, 0, 1, 1
};

static const short _gpsdata_parser_fsm_index_offsets[] = {
	0, 0, 3, 5, 9, 12, 14, 16, 
	18, 20, 22, 24, 26, 28, 30, 32, 
	34, 36, 38, 40, 42, 44, 46, 48, 
	50, 52, 54, 56, 58, 61, 63, 65, 
	67, 69, 71, 73, 75, 77, 79, 81, 
	83, 85, 88, 90, 92, 94, 96, 99, 
	103, 106, 108, 111, 115, 118, 120, 123, 
	125, 127, 131, 134, 136, 139, 141, 143, 
	148, 150, 154, 158, 161, 163, 166, 170, 
	174, 178, 182, 184, 187, 189, 192, 194, 
	196, 198, 202, 206, 210, 214, 218, 222, 
	226, 230, 234, 238, 242, 246, 250, 253, 
	255, 258, 262, 265, 267, 270, 274, 277, 
	279, 283, 288, 292, 296, 298, 301, 303, 
	306, 308, 311, 313, 316, 318, 321, 323, 
	326, 328, 331, 333, 336, 338, 341, 343, 
	346, 348, 351, 353, 356, 358, 360, 362, 
	364, 366, 368, 371, 375, 379, 383, 388, 
	393, 397, 401, 406, 411, 415, 419, 424, 
	429, 433, 437, 442, 444, 448, 450, 453, 
	455, 458, 460, 463, 465, 469, 471, 474, 
	476, 479, 481, 484, 486, 490, 492, 495, 
	497, 500, 502, 505, 507, 511, 513, 516, 
	518, 521, 523, 526, 528, 530, 532, 534, 
	536, 538, 540, 542, 544, 546, 548, 550, 
	552, 554, 556, 559, 561, 563, 565, 567, 
	569, 571, 573, 575, 577, 579, 581, 584, 
	586, 588, 590, 592, 594, 596, 598, 600, 
	602, 604, 606, 608, 611, 613, 617, 620, 
	622, 625, 629, 632, 634, 637, 639, 641, 
	643, 645, 647, 649, 651, 656, 660, 663, 
	666, 668, 671, 673, 676, 680, 684, 688, 
	690, 692, 694, 698, 701, 703, 706, 708, 
	710, 715, 717, 719, 723, 726, 728, 731, 
	733, 735, 739, 742, 744, 747, 749, 753, 
	757, 760, 762, 765, 769, 773, 776, 778, 
	780, 782, 784, 787, 789, 792, 794, 796, 
	798, 800, 802, 804, 809
};

static const short _gpsdata_parser_fsm_indicies[] = {
	0, 2, 1, 3, 1, 4, 5, 6, 
	1, 7, 8, 1, 9, 1, 10, 1, 
	11, 1, 12, 1, 13, 1, 14, 1, 
	15, 1, 16, 1, 17, 1, 18, 1, 
	19, 1, 20, 1, 21, 1, 22, 1, 
	23, 1, 24, 1, 25, 1, 26, 1, 
	27, 1, 28, 1, 29, 1, 30, 1, 
	31, 1, 32, 32, 1, 33, 1, 34, 
	1, 35, 1, 36, 1, 37, 1, 38, 
	1, 39, 1, 40, 1, 41, 1, 42, 
	1, 43, 1, 44, 1, 45, 45, 1, 
	46, 1, 47, 1, 48, 1, 49, 1, 
	50, 51, 1, 52, 53, 54, 1, 53, 
	54, 1, 55, 1, 56, 55, 1, 57, 
	58, 59, 1, 58, 59, 1, 60, 1, 
	61, 60, 1, 62, 1, 63, 1, 64, 
	65, 66, 1, 65, 66, 1, 67, 1, 
	68, 67, 1, 69, 1, 70, 1, 71, 
	72, 73, 74, 1, 75, 1, 76, 76, 
	76, 1, 77, 77, 77, 1, 73, 74, 
	1, 78, 1, 79, 78, 1, 79, 80, 
	81, 1, 68, 82, 83, 1, 61, 84, 
	85, 1, 56, 86, 87, 1, 50, 1, 
	88, 89, 1, 90, 1, 91, 91, 1, 
	92, 1, 93, 1, 94, 1, 95, 96, 
	97, 1, 98, 99, 100, 1, 101, 102, 
	103, 1, 104, 105, 106, 1, 107, 108, 
	109, 1, 110, 111, 112, 1, 113, 114, 
	115, 1, 116, 117, 118, 1, 119, 120, 
	121, 1, 122, 123, 124, 1, 125, 126, 
	127, 1, 128, 129, 130, 1, 131, 132, 
	133, 1, 132, 133, 1, 134, 1, 135, 
	134, 1, 136, 137, 138, 1, 137, 138, 
	1, 139, 1, 140, 139, 1, 141, 142, 
	143, 1, 142, 143, 1, 144, 1, 145, 
	146, 144, 1, 145, 146, 147, 148, 1, 
	140, 149, 150, 1, 135, 151, 152, 1, 
	130, 1, 153, 154, 1, 127, 1, 155, 
	156, 1, 124, 1, 157, 158, 1, 121, 
	1, 159, 160, 1, 118, 1, 161, 162, 
	1, 115, 1, 163, 164, 1, 112, 1, 
	165, 166, 1, 109, 1, 167, 168, 1, 
	106, 1, 169, 170, 1, 103, 1, 171, 
	172, 1, 100, 1, 173, 174, 1, 97, 
	1, 175, 176, 1, 177, 1, 178, 1, 
	179, 1, 180, 1, 181, 1, 182, 1, 
	183, 184, 1, 185, 186, 187, 1, 188, 
	189, 190, 1, 191, 192, 193, 1, 194, 
	195, 196, 197, 1, 75, 198, 199, 200, 
	1, 201, 202, 203, 1, 204, 205, 206, 
	1, 194, 207, 208, 209, 1, 75, 210, 
	211, 212, 1, 213, 214, 215, 1, 216, 
	217, 218, 1, 194, 219, 220, 221, 1, 
	75, 222, 223, 224, 1, 225, 226, 227, 
	1, 228, 229, 230, 1, 194, 231, 232, 
	233, 1, 233, 1, 234, 235, 236, 1, 
	230, 1, 237, 238, 1, 227, 1, 239, 
	240, 1, 224, 1, 241, 242, 1, 221, 
	1, 234, 243, 244, 1, 218, 1, 245, 
	246, 1, 215, 1, 247, 248, 1, 212, 
	1, 249, 250, 1, 209, 1, 234, 251, 
	252, 1, 206, 1, 253, 254, 1, 203, 
	1, 255, 256, 1, 200, 1, 257, 258, 
	1, 197, 1, 234, 259, 260, 1, 193, 
	1, 261, 262, 1, 190, 1, 263, 264, 
	1, 187, 1, 265, 266, 1, 183, 1, 
	267, 1, 268, 1, 269, 1, 270, 1, 
	271, 1, 272, 1, 273, 1, 274, 1, 
	275, 1, 276, 1, 277, 1, 278, 1, 
	279, 1, 280, 1, 281, 281, 1, 282, 
	1, 283, 1, 284, 1, 285, 1, 286, 
	1, 287, 1, 288, 1, 289, 1, 290, 
	1, 291, 1, 292, 1, 293, 293, 1, 
	294, 1, 295, 1, 296, 1, 297, 1, 
	298, 1, 299, 1, 300, 1, 301, 1, 
	302, 1, 303, 1, 304, 1, 305, 1, 
	306, 306, 1, 307, 1, 308, 309, 310, 
	1, 309, 310, 1, 311, 1, 312, 311, 
	1, 313, 314, 315, 1, 314, 315, 1, 
	316, 1, 317, 316, 1, 318, 1, 319, 
	1, 320, 1, 321, 1, 322, 1, 323, 
	1, 324, 1, 325, 326, 327, 328, 1, 
	329, 330, 330, 1, 331, 331, 1, 75, 
	71, 1, 329, 1, 327, 328, 1, 332, 
	1, 333, 332, 1, 333, 334, 335, 1, 
	317, 336, 337, 1, 312, 338, 339, 1, 
	340, 1, 341, 1, 342, 1, 343, 344, 
	345, 1, 344, 345, 1, 346, 1, 347, 
	346, 1, 348, 1, 349, 1, 350, 351, 
	352, 353, 1, 354, 1, 355, 1, 356, 
	357, 358, 1, 357, 358, 1, 359, 1, 
	360, 359, 1, 361, 1, 362, 1, 363, 
	364, 365, 1, 364, 365, 1, 366, 1, 
	367, 366, 1, 368, 1, 367, 369, 370, 
	1, 360, 371, 372, 1, 352, 353, 1, 
	373, 1, 374, 373, 1, 374, 375, 376, 
	1, 347, 377, 378, 1, 379, 380, 1, 
	381, 1, 382, 1, 383, 1, 384, 1, 
	385, 386, 1, 386, 1, 387, 388, 1, 
	389, 1, 390, 1, 391, 1, 392, 1, 
	393, 1, 71, 1, 394, 394, 395, 394, 
	1, 396, 396, 397, 396, 1, 0
};

static const short _gpsdata_parser_fsm_trans_targs[] = {
	2, 0, 285, 3, 4, 188, 255, 5, 
	75, 6, 7, 8, 9, 10, 11, 12, 
	13, 14, 15, 16, 17, 18, 19, 20, 
	21, 22, 23, 24, 25, 26, 27, 28, 
	29, 30, 31, 32, 33, 34, 35, 36, 
	37, 38, 39, 40, 41, 42, 43, 44, 
	45, 46, 47, 74, 48, 49, 73, 50, 
	51, 52, 53, 72, 54, 55, 56, 57, 
	58, 59, 71, 60, 61, 62, 63, 64, 
	67, 68, 70, 65, 66, 300, 69, 64, 
	68, 70, 59, 71, 53, 72, 49, 73, 
	76, 132, 77, 78, 79, 80, 81, 82, 
	130, 131, 83, 128, 129, 84, 126, 127, 
	85, 124, 125, 86, 122, 123, 87, 120, 
	121, 88, 118, 119, 89, 116, 117, 90, 
	114, 115, 91, 112, 113, 92, 110, 111, 
	93, 108, 109, 94, 95, 107, 96, 97, 
	98, 99, 106, 100, 101, 102, 103, 105, 
	104, 65, 64, 103, 105, 99, 106, 95, 
	107, 93, 109, 92, 111, 91, 113, 90, 
	115, 89, 117, 88, 119, 87, 121, 86, 
	123, 85, 125, 84, 127, 83, 129, 82, 
	131, 133, 134, 135, 136, 137, 138, 139, 
	187, 140, 185, 186, 141, 183, 184, 142, 
	181, 182, 65, 143, 179, 180, 144, 177, 
	178, 145, 175, 176, 146, 173, 174, 147, 
	171, 172, 148, 169, 170, 149, 167, 168, 
	150, 165, 166, 151, 163, 164, 152, 161, 
	162, 153, 159, 160, 154, 157, 158, 64, 
	155, 156, 65, 64, 156, 154, 158, 153, 
	160, 152, 162, 151, 164, 150, 166, 149, 
	168, 148, 170, 147, 172, 146, 174, 145, 
	176, 144, 178, 143, 180, 142, 182, 141, 
	184, 140, 186, 189, 190, 191, 192, 193, 
	194, 195, 196, 197, 198, 199, 200, 201, 
	202, 203, 204, 205, 206, 207, 208, 209, 
	210, 211, 212, 213, 214, 215, 216, 217, 
	218, 219, 220, 221, 222, 223, 224, 225, 
	226, 227, 228, 229, 230, 231, 254, 232, 
	233, 234, 235, 253, 236, 237, 238, 239, 
	240, 241, 242, 243, 244, 245, 249, 250, 
	252, 246, 248, 247, 251, 245, 250, 252, 
	235, 253, 231, 254, 256, 257, 258, 259, 
	260, 284, 261, 262, 263, 264, 265, 280, 
	281, 283, 266, 267, 268, 269, 279, 270, 
	271, 272, 273, 274, 275, 278, 276, 277, 
	248, 275, 278, 269, 279, 282, 265, 281, 
	283, 260, 284, 286, 294, 287, 288, 289, 
	290, 291, 292, 293, 292, 247, 295, 296, 
	297, 298, 299, 1, 299, 1
};

static const short _gpsdata_parser_fsm_trans_actions[] = {
	167, 0, 167, 7, 7, 7, 7, 7, 
	7, 11, 7, 181, 44, 185, 47, 189, 
	50, 7, 193, 56, 53, 7, 197, 59, 
	68, 65, 7, 80, 77, 74, 71, 83, 
	92, 7, 201, 62, 59, 68, 65, 7, 
	80, 77, 74, 71, 86, 95, 7, 101, 
	7, 119, 7, 119, 131, 122, 205, 128, 
	217, 131, 122, 205, 128, 229, 7, 7, 
	131, 122, 205, 128, 233, 7, 7, 7, 
	131, 122, 205, 0, 5, 5, 128, 140, 
	7, 125, 7, 125, 7, 125, 7, 125, 
	14, 17, 7, 104, 7, 107, 7, 146, 
	131, 205, 146, 131, 205, 146, 131, 205, 
	146, 131, 205, 146, 131, 205, 146, 131, 
	205, 146, 131, 205, 146, 131, 205, 146, 
	131, 205, 146, 131, 205, 146, 131, 205, 
	146, 131, 205, 131, 122, 205, 128, 221, 
	131, 122, 205, 128, 217, 131, 122, 205, 
	128, 134, 225, 7, 125, 7, 125, 7, 
	125, 253, 125, 253, 125, 253, 125, 253, 
	125, 253, 125, 253, 125, 253, 125, 253, 
	125, 253, 125, 253, 125, 253, 125, 253, 
	125, 7, 149, 7, 152, 7, 119, 7, 
	119, 155, 131, 205, 158, 131, 205, 161, 
	131, 205, 3, 164, 131, 205, 155, 131, 
	205, 158, 131, 205, 161, 131, 205, 164, 
	131, 205, 155, 131, 205, 158, 131, 205, 
	161, 131, 205, 164, 131, 205, 155, 131, 
	205, 158, 131, 205, 161, 131, 205, 164, 
	131, 205, 137, 269, 125, 265, 125, 261, 
	125, 257, 125, 269, 125, 265, 125, 261, 
	125, 257, 125, 269, 125, 265, 125, 261, 
	125, 257, 125, 269, 125, 265, 125, 261, 
	125, 257, 125, 7, 20, 7, 181, 44, 
	185, 47, 189, 50, 7, 193, 56, 53, 
	7, 113, 7, 197, 59, 68, 65, 7, 
	80, 77, 74, 71, 83, 92, 7, 201, 
	62, 59, 68, 65, 7, 80, 77, 74, 
	71, 86, 95, 7, 131, 122, 205, 128, 
	237, 131, 122, 205, 128, 245, 173, 32, 
	177, 35, 273, 41, 38, 89, 131, 122, 
	205, 7, 98, 110, 128, 213, 7, 125, 
	7, 125, 7, 125, 7, 23, 7, 131, 
	122, 205, 128, 245, 7, 7, 143, 131, 
	122, 205, 7, 7, 131, 122, 205, 128, 
	237, 7, 7, 131, 122, 205, 128, 241, 
	7, 7, 125, 7, 125, 128, 249, 7, 
	125, 7, 125, 7, 7, 7, 7, 26, 
	7, 131, 278, 140, 209, 116, 7, 29, 
	7, 7, 0, 1, 9, 170
};

static const short _gpsdata_parser_fsm_eof_actions[] = {
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 9
};

static const int gpsdata_parser_fsm_start = 299;
static const int gpsdata_parser_fsm_first_final = 299;
static const int gpsdata_parser_fsm_error = 0;

static const int gpsdata_parser_fsm_en_main = 299;


#line 518 "gpsparser.rl"

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
    fsm->_num_sats = 0;
    fsm->mode1 = GPSDATA_MODE_UNSET;
    fsm->mode2 = GPSDATA_MODE_UNSET;
    fsm->mode_common = GPSDATA_MODE_UNSET;
    fsm->_posfix = GPSDATA_POSFIX_NOFIX;
    fsm->_rmc_valid = false;
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
    fsm->_num_msg_idx = 0;
    fsm->_num_msg_max = 0;
    fsm->_gsv_sat_idx = 0;
    memset(fsm->_gsv_sats, 0, sizeof(fsm->_gsv_sats));
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
        fprintf(fp, "msgid: %s\n", gpsdata_msgid_tostring(fsm->msgid));
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
        fprintf(fp, "GSV number of messages: %d 0-based index of this message: %d\n",
                fsm->_num_msg_max, fsm->_num_msg_idx);
        fprintf(fp, "GSV Satellites: found: %d\n", fsm->_gsv_sat_idx);
        for (uint8_t i = 0; i < fsm->_gsv_sat_idx && i < 4; ++i) {
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

static void gpsdata_parser_internal_init(gpsdata_parser_t *fsm)
{
    //initialize
    if (fsm) {
        fsm->p = fsm->pe = fsm->eof = NULL;
        fsm->cs = 0;
        if (fsm->clean_state)
            fsm->clean_state(fsm);
        
#line 759 "gpsparser.c"
	{
	 fsm->cs = gpsdata_parser_fsm_start;
	}

#line 629 "gpsparser.rl"
    }
}

static void gpsdata_parser_internal_fini(gpsdata_parser_t *fsm)
{
}

static void gpsdata_parser_internal_reset(gpsdata_parser_t *fsm)
{
    if (fsm) {
        if (fsm->cs == 0) {
            fsm->cs = 299;
        }
    }
}

static int gpsdata_parser_internal_execute(gpsdata_parser_t *fsm, const char *bytes, size_t len)
{
    if (!fsm || !bytes || len == 0) {
        return -1;
    }
    fsm->p = bytes;
    fsm->pe = bytes + len;
    
#line 789 "gpsparser.c"
	{
	int _klen;
	unsigned int _trans;
	const char *_acts;
	unsigned int _nacts;
	const char *_keys;

	if ( ( fsm->p) == ( fsm->pe) )
		goto _test_eof;
	if (  fsm->cs == 0 )
		goto _out;
_resume:
	_keys = _gpsdata_parser_fsm_trans_keys + _gpsdata_parser_fsm_key_offsets[ fsm->cs];
	_trans = _gpsdata_parser_fsm_index_offsets[ fsm->cs];

	_klen = _gpsdata_parser_fsm_single_lengths[ fsm->cs];
	if ( _klen > 0 ) {
		const char *_lower = _keys;
		const char *_mid;
		const char *_upper = _keys + _klen - 1;
		while (1) {
			if ( _upper < _lower )
				break;

			_mid = _lower + ((_upper-_lower) >> 1);
			if ( (*( fsm->p)) < *_mid )
				_upper = _mid - 1;
			else if ( (*( fsm->p)) > *_mid )
				_lower = _mid + 1;
			else {
				_trans += (unsigned int)(_mid - _keys);
				goto _match;
			}
		}
		_keys += _klen;
		_trans += _klen;
	}

	_klen = _gpsdata_parser_fsm_range_lengths[ fsm->cs];
	if ( _klen > 0 ) {
		const char *_lower = _keys;
		const char *_mid;
		const char *_upper = _keys + (_klen<<1) - 2;
		while (1) {
			if ( _upper < _lower )
				break;

			_mid = _lower + (((_upper-_lower) >> 1) & ~1);
			if ( (*( fsm->p)) < _mid[0] )
				_upper = _mid - 2;
			else if ( (*( fsm->p)) > _mid[1] )
				_lower = _mid + 2;
			else {
				_trans += (unsigned int)((_mid - _keys)>>1);
				goto _match;
			}
		}
		_trans += _klen;
	}

_match:
	_trans = _gpsdata_parser_fsm_indicies[_trans];
	 fsm->cs = _gpsdata_parser_fsm_trans_targs[_trans];

	if ( _gpsdata_parser_fsm_trans_actions[_trans] == 0 )
		goto _again;

	_acts = _gpsdata_parser_fsm_actions + _gpsdata_parser_fsm_trans_actions[_trans];
	_nacts = (unsigned int) *_acts++;
	while ( _nacts-- > 0 )
	{
		switch ( *_acts++ )
		{
	case 0:
#line 87 "gpsparser.rl"
	{ if (fsm->clean_state) fsm->clean_state(fsm); }
	break;
	case 1:
#line 88 "gpsparser.rl"
	{ fsm->msgid = GPSDATA_MSGID_GPGGA; }
	break;
	case 2:
#line 89 "gpsparser.rl"
	{ fsm->msgid = GPSDATA_MSGID_GPGSA; }
	break;
	case 3:
#line 90 "gpsparser.rl"
	{ fsm->msgid = GPSDATA_MSGID_GPGSV; }
	break;
	case 4:
#line 91 "gpsparser.rl"
	{ fsm->msgid = GPSDATA_MSGID_GPRMC; }
	break;
	case 5:
#line 92 "gpsparser.rl"
	{ fsm->msgid = GPSDATA_MSGID_GPVTG; }
	break;
	case 6:
#line 93 "gpsparser.rl"
	{ fsm->msgid = GPSDATA_MSGID_PGTOP; }
	break;
	case 7:
#line 94 "gpsparser.rl"
	{ fsm->msgid = GPSDATA_MSGID_PMTK;  }
	break;
	case 8:
#line 96 "gpsparser.rl"
	{ fsm->_tm.tm_mday  = 0; }
	break;
	case 9:
#line 97 "gpsparser.rl"
	{ fsm->_tm.tm_mday += ((*( fsm->p)) - '0'); }
	break;
	case 10:
#line 98 "gpsparser.rl"
	{ fsm->_tm.tm_mday += 10 * ((*( fsm->p)) - '0'); }
	break;
	case 11:
#line 99 "gpsparser.rl"
	{ fsm->_tm.tm_mon   = 0; }
	break;
	case 12:
#line 100 "gpsparser.rl"
	{ fsm->_tm.tm_mon  += ((*( fsm->p)) - '0'); }
	break;
	case 13:
#line 101 "gpsparser.rl"
	{ fsm->_tm.tm_mon  += 10 * ((*( fsm->p)) - '0'); }
	break;
	case 14:
#line 102 "gpsparser.rl"
	{ fsm->_tm.tm_mon--; }
	break;
	case 15:
#line 103 "gpsparser.rl"
	{ fsm->_tm.tm_year  = 0; }
	break;
	case 16:
#line 104 "gpsparser.rl"
	{ fsm->_tm.tm_year += ((*( fsm->p)) - '0'); }
	break;
	case 17:
#line 105 "gpsparser.rl"
	{ fsm->_tm.tm_year += 10 * ((*( fsm->p)) - '0'); }
	break;
	case 18:
#line 108 "gpsparser.rl"
	{ fsm->_tm.tm_hour = 0; }
	break;
	case 19:
#line 109 "gpsparser.rl"
	{ fsm->_tm.tm_hour += ((*( fsm->p)) - '0'); }
	break;
	case 20:
#line 110 "gpsparser.rl"
	{ fsm->_tm.tm_hour += 10 * ((*( fsm->p)) - '0'); }
	break;
	case 21:
#line 111 "gpsparser.rl"
	{ fsm->_tm.tm_min = 0; }
	break;
	case 22:
#line 112 "gpsparser.rl"
	{ fsm->_tm.tm_min += ((*( fsm->p)) - '0'); }
	break;
	case 23:
#line 113 "gpsparser.rl"
	{ fsm->_tm.tm_min += 10 * ((*( fsm->p)) - '0'); }
	break;
	case 24:
#line 114 "gpsparser.rl"
	{ fsm->_tm.tm_sec = 0; }
	break;
	case 25:
#line 115 "gpsparser.rl"
	{ fsm->_tm.tm_sec += ((*( fsm->p)) - '0'); }
	break;
	case 26:
#line 116 "gpsparser.rl"
	{ fsm->_tm.tm_sec += 10 * ((*( fsm->p)) - '0'); }
	break;
	case 27:
#line 117 "gpsparser.rl"
	{ fsm->_tm_msec = 0; }
	break;
	case 28:
#line 118 "gpsparser.rl"
	{ fsm->_tm_msec += ((*( fsm->p)) - '0'); }
	break;
	case 29:
#line 119 "gpsparser.rl"
	{ fsm->_tm_msec += 10 * ((*( fsm->p)) - '0'); }
	break;
	case 30:
#line 120 "gpsparser.rl"
	{ fsm->_tm_msec += 100 * ((*( fsm->p)) - '0'); }
	break;
	case 31:
#line 122 "gpsparser.rl"
	{
        fsm->_lll.degrees = 0;
        fsm->_lll.minutes = 0;
        fsm->_lll.direction = GPSDATA_DIRECTION_UNSET;
    }
	break;
	case 32:
#line 127 "gpsparser.rl"
	{ fsm->_lll.degrees += ((*( fsm->p)) - '0'); }
	break;
	case 33:
#line 128 "gpsparser.rl"
	{ fsm->_lll.degrees += 10 * ((*( fsm->p)) - '0'); }
	break;
	case 34:
#line 129 "gpsparser.rl"
	{ fsm->_lll.degrees += 100 * ((*( fsm->p)) - '0'); }
	break;
	case 35:
#line 130 "gpsparser.rl"
	{ fsm->_lll.minutes += ((*( fsm->p)) - '0'); }
	break;
	case 36:
#line 131 "gpsparser.rl"
	{ fsm->_lll.minutes += 10 * ((*( fsm->p)) - '0'); }
	break;
	case 37:
#line 132 "gpsparser.rl"
	{ fsm->_lll.minutes += 0.0001 * ((*( fsm->p)) - '0'); }
	break;
	case 38:
#line 133 "gpsparser.rl"
	{ fsm->_lll.minutes += 0.001 * ((*( fsm->p)) - '0'); }
	break;
	case 39:
#line 134 "gpsparser.rl"
	{ fsm->_lll.minutes += 0.01 * ((*( fsm->p)) - '0'); }
	break;
	case 40:
#line 135 "gpsparser.rl"
	{ fsm->_lll.minutes += 0.1 * ((*( fsm->p)) - '0'); }
	break;
	case 41:
#line 136 "gpsparser.rl"
	{
        fsm->_lat.direction = fsm->_lll.direction;
        fsm->_lat.degrees = fsm->_lll.degrees;
        fsm->_lat.minutes = fsm->_lll.minutes;
    }
	break;
	case 42:
#line 141 "gpsparser.rl"
	{
        fsm->_lon.direction = fsm->_lll.direction;
        fsm->_lon.degrees = fsm->_lll.degrees;
        fsm->_lon.minutes = fsm->_lll.minutes;
    }
	break;
	case 43:
#line 146 "gpsparser.rl"
	{
        fsm->_magvar_direction = GPSDATA_DIRECTION_UNSET;
        if (!isnanf(fsm->_tmp_float)) {
            fsm->_magvar_degrees = fsm->_tmp_float;
        } else {
            GPSUTILS_DEBUG("GPVTG magnetic variation is empty/nan\n");
            fsm->_magvar_degrees = NAN;
        }
        fsm->_tmp_float = NAN;
    }
	break;
	case 44:
#line 156 "gpsparser.rl"
	{
        fsm->_lat.direction = ((*( fsm->p)) == 'N') ? GPSDATA_DIRECTION_NORTH : GPSDATA_DIRECTION_SOUTH; 
    }
	break;
	case 45:
#line 159 "gpsparser.rl"
	{
        fsm->_lon.direction = ((*( fsm->p)) == 'E') ? GPSDATA_DIRECTION_EAST : GPSDATA_DIRECTION_WEST; 
    }
	break;
	case 46:
#line 162 "gpsparser.rl"
	{
        if ((*( fsm->p)) == 'E') {
            fsm->_magvar_direction = GPSDATA_DIRECTION_EAST;
        } else if ((*( fsm->p)) == 'W') {
            fsm->_magvar_direction = GPSDATA_DIRECTION_WEST;
        } else {
            GPSUTILS_DEBUG("GPRMC magnetic variation direction is empty/null\n");
            fsm->_magvar_direction = GPSDATA_DIRECTION_UNSET;
        }
    }
	break;
	case 47:
#line 172 "gpsparser.rl"
	{
        if ((*( fsm->p)) == '0') {
            fsm->_posfix = GPSDATA_POSFIX_NOFIX;
        } else if ((*( fsm->p)) == '1') {
            fsm->_posfix = GPSDATA_POSFIX_GPSFIX;
        } else if ((*( fsm->p)) == '2') {
            fsm->_posfix = GPSDATA_POSFIX_DGPSFIX;
        }
    }
	break;
	case 48:
#line 181 "gpsparser.rl"
	{
        if ((*( fsm->p)) == 'M') {
            fsm->mode1 = GPSDATA_MODE1_MANUAL;
        } else if ((*( fsm->p)) == 'A') {
            fsm->mode1 = GPSDATA_MODE1_AUTOMATIC;
        }
    }
	break;
	case 49:
#line 188 "gpsparser.rl"
	{
        if ((*( fsm->p)) == '1') {
            fsm->mode2 = GPSDATA_MODE2_NOFIX;
        } else if ((*( fsm->p)) == '2') {
            fsm->mode2 = GPSDATA_MODE2_2DFIX;
        } else if ((*( fsm->p)) == '3') {
            fsm->mode2 = GPSDATA_MODE2_3DFIX;
        }
    }
	break;
	case 50:
#line 198 "gpsparser.rl"
	{
        if ((*( fsm->p)) == 'A') {
            fsm->mode_common = GPSDATA_MODE_AUTONOMOUS;
        } else if ((*( fsm->p)) == 'D') {
            fsm->mode_common = GPSDATA_MODE_DIFFERENTIAL;
        } else if ((*( fsm->p)) == 'E') {
            fsm->mode_common = GPSDATA_MODE_ESTIMATED;
        }
    }
	break;
	case 51:
#line 208 "gpsparser.rl"
	{
        if ((*( fsm->p)) == 'A') {
            fsm->_rmc_valid = true;
        } else if ((*( fsm->p)) == 'V') {
            fsm->_rmc_valid = true;
        } else {
            GPSUTILS_ERROR("GPRMC Status can be either 'A' or 'V', not '%c'\n", (*( fsm->p))); 
        }
    }
	break;
	case 52:
#line 218 "gpsparser.rl"
	{
        fsm->_pgtop_fntype = fsm->_pgtop_fntype * 10 + ((*( fsm->p)) - '0');
    }
	break;
	case 53:
#line 222 "gpsparser.rl"
	{
        fsm->_pgtop_value = ((*( fsm->p)) - '0');
        if (!(fsm->_pgtop_value >= 1 && fsm->_pgtop_value <= 3)) {
            GPSUTILS_ERROR("PGTOP value should be in [1,3]. We have: %d", fsm->_pgtop_value);
        }
    }
	break;
	case 54:
#line 229 "gpsparser.rl"
	{
        fsm->_num_sats = fsm->_num_sats * 10 + ((*( fsm->p)) - '0');
    }
	break;
	case 55:
#line 232 "gpsparser.rl"
	{
        fsm->_tmp_real[0] = 0;
        fsm->_tmp_real[1] = 0;
        fsm->_tmp_real[2] = 1;
        fsm->_tmp_real[3] = 1;
        fsm->_tmp_float = NAN;
    }
	break;
	case 56:
#line 239 "gpsparser.rl"
	{
        fsm->_tmp_real[0]  = fsm->_tmp_real[0] * 10 + ((*( fsm->p)) - '0');
    }
	break;
	case 57:
#line 242 "gpsparser.rl"
	{
        fsm->_tmp_real[1]  = fsm->_tmp_real[1] * 10 + ((*( fsm->p)) - '0');
        fsm->_tmp_real[2] *= 10;
    }
	break;
	case 58:
#line 246 "gpsparser.rl"
	{
        fsm->_tmp_real[3] = -1;
    }
	break;
	case 59:
#line 249 "gpsparser.rl"
	{
        // if we do not use float here the compiler does the RHS in ints and
        // then converts to float which is not what we want
        fsm->_tmp_float = ((float)fsm->_tmp_real[0] +
            ((float)fsm->_tmp_real[1] / fsm->_tmp_real[2])) * (float)fsm->_tmp_real[3];
        /*GPSUTILS_DEBUG("tmp_real: %d %d %d %d %f\n",
            fsm->_tmp_real[0], fsm->_tmp_real[1],
            fsm->_tmp_real[2], fsm->_tmp_real[3],
            fsm->_tmp_float);*/
    }
	break;
	case 60:
#line 260 "gpsparser.rl"
	{
        fsm->_hdop = fsm->_tmp_float;
        fsm->_tmp_float = NAN;
    }
	break;
	case 61:
#line 264 "gpsparser.rl"
	{
        fsm->_pdop = fsm->_tmp_float;
        fsm->_tmp_float = NAN;
    }
	break;
	case 62:
#line 268 "gpsparser.rl"
	{
        fsm->_vdop = fsm->_tmp_float;
        fsm->_tmp_float = NAN;
    }
	break;
	case 63:
#line 272 "gpsparser.rl"
	{
        fsm->_altitude = fsm->_tmp_float;
        fsm->_tmp_float = NAN;
    }
	break;
	case 64:
#line 276 "gpsparser.rl"
	{
        fsm->_geoidal = fsm->_tmp_float;
        fsm->_tmp_float = NAN;
    }
	break;
	case 65:
#line 280 "gpsparser.rl"
	{
        fsm->_speed_knots = fsm->_tmp_float;
        fsm->_tmp_float = NAN;
    }
	break;
	case 66:
#line 284 "gpsparser.rl"
	{
        fsm->_speed_kmph = fsm->_tmp_float;
        fsm->_tmp_float = NAN;
    }
	break;
	case 67:
#line 288 "gpsparser.rl"
	{
        fsm->_course_degrees = fsm->_tmp_float;
        fsm->_tmp_float = NAN;
    }
	break;
	case 68:
#line 292 "gpsparser.rl"
	{
        if (!isnanf(fsm->_tmp_float)) {
            fsm->_heading_degrees = fsm->_tmp_float;
        } else {
            GPSUTILS_DEBUG("GPVTG magnetic heading is empty/nan\n");
            fsm->_heading_degrees = NAN;
        }
        fsm->_tmp_float = NAN;
    }
	break;
	case 69:
#line 302 "gpsparser.rl"
	{
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
	break;
	case 70:
#line 317 "gpsparser.rl"
	{
        fsm->_num_msg_max = ((*( fsm->p)) - '0');
    }
	break;
	case 71:
#line 320 "gpsparser.rl"
	{
        fsm->_num_msg_idx = ((*( fsm->p)) - '0') - 1; // index is 1-based so we make it 0-based
    }
	break;
	case 72:
#line 323 "gpsparser.rl"
	{
        if (!isnanf(fsm->_tmp_float)) {
            uint8_t num = ((uint32_t)(fsm->_tmp_float)) & 0xFF;
            fsm->_tmp_float = NAN;
            if (num >= 1 && num <= 32) {
                GPSUTILS_DEBUG("Satellite ID: %d\n", num);
                fsm->_gsv_sats[fsm->_gsv_sat_idx].satellite_id = num;
            } else {
                GPSUTILS_ERROR("GPGSV satellite_id is not in [1,32]: %d\n", num);
                fsm->_gsv_sats[fsm->_gsv_sat_idx].satellite_id = 0;
            }
        } else {
            GPSUTILS_DEBUG("GPGSV satelliteid is empty/nan\n");
            fsm->_gsv_sats[fsm->_gsv_sat_idx].satellite_id = 0;
        }
    }
	break;
	case 73:
#line 339 "gpsparser.rl"
	{
        if (!isnanf(fsm->_tmp_float)) {
            uint8_t num = ((uint32_t)(fsm->_tmp_float)) & 0xFF;
            fsm->_tmp_float = NAN;
            if (num >= 0 && num <= 90) {
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
	break;
	case 74:
#line 358 "gpsparser.rl"
	{
        if (!isnanf(fsm->_tmp_float)) {
            uint16_t num = ((uint32_t)(fsm->_tmp_float)) & 0x0000FFFF;
            fsm->_tmp_float = NAN;
            if (num >= 0 && num < 360) {
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
	break;
	case 75:
#line 377 "gpsparser.rl"
	{
        if (!isnanf(fsm->_tmp_float)) {
            uint8_t num = ((uint32_t)(fsm->_tmp_float)) & 0xFF;
            fsm->_tmp_float = NAN;
            if (num >= 0 && num <= 99) {
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
	break;
	case 76:
#line 399 "gpsparser.rl"
	{
        if ((*( fsm->p)) <= '9') {
            fsm->_checksum = fsm->_checksum * 16 + ((*( fsm->p)) - '0');
        } else {
            fsm->_checksum = fsm->_checksum * 16 + ((toupper((*( fsm->p))) - 'A') + 10);
        }
    }
	break;
	case 77:
#line 406 "gpsparser.rl"
	{ fsm->_calc_checksum = 0; }
	break;
	case 78:
#line 407 "gpsparser.rl"
	{ fsm->_calc_checksum ^= (*( fsm->p)); }
	break;
	case 79:
#line 408 "gpsparser.rl"
	{
        if (fsm->_calc_checksum != fsm->_checksum) {
            GPSUTILS_ERROR("Checksum does not match. Expected: %x Calculated: %x\n",
                    fsm->_checksum, fsm->_calc_checksum);
        }
    }
	break;
#line 1418 "gpsparser.c"
		}
	}

_again:
	if (  fsm->cs == 0 )
		goto _out;
	if ( ++( fsm->p) != ( fsm->pe) )
		goto _resume;
	_test_eof: {}
	if ( ( fsm->p) == ( fsm->eof) )
	{
	const char *__acts = _gpsdata_parser_fsm_actions + _gpsdata_parser_fsm_eof_actions[ fsm->cs];
	unsigned int __nacts = (unsigned int) *__acts++;
	while ( __nacts-- > 0 ) {
		switch ( *__acts++ ) {
	case 79:
#line 408 "gpsparser.rl"
	{
        if (fsm->_calc_checksum != fsm->_checksum) {
            GPSUTILS_ERROR("Checksum does not match. Expected: %x Calculated: %x\n",
                    fsm->_checksum, fsm->_calc_checksum);
        }
    }
	break;
#line 1443 "gpsparser.c"
		}
	}
	}

	_out: {}
	}

#line 653 "gpsparser.rl"
    if (fsm->cs == 0) {
        GPSUTILS_ERROR("Error in parsing. fsm->cs: %d\t Len: %zu Buffer: \n", fsm->cs, len);
        gpsutils_hex_dump((const uint8_t *)bytes, len, GPSUTILS_LOG_PTR);
        return -1;
    } else if (fsm->cs >= 299) {
        fsm->cs = 299;
        return 1;
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
        free(fsm);
        fsm = NULL;
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
            gpsdata_data_t **outp, size_t *olen)
{
    if (!fsm || !data || len == 0) {
        GPSUTILS_DEBUG("Invalid input arguments\n");
        return -1;
    }
    if (!fsm->execute) {
        GPSUTILS_ERROR("Invalid function setup for parsing\n");
        return -1;
    }
    int rc = 0;
    if ((rc = fsm->execute(fsm, data, len)) < 0) {
        GPSUTILS_ERROR("Failed to parse data\n");
        if (fsm->dump_state)
            fsm->dump_state(fsm, stdout);
        return rc;
    }
    //TODO: add retrieved data to outp
    return rc;
}

