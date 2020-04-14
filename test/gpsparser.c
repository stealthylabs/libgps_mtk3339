/*
 * COPYRIGHT: 2015-2020 Stealthy Labs LLC
 * ORIGINAL DATE: 22nd April 2015
 * MODIFIED DATE: 16th Oct 2019
 * MODIFIED SOFTWARE: libgps_mtk3339
 */
#include <gpsdata.h>
#ifdef HAVE_FCNTL_H
    #include <fcntl.h>
#endif
#ifdef HAVE_CUNIT
    #include <CUnit/CUnit.h>
    #include <CUnit/Basic.h>
#endif

void test_parse_pmtk()
{
    int rc = 0;
    gpsutils_timer_t tt;
    const char *pmtk =
        "$PMTK001,220,3*30\r\n$PMTK001,314,3*36\r\n$PMTK001,103,2*30\r\n";
    gpsdata_parser_t *fsm = gpsdata_parser_create();
    CU_ASSERT_PTR_NOT_NULL(fsm);
    GPSUTILS_INFO("\n\nInput buffer: %s\n\n", pmtk);

    gpsdata_data_t *outp = NULL;
    size_t onum = 0;
    gpsutils_timer_start(&tt);
    rc = gpsdata_parser_parse(fsm, pmtk, strlen(pmtk), &outp, &onum);
    gpsutils_timer_stop(&tt);
    GPSUTILS_INFO("Time taken to parse input: %lf\n", tt.time_taken);
    gpsdata_parser_dump_state(fsm, stdout);
    CU_ASSERT(rc >= 0);
    CU_ASSERT_EQUAL(onum, 0);
    CU_ASSERT_PTR_NULL(outp);
    gpsdata_list_dump(outp, stdout);
    for (size_t idx = 0; idx <= strlen(pmtk); idx++) { // includes the null character
        rc = gpsdata_parser_parse(fsm, &pmtk[idx], 1, &outp, &onum);
        CU_ASSERT(rc >= 0);
        CU_ASSERT_EQUAL(onum, 0);
        CU_ASSERT_PTR_NULL(outp);
        gpsdata_list_dump(outp, stdout);
    }
    gpsdata_parser_dump_state(fsm, stderr);
    gpsdata_list_free(&outp);
    gpsdata_parser_free(fsm);
}

void test_parse_pgtop()
{
    int rc = 0;
    gpsutils_timer_t tt;
    const char *pgtop = "$PGTOP,11,3*6F\r\n";
    gpsdata_parser_t *fsm = gpsdata_parser_create();
    CU_ASSERT_PTR_NOT_NULL(fsm);
    GPSUTILS_INFO("\n\nInput buffer: %s\n\n", pgtop);

    gpsdata_data_t *outp = NULL;
    size_t onum = 0;

    gpsutils_timer_start(&tt);
    rc = gpsdata_parser_parse(fsm, pgtop, strlen(pgtop), &outp, &onum);
    gpsutils_timer_stop(&tt);
    GPSUTILS_INFO("Time taken to parse input: %lfs\n", tt.time_taken);
    CU_ASSERT(rc >= 0);
    gpsdata_parser_dump_state(fsm, stdout);
    CU_ASSERT_EQUAL(onum, 1);
    CU_ASSERT_PTR_NOT_NULL(outp);
    gpsdata_list_dump(outp, stdout);
    ssize_t cnt = gpsdata_list_count(outp);
    GPSUTILS_INFO("Parsed message into %zd objects\n", cnt);
    CU_ASSERT_EQUAL(cnt, onum);

    for (size_t i = 0; i < strlen(pgtop) + 1; i++) { // includes the null character
        rc = gpsdata_parser_parse(fsm, &pgtop[i], 1, &outp, &onum);
        CU_ASSERT(rc >= 0);
    }
    cnt = gpsdata_list_count(outp);
    GPSUTILS_INFO("Parsed message into %zd objects\n", cnt);
    CU_ASSERT_EQUAL(cnt, 2);
    gpsdata_list_free(&outp);
    gpsdata_parser_dump_state(fsm, stderr);
    gpsdata_parser_free(fsm);
}

void test_parse_gpgga()
{
    const char *gpgga =
"$GPGGA,185916.000,4048.5993,N,07418.5416,W,1,07,1.09,107.2,M,-34.2,M,,*5D\r\n";
    
    int rc = 0;
    gpsutils_timer_t tt;
    size_t gpgga_len = strlen(gpgga);
    gpsdata_parser_t *fsm = gpsdata_parser_create();
    CU_ASSERT_PTR_NOT_NULL(fsm);

    gpsdata_data_t *outp = NULL;
    size_t onum = 0;
    GPSUTILS_INFO("\n\nInput buffer: %s\n\n", gpgga);
    gpsutils_timer_start(&tt);
    rc = gpsdata_parser_parse(fsm, gpgga, gpgga_len, &outp, &onum);
    gpsutils_timer_stop(&tt);
    GPSUTILS_INFO("Time taken to parse input: %lfs\n", tt.time_taken);
    GPSUTILS_DEBUG("onum %zu\n", onum);
    gpsdata_parser_dump_state(fsm, stdout);
    CU_ASSERT(rc >= 0);
    CU_ASSERT_EQUAL(onum, 1);
    CU_ASSERT_PTR_NOT_NULL(outp);
    gpsdata_list_dump(outp, stdout);
    ssize_t cnt = gpsdata_list_count(outp);
    GPSUTILS_INFO("Parsed message into %zd objects\n", cnt);
    CU_ASSERT_EQUAL(cnt, onum);

    /* split up the message into two to see if half-buffers can get parsed */
    char *gpgga_1 = malloc(64 * sizeof(char));
    CU_ASSERT_PTR_NOT_NULL(gpgga_1);
    memcpy(gpgga_1, &gpgga[0], 64);
    char *gpgga_2 = malloc((gpgga_len - 64) * sizeof(char));
    CU_ASSERT_PTR_NOT_NULL(gpgga_2);
    memcpy(gpgga_2, &gpgga[64], gpgga_len - 64);
    
    rc = gpsdata_parser_parse(fsm, gpgga_1, 64, &outp, &onum);
    CU_ASSERT(rc >= 0);
    GPSUTILS_DEBUG("onum %zu\n", onum);
    CU_ASSERT_EQUAL(onum, 1);
    CU_ASSERT_PTR_NOT_NULL(outp);
    gpsdata_parser_dump_state(fsm, stdout);
    rc = gpsdata_parser_parse(fsm, gpgga_2, gpgga_len - 64, &outp, &onum);
    GPSUTILS_DEBUG("onum %zu\n", onum);
    CU_ASSERT(rc >= 0);
    gpsdata_parser_dump_state(fsm, stdout);
    CU_ASSERT_EQUAL(onum, 1);
    CU_ASSERT_PTR_NOT_NULL(outp);
    cnt = gpsdata_list_count(outp);
    GPSUTILS_INFO("Parsed message into %zd objects\n", cnt);
    CU_ASSERT_EQUAL(cnt, 2);

    gpsdata_list_free(&outp);
    gpsdata_parser_free(fsm);
    GPSUTILS_FREE(gpgga_1);
    GPSUTILS_FREE(gpgga_2);
}

void test_parse_gpgsa()
{
    const char *gpgsa =
        "$GPGSA,A,3,29,21,26,15,18,09,06,10,,,,,2.32,0.95,2.11*00";
    int rc = 0;
    gpsutils_timer_t tt;
    size_t gpgsa_len = strlen(gpgsa);
    gpsdata_parser_t *fsm = gpsdata_parser_create();
    CU_ASSERT_PTR_NOT_NULL(fsm);

    gpsdata_data_t *outp = NULL;
    size_t onum = 0;
    GPSUTILS_INFO("\n\nInput buffer: %s\n\n", gpgsa);
    gpsutils_timer_start(&tt);
    rc = gpsdata_parser_parse(fsm, gpgsa, gpgsa_len, &outp, &onum);
    gpsutils_timer_stop(&tt);
    GPSUTILS_INFO("Time taken to parse input: %lfs\n", tt.time_taken);
    gpsdata_parser_dump_state(fsm, stdout);
    CU_ASSERT(rc >= 0);
    CU_ASSERT_EQUAL(onum, 0);
    CU_ASSERT_PTR_NULL(outp);
    /* split up the message into two to see if half-buffers can get parsed */
    char *gpgsa_1 = malloc(gpgsa_len / 2 * sizeof(char));
    CU_ASSERT_PTR_NOT_NULL(gpgsa_1);
    memcpy(gpgsa_1, &gpgsa[0], gpgsa_len / 2);
    char *gpgsa_2 = malloc((gpgsa_len - gpgsa_len / 2 + 1) * sizeof(char));
    CU_ASSERT_PTR_NOT_NULL(gpgsa_2);
    memcpy(gpgsa_2, &gpgsa[gpgsa_len / 2], gpgsa_len - gpgsa_len / 2);
    
    rc = gpsdata_parser_parse(fsm, gpgsa_1, gpgsa_len / 2, &outp, &onum);
    CU_ASSERT(rc >= 0);
    gpsdata_parser_dump_state(fsm, stdout);
    rc = gpsdata_parser_parse(fsm, gpgsa_2, gpgsa_len - gpgsa_len / 2, &outp, &onum);
    CU_ASSERT(rc >= 0);
    gpsdata_parser_dump_state(fsm, stdout);
    CU_ASSERT_EQUAL(onum, 0);
    CU_ASSERT_PTR_NULL(outp);

    GPSUTILS_FREE(gpgsa_1);
    GPSUTILS_FREE(gpgsa_2);
    gpsdata_parser_free(fsm);
}

void test_parse_gpgsv()
{
    const char *gpgsv =
        "$GPGSV,3,1,09,29,36,029,42,21,46,314,43,26,44,020,43,15,21,321,39*7D\n$GPGSV,3,2,09,18,26,314,40,09,57,170,44,06,20,229,37,10,26,084,37*77\n$GPGSV,3,3,09,07,,,26*73\n";
    const char *gpgsv1 = 
"$GPGSV,2,1,07,21,67,278,18,15,66,048,38,20,46,302,26,24,38,154,13*72\r\n$GPGSV,2,2,07,13,33,048,22,10,19,289,17,41,,,*79\r\n";

    int rc = 0;
    gpsutils_timer_t tt;
    size_t gpgsv_len = strlen(gpgsv);
    gpsdata_parser_t *fsm = gpsdata_parser_create();
    CU_ASSERT_PTR_NOT_NULL(fsm);

    gpsdata_data_t *outp = NULL;
    size_t onum = 0;
    GPSUTILS_INFO("\n\nInput buffer: %s\n\n", gpgsv);
    gpsutils_timer_start(&tt);
    rc = gpsdata_parser_parse(fsm, gpgsv, gpgsv_len, &outp, &onum);
    gpsutils_timer_stop(&tt);
    GPSUTILS_INFO("Time taken to parse input: %lfs\n", tt.time_taken);
    gpsdata_parser_dump_state(fsm, stdout);
    CU_ASSERT(rc >= 0);
    CU_ASSERT_EQUAL(onum, 0);
    CU_ASSERT_PTR_NULL(outp);

    GPSUTILS_INFO("\n\nInput buffer: %s\n\n", gpgsv1);
    gpsutils_timer_start(&tt);
    rc = gpsdata_parser_parse(fsm, gpgsv1, strlen(gpgsv1), &outp, &onum);
    gpsutils_timer_stop(&tt);
    GPSUTILS_INFO("Time taken to parse input: %lfs\n", tt.time_taken);
    gpsdata_parser_dump_state(fsm, stdout);
    CU_ASSERT(rc >= 0);
    CU_ASSERT_EQUAL(onum, 0);
    CU_ASSERT_PTR_NULL(outp);

    gpsdata_parser_free(fsm);
}

void test_parse_gprmc()
{
    const char *gprmc =
        "$GPRMC,064951.000,A,2307.1256,N,12016.4438,E,0.03,165.48,260406,3.05,W,A*2C\n";
    int rc = 0;
    gpsutils_timer_t tt;
    size_t gprmc_len = strlen(gprmc);
    gpsdata_parser_t *fsm = gpsdata_parser_create();
    CU_ASSERT_PTR_NOT_NULL(fsm);

    GPSUTILS_INFO("\n\nInput buffer: %s\n\n", gprmc);
    gpsutils_timer_start(&tt);
    gpsdata_data_t *outp = NULL;
    size_t onum = 0;
    rc = gpsdata_parser_parse(fsm, gprmc, gprmc_len, &outp, &onum);
    gpsutils_timer_stop(&tt);
    GPSUTILS_INFO("Time taken to parse input: %lfs\n", tt.time_taken);
    gpsdata_parser_dump_state(fsm, stdout);
    CU_ASSERT(rc >= 0);
    CU_ASSERT_EQUAL(onum, 1);
    CU_ASSERT_PTR_NOT_NULL(outp);
    gpsdata_list_dump(outp, stdout);
    ssize_t cnt = gpsdata_list_count(outp);
    GPSUTILS_INFO("Parsed message into %zd objects\n", cnt);
    CU_ASSERT_EQUAL(cnt, onum);
    gpsdata_list_free(&outp);
    gpsdata_parser_free(fsm);
}

void test_parse_gpvtg()
{
    const char *gpvtg = "$GPVTG,7.37,T,,M,1.10,N,2.04,K,A*38\n";
    int rc = 0;
    gpsutils_timer_t tt;
    size_t gpvtg_len = strlen(gpvtg);
    gpsdata_parser_t *fsm = gpsdata_parser_create();
    CU_ASSERT_PTR_NOT_NULL(fsm);

    gpsdata_data_t *outp = NULL;
    size_t onum = 0;
    GPSUTILS_INFO("\n\nInput buffer: %s\n\n", gpvtg);
    gpsutils_timer_start(&tt);
    rc = gpsdata_parser_parse(fsm, gpvtg, gpvtg_len, &outp, &onum);
    gpsutils_timer_stop(&tt);
    GPSUTILS_INFO("Time taken to parse input: %lfs\n", tt.time_taken);
    gpsdata_parser_dump_state(fsm, stdout);
    CU_ASSERT(rc >= 0);
    CU_ASSERT_EQUAL(onum, 1);
    CU_ASSERT_PTR_NOT_NULL(outp);
    gpsdata_list_dump(outp, stdout);
    ssize_t cnt = gpsdata_list_count(outp);
    GPSUTILS_INFO("Parsed message into %zd objects\n", cnt);
    CU_ASSERT_EQUAL(cnt, onum);
    gpsdata_list_free(&outp);

    gpsdata_parser_free(fsm);
}

int main(int argc, char **argv)
{
    int err = 0;
    CU_pSuite suite = NULL;
#ifndef NDEBUG
    GPSUTILS_LOGLEVEL_SET(DEBUG);
#else
    GPSUTILS_LOGLEVEL_SET(INFO);
#endif
    if (CU_initialize_registry() != CUE_SUCCESS) {
        GPSUTILS_ERROR("%s\n", CU_get_error_msg());
        return CU_get_error();
    }
    do {
        suite = CU_add_suite(argv[0], NULL, NULL);
        if (suite == NULL) {
            GPSUTILS_ERROR("%s\n",
                    CU_get_error_msg());
            break;
        }
        if (!CU_ADD_TEST(suite, test_parse_pmtk))
            break;
        if (!CU_ADD_TEST(suite, test_parse_pgtop))
            break;
        if (!CU_ADD_TEST(suite, test_parse_gpgga))
            break;
        if (!CU_ADD_TEST(suite, test_parse_gpgsa))
            break;
        if (!CU_ADD_TEST(suite, test_parse_gpgsv))
            break;
        if (!CU_ADD_TEST(suite, test_parse_gprmc))
            break;
        if (!CU_ADD_TEST(suite, test_parse_gpvtg))
            break;
        /* set the mode of
         * the test run in
         * debug/release
         * mode*/
        CU_basic_set_mode(CU_BRM_VERBOSE);
        CU_basic_run_tests();
    } while (0);
    err = CU_get_number_of_tests_failed();
    CU_cleanup_registry();
    return err;
}
